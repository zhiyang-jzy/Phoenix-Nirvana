cmake_minimum_required(VERSION 3.24)
project(phoenix_nirvana)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
file(GLOB phoenix_src src/phoenix/*.cpp src/phoenix/utils/*.cpp src/phoenix/core/*.cpp src/phoenix/cameras/*.cpp
        src/phoenix/samplers/*.cpp src/phoenix/integrators/*.cpp src/phoenix/shapes/*.cpp src/phoenix/emitters/*.cpp src/phoenix/bsdfs/*.cpp)


find_package(ZLIB)
find_package(Eigen3)
find_package(spdlog)
find_package(Embree)
find_package(magic_enum CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED tbb)
find_package(assimp CONFIG REQUIRED)

add_executable(phoenix_nirvana ${phoenix_src})
target_link_libraries(phoenix_nirvana ZLIB::ZLIB Eigen3::Eigen spdlog::spdlog embree magic_enum::magic_enum TBB::tbb assimp::assimp)