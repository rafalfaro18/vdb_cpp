#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
int main()
{
    openvdb::initialize();
    // Create a FloatGrid and populate it with a narrow-band
    // signed distance field of a sphere.
    openvdb::FloatGrid::Ptr grid =
        openvdb::tools::createLevelSetSphere<openvdb::FloatGrid>(
            /*radius=*/50.0, /*center=*/openvdb::Vec3f(1.5, 2, 3),
            /*voxel size=*/0.5, /*width=*/4.0);
    // Associate some metadata with the grid.
    grid->insertMeta("radius", openvdb::FloatMetadata(50.0));
    // Name the grid "density".
    grid->setName("density");

    // Add the grid pointer to a container.
    openvdb::GridPtrVec grids;
    grids.push_back(grid);

    // Create an empty velocity grid with gravity as background value
    auto gravity = openvdb::Vec3SGrid::create(openvdb::Vec3s(0, -9.81, 0));
    gravity->setName("velocity");
    gravity->insertMeta("class", openvdb::StringMetadata("staggered"));
    grids.push_back(gravity);
    
    // Create a VDB file object and write out the grid.
    openvdb::io::File("gravity.vdb").write(grids);
}