cmake_minimum_required(VERSION 3.18)
project(Project)
set(OPENVDB_LIBRARY_SOURCE_FILES
  main.cpp
)
set(OPENVDB_GRAVITY_SOURCE_FILES
  gravity.cpp
)
#add_library(myapp STATIC ${OPENVDB_LIBRARY_SOURCE_FILES})
add_executable(myapp ${OPENVDB_LIBRARY_SOURCE_FILES})
add_executable(gravity ${OPENVDB_GRAVITY_SOURCE_FILES})
set(TBB_INCLUDEDIR "C:/vcpkg/installed/x64-windows/include")
set(Boost_INCLUDE_DIR "C:/vcpkg/installed/x64-windows/include")
set(Blosc_INCLUDE_DIR "C:/vcpkg/installed/x64-windows/include")
set(ZLIB_INCLUDE_DIR "C:/vcpkg/installed/x64-windows/include")
set(TBB_LIBRARYDIR "C:/vcpkg/installed/x64-windows/lib")
set(Blosc_LIBRARY "C:/vcpkg/installed/x64-windows/lib")
set(ZLIB_LIBRARY "C:/vcpkg/installed/x64-windows/lib")
list(APPEND CMAKE_MODULE_PATH "C:/Program Files/OpenVDB/lib/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)
target_link_libraries(myapp OpenVDB::openvdb)
target_link_libraries(gravity OpenVDB::openvdb)