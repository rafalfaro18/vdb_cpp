#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeAdvect.h>
int main()
{
    openvdb::initialize();
    // Create an empty floating-point grid with background value 0.
    openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create();

    // Get an accessor for coordinate-based access to voxels.
    openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
    // Define a coordinate with large signed indices.
    openvdb::Coord xyz(1, -2, 3);
    accessor.setValue(xyz, 1.0);

    xyz.reset(1, 2, -3);
    accessor.setValue(xyz, 2.0);

    // Associate a scaling transform with the grid that sets the voxel size
    // to 0.197 units in world space.
    grid->setTransform(openvdb::math::Transform::createLinearTransform(/*voxel size=*/0.197));
    
    // Name the grid "density".
    grid->setName("density");
    // Associate a scaling transform with the grid that sets the voxel size
    // to 0.5 units in world space.
    grid->setTransform(
        openvdb::math::Transform::createLinearTransform(/*voxel size=*/0.5));
    // Identify the grid as a fog volume.
    grid->setGridClass(openvdb::GRID_FOG_VOLUME);

    // Add the grid pointer to a container.
    openvdb::GridPtrVec grids;
    grids.push_back(grid);

    // Create an empty velocity grid with gravity as background value
    auto velocity = openvdb::Vec3SGrid::create();
    openvdb::Vec3SGrid::Accessor accessorV = velocity->getAccessor();
    xyz.reset(1, -2, 3);
    accessorV.setValue(xyz, openvdb::Vec3s(3.74609, -0.206909, 2.40625));

    xyz.reset(1, 2, -3);
    accessorV.setValue(xyz, openvdb::Vec3s(-3.97266, -0.139526, 2.60938));

    // Associate a scaling transform with the grid that sets the voxel size
    // to 0.197 units in world space.
    velocity->setTransform(openvdb::math::Transform::createLinearTransform(/*voxel size=*/0.197));
    
    velocity->setName("velocity");
    // Identify the grid as a staggered.
    velocity->setGridClass(openvdb::GRID_STAGGERED);
    grids.push_back(velocity);

    // TODO: Put this in a for loop
    auto advect = openvdb::tools::VolumeAdvection(*velocity);
    auto newGrid = advect.advect<openvdb::FloatGrid, openvdb::tools::PointSampler>(*grid, 1.0);
    
    // Create a VDB file object and write out the grid.
    openvdb::io::File("velocity_0000.vdb").write(grids);
    openvdb::io::File("velocity_0001.vdb").write({newGrid});
}