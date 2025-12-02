#include "collider.h"
#include "../../helpers/helper_methods.h"



Collider::Collider(float _x, float _y, float _r): position(_x, _y), radius(_r) {}

bool Collider::checkCollision(const Collider& other) const {
	return distanceVec2D(position, other.position) < (radius + other.radius);
}
