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

    // Define a local function that doubles the value to which the given
    // value iterator points.
    struct Local {
        static inline void op(const openvdb::FloatGrid::ValueOnIter& iter) {
            iter.setValue(1.0);
        }
    };
    
    // Generate a level set grid. Note: This gets rendered perfectly in Houdini but in Blender looks like a square except in Cycles, but then it looks low poly
    openvdb::FloatGrid::Ptr sphereGrid = openvdb::tools::createLevelSetSphere<openvdb::FloatGrid>(/*radius=*/20.0, /*center=*/openvdb::Vec3f(1.5, 2, 3), /*voxel size=*/0.5);
    sphereGrid->setName("density");

    // openvdb::tools::foreach(sphereGrid->beginValueOn(), Local::op);
    
    // Create an empty velocity grid with gravity as background value
    auto gravity = openvdb::Vec3SGrid::create(openvdb::Vec3s(0, -9.81, 0));
    gravity->setName("gravity");
    
    // Create a VDB file object and write out the grid.
    openvdb::io::File("velocity_0000.vdb").write({gravity, sphereGrid});
    //for (int i = 1; i<=250; i++)
    //{

        // Advect points in-place using gravity velocity grid
    //    openvdb::points::advectPoints(*sphereGrid, *gravity,
    //        /*integrationOrder=*/4, /*dt=*/1.0/24.0, /*timeSteps=*/1);
        
    //    std::string name = "velocity_" + my_to_string2(i) + ".vdb";
        
        
    //    openvdb::io::File(name).write({gravity, sphereGrid});    
    //}
    
}