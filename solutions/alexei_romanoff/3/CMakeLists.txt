cmake_minimum_required(VERSION 2.8)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set( CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "Configurations" FORCE )
project( ${SOLUTION_NAME} )
include( environment.cmake required )

binaries( 3_4 3_5 3_6 )

#set(binary_reader_DEBUG 1)
find_package( binary_reader ${SEARCH_PARAMETERS} COMPONENTS
        binary_reader
)
compile()

#compile_project( ${module_name} "*.cpp" "*.h" BINARY binaries binary_reader )
