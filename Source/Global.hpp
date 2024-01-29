#pragma once
#include <vector>
#include <array>
#include "fmt/core.h"


// GENERAL
const auto SCREEN_HEIGHT = 768;
const auto SCREEN_WIDTH = 1024;
const unsigned int TILE_SIZE = 32;
const auto ENEMY_UPDATE_AREA = 256;
const auto BOSS_UPDATE_AREA = 512;
const auto SATYR_X_SIZE = 16;

const auto SATYR_SPAWN_X = 1100;
const auto SATYR_SPAWN_Y = 450;


// Movement
const auto SATYR_ACCELERATION = 0.25f;
const auto SATYR_WALK_VELOCITY = 3.f;
const auto SATYR_JUMP_VELOCITY = -6.f;
const auto GRAVITY = 0.25f;
const auto MAX_VERTICAL_SPEED = 10.f;


// PROJECTILES
const auto ARROW_DMG = 1;
const auto ARROW_SPEED = 5;     // Zmienić nazwę na ARROW_X_VELOCITY
const auto ARROW_X_SIZE = 14;   //zmienic na 14
const auto ARROW_Y_SIZE = 5;
const auto ARROW_RANGE = 100;

const auto ROCK_DMG = 1;
const auto ROCK_TROLL_DMG = 1;
const auto ROCK_SPEED = 5;      // Zmienić nazwę na ROCK_X_VELOCITY
const auto ROCK_Y_VELOCITY = 5;
const auto ROCK_X_SIZE = 14;
const auto ROCK_Y_SIZE = 14;
const auto ROCK_RANGE = 1000;
const auto ROCK_MAX_BOUNCES = 12;
const auto ROCK_MAX_Y = 50;

const auto STAFF_DMG = 2;
const auto STAFF_MOB_DMG = 2;
const auto STAFF_PROJ_X_SIZE = 14;
const auto STAFF_PROJ_Y_SIZE = 32;
const auto STAFF_PROJ_Y_VELOCITY = GRAVITY * 30;


// CAPYBARA
const auto CAPY_SPEED = 4;
const short CAPY_HEALTH = 3;
const auto CAPY_X_SIZE = 24;
const auto CAPY_Y_SIZE = 20;


// SKELETON
const auto SKELETON_SPEED = 1;
const short SKELETON_HEALTH = 4;
const auto SKELETON_X_SIZE = 37;
const auto SKELETON_Y_SIZE = 64;
const auto SKELETON_X_RANGE = 400;
const auto SKELETON_Y_RANGE = 400;
const auto SKELETON_SHOOTING_TIMER_MAX = 90;
const auto SKELETON_MAX_MOVEMENT_TIMER = 600;

// TROLL
const auto TROLL_SPEED = 1;
const short TROLL_HEALTH = 10;
const auto TROLL_X_SIZE = 28;
const auto TROLL_Y_SIZE = 64;
const auto TROLL_X_RANGE = 600;
const auto TROLL_Y_RANGE = 600;
const auto TROLL_SHOOTING_TIMER_MAX = 40;
const auto TROLL_MAX_MOVEMENT_TIMER = 600;

// GREY KNIGHT BOSS 1
const auto GREY_KNIGHT_SPEED = 10;
const auto GREY_KNIGHT_HEALTH = 50;
const auto GREY_KNIGHT_X_SIZE = 26;
const auto GREY_KNIGHT_Y_SIZE = 30;
const auto GREY_KNIGHT_MAX_MOVEMENT_TIMER = 500;
const auto GREY_KNIGHT_DEFAULT_MOVEMENT_TIMER = 300;

// BLACK KNIGHT BOSS 2
const auto BLACK_KNIGHT_SPEED = 8;
const auto BLACK_KNIGHT_HEALTH = 50;
const auto BLACK_KNIGHT_X_SIZE = 46;
const auto BLACK_KNIGHT_Y_SIZE = 28;
const auto BLACK_KNIGHT_MAX_MOVEMENT_TIMER = 500;
const auto BLACK_KNIGHT_DETAULT_MOVEMENT_TIMER = 300;
const auto BLACK_KNIGHT_SHOOTING_TIMER_MAX = 100;
const auto BLACK_KNIGHT_X_RANGE = 600;
const auto BLACK_KNIGHT_Y_RANGE = 600;

// WIZARD BOSS 3
const auto WIZARD_HEALTH = 150;
const auto WIZARD_X_SIZE = 27;
const auto WIZARD_Y_SIZE = 63;
const auto WIZARD_X_VELOCITY = 6;
const auto WIZARD_DEFAULT_MOVEMENT_TIMER = 200;
const auto WIZARD_MAX_MOVEMENT_TIMER = 200;
const auto WIZARD_SHOOTING_TIMER_MAX = 50;
const auto WIZARD_X_RANGE = 600;
const auto WIZARD_Y_RANGE = 600;


enum class TerrainTile {
    GROUND_TOP_LEFT,            // sf::Color(25,25,25)
    GROUND_TOP,                 // sf::Color(50,50,50)
    GROUND_TOP_RIGHT,           // sf::Color(75,75,75)
    GROUND_RIGHT,               // sf::Color(100,100,100)
    GROUND_BOTTOM_RIGHT,        // sf::Color(125,125,125)
    GROUND_BOTTOM,              // sf::Color(150,150,150)
    GROUND_BOTTOM_LEFT,         // sf::Color(175,175,175)
    GROUND_LEFT,                // sf::Color(200,200,200)
    GROUND_MIDDLE,              // sf::Color(0,0,0)
    GROUND_PILLAR_TOP,          // sf::Color(0,0,200)
    GROUND_PILLAR_MIDDLE,       // sf::Color(0,0,150)
    GROUND_PILLAR_BOTTOM,       // sf::Color(0,0,100)
    GROUND_DEEP_1,              // sf::Color(50,50,0)
    GROUND_DEEP_2,              // sf::Color(100,100,0)
    GROUND_DEEP_3,              // sf::Color(200,200,0)
    IRREGULAR_GROUND,           // sf::Color(150,150,0)
    GRASS_DEFAULT,              // sf::Color(0,250,0)
    GRASS_DENSER,               // sf::Color(0,200,0)
    GRASS_ABOVE,                // sf::Color(0,150,0)
    EMPTY,                      // sf::Color()

    CHECKPOINT,                 // sf::Color(0,50,0)
    FINISH,                     // sf::Color(0,100,0)

    COIN,                       // sf::Color(255, 209, 69)
    HEART,                      // sf::Color(239, 58, 12)
    BOW,                        // sf::Color(26,4,4)
    ROCK,                       // sf::Color(28,28,28)
    STAFF,                      // sf::Color(27,69,108)
    COLLECTED,

    // To troche dziwine aby przeciwnicy byli typu TerrainType, ale dopisałem to dopiero po
    // Skończeniu logiki mapy, i troszke pracy by zajęło zmienianie wszystkiego w innych plikach
    CAPYBARA,                   // sf::Color(250,0,0)
    SKELETON,                   // sf::Color(200,0,0)
    IMMOBILE_SKELETON,          // sf::Color(199,0,0)
    GREY_KNIGHT,                // sf::Color(150,0,0)
    TROLL,                      // sf::Color(100,0,0)
    IMMOBILE_TROLL,             // sf::Color(99,0,0)
    BLACK_KNIGHT,               // sf::Color(50,0,0)
    WIZARD,                     // sf::Color(233,181,163)
};




typedef std::vector<std::array<TerrainTile, SCREEN_HEIGHT / TILE_SIZE>> Map;

// Musi być const, inaczej będzie multiple definition of `allCollisionTiles'
const auto allCollisionTiles = std::vector<TerrainTile>{
    TerrainTile::GROUND_TOP_LEFT, TerrainTile::GROUND_TOP, TerrainTile::GROUND_TOP_RIGHT,
    TerrainTile::GROUND_RIGHT, TerrainTile::GROUND_BOTTOM_RIGHT, TerrainTile::GROUND_BOTTOM,
    TerrainTile::GROUND_BOTTOM_LEFT, TerrainTile::GROUND_LEFT, TerrainTile::GROUND_MIDDLE,
    TerrainTile::GROUND_PILLAR_TOP, TerrainTile::GROUND_PILLAR_MIDDLE, TerrainTile::GROUND_PILLAR_BOTTOM,
    TerrainTile::GROUND_DEEP_1, TerrainTile::GROUND_DEEP_2, TerrainTile::GROUND_DEEP_3,
    TerrainTile::IRREGULAR_GROUND
};


