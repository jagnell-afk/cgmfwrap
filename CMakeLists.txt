cmake_minimum_required(VERSION 3.15)
project(cgmfwrap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.12.0
)
FetchContent_MakeAvailable(pybind11)

# CGMF include directories
include_directories(
    /nfs/stak/users/agnellj/project/CGMF/libcgmf/src
    /nfs/stak/users/agnellj/project/CGMF/libcgmf/include
    /nfs/stak/users/agnellj/project/CGMF/data
    
)

add_library(cgmfwrap MODULE
    cgmf_wrapper.cpp
)

target_link_libraries(cgmfwrap PRIVATE
    pybind11::module
    /nfs/stak/users/agnellj/project/CGMF/build/libcgmf/libcgmf.so
)

# 
#cd)
# Ensure Python can find libcgmf.dylib at runtime
#set_target_properties(cgmfwrap PROPERTIES
    #BUILD_RPATH "@loader_path"
    #INSTALL_RPATH "@loader_path"
#)

# Remove "lib" prefix so Python can import it
set_target_properties(cgmfwrap PROPERTIES PREFIX "")

if(APPLE)
    set_target_properties(cgmfwrap PROPERTIES SUFFIX ".so")
endif()
