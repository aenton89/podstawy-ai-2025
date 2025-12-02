# Projekty na przedmiot Podstawy AI (2025)

## 1. chyba niepotrzebna notka
korzysta z czcionki Public Sans (SIL Open Font License)

## 2. PROJEKT 1 (strona 85. - chapter 3); TERMIN (chyba) 06.12
### 2.1. ogólna zasada działania:
#### 1) steering behaviours obliczane w trzech stanach:
```c++
if (parent->getState() == State::HideExplore)
    steeringForce = hideExplore();
else if (parent->getState() == State::Attack)
    steeringForce = attack();
else
    steeringForce = randomWander();
```

hide/explore:
```c++
forceSum += wallAvoidance() * MULT_WALL_AVOIDANCE;
forceSum += obstacleAvoidance(parent->player->game->obstacles, parent->player->game->enemies) * MULT_OBSTACLE_AVOIDANCE;
forceSum += wander() * MULT_WANDER;
forceSum += hide(parent->player->game->obstacles) * MULT_HIDE;
forceSum += flee(parent->player->getPosition()) * MULT_FLEE;
forceSum += separation(parent->player->game->enemies) * MULT_SEPARATION;
forceSum += alignment(parent->player->game->enemies) * MULT_ALIGNMENT;
forceSum += cohesion(parent->player->game->enemies) * MULT_COHESION;
```

attack:
```c++
forceSum += wallAvoidance() * MULT_WALL_AVOIDANCE;
forceSum += obstacleAvoidance(parent->player->game->obstacles, parent->player->game->enemies) * MULT_OBSTACLE_AVOIDANCE;
forceSum += seek(parent->player->getPosition()) * MULT_SEEK;
```

random wander:
```c++
forceSum += wallAvoidance() * MULT_WALL_AVOIDANCE;
forceSum += obstacleAvoidance(parent->player->game->obstacles, parent->player->game->enemies) * MULT_OBSTACLE_AVOIDANCE;
forceSum += wander() * MULT_WANDER;
forceSum += flee(parent->player->getPosition()) * MULT_FLEE;
```
#### 2) logika przejść między stanami:
```c++
if (enemy->getState() == State::Attack)
    continue;

// lista wrogów w stanie HideExplore
auto nearbyList = enemy->getNeighbours(enemies, GROUP_DISTANCE);
if (static_cast<int>(nearbyList.size()) >= GROUP_THRESHOLD) {
    for (Enemy* e : nearbyList) {
        if (e->getState() != State::Attack)
            e->setState(State::Attack);
    }

    enemy->setState(State::Attack);
}
```
dodatkowo jeśli wróg się nie rusza przez dłuższy czas to przechodzi w stan RandomWander na określony czas

### 2.2 DONE:
- podstawowa mapa: przeszkody to pare rozmieszczonych okręgów
- implementacja kolizji: gracz/przeciwnicy/przeszkody/krawędź ściany
- collider gracza to okrąg (mimo, że jest reprezentowany przez trójkąt)
- exit pod ESC; sterowanie pod WSAD + myszke (LPM do strzelania); debug pod LeftALT
- strzelanie raycast'em
- przeciwników jest dużo: prawie losowo rozmieszczani (żeby nie było że na przeszkodzie/graczu)
- raycast zabija przeciwników
- przeciwnicy atakują melee
- sterowani przez steering behaviours

## 3. run:
### 3.0 prerequisites
- install cmake
- install conan
- install Visual C++ 17 (2022)
> [!IMPORTANT]
> or any other C++ compiler supported by conan and cmake - change generator in cmake command below accordingly

### 3.1 detect conan profile
```
conan profile detect --force
```

### 3.2 install dependencies (SFML, etc.)
```
conan install . --output-folder=build --build=missing
```
```
cd build
```
```
.\conanbuild.bat
```
> [!WARNING]
> till this day don't know why and what this does

### 3.3 cmake build
```
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
```
```
cmake --build . --config Release
```

### 3.4 run
> [!IMPORTANT]
> depending on which project you want to run
```
.\project1\Release\project1.exe
```
or
```
.\project2\Release\project2.exe
```
