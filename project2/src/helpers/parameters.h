#pragma once



namespace Parameters {
	constexpr float MAP_WIDTH = 1600.f;
	constexpr float MAP_HEIGHT = 900.f;

	constexpr float BOT_RADIUS = 15.f;
	constexpr float BOT_MAX_HEALTH = 100.f;
	// pikseli na sekundę
	constexpr float BOT_SPEED = 100.f;

	constexpr float RAILGUN_DAMAGE = 40.f;
	// w sekundach
	constexpr float RAILGUN_RELOAD_TIME = 2.5f;
	constexpr int RAILGUN_MAX_AMMO = 10;

	constexpr float ROCKET_DAMAGE = 30.f;
	// pikseli na sekundę
	constexpr float ROCKET_SPEED = 200.f;
	constexpr float ROCKET_EXPLOSION_RADIUS = 50.f;
	constexpr float ROCKET_RELOAD_TIME = 1.5f;
	constexpr int ROCKET_MAX_AMMO = 15;

	constexpr int AMMO_PACK_RAILGUN_AMOUNT = 3;
	constexpr int AMMO_PACK_ROCKET_AMOUNT = 5;
	constexpr float HEALTH_PACK_AMOUNT = 30.f;
}