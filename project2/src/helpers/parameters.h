#pragma once



namespace Parameters {
		// Rozgrywka
	constexpr int MAX_BOTS = 4;

		// Mapa
	constexpr float MAP_WIDTH = 1600.f;
	constexpr float MAP_HEIGHT = 900.f;

		// Boty
	constexpr float BOT_RADIUS = 15.f;
	constexpr float BOT_MAX_HEALTH = 100.f;
	// pikseli na sekundę
	constexpr float BOT_SPEED = 100.f;

		// Railgun
	constexpr float RAILGUN_DAMAGE = 100.f;
	// w sekundach
	constexpr float RAILGUN_RELOAD_TIME = 2.5f;
	constexpr int RAILGUN_MAX_AMMO = 10;
	constexpr float RAILGUN_RANGE = 2000.f;

		// Rocket
	constexpr float ROCKET_DAMAGE = 60.f;
	// pikseli na sekundę
	constexpr float ROCKET_SPEED = 400.f;
	constexpr float ROCKET_EXPLOSION_RADIUS = 50.f;
	constexpr float ROCKET_RELOAD_TIME = 1.0f;
	constexpr int ROCKET_MAX_AMMO = 15;
	constexpr float ROCKET_RANGE = 500.f;

		// Pickupy
	constexpr int AMMO_PACK_RAILGUN_AMOUNT = 3;
	constexpr int AMMO_PACK_ROCKET_AMOUNT = 5;
	constexpr float HEALTH_PACK_AMOUNT = 30.f;
}