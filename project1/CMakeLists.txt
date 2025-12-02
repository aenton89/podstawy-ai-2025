project(project1)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML COMPONENTS graphics window system audio REQUIRED)

add_executable(project1
        src/main.cpp
        src/components/collision/collider.cpp
        src/components/steering_behaviours/steering_behaviours.cpp
        src/game/game.cpp
        src/game_objects/game_object.cpp
        src/game_objects/obstacle/obstacle.cpp
        src/game_objects/player/player.cpp
        src/game_objects/player/raycast.cpp
        src/game_objects/enemy/enemy.cpp
)

target_link_libraries(project1 sfml-graphics sfml-window sfml-system sfml-audio)
target_include_directories(project1 PRIVATE ${SFML_INCLUDE_DIR})
target_compile_definitions(project1 PRIVATE SFML_STATIC)
