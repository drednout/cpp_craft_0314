cmake_minimum_required(VERSION 2.8)
#if you have custom boost installed, please uncomment
#this line with proper boost path
#export BOOST_ROOT=~/usr/boost_1_47_0

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set( CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "Configurations" FORCE )
project( ${SOLUTION_NAME} )
include( environment.cmake required )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
binaries( 2_4 2_5 2_6 )

compile()
