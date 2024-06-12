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

    const openvdb::Vec3fGrid::Ptr someGrid = openvdb::Vec3fGrid::create(openvdb::Vec3f(1, 1, 1));
    gravity->setName("density");
    
    // Create a VDB file object and write out the grid.
    openvdb::io::File("velocity_0000.vdb").write({gravity, someGrid});

    for (int i = 1; i<=250; i++)
    {
        // Advect points in-place using gravity velocity grid
        //openvdb::points::future::Advect(*sphereGrid, *gravity, /*integrationOrder=*/4, /*dt=*/1.0/24.0, /*timeSteps=*/1);
        auto a = openvdb::tools::VolumeAdvection<openvdb::Vec3fGrid, false, openvdb::util::NullInterrupter>(*gravity);
        a.setIntegrator(openvdb::tools::Scheme::SEMI);
        auto newGrid = a.advect<openvdb::Vec3fGrid, openvdb::tools::PointSampler>(*someGrid, 1);
        newGrid->setName("density");
        
        std::string name = "velocity_" + my_to_string2(i) + ".vdb";

        openvdb::io::File(name).write({gravity, newGrid});    
    }
    
}