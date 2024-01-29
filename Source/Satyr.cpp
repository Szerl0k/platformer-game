#include <memory>
#include <vector>
#include <cmath>

#include "Satyr.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "GlobalMethods.hpp"
#include "MapManager.hpp"
#include "Projectile.hpp"


Satyr::Satyr(const int canUse) :
    Character(),
    experience(0),
    canUseBow(false),
    canUseRock(false),
    canUseStaff(false),
    resetLevel(false),
    nextLevel(false),
    isDead(false),
    isMoving(false),
    isCrouching(false),
    onGround(false),
    yVelocity(0.f),
    satyrCoins(0),
    satyrHearts(1),
    position(SATYR_SPAWN_X,SATYR_SPAWN_Y)
{

    if (canUse == 1) {
        canUseBow = true;
    } else if (canUse == 2) {
        canUseBow = true;
        canUseRock = true;
    }

    texture.loadFromFile(R"(..\Resources\Satyr.png)");

    sprite.setTexture(texture);

    lastCheckpoint = sf::Vector2i(SATYR_SPAWN_X,SATYR_SPAWN_Y);

}

// Wyjaśnienie w Projectile.cpp
Satyr::operator bool() const {
    return true;
}

bool Satyr::getNextLevel() const { return nextLevel; }

void Satyr::setCoins(const int value) { satyrCoins = value; }

int Satyr::getCoins() const { return satyrCoins; }

int Satyr::getHearts() const {return satyrHearts;}

void Satyr::setX(const float& x) {
    position.x = x;
}

void Satyr::setY(const float& y) {
    position.y = y;
}

float Satyr::getX() const {
    return position.x;
}

float Satyr::getY() const {
    return position.y;
}

void Satyr::loseHeart() {

    satyrHearts--;

    if (satyrHearts <= 0) {
        reset();
        resetLevel = true;
        satyrHearts = 1;
        canUseBow = false;
        canUseRock = false;
        experience = 0;
    } else {
        die();
    }
}

void Satyr::die() {

    setX(lastCheckpoint.x);
    setY(lastCheckpoint.y);



}

void Satyr::reset() {

    setX(SATYR_SPAWN_X);
    setY(SATYR_SPAWN_Y);
    lastCheckpoint = sf::Vector2i(SATYR_SPAWN_X,SATYR_SPAWN_Y);
    nextLevel = false;

}



sf::FloatRect Satyr::getHitBox() const {

    return {getX(), getY(), SATYR_X_SIZE, TILE_SIZE};

}

void Satyr::draw(sf::RenderWindow& window) {



    sprite.setPosition(getX(), getY());

    if ( !isDead ) {

        if ( !onGround ){

            sprite.setPosition(round(getX()), TILE_SIZE + round(getY()));


        } else {

            if ( xVelocity == 0 ) {

                sprite.setPosition(round(getX()), TILE_SIZE + round(getY()));

            }

        }

        if ( onGround ) {

            // Czy się nie rusza
            if (xVelocity == 0) {
                sprite.setPosition(round(getX()), round(getY()));

            }


        }

    }

    window.draw(sprite);


}

/*
 * Kod inspirowany: https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Mario.cpp
 * Inspirowane elementy to głównie kod, który wykorzystuje logikę kolizji
 *
 */
void Satyr::update(MapManager& mapManager, const std::vector<std::shared_ptr<Enemy>>& enemies) {

    if ( !isDead ) {

        isMoving = false;

        auto collision = std::vector<int>();

        auto hitBox = getHitBox();

        if ( !isCrouching ) {

            if ( sf::Keyboard::isKeyPressed(sf::Keyboard::D) ) {


                isMoving = true;

                xVelocity = SATYR_WALK_VELOCITY; //SATYR_WALK_VELOCITY

            }
            else if ( sf::Keyboard::isKeyPressed(sf::Keyboard::A) ) {

                isMoving = true;

                xVelocity = -SATYR_WALK_VELOCITY; // -SATYR_WALK_VELOCITY

            }


        }

        if ( !isMoving ) {

            if (xVelocity != 0)
                xVelocity = 0;
        }

        hitBox = getHitBox();
        hitBox.left += xVelocity;

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);

        // Komentarz dla mnie, żeby pamiętać do robi std::all_of
        // cplusplus.com : returns true if predicate returns true for all the elements in range or if the range is empty, and false otherwise
        // Kolizja pozioma
        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            isMoving = false;

            if ( xVelocity > 0 ){
                setX( SATYR_X_SIZE * ( ceil((xVelocity + getX()) / SATYR_X_SIZE) - 1 ));
            }
            else if (xVelocity < 0 ) {

                setX( SATYR_X_SIZE * (1 + floor((xVelocity + getX()) / SATYR_X_SIZE)));

            }

            // Tutaj chyba moze byc kod zeby "łagodnie" się zatrzymywać, ale tego chyba nie bede dodawać do gry

            xVelocity = 0;




        } else {

            setX(getX() + xVelocity);

        }

        // skakanie?
        hitBox = getHitBox();
        hitBox.top++;

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);

        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::W) ) {


            if (yVelocity == 0 and !std::all_of(collision.begin(), collision.end(), [](const int value) {return 0 == value;})) {

                yVelocity = SATYR_JUMP_VELOCITY;

            } else {

                yVelocity = std::min(GRAVITY + yVelocity, MAX_VERTICAL_SPEED);

            }

        } else {
            yVelocity = std::min(GRAVITY + yVelocity, MAX_VERTICAL_SPEED);

        }

        hitBox = getHitBox();
        hitBox.top += yVelocity;

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);

        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return 0==value;})) {

            if (yVelocity < 0) {

                setY(TILE_SIZE * (1 + floor((yVelocity + getY()) / TILE_SIZE)));

            } else if (yVelocity > 0) {

                setY(TILE_SIZE * (ceil((yVelocity + getY()) / TILE_SIZE ) - 1));

            }

            yVelocity = 0;

        } else {

            setY(getY() + yVelocity);

        }



        hitBox = getHitBox();
        hitBox.top++;

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);

        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {
            onGround = true;
        }

        if (getY() >= SCREEN_HEIGHT - getHitBox().height) {
            loseHeart();
        }

        for (auto& enemy : enemies ) {

            if (getHitBox().intersects(enemy->getHitBox()))
                loseHeart();

        }

        // Sprawdzanie checkpointow

        hitBox = getHitBox();
        hitBox.left += xVelocity;

        collision = mapManager.mapCollision({TerrainTile::CHECKPOINT}, hitBox);

        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            lastCheckpoint = sf::Vector2i(getX(), getY());

        }


        // Sprawdzanie finish line

        hitBox = getHitBox();
        //hitBox.left += xVelocity;

        collision = mapManager.mapCollision({TerrainTile::FINISH}, hitBox);

        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            fmt::println("FINISH");
            nextLevel = true;

        }

        // Zbieranie Łuku

        hitBox = getHitBox();
        hitBox.left += xVelocity;

        auto toDelete = std::vector<sf::Vector2i>();

        collision = mapManager.mapCollision({TerrainTile::BOW}, toDelete, hitBox);

        for (const sf::Vector2i bow : toDelete) {
            mapManager.setMapCell(bow.x, bow.y, TerrainTile::COLLECTED);
            canUseBow = true;
        }

        // Zbieranie Kamienia

        hitBox = getHitBox();
        hitBox.left += xVelocity;

        toDelete = std::vector<sf::Vector2i>();

        collision = mapManager.mapCollision({TerrainTile::ROCK}, toDelete, hitBox);

        for (const sf::Vector2i rock : toDelete) {
            mapManager.setMapCell(rock.x, rock.y, TerrainTile::COLLECTED);
            canUseRock = true;
        }

        // Zbieranie magicznego kija

        hitBox = getHitBox();
        hitBox.left += xVelocity;

        toDelete = std::vector<sf::Vector2i>();

        collision = mapManager.mapCollision({TerrainTile::STAFF}, toDelete, hitBox);

        for (const sf::Vector2i staff : toDelete) {
            mapManager.setMapCell(staff.x, staff.y, TerrainTile::COLLECTED);
            canUseStaff = true;

        }


        // Zbieranie coinów

        hitBox = getHitBox();

        toDelete = std::vector<sf::Vector2i>();

        // Zbieramy monety i serduszka
        mapManager.mapCollision({TerrainTile::COIN}, toDelete, hitBox);

        for (const sf::Vector2i& coin : toDelete) {

            mapManager.setMapCell(coin.x, coin.y, TerrainTile::COLLECTED);
            satyrCoins++;

        }
        mapManager.mapCollision({TerrainTile::HEART}, toDelete, hitBox);

        for (const sf::Vector2i& heart : toDelete) {

            mapManager.setMapCell(heart.x, heart.y, TerrainTile::COLLECTED);
            satyrHearts++;

        }




    }

}

std::string Satyr::type() const {return "Satyr";}
