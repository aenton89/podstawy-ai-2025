# SFML 2.6.2 setup (on Windows)

## 1. required:
- CMake
- Conan
- Visual C++ 17 (2022)

## 2. PROJEKT 1 (strona 85. - chapter 3); TERMIN (chyba) 06.12
### 2.1 chyba niepotrzebna notka
korzysta z czcionki Public Sans (SIL Open Font License)

### 2.2. ogólna zasada działania:
#### 1) steering behaviours obliczane w dwóch stanach:
```c++
if (parent->getState() == State::Hide_Explore)
    steeringForce = hide_explore();
else
    steeringForce = attack();
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

#### 2) logika przejść między stanami:
```c++
if (enemy->getState() == State::Attack)
    continue;

// lista wrogów w stanie Hide_Explore
auto nearbyHEList = enemy->checkNeighborExploring(enemies, 100.f);
if (static_cast<int>(nearbyHEList.size()) >= ATTACK_THRESHOLD) {
    for (Enemy* e : nearbyHEList)
        e->setState(State::Attack);    

    enemy->setState(State::Attack);
}
```

### 2.3 pytania do prowadzącego/notatki do przemyślenia:
- separacja * 0.3 (ogólnie mniejsza?
- cooldown do tworzenai grup ataku?
- w updateAgentState() - co z powrotem do stanu Hide_Explore? czy trzeba to zrobić? czy jak już atakują to na raz (point of no return)?
- w updateAgentState() może być reakcja łancuchowa? czy ona ma być? opcja poprawy niżej:
```c++
void Game::updateAgentsState() const {
    for (auto& enemy : enemies) {
        if (enemy->getState() == State::Attack)
            continue;
        
        auto nearbyHEList = enemy->checkNeighborExploring(enemies, 100.f);
        
        // czy w pobliżu NIE MA już atakujących botów
        bool nearAttackers = false;
        for (const auto& other : enemies) {
            if (other->getState() == State::Attack) {
                float dist = distanceVec2D(enemy->getPosition(), other->getPosition());
                // strefa buforowa
                if (dist < 200.f) {
                    nearAttackers = true;
                    break;
                }
            }
        }
        
        if (!nearAttackers && static_cast<int>(nearbyHEList.size()) >= ATTACK_THRESHOLD) {
            for (Enemy* e : nearbyHEList)
                e->setState(State::Attack);
            enemy->setState(State::Attack);
        }
    }
}
```

### 2.4 DONE:
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
```
.\Release\SFML-Base.exe
```