#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeAdvect.h>
#include <openvdb/points/PointAdvect.h>
#include <iostream>
#include <openvdb/points/PointCount.h>

std::string my_to_string2(int n, int width = 4)
{
    auto s = std::to_string(n);
    if (int(s.size()) < width) s.insert(0, width - s.size(), '0');
    return s;
}

int main()
{
    // Initialize grid types and point attributes types.
    openvdb::initialize();
    // Generate a level set grid.
    openvdb::FloatGrid::Ptr sphereGrid =
        openvdb::tools::createLevelSetSphere<openvdb::FloatGrid>(/*radius=*/20.0,
            /*center=*/openvdb::Vec3f(1.5, 2, 3), /*voxel size=*/0.5);
    // Retrieve the number of leaf nodes in the grid.
    openvdb::Index leafCount = sphereGrid->tree().leafCount();
    // Use the topology to create a PointDataTree
    openvdb::points::PointDataTree::Ptr pointTree(
        new openvdb::points::PointDataTree(sphereGrid->tree(), 0, openvdb::TopologyCopy()));
    // Ensure all tiles have been voxelized
    pointTree->voxelizeActiveTiles();
    // Define the position type and codec using fixed-point 16-bit compression.
    using PositionAttribute = openvdb::points::TypedAttributeArray<openvdb::Vec3f,
        openvdb::points::FixedPointCodec<false>>;
    openvdb::NamePair positionType = PositionAttribute::attributeType();
    // Create a new Attribute Descriptor with position only
    openvdb::points::AttributeSet::Descriptor::Ptr descriptor(
        openvdb::points::AttributeSet::Descriptor::create(positionType));
    // Determine the number of points / voxel and points / leaf.
    openvdb::Index pointsPerVoxel = 8;
    openvdb::Index voxelsPerLeaf = openvdb::points::PointDataGrid::TreeType::LeafNodeType::SIZE;
    openvdb::Index pointsPerLeaf = pointsPerVoxel * voxelsPerLeaf;
    // Iterate over the leaf nodes in the point tree.
    for (auto leafIter = pointTree->beginLeaf(); leafIter; ++leafIter) {
        // Initialize the attributes using the descriptor and point count.
        leafIter->initializeAttributes(descriptor, pointsPerLeaf);
        // Initialize the voxel offsets
        openvdb::Index offset(0);
        for (openvdb::Index index = 0; index < voxelsPerLeaf; ++index) {
            offset += pointsPerVoxel;
            leafIter->setOffsetOn(index, offset);
        }
    }
    // Create the points grid.
    openvdb::points::PointDataGrid::Ptr points =
        openvdb::points::PointDataGrid::create(pointTree);
    // Set the name of the grid.
    points->setName("Points");
    // Copy the transform from the sphere grid.
    points->setTransform(sphereGrid->transform().copy());
    // Randomize the point positions.
    std::mt19937 generator(/*seed=*/0);
    std::uniform_real_distribution<> distribution(-0.5, 0.5);
    // Iterate over the leaf nodes in the point tree.
    for (auto leafIter = points->tree().beginLeaf(); leafIter; ++leafIter) {
        // Create an AttributeWriteHandle for position.
        // Note that the handle only requires the value type, not the codec.
        openvdb::points::AttributeArray& array = leafIter->attributeArray("P");
        openvdb::points::AttributeWriteHandle<openvdb::Vec3f> handle(array);
        // Iterate over the point indices in the leaf.
        for (auto indexIter = leafIter->beginIndexOn(); indexIter; ++indexIter) {
            // Compute a new random position (in the range -0.5 => 0.5).
            openvdb::Vec3f positionVoxelSpace(distribution(generator));
            // Set the position of this point.
            // As point positions are stored relative to the voxel center, it is
            // not necessary to convert these voxel space values into
            // world-space during this process.
            handle.set(*indexIter, positionVoxelSpace);
        }
    }

    // Create an empty velocity grid with gravity as background value
    auto gravity = openvdb::Vec3SGrid::create(openvdb::Vec3s(0, -9.81, 0));
    
    // Create a VDB file object and write out the grid.
    openvdb::io::File("velocity_0000.vdb").write({gravity, points});
    for (int i = 1; i<=250; i++)
    {

        // Advect points in-place using gravity velocity grid
        openvdb::points::advectPoints(*points, *gravity,
            /*integrationOrder=*/4, /*dt=*/1.0/24.0, /*timeSteps=*/1);
        
        std::string name = "velocity_" + my_to_string2(i) + ".vdb";
        
        
        openvdb::io::File(name).write({gravity, points});    
    }
    
}