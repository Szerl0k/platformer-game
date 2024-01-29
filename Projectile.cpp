#include <cmath>

#include "Satyr.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "GlobalMethods.hpp"
#include "MapManager.hpp"
#include "Projectile.hpp"

#include "SFML/Graphics.hpp"


/*
 *      OGÓLNY PRZYPIS DLA Projectile.cpp
 *
 * Kod zawarty w tym pliku zawiera inspiracje w metodach update() z:
 * https://github.com/Kofybrek/Super-Mario-Bros/tree/Main/Source
 *
 * Chodzi tu głównie o logikę kolizji wykorzystywaną przez metody update()
 *
 */


short Projectile::getDmg() const { return dmg;}

void Projectile::setX(const float &x) {position.x = x;}

void Projectile::setY(const float &y) {position.y = y;}

float Projectile::getX() const {return position.x;}

float Projectile::getY() const {return position.y;}

bool Projectile::getDelete() const { return shouldBeDeleted;}


void Arrow::flip(const int& i_direction) {

    if (direction != i_direction)
        sprite.scale(-1, -1);

}

Arrow::~Arrow() {
    fmt::println("Destroyed arrow");
}

void Arrow::draw(const unsigned view_x, sf::RenderWindow& window) {

    // Każdy pocisk ma swój maksymalny range, który reprezentuje ile klatek przeleciał dany pocisk
    // Jeśli pocisk już "przekroczy" swój range, to nie jest on rysowany, I ten obiekt jest usuwany z pamięci.
    // Na szczęście używam smart pointerów, i nie muszę ich ręcznie usuwać :)

    sprite.setPosition(round(getX()), round(getY()));
    sprite.setTexture(texture);

    window.draw(sprite);

}

void Arrow::update(
        const unsigned view_x,
        const std::vector<std::shared_ptr<Projectile>>& projectiles,
        std::vector<std::shared_ptr<Enemy>>& enemies,
        const MapManager& mapManager,
        Satyr& satyr,
        Character& characterType
) {


    auto collision = std::vector<int>();

    auto hitBox = getHitBox();


    collision = mapManager.mapCollision(allCollisionTiles, hitBox);

    if ( !std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

        auto tmpDirection = direction;
        direction *= -1;
        flip(tmpDirection);


    }



    // Wykonujemy poniższy kod w zależności, czy bierzemy pod uwagę pociski satyra czy wrogów.
    // NOT enemies.empty => pociski Satyra
    // enemies.empty (enemies = "dummy vector") => pociski wrogów
    // Idea jest podobna jak tabela DUAL typowej dla baz danych ORACLE.
    if (!enemies.empty()) {

        // Usuwanie siebie i przeciwnika gdy uderzą w przeciwnika
        for(auto& enemy : enemies) {

            if ( getHitBox().intersects(enemy->getHitBox()) ) {

                enemy->recieveDmg(getDmg());
                shouldBeDeleted = true;

            }

        }
    } else {

        if(getHitBox().intersects(satyr.getHitBox())) {

            shouldBeDeleted = true;
            satyr.loseHeart();

        }

    }



    // Usuwamy pociski gdy przekroczą swój range
    if (rangeCount >= ARROW_RANGE) {

        shouldBeDeleted = true;

    } else {
        // Nadajemy im trajektorie
        if (direction == 0) {

            if (characterType.getXVelocity() < 0) {

                auto tmpDirection = direction;
                direction = -1;
                flip(tmpDirection);

            } else {

                direction = 1;

            }

        } else {

            if (direction == -1) {
                setX(getX() - ARROW_SPEED);
            } else {
                setX(getX() + ARROW_SPEED);
            }


        }

        rangeCount++;

    }


}

sf::FloatRect Arrow::getHitBox() const { return {getX(), getY(), ARROW_X_SIZE, ARROW_Y_SIZE}; }


//      ROCK


Rock::~Rock() { fmt::println("Destroyed Rock"); }

void Rock::flip(const int &i_direction) { if (direction != i_direction) sprite.scale(-1,-1); }

void Rock::draw(const unsigned int view_x, sf::RenderWindow &window) {

    sprite.setPosition(round(getX()), round(getY()));
    sprite.setTexture(texture);

    window.draw(sprite);

}

void Rock::update(const unsigned int view_x, const std::vector<std::shared_ptr<Projectile>> &projectiles,
                  std::vector<std::shared_ptr<Enemy>> &enemies, const MapManager &mapManager, Satyr &satyr,
                  Character &characterType) {


    if (!enemies.empty()) {

        for (auto& enemy : enemies) {

            if (getHitBox().intersects(enemy->getHitBox())) {

                enemy->recieveDmg(getDmg());

                shouldBeDeleted = true;

            }

        }

    } else {

        if (getHitBox().intersects(satyr.getHitBox())) {

            shouldBeDeleted = true;
            satyr.loseHeart();

        }

    }

    // Taki sam kod jak w przypadku Arrow, aby zmienić kierunek, gdy napotkamy ścianę
    auto collision = std::vector<int>();

    auto hitBox = getHitBox();

    hitBox.top++;

    collision = mapManager.mapCollision(allCollisionTiles, hitBox);


    // Kamień powinien się odbijać od ziemi (jak żabki na wodzie), a gdy napotka ścianę, to się zatrzymuje
    // Nie potrafiłem na podstawie tego sposobu wyliczania kolizji sprawić, żeby były wykrywane tylko
    // kolizję z "lewej lub prawej" strony kamienia, dlatego zdecydowałem się na inne rozwiązanie problemu.
    //
    // Gdyby nie kontrolować tego zachowania, to kamień po wykryciu kolizji poziomej z dowolną płytą terenu zacznie się
    // w jego "środku" odbijać co piksel. Ciężko to opisać słownie.
    // Dlatego będziemy liczyć odbicia kamienia "w środku" płytki terenu.
    // Gdy odbicia kamienia w środku przekroczą ROCK_MAX_BOUNCES, to kamień jest niszczony.

    if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {


        yVelocity *= -1;

        if (yVelocity > 0) {

            yVelocity /= 2;

        } else {

            yVelocity *= 2;

        }

        bounces++;
    }





    // Usuwamy pocisk gdy przekroczą swój zasięg

    if (rangeCount >= ROCK_RANGE or bounces >= ROCK_MAX_BOUNCES) {

        shouldBeDeleted = true;

    } else {

        if (direction == 0) {

            if (characterType.getXVelocity() < 0) {

                direction = 1;
                auto tmpDirection = direction;
                direction *= -1;
                flip(tmpDirection);

            } else {

                direction = 1;

            }

        } else {

            if (direction == -1) {
                setX(getX() - ROCK_SPEED);
                setY(getY() - yVelocity / 4);
            } else {
                setX(getX() + ROCK_SPEED);
                setY(getY() - yVelocity / 4);
            }

            yVelocity--;





        }

        rangeCount++;

    }


}

sf::FloatRect Rock::getHitBox() const { return {getX(), getY(), ROCK_X_SIZE, ROCK_Y_SIZE}; }



StaffProj::~StaffProj() {fmt::println("Destroyed Key");}

void StaffProj::flip(const int& i_direction) {

}

void StaffProj::draw(const unsigned int view_x, sf::RenderWindow &window) {

    sprite.setPosition(round(getX()), round(getY()));
    sprite.setTexture(texture);

    window.draw(sprite);

}

void StaffProj::update(const unsigned int view_x, const std::vector<std::shared_ptr<Projectile>> &projectiles,
                       std::vector<std::shared_ptr<Enemy>> &enemies, const MapManager &mapManager, Satyr &satyr,
                       Character &characterType) {


    if(!enemies.empty()) {

        for(auto& enemy: enemies) {

            if (getHitBox().intersects(enemy->getHitBox())) {

                enemy->recieveDmg(getDmg());
                shouldBeDeleted = true;

            }

        }

    } else if (getHitBox().intersects(satyr.getHitBox())) {

        shouldBeDeleted = true;
        satyr.loseHeart();

    }

    if(getY() >= SCREEN_HEIGHT) {

        shouldBeDeleted = true;

    } else {



        setY(getY() + STAFF_PROJ_Y_VELOCITY);

    }


}

sf::FloatRect StaffProj::getHitBox() const { return sf::FloatRect(getX(), getY(), STAFF_PROJ_X_SIZE, STAFF_PROJ_Y_SIZE);}


