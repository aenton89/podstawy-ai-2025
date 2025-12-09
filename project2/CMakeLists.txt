project(project2)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML COMPONENTS graphics window system audio REQUIRED)

add_executable(project2
        src/main.cpp
        src/game/game.cpp
        src/bots/bot.cpp
        src/weapons/weapon.cpp
        src/weapons/rail_gun/rail_gun.cpp
        src/weapons/rocket_launcher/rocket_launcher.cpp
)

target_link_libraries(project2 sfml-graphics sfml-window sfml-system)
target_include_directories(project2 PRIVATE ${SFML_INCLUDE_DIR})
target_compile_definitions(project2 PRIVATE SFML_STATIC)
