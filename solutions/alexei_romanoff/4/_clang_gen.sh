#!/bin/bash
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
export SOLUTION_NAME=puzzle_4_solution
#export BOOST_ROOT=~/usr/boost_1_47_0
#export BINARY_READER_ROOT="/home/dr/devel/cppcraft/cpp_craft_0314/solutions/ihar_marozau/2/"

export BUILD_TYPE=Debug
echo $1
if [ "$1" == 'Release' ];
then
	export BUILD_TYPE=Release
fi

export BUILD_FOLDER=_build_"$BUILD_TYPE"_64

if [ ! -d $BUILD_FOLDER ];
then
	mkdir $BUILD_FOLDER
fi

cd $BUILD_FOLDER
cmake -DVERBOSE=ON -DBOOST_STAGE_FOLDER_WITH_ADDRESS_MODEL=ON -DSOLUTION_NAME=$SOLUTION_NAME -DCMAKE_BUILD_TYPE=$BUILD_TYPE -G "Unix Makefiles" ../ 
cd ../
