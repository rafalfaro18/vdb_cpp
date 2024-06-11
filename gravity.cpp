#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeAdvect.h>

std::string my_to_string2(int n, int width = 4)
{
    auto s = std::to_string(n);
    if (int(s.size()) < width) s.insert(0, width - s.size(), '0');
    return s;
}

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

    xyz.reset(1, -100, 1);
    accessor.setValue(xyz, 1.0);

    xyz.reset(1, 100, 1);
    accessor.setValue(xyz, 1.0);

    xyz.reset(1, 0, 100);
    accessor.setValue(xyz, 1.0);

    xyz.reset(1, 0, -100);
    accessor.setValue(xyz, 1.0);

    xyz.reset(100, 0, 0);
    accessor.setValue(xyz, 1.0);

    xyz.reset(-100, 0, 0);
    accessor.setValue(xyz, 1.0);

    // Associate a scaling transform with the grid that sets the voxel size
    // to 1 units in world space.
    grid->setTransform(openvdb::math::Transform::createLinearTransform(/*voxel size=*/1));
    
    // Name the grid "density".
    grid->setName("density");
    
    // Identify the grid as a fog volume.
    grid->setGridClass(openvdb::GRID_FOG_VOLUME);

    // Add the grid pointer to a container.
    openvdb::GridPtrVec grids;
    grids.push_back(grid);

    // Create an empty velocity grid with gravity as background value
    auto velocity = openvdb::Vec3SGrid::create();
    openvdb::Vec3SGrid::Accessor accessorV = velocity->getAccessor();
    xyz.reset(1, -2, 3);
    accessorV.setValue(xyz, openvdb::Vec3s(0, -0.065, 0));

    xyz.reset(1, 2, -3);
    accessorV.setValue(xyz, openvdb::Vec3s(0, 0.065, 0));

    // Associate a scaling transform with the grid that sets the voxel size
    // to 1 units in world space.
    velocity->setTransform(openvdb::math::Transform::createLinearTransform(/*voxel size=*/1));
    
    velocity->setName("velocity");
    // Identify the grid as a staggered.
    velocity->setGridClass(openvdb::GRID_STAGGERED);
    grids.push_back(velocity);

    // Create a VDB file object and write out the grid.
    openvdb::io::File("velocity_0000.vdb").write(grids);

    openvdb::FloatGrid::Ptr oldGrid;
    for (int i = 1; i<=250; i++)
    {
        auto advect = openvdb::tools::VolumeAdvection(*velocity);
        // Add the grid pointer to a container.
        openvdb::GridPtrVec newGrids;
        
        if(i == 1)
        {
            auto newGrid = advect.advect<openvdb::FloatGrid, openvdb::tools::PointSampler>(*grid, 1);
            newGrids.push_back(newGrid);
            oldGrid = grid->deepCopy();
        } else
        {
            auto newGrid = advect.advect<openvdb::FloatGrid, openvdb::tools::PointSampler>(*oldGrid, 1);
            newGrids.push_back(newGrid);
            oldGrid = newGrid->deepCopy();
        }
        
        std::string name = "velocity_" + my_to_string2(i) + ".vdb";
        
        newGrids.push_back(velocity);
        openvdb::io::File(name).write(newGrids);    
    }
    
}