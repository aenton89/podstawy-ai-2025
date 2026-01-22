#pragma once
#include "../weapon.h"
#include "../../helpers/parameters.h"



class RailGun : public Weapon {
public:
	RailGun();
	void shoot() override;
	void addAmmo(int amount) override;
};
