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
	constexpr float BOT_SPEED = 100.f;	// pikseli na sekundę
	constexpr float SHOT_INACCURACY = 0.15f;

		// Railgun
	constexpr float RAILGUN_DAMAGE = 70.f;
	constexpr float RAILGUN_RELOAD_TIME = 2.5f;	// w sekundach
	constexpr float RAILGUN_RANGE = 2000.f;
	constexpr int RAILGUN_MAX_AMMO = 10;
	constexpr int RAILGUN_RESPAWN_AMMO = 6;

		// Rocket
	constexpr float ROCKET_DAMAGE = 40.f;
	constexpr float ROCKET_SPEED = 400.f;	// pikseli na sekundę
	constexpr float ROCKET_EXPLOSION_RADIUS = 50.f;
	constexpr float ROCKET_RELOAD_TIME = 1.0f;	// w sekundach
	constexpr float ROCKET_RANGE = 500.f;
	constexpr int ROCKET_MAX_AMMO = 15;
	constexpr int ROCKET_RESPAWN_AMMO = 10;

		// Pickupy
	constexpr int AMMO_PACK_RAILGUN_AMOUNT = 4;
	constexpr int AMMO_PACK_ROCKET_AMOUNT = 5;
	constexpr float HEALTH_PACK_AMOUNT = 50.f;

		// FSM
	constexpr float VISION_RANGE = 400.f;
	constexpr float PICKUP_DETECTION_RANGE = 250.f;
	constexpr float CLOSE_RANGE = 200.f;
	constexpr float MIN_AMMO_THRESHOLD = 3;
	constexpr float MIN_HP_THRESHOLD = 40.f;

	constexpr float ROAM_CHANGE_INTERVAL = 3.f;
	constexpr float SEARCH_INTERVAL = 2.f;
	constexpr float PURSUIT_TIMEOUT = 5.f;
	constexpr float TARGET_LOSS_TIMEOUT = 3.f;
	constexpr float FLEE_DURATION = 5.f;
}