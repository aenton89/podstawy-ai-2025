project(project2)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML COMPONENTS graphics window system audio REQUIRED)

add_executable(project2
        src/main.cpp
)

target_link_libraries(project2 sfml-graphics sfml-window sfml-system)
target_include_directories(project2 PRIVATE ${SFML_INCLUDE_DIR})
target_compile_definitions(project2 PRIVATE SFML_STATIC)
