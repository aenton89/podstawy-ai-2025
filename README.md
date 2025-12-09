# Projekty na przedmiot Podstawy AI (2025)

## 1. chyba niepotrzebna notka
korzysta z czcionki Public Sans (SIL Open Font License)

## 2. PROJEKT 1 
- steering behaviours - chapter 3 (str. 85) 
- TERMIN - 06.12

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

## 3. PROJEKT 2
- FSM - chapter 2 (str. 43)
- more FSM - chapter 6 (str. 249 dokładniej od 285)
- graphs and pathfinding - chapter 8 (str. 333)

### 3.1. TO DO:
- railgun i rocket launcher
- zdrowie i amunicja do podniesienia
- mapa (prostokąt) z przeszkodami (wielokąty), nie da się wyjść za ekran
- 4 boty (bez gracza) reprezentowane przez okręgi
- boty korzystają z FSM
- pathfinding (A*), graf generowany przez flood fill
- poruszanie się w 8 kierunkach (kratownica z punktami co rozmiar bota)

### 3.2 DONE:
- narazie nic

## 4. run:
### 4.0 prerequisites
- install cmake
- install conan
- install Visual C++ 17 (2022)
> [!IMPORTANT]
> or any other C++ compiler supported by conan and cmake - change generator in cmake command below accordingly

### 4.1 detect conan profile
```
conan profile detect --force
```

### 4.2 install dependencies (SFML, etc.)
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

### 4.3 cmake build
```
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
```
```
cmake --build . --config Release
```

### 4.4 run
> [!IMPORTANT]
> depending on which project you want to run
```
.\project1\Release\project1.exe
```
or
```
.\project2\Release\project2.exe
```
