#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeAdvect.h>
#include <openvdb/points/PointAdvect.h>
#include <iostream>
#include <openvdb/tools/ValueTransformer.h>

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
    
    // Create an empty velocity grid with gravity as background value
    openvdb::Vec3fGrid::Ptr gravity = openvdb::Vec3fGrid::create(openvdb::Vec3f(0, -9.81, 0));
    gravity->setName("gravity");

    const openvdb::Vec3fGrid::Ptr someGrid = openvdb::Vec3fGrid::create(openvdb::Vec3f(0, 0, 0));
    auto access = someGrid->getAccessor();
    // Define a coordinate with large signed indices.
    openvdb::Coord xyz(0, 0, 0);
    access.setValue(xyz, openvdb::Vec3f(1,1,1));
    xyz.reset(10, 10, 10);
    access.setValue(xyz, openvdb::Vec3f(1,1,1));
    xyz.reset(-10, -10, -10);
    access.setValue(xyz, openvdb::Vec3f(1,1,1));
    xyz.reset(0, -50, 0);
    access.setValue(xyz, openvdb::Vec3f(1,1,1));
    xyz.reset(0, 50, 0);
    access.setValue(xyz, openvdb::Vec3f(1,1,1));

    // TODO: Set voxel size 1 for both, gravity and someGrid
    
    gravity->setName("velocity");
    
    // Create a VDB file object and write out the grid.
    //openvdb::io::File("velocity_0000.vdb").write({gravity, someGrid});
    openvdb::io::File("velocity_0000.vdb").write({someGrid, gravity});
    
    openvdb::Vec3fGrid::Ptr prevGrid = someGrid->copy();
    
    for (int i = 1; i<=250; i++)
    {
        auto a = openvdb::tools::VolumeAdvection<openvdb::Vec3fGrid, false, openvdb::util::NullInterrupter>(*gravity);
        auto newGrid = a.advect<openvdb::Vec3fGrid, openvdb::tools::BoxSampler>(*prevGrid, 1);
        newGrid->setName("density");
        
        std::string name = "velocity_" + my_to_string2(i) + ".vdb";
    
        openvdb::io::File(name).write({newGrid, gravity});
        prevGrid = newGrid->copy();
    }
    
}