CMAKE_MINIMUM_REQUIRED(VERSION 3.22)
project(Apptools VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)


add_subdirectory(src/Persistence)

add_subdirectory(src/CLI)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
