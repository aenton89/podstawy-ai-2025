#pragma once
#include "../weapon.h"
#include "../../helpers/parameters.h"



class RocketLauncher : public Weapon {
public:
	RocketLauncher();
	void shoot() override;
	void addAmmo(int amount) override;
};
