#pragma once
#include "../game_object.h"



class Obstacle : public GameObject {
public:
	explicit Obstacle(float _x = 0, float _y = 0, float _radius = 20.f);
};
