# Additional CMake configuration file for building the examples of the 
# SuperNOVAS library. The examples are also added to the test suite so they 
# may be checked for functionality.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_EXAMPLES=ON option.
#
# Author: Attila Kovacs

# ----------------------------------------------------------------------------
# Generate example programs

if(ENABLE_CPP)
    add_subdirectory(cpp)
else()
    add_subdirectory(c99)
endif()
    

   
