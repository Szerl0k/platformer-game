#pragma once

#include <memory>

#include "SFML/Graphics.hpp"
#include "MapManager.hpp"

#include "Enemy.hpp"



// Kod inspirowany kodem z: https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/ConvertSketch.cpp
void convertSketch (int currentLevel,std::vector<std::shared_ptr<Enemy>>& enemies , MapManager& mapManager);
