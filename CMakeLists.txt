project(pyglasstools)
cmake_minimum_required(VERSION 2.8.12)
set( CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_NO_BUILTIN_CHRPATH TRUE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include(SetupPybind11.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pyglasstools/extern/pybind11/include)
include(CompilerFlagsSetup.cmake)


# plugins must be built as shared libraries
if (ENABLE_STATIC)
    message(SEND_ERROR "Needs to be built as shared library!")
endif (ENABLE_STATIC)

set(BUILD_SHARED_LIBS on)

#Set the install directory to be the site-packages folder
#execute_process(
#  COMMAND "${PYTHON_EXECUTABLE}" -c "if True:
#    import site 
#    print(site.getsitepackages()[0])"
#  OUTPUT_VARIABLE PYTHON_MODULE_BASE_DIR
#  OUTPUT_STRIP_TRAILING_WHITESPACE)
#set(CMAKE_INSTALL_PREFIX ${PYTHON_MODULE_BASE_DIR})

# include current source directory as its own include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# add subdirectories to process CMake files inside
add_subdirectory(${PROJECT_NAME})

#copy setup.py do I need it?
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py ${CMAKE_CURRENT_BINARY_DIR}/setup.py COPYONLY)
