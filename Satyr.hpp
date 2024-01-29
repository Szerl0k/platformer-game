#pragma once

#include <memory>
#include <vector>

#include "Character.hpp"

// czyt. w Enemy.cpp
class Enemy;
#include "Enemy.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Projectile.hpp"


class Satyr : public Character {

    bool nextLevel;

    bool isDead;

    bool isMoving;

    bool onGround;

    bool isCrouching;

    float yVelocity;

    int satyrCoins;

    int satyrHearts;

    sf::Sprite sprite;

    sf::Vector2f position;

    sf::Texture texture;

    sf::Vector2i lastCheckpoint;


public:

    Satyr(const int canUse);

    std::string type() const;

    int experience;

    bool canUseBow;

    bool canUseRock;

    bool canUseStaff;

    bool resetLevel;

    operator bool() const;

    bool getNextLevel() const;

    void setCoins(const int value);

    int getCoins() const;

    int getHearts() const;

    // zamiast pól x i y
    void setX(const float& x);

    void setY(const float& y);

    float getX() const;

    float getY() const;

    void loseHeart();

    void die();

    void reset();


    sf::FloatRect getHitBox() const;

    void draw(sf::RenderWindow& window);

    /*
     * Kod inspirowany: https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/Mario.hpp
     * Głownie kod, który wykorzystuje logikę kolizji wspomnianą wcześniejw MapManager.hpp/.cpp
     */
    void update(MapManager& mapManager, const std::vector<std::shared_ptr<Enemy>>& enemies);


};
