# OpenVDB C++ Cpp Example(s)

## main.cpp
A basic example

## gravity.cpp
A working example of computing gravity. It currently can only be visualized in houdini which does support openvdb::points::PointDataGrid type grids which is unsupported by Blender my favorite DCC.

So a workaround I plan to explore is converting the openvdb::points::PointDataGrid to a openvdb::FloatGrid (voxelize it) apparently the Advect stuff is for points. 