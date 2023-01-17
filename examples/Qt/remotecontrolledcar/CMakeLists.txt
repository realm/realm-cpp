cmake_minimum_required(VERSION 3.16)
project(remotecontrolledcar LANGUAGES CXX)

set(CPPREALM_NO_TESTS 1)
add_subdirectory(../../../ cpprealm-build)

add_subdirectory(car)
add_subdirectory(controller)
