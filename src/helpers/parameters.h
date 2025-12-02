#pragma once

#define PLAYER_SIZE 15.f
#define PLAYER_SPEED 200.f

#define ENEMY_SIZE 15.f
// na długość wektora prędkości
#define MAX_ENEMY_SPEED 50.f

#define MAX_ENEMIES_AMOUNT 40
#define MIN_ENEMY_EDGE_DIST 20.f
#define MAX_ENEMY_EDGE_DIST 100.f

#define MIN_OBSTACLE_SIZE 20.f
#define MAX_OBSTACLE_SIZE 40.f
#define MAX_OBSTACLES_AMOUNT 12

// pod obstacle avoidance
#define MIN_DETECTION_BOX_LENGTH 100.f
#define MAX_AVOID_FORCE 100.f

// pod wander
#define WANDER_RADIUS 20.f
#define WANDER_DISTANCE 50.f
#define WANDER_JITTER 0.01f

// pod flee
#define PANIC_DISTANCE_SQ (200.f * 200.f)

// pod arrive
#define DECELERATION_TWEAK 0.3f

// pod zachowania grupowe
#define NEIGHBOR_RADIUS 100.f

// wagi dla steering behaviours
#define MULT_WALL_AVOIDANCE 1.0f
#define MULT_OBSTACLE_AVOIDANCE 2.0f
#define MULT_WANDER 1.0f
#define MULT_HIDE 1.0f
#define MULT_FLEE 1.0f
#define MULT_SEPARATION 50.0f
#define MULT_ALIGNMENT 5.0f
#define MULT_COHESION 0.2f
#define MULT_SEEK 1.0f

// ilosc wrogów by przejść w tryb ATTACK
#define ATTACK_THRESHOLD 4

// pod metodę "Weighted Truncated Running Sum with Prioritization" - nieużyte na razie
#define MAX_FORCE 80.f