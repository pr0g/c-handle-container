cmake_minimum_required(VERSION 3.13)
project(container LANGUAGES C)
add_executable(${PROJECT_NAME} main.c container.c)

find_package(unity)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE unity
)
