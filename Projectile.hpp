#pragma once

#include <memory>
#include <iostream>

#include "Enemy.hpp"
#include "Satyr.hpp"

#include <fmt/core.h>

#include "Global.hpp"

#include "MapManager.hpp"
#include "SFML/Graphics.hpp"


/*
 *      OGÓLNY PRZYPIS DLA Projectile.hpp
 *
 * Kod zawarty w tym pliku zawiera inspiracje w metodach update() z:
 * https://github.com/Kofybrek/Super-Mario-Bros/tree/Main/Source
 *
 * Chodzi tu głównie o logikę kolizji wykorzystywaną przez metody update()
 *
 */

class Projectile : public std::enable_shared_from_this<Projectile>{

protected:

    short dmg;

    bool friendly;

    bool shouldBeDeleted;

    float xVelocity;

    float yVelocity;

    int direction;  // -1 "left"; 0 no direction; 1 "right"


    sf::Vector2f position;


public:

    template<typename EnemyType>
    Projectile(const short dmg,EnemyType& t) :
            dmg(dmg),
            friendly(t),
            shouldBeDeleted(false),
            xVelocity(0),
            yVelocity(0),
            direction(0),

            // Chcemy aby projectile pojawiały się na środku postaci

            position(t.getX() + TILE_SIZE / 2, t.getY() + TILE_SIZE / 2)
    {
                if (!friendly)
                    if (t.type() == "BlackKnight") {
                        position = sf::Vector2f(t.getX() + BLACK_KNIGHT_X_SIZE / 1.5, t.getY() + BLACK_KNIGHT_Y_SIZE / 1.5);
                    } else {
                        position = sf::Vector2f(t.getX() + SKELETON_X_SIZE / 1.5, t.getY() + SKELETON_Y_SIZE / 1.5);
                    }

    }

    short getDmg() const;

    void setX(const float& x);

    void setY(const float& y);

    float getX() const;

    float getY() const;

    bool getDelete() const;

    virtual void flip(const int& i_direction) = 0;
    // Pure virtual bo każdy projectile ma inny rozmiar, czyli inny hitbox
    virtual sf::FloatRect getHitBox() const = 0;

    virtual void draw(const unsigned view_x, sf::RenderWindow& window) = 0;

    // Wyjaśnienia w Character.hpp
    virtual void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Projectile>>& projectiles,
            std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr,
            Character& characterType
    ) = 0;

};




//      ARROW


class Arrow : public Projectile {

    sf::Sprite sprite;

    sf::Texture texture;

    int rangeCount;


public:
    template<typename EnemyType>
    Arrow(const short dmg, EnemyType& t) :
        Projectile(dmg, t),
        rangeCount(0)
    {

        if ( friendly ) {

            texture.loadFromFile(R"(..\Resources\FriendlyArrow.png)");

        } else {

            texture.loadFromFile(R"(..\Resources\UnfriendlyArrow.png)");

        }

        sprite.setOrigin(ARROW_X_SIZE / 2, ARROW_Y_SIZE / 2);


    }

    ~Arrow();

    void flip(const int& i_direction);

    sf::FloatRect getHitBox() const;

    void draw(const unsigned view_x, sf::RenderWindow& window);

    // Wygląda to dziwnie, więc wyjaśnię:
    // characterType to jest objekt, pod który przekazujemy dowolnego Enemy albo Satyra
    // W takim razie po co nam Satyr? Żeby gdy bedziemy strzelać jako Enemy do
    // Satyra to potrzebujemy jego koordynatów. Vice versa jest nam potrzeby
    // Wektor enemies
    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Projectile>>& projectiles,
            std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr,
            Character& characterType
    );




};



// ROCK

class Rock : public Projectile {

    sf::Sprite sprite;

    sf::Texture texture;

    int rangeCount;

    int bounces;


public:
    template<typename EnemyType>
    Rock(const short dmg, EnemyType& t) :
        Projectile(dmg, t),
        rangeCount(0),
        bounces(0)
    {

            if ( friendly ) {

                texture.loadFromFile(R"(..\Resources\FriendlyRock.png)");

            } else {

                texture.loadFromFile(R"(..\Resources\UnfriendlyRock.png)");

            }

            sprite.setOrigin( ROCK_X_SIZE / 2, ROCK_Y_SIZE / 2 );
        setY(getY() - 20);

    }

    ~Rock();

    void flip(const int& i_direction);

    sf::FloatRect getHitBox() const;

    void draw(const unsigned view_x, sf::RenderWindow& window);

    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Projectile>>& projectiles,
            std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr,
            Character& characterType
    );

};


class StaffProj : public Projectile {


    sf::Sprite sprite;

    sf::Texture texture;


public:
    template<typename EnemyType>
    StaffProj(const short dmg, EnemyType& t) :
            Projectile(dmg, t)
    {
       if (friendly) {

           texture.loadFromFile(R"(..\Resources\StaffProjFriendly.png)");

        } else {

           texture.loadFromFile(R"(..\Resources\StaffProjUnfriendly.png)");

        }

        sprite.setOrigin(STAFF_PROJ_X_SIZE / 2, STAFF_PROJ_Y_SIZE / 2);

        // Nadpisujemy pozycję ustawioną przez kostruktor Projectile

        position = sf::Vector2f(t.getX() + TILE_SIZE * 5, 0);

        if (t.getXVelocity() < 0) {
            position = sf::Vector2f(t.getX() - TILE_SIZE * 5, 0);
        } else {
            position = sf::Vector2f(t.getX() + TILE_SIZE * 5, 0);
        }

    }

    ~StaffProj();

    void flip(const int& i_direction);

    sf::FloatRect getHitBox() const;

    void draw(const unsigned view_x, sf::RenderWindow& window);

    // Wygląda to dziwnie, więc wyjaśnię:
    // characterType to jest objekt, pod który przekazujemy dowolnego Enemy albo Satyra
    // W takim razie po co nam Satyr? Żeby gdy bedziemy strzelać jako Enemy do
    // Satyra to potrzebujemy jego koordynatów. Vice versa jest nam potrzeby
    // Wektor enemies
    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Projectile>>& projectiles,
            std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr,
            Character& characterType
    );


};

