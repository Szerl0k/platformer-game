#include <memory>
#include <cmath>
#include <vector>
#include <random>

#include "Satyr.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "GlobalMethods.hpp"
#include "MapManager.hpp"
#include "Projectile.hpp"

/*
 *      OGÓLNY PRZYPIS DLA PLIKU ENEMY.cpp
 *
 * Ten plik zawiera metody update() inspirowane kodem:
 * https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Goomba.cpp
 * https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Koopa.cpp
 *
 * Szczegółowo chodzi tu o zastosowanie logiki kolizji w tych metodach.
 *
 */


// ENEMY

Enemy::Enemy(const float x, const float y, const short health) :
    Character(),
    isDead(false),
    yVelocity(0),
    health(health)
{
    position.x = x;
    position.y = y;

}

Enemy::operator bool() const { return false; }

bool Enemy::getDead() const { return isDead; }

short Enemy::getHealth() const {return health;}

void Enemy::die() {isDead = true;}

float Enemy::getX() const { return position.x;}

float Enemy::getY() const {return position.y;}

void Enemy::recieveDmg(const short dmg) {
    health -= dmg;
    if (health <= 0)
        isDead = true;
}



                /* ===========================================   CAPYBARA   =========================================== */




Capybara::Capybara(const float x, const float y, short health) :
        Enemy(x, y + TILE_SIZE + CAPY_Y_SIZE / 2 , health)
{
    xVelocity = CAPY_SPEED;
    if(texture.loadFromFile(R"(..\Resources\Capybara.png)"))
        fmt::println("Capybara created");

    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
}

Capybara::~Capybara() { fmt::println("Capybara destroyed");}

bool Capybara::getDead() const { return isDead; }

short Capybara::getHealth() const {return health;}

void Capybara::die() {if (health <= 0 ) isDead = true;}

void Capybara::recieveDmg(const short dmg) {
    health -= dmg;
    die();
}

float Capybara::getX() const { return position.x; }

float Capybara::getY() const { return position.y; }

void Capybara::setX(const float x) { position.x = x; }

void Capybara::setY(const float y) { position.y = y; }

void Capybara::draw(const unsigned int view_x, sf::RenderWindow &window) {

    sprite.setPosition(round(getX()), getY());
    sprite.setTexture(texture);

    window.draw(sprite);

}

void Capybara::update(const unsigned int view_x, const std::vector<std::shared_ptr<Enemy>> &enemies,
                      const MapManager &mapManager, Satyr &satyr) {

    // Aktualizowanie kapibary wyłącznie w środku widoku

    if (
            -TILE_SIZE / getY() and
            getX() >= static_cast<int>(view_x) - TILE_SIZE - ENEMY_UPDATE_AREA and
            getX() < ENEMY_UPDATE_AREA + SCREEN_WIDTH + view_x and
            getY() < SCREEN_HEIGHT
    ) {

        auto collision = std::vector<int>();

        sf::FloatRect hitBox = getHitBox();
        hitBox.top += yVelocity;

        yVelocity = std::min(GRAVITY + yVelocity, MAX_VERTICAL_SPEED);

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);

        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            if (0 > yVelocity) {

                setY( (TILE_SIZE - CAPY_Y_SIZE) * (1 + floor((yVelocity + getY()) / (TILE_SIZE - CAPY_Y_SIZE))));


            } else {

                setY( (TILE_SIZE - CAPY_Y_SIZE) * (ceil((yVelocity + getY()) / (TILE_SIZE - CAPY_Y_SIZE) ) - 1 ));

            }

            yVelocity = 0;

        }


        if ( !getDead() ) {

            hitBox = getHitBox();
            hitBox.left += xVelocity;

            collision = mapManager.mapCollision(allCollisionTiles, hitBox);

            if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

                if (0 < xVelocity) {

                    setX( TILE_SIZE * (ceil((xVelocity + getX()) / TILE_SIZE) - 1));

                } else {

                    setX( TILE_SIZE * (1 + floor((xVelocity + getX()) / TILE_SIZE)));

                }
                sprite.setTexture(texture);
                if (xVelocity > 0) {
                    sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                } else {
                    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                }
                xVelocity *= -1;


            } else {

                setX( getX() + xVelocity );

            }

        }

        setY(getY() + yVelocity);

    }



    if (getY() > SCREEN_HEIGHT) {
        isDead = true;
    }

}


sf::FloatRect Capybara::getHitBox() const { return {getX(), getY(), CAPY_X_SIZE, CAPY_Y_SIZE}; }







                /* ===========================================   SKELETON   =========================================== */



Skeleton::Skeleton(const float x, const float y, const short health, const bool immobile) :
    Enemy(x,y - TILE_SIZE,health),
    movementTimer(0),
    shootingTimer(0),
    immobile(immobile)
{
    projectiles = std::vector<std::shared_ptr<Projectile>>();

    range = sf::FloatRect();

    xVelocity = SKELETON_SPEED;

    if (texture.loadFromFile(R"(..\Resources\Skeleton.png)"))
        fmt::println("Skeleton created");

    sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
}

Skeleton::~Skeleton() { fmt::println("Skeleton destroyed"); }



std::string Skeleton::type() const { return "Skeleton";}

bool Skeleton::getDead() const { return isDead; }

short Skeleton::getHealth() const {return health;}

void Skeleton::die() { if (health <= 0) isDead = true; }

void Skeleton::recieveDmg(const short dmg) {
    health -= dmg;
    die();
}

float Skeleton::getX() const { return position.x; }

float Skeleton::getY() const { return position.y; }

void Skeleton::setX(const float x) { position.x = x; }

void Skeleton::setY(const float y) { position.y = y; }

void Skeleton::draw(const unsigned int view_x, sf::RenderWindow &window) {

    sprite.setPosition(round(getX()), getY());
    sprite.setTexture(texture);

    window.draw(sprite);

    for (auto& projectile : projectiles) {
        projectile->draw(view_x, window);
    }

}


void Skeleton::update(const unsigned int view_x, const std::vector<std::shared_ptr<Enemy>> &enemies,
                      const MapManager &mapManager, Satyr &satyr) {

    movementTimer++;
    shootingTimer++;

    range = sf::FloatRect(getX() - SKELETON_X_RANGE, getY() - SKELETON_Y_RANGE, 3 * SKELETON_X_RANGE, 2*SKELETON_Y_RANGE);

    if (
            -TILE_SIZE / getY() and
            getX() >= static_cast<int>(view_x) - TILE_SIZE - ENEMY_UPDATE_AREA and
            getX() < ENEMY_UPDATE_AREA + SCREEN_WIDTH + view_x and
            getY() < SCREEN_HEIGHT
    ) {



        auto collision = std::vector<int>();

        sf::FloatRect hitBox = getHitBox();
        hitBox.top += yVelocity;

        yVelocity = std::min(GRAVITY + yVelocity, MAX_VERTICAL_SPEED);

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);



        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            if (0 > yVelocity) {

                    setY( (SKELETON_Y_SIZE) * (1 + floor((yVelocity + getY()) / (SKELETON_Y_SIZE))));

            } else {

                    setY( (SKELETON_Y_SIZE) * (ceil((yVelocity + getY()) / (SKELETON_Y_SIZE) ) - 1 ));

            }

            yVelocity = 0;

        }


        if ( !getDead() ) {

            if (!immobile) {

                hitBox = getHitBox();
                hitBox.left += xVelocity;

                collision = mapManager.mapCollision(allCollisionTiles, hitBox);

                if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

                    if (0 < xVelocity) {

                        setX( TILE_SIZE * (ceil((xVelocity + getX()) / TILE_SIZE) - 1));

                    } else {

                        setX( TILE_SIZE * (1 + floor((xVelocity + getX()) / TILE_SIZE)));

                    }
                    sprite.setTexture(texture);
                    xVelocity *= -1;
                    if (xVelocity > 0) {
                        sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                    } else {
                        sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                    }


                } else {

                    if (movementTimer > SKELETON_MAX_MOVEMENT_TIMER){
                        xVelocity *= -1;
                        movementTimer = 0;

                        if (xVelocity > 0) {
                            sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                        } else {
                            sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                        }

                    }

                    setX( getX() + xVelocity );

                }
            } else {
                xVelocity = -1;
            }

        }


        setY(getY() + yVelocity);



    }

    if (satyr.getHitBox().intersects(range) and shootingTimer > SKELETON_SHOOTING_TIMER_MAX) {

        auto satyrPosition = sf::Vector2f(satyr.getX(), satyr.getY());


        projectiles.push_back(std::make_shared<Arrow>(ARROW_DMG, *this));


        shootingTimer = 0;

    }

    auto dummy = std::vector<std::shared_ptr<Enemy>>();


    for (auto i = 0 ; i < projectiles.size() ; i++) {

        projectiles[i]->update(view_x, projectiles, dummy, mapManager, satyr, *this);

    }

    for (auto i = 0 ; i < projectiles.size() ; i++) {

        if (projectiles[i]->getDelete()) {

            projectiles.erase(projectiles.begin() + i);

        }

    }


    if (getY() > SCREEN_HEIGHT) {
        isDead = true;
    }


}

sf::FloatRect Skeleton::getHitBox() const { return sf::FloatRect(getX(), getY(), SKELETON_X_SIZE, SKELETON_Y_SIZE); }


        /* ===========================================   TROLL   =========================================== */


Troll::Troll(const float x, const float y, const short health, const bool immobile) :
        Enemy(x, y, health),
        movementTimer(0),
        shootingTimer(0),
        immobile(immobile)

{

    projectiles = std::vector<std::shared_ptr<Projectile>>();

    range = sf::FloatRect();

    xVelocity = TROLL_SPEED;

    if (texture.loadFromFile(R"(..\Resources\Troll.png)"))
        fmt::println("Troll created");

    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));

}

Troll::~Troll() { fmt::println("Troll destroyed"); }



bool Troll::getDead() const { return isDead; }

short Troll::getHealth() const {return health;}

void Troll::die() { if (health <= 0) isDead = true; }

void Troll::recieveDmg(const short dmg) {
    health -= dmg;
    die();
}

float Troll::getX() const { return position.x; }

float Troll::getY() const { return position.y; }

void Troll::setX(const float x) { position.x = x; }

void Troll::setY(const float y) { position.y = y; }


void Troll::draw(const unsigned int view_x, sf::RenderWindow &window) {

    sprite.setPosition(round(getX()), getY());

    sprite.setTexture(texture);

    window.draw(sprite);

    for (auto& projectile : projectiles) {

        projectile->draw(view_x, window);

    }


}

void Troll::update(const unsigned int view_x, const std::vector<std::shared_ptr<Enemy>> &enemies,
                   const MapManager &mapManager, Satyr &satyr) {

    movementTimer++;
    shootingTimer++;

    range = sf::FloatRect(getX() - TROLL_X_RANGE, getY() - TROLL_Y_RANGE, 3 * TROLL_X_RANGE, 2*TROLL_Y_RANGE);

    if (
            -TILE_SIZE / getY() and
            getX() >= static_cast<int>(view_x) - TILE_SIZE - ENEMY_UPDATE_AREA and
            getX() < ENEMY_UPDATE_AREA + SCREEN_WIDTH + view_x and
            getY() < SCREEN_HEIGHT
    ) {

        auto collision = std::vector<int>();

        sf::FloatRect hitBox = getHitBox();
        hitBox.top += yVelocity;


        yVelocity = std::min(GRAVITY + yVelocity, 1.f);



        collision = mapManager.mapCollision(allCollisionTiles, hitBox);

        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            if (0 > yVelocity) {

                setY( (TILE_SIZE) * (1 + floor((yVelocity + getY()) / (TILE_SIZE))));

            } else {

                setY( (TILE_SIZE) * (ceil((yVelocity + getY()) / (TILE_SIZE) ) - 1 ));

            }

            yVelocity = 0;

        } else {



        }

        if (!getDead()) {

            if (!immobile) {
                hitBox = getHitBox();
                hitBox.left += xVelocity;

                collision = mapManager.mapCollision(allCollisionTiles, hitBox);

                if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

                    if (0 < xVelocity) {

                        setX( TILE_SIZE * (ceil((xVelocity + getX()) / TILE_SIZE) - 1));

                    } else {

                        setX( TILE_SIZE * (1 + floor((xVelocity + getX()) / TILE_SIZE)));

                    }

                    sprite.setTexture(texture);
                    xVelocity *= -1;

                    if (xVelocity > 0) {
                        sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                    } else {
                        sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                    }

                } else {

                    if (movementTimer > TROLL_MAX_MOVEMENT_TIMER) {

                        xVelocity *= -1;
                        movementTimer = 0;

                        if (xVelocity > 0) {
                            sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                        } else {
                            sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                        }

                    }

                    setX(getX() + xVelocity);


                }
            } else {
                xVelocity = -1;
            }

        }

        setY(getY() + yVelocity);

    }

    if (satyr.getHitBox().intersects(range) and shootingTimer > TROLL_SHOOTING_TIMER_MAX) {


        projectiles.push_back(std::make_shared<Rock>(ROCK_TROLL_DMG, *this));
        shootingTimer = 0;

    }

    auto dummy = std::vector<std::shared_ptr<Enemy>>();

    for (auto i = 0; i < projectiles.size() ; i++) {

        projectiles[i]->update(view_x, projectiles, dummy, mapManager, satyr, *this);

    }

    for (auto i = 0 ; i < projectiles.size() ; i++) {

        if (projectiles[i]->getDelete()) {

            projectiles.erase(projectiles.begin() + i);

        }

    }

    if (getY() > SCREEN_HEIGHT) {
        isDead = true;
    }


}


sf::FloatRect Troll::getHitBox() const { return {getX(), getY(), TROLL_X_SIZE, TROLL_Y_SIZE};}

std::string Troll::type() const {return "Troll";}

/* ===========================================   BOSS GREY KNIGHT   =========================================== */


GreyKnight::GreyKnight(const float x, const float y, const short health) :
        Enemy(x, y, health),
        movementTimer(GREY_KNIGHT_DEFAULT_MOVEMENT_TIMER)
{
    xVelocity = GREY_KNIGHT_SPEED;

    if (texture.loadFromFile(R"(..\Resources\GreyKnight.png)")) {
        fmt::println("Grey knight created");
    }

    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
}

GreyKnight::~GreyKnight() { fmt::println("Grey knight destroyed"); }

bool GreyKnight::getDead() const { return isDead; }

short GreyKnight::getHealth() const {return health;}

void GreyKnight::die() { if (health <= 0) isDead = true; }

void GreyKnight::recieveDmg(const short dmg) {
    health -= dmg;
    die();
}

float GreyKnight::getX() const { return position.x; }

float GreyKnight::getY() const { return position.y; }

void GreyKnight::setX(const float x) { position.x = x; }

void GreyKnight::setY(const float y) { position.y = y; }

void GreyKnight::draw(const unsigned int view_x, sf::RenderWindow &window) {

    sprite.setPosition(round(getX()), getY());
    sprite.setTexture(texture);

    window.draw(sprite);
}

void GreyKnight::update(const unsigned int view_x, const std::vector<std::shared_ptr<Enemy>> &enemies,
                    const MapManager &mapManager, Satyr &satyr) {

    movementTimer++;

    if (
            -TILE_SIZE / getY() and
            getX() >= static_cast<int>(view_x) - TILE_SIZE - BOSS_UPDATE_AREA and
            getX() < BOSS_UPDATE_AREA + SCREEN_WIDTH + view_x and
            getY() < SCREEN_HEIGHT
            ) {

        auto collision = std::vector<int>();

        sf::FloatRect hitBox = getHitBox();
        hitBox.top += yVelocity;

        yVelocity = std::min(GRAVITY + yVelocity, MAX_VERTICAL_SPEED);

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);

        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            if (0 > yVelocity) {

                setY( (TILE_SIZE - GREY_KNIGHT_Y_SIZE) * (1 + floor((yVelocity + getY()) / (TILE_SIZE - GREY_KNIGHT_Y_SIZE))));


            } else {

                setY( (TILE_SIZE - GREY_KNIGHT_Y_SIZE) * (ceil((yVelocity + getY()) / (TILE_SIZE - GREY_KNIGHT_Y_SIZE) ) - 1 ));

            }

            yVelocity = 0;

        }


        if ( !getDead() ) {

            hitBox = getHitBox();
            hitBox.left += xVelocity;

            collision = mapManager.mapCollision(allCollisionTiles, hitBox);

            if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

                if (0 < xVelocity) {

                    setX( TILE_SIZE * (ceil((xVelocity + getX()) / TILE_SIZE) - 1));

                } else {

                    setX( TILE_SIZE * (1 + floor((xVelocity + getX()) / TILE_SIZE)));

                }
                sprite.setTexture(texture);
                if (xVelocity > 0) {
                    sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                } else {
                    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                }
                xVelocity *= -1;


            } else {

                setX( getX() + xVelocity );

            }

        }

        setY(getY() + yVelocity);

    }


    if (movementTimer > GREY_KNIGHT_MAX_MOVEMENT_TIMER) {

        // Mechanizm losowych liczb z ChatGPT.
        // Losujemy dowolnego inta z przedziału 0-GREY_KNIGHT_MAX_MOVEMENT_TIMER <- czyli pewnie 500;

        auto dev = std::random_device();
        auto rng = std::mt19937(dev());
        auto dist6 = std::uniform_int_distribution<std::mt19937::result_type>(0,GREY_KNIGHT_MAX_MOVEMENT_TIMER);

        movementTimer = dist6(rng);

        xVelocity *= -1;

        if (xVelocity > 0) {
            sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
        } else {
            sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
        }

    }


    if (getY() > SCREEN_HEIGHT) {
        isDead = true;
    }

}

sf::FloatRect GreyKnight::getHitBox() const { return sf::FloatRect( getX(), getY(), GREY_KNIGHT_X_SIZE, GREY_KNIGHT_Y_SIZE );}



    /* ===========================================   BOSS BLACK KNIGHT   =========================================== */


BlackKnight::BlackKnight(const float x, const float y, const short health) :
        Enemy(x,y,health),
        movementTimer(BLACK_KNIGHT_DETAULT_MOVEMENT_TIMER),
        shootingTimer(0)
{

    projectiles = std::vector<std::shared_ptr<Projectile>>();

    range = sf::FloatRect();

    xVelocity = BLACK_KNIGHT_SPEED;

    if (texture.loadFromFile(R"(..\Resources\BlackKnight.png)"))
        fmt::println("Black knight created");

    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));


}

BlackKnight::~BlackKnight() {fmt::println("Black knight destroyed");}



std::string BlackKnight::type() const { return "BlackKnight";}

bool BlackKnight::getDead() const { return isDead; }

short BlackKnight::getHealth() const {return health;}

void BlackKnight::die() { if (health <= 0) isDead = true;}

void BlackKnight::recieveDmg(const short dmg) {
    health -= dmg;
    die();
}

float BlackKnight::getX() const {return position.x;}

float BlackKnight::getY() const { return position.y; }

void BlackKnight::setX(const float x) { position.x = x; }

void BlackKnight::setY(const float y) { position.y = y; }

void BlackKnight::draw(const unsigned int view_x, sf::RenderWindow &window) {

    sprite.setPosition(round(getX()), getY());
    sprite.setTexture(texture);

    window.draw(sprite);

    for (auto& projectile : projectiles) {
        projectile->draw(view_x, window);
    }

}

void BlackKnight::update(const unsigned int view_x, const std::vector<std::shared_ptr<Enemy>> &enemies,
                         const MapManager &mapManager, Satyr &satyr) {

    movementTimer++;
    shootingTimer++;

    range = sf::FloatRect(getX() - BLACK_KNIGHT_X_RANGE, getY() - BLACK_KNIGHT_Y_RANGE, 3 * BLACK_KNIGHT_X_RANGE, 2 * BLACK_KNIGHT_Y_RANGE);

    if (
            -TILE_SIZE / getY() and
            getX() >= static_cast<int>(view_x) - TILE_SIZE - BOSS_UPDATE_AREA and
            getX() < BOSS_UPDATE_AREA + SCREEN_WIDTH + view_x and
            getY() < SCREEN_HEIGHT
    ) {

        auto collision = std::vector<int>();

        sf::FloatRect hitBox = getHitBox();
        hitBox.top += yVelocity;

        yVelocity = std::min(GRAVITY + yVelocity, MAX_VERTICAL_SPEED);

        collision = mapManager.mapCollision(allCollisionTiles, hitBox);


        if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

            if (0 > yVelocity) {

                setY( (TILE_SIZE - BLACK_KNIGHT_Y_SIZE) * (1 + floor((yVelocity + getY()) / (TILE_SIZE - BLACK_KNIGHT_Y_SIZE))));


            } else {

                setY( (TILE_SIZE - BLACK_KNIGHT_Y_SIZE) * (ceil((yVelocity + getY()) / (TILE_SIZE - BLACK_KNIGHT_Y_SIZE) ) - 1 ));

            }

            yVelocity = 0;

        }

        if ( !getDead() ) {

            hitBox = getHitBox();
            hitBox.left += xVelocity;

            collision = mapManager.mapCollision(allCollisionTiles, hitBox);

            if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

                if (0 < xVelocity) {

                    setX( TILE_SIZE * (ceil((xVelocity + getX()) / TILE_SIZE) - 1));

                } else {

                    setX( TILE_SIZE * (1 + floor((xVelocity + getX()) / TILE_SIZE)));

                }
                sprite.setTexture(texture);
                if (xVelocity > 0) {
                    sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                } else {
                    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                }
                xVelocity *= -1;


            } else {

                setX( getX() + xVelocity );

            }


        }

        setY(getY() + yVelocity);

    }


    if (movementTimer > BLACK_KNIGHT_MAX_MOVEMENT_TIMER) {

        // Mechanizm losowych liczb z ChatGPT.
        // Losujemy dowolnego inta z przedziału 0 - BLACK_KNIGHT_MAX_MOVEMENT_TIMER <- czyli pewnie 500;

        auto dev = std::random_device();
        auto rng = std::mt19937(dev());
        auto dist6 = std::uniform_int_distribution<std::mt19937::result_type>(0,BLACK_KNIGHT_MAX_MOVEMENT_TIMER);

        movementTimer = dist6(rng);

        xVelocity *= -1;

        if (xVelocity > 0) {
            sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
        } else {
            sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
        }

    }

    if (satyr.getHitBox().intersects(range) and shootingTimer > BLACK_KNIGHT_SHOOTING_TIMER_MAX) {


        projectiles.push_back(std::make_shared<Arrow>(ARROW_DMG, *this));
        shootingTimer = 0;

    }

    auto dummy = std::vector<std::shared_ptr<Enemy>>();

    for (auto i = 0 ; i < projectiles.size() ; i++) {

        projectiles[i]->update(view_x, projectiles, dummy, mapManager, satyr, *this);

    }

    for (auto i = 0 ; i < projectiles.size() ; i++) {

        if (projectiles[i]->getDelete()) {

            projectiles.erase(projectiles.begin() + i);

        }

    }

}

sf::FloatRect BlackKnight::getHitBox() const { return sf::FloatRect( getX(), getY(), BLACK_KNIGHT_X_SIZE, BLACK_KNIGHT_Y_SIZE );}



/* ===========================================   BOSS  WIZARD   =========================================== */


Wizard::Wizard(const float x, const float y, const short health) :
        Enemy(x,y,health),
        movementTimer(WIZARD_DEFAULT_MOVEMENT_TIMER),
        shootingTimer(0)
{

    projectiles = std::vector<std::shared_ptr<Projectile>>();

    range = sf::FloatRect();

    xVelocity = WIZARD_X_VELOCITY;

    if (texture.loadFromFile(R"(..\Resources\Wizard.png)"))
        fmt::println("Wizard created");

    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));

}

Wizard::~Wizard() {fmt::println("Wizard destroyed");}



std::string Wizard::type() const {return "Wizard";}

bool Wizard::getDead() const { return isDead; }

short Wizard::getHealth() const {return health;}

void Wizard::die() { if (health <= 0) isDead = true;}

void Wizard::recieveDmg(const short dmg) {
    health -= dmg;
    die();
}

float Wizard::getX() const {return position.x;}

float Wizard::getY() const { return position.y; }

void Wizard::setX(const float x) { position.x = x; }

void Wizard::setY(const float y) { position.y = y; }

void Wizard::draw(const unsigned int view_x, sf::RenderWindow &window) {


    sprite.setPosition(round(getX()), getY());
    sprite.setTexture(texture);

    window.draw(sprite);

    for (auto& projectile : projectiles) {

        projectile->draw(view_x, window);

    }

}

void Wizard::update(const unsigned int view_x, const std::vector<std::shared_ptr<Enemy>> &enemies,
                    const MapManager &mapManager, Satyr &satyr) {

    movementTimer++;
    shootingTimer++;

    range = sf::FloatRect(getX() - WIZARD_X_RANGE, getY() - WIZARD_Y_RANGE, 3 * WIZARD_X_RANGE, 2 * WIZARD_Y_RANGE);


    if (
            -TILE_SIZE / getY() and
            getX() >= static_cast<int>(view_x) - TILE_SIZE - BOSS_UPDATE_AREA and
            getX() < BOSS_UPDATE_AREA + SCREEN_WIDTH + view_x and
            getY() < SCREEN_HEIGHT
    ) {

        auto collision = std::vector<int>();

        sf::FloatRect hitBox = getHitBox();


        // Ustawiamy czarodzieja na stałej wysokości, takiej, żeby zarówno statyr mogl trafiac w niego pociskami, jak i mógł swobodnie latać nad satyrem
        setY(SCREEN_HEIGHT / 2 + 50);

        if ( !getDead() ) {

            hitBox = getHitBox();
            hitBox.left += xVelocity;

            collision = mapManager.mapCollision(allCollisionTiles, hitBox);

            if (!std::all_of(collision.begin(), collision.end(), [](const int value) {return value == 0;})) {

                if (0 < xVelocity) {

                    setX( TILE_SIZE * (ceil((xVelocity + getX()) / TILE_SIZE) - 1));

                } else {

                    setX( TILE_SIZE * (1 + floor((xVelocity + getX()) / TILE_SIZE)));

                }
                sprite.setTexture(texture);
                if (xVelocity > 0) {
                    sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
                } else {
                    sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
                }
                xVelocity *= -1;


            } else {

                setX( getX() + xVelocity );

            }

        }

        setY(getY() + yVelocity);


    }

    if (movementTimer > WIZARD_MAX_MOVEMENT_TIMER) {

        auto dev = std::random_device();
        auto rng = std::mt19937(dev());
        auto dist6 = std::uniform_int_distribution<std::mt19937::result_type>(0,BLACK_KNIGHT_MAX_MOVEMENT_TIMER);

        movementTimer = dist6(rng);

        xVelocity *= -1;

        if (xVelocity > 0) {
            sprite.setTextureRect(sf::IntRect(0,0, texture.getSize().x / 2, texture.getSize().y));
        } else {
            sprite.setTextureRect(sf::IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y));
        }


    }

    if (satyr.getHitBox().intersects(range) and shootingTimer > WIZARD_SHOOTING_TIMER_MAX) {



        projectiles.push_back(std::make_shared<StaffProj>(STAFF_DMG, *this));
        shootingTimer = 0;

    }

    auto dummy = std::vector<std::shared_ptr<Enemy>>();


    for (auto i = 0 ; i < projectiles.size() ; i++) {

        projectiles[i]->update(view_x, projectiles, dummy, mapManager, satyr, *this);

    }

    for (auto i = 0 ; i < projectiles.size() ; i++) {

        if (projectiles[i]->getDelete()) {

            projectiles.erase(projectiles.begin() + i);

        }

    }


}


sf::FloatRect Wizard::getHitBox() const { return sf::FloatRect( getX(), getY(), WIZARD_X_SIZE, WIZARD_Y_SIZE );}