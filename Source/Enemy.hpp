#pragma once

#include <memory>
#include <vector>

// W moim kodzie taka redefinicja czasami się pojawia. Z tego co zrozumiałem po researchu, to
// Bez tych forward declarations, kompilator "głupiał" i nie mógł znaleźć deklaracji wskutek cyklicznych
// zależności między plikami nagłówkowymi.
class Satyr;
#include "Satyr.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
class Projectile;
#include "Projectile.hpp"

#include "SFML/Graphics.hpp"

/*
 *          OGÓLNY PRZYPIS DLA PLIKU ENEMY.hpp
 *
 * W kodzie tego pliku wszystkie metody update() są inspirowane kodem :
 * https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/Enemy.hpp
 *
 * (Jako że w moim jednym pliku Enemy.hpp postanowiłem usmieścić wszystkie klasy reprezentujące
 * rodzaje przeciwników, to również umieszczam link do dwóch innych źródeł, które zawierają
 * podobne metody update() )
 *
 * https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/Goomba.hpp
 * https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/Koopa.hpp
 */

class Enemy : public std::enable_shared_from_this<Enemy>, public Character {

protected:
    bool isDead;

    float yVelocity;

    short health;

    sf::Vector2f position;



public:
    Enemy(const float x, const float y, const short health);

    operator bool() const;

    virtual bool getDead() const;

    virtual short getHealth() const;

    virtual void die();

    virtual float getX() const;

    virtual float getY() const;

    virtual void recieveDmg(const short dmg);

    virtual void draw(const unsigned view_x, sf::RenderWindow& window) = 0;

    virtual void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr
    )= 0;

    virtual sf::FloatRect getHitBox() const = 0;

};


// CAPYBARA

class Capybara : public Enemy {

    sf::Sprite sprite;

    sf::Texture texture;


public:

    Capybara(const float x, const float y, const short health);

    ~Capybara();

    bool getDead() const;

    short getHealth() const;

    void die();

    void recieveDmg(const short dmg);

    float getX() const;

    float getY() const;

    void setX(const float x);

    void setY(const float y);

    void draw(const unsigned view_x, sf::RenderWindow& window);

    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr
    );

    sf::FloatRect getHitBox() const;

};



// SKELETON

class Skeleton : public Enemy {

    int movementTimer;

    int shootingTimer;

    bool immobile;

    sf::Sprite sprite;

    sf::Texture texture;

    std::vector<std::shared_ptr<Projectile>> projectiles;

    sf::FloatRect range;


public:

    Skeleton(const float x, const float y, const short health, const bool immobile);

    ~Skeleton();

    std::string type() const;

    bool getDead() const;

    short getHealth() const;

    void die();

    void recieveDmg(const short dmg);

    float getX() const;

    float getY() const;

    void setX(const float x);

    void setY(const float y);

    void draw(const unsigned view_x, sf::RenderWindow& window);

    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr
    );

    sf::FloatRect getHitBox() const;

};


// TROLL

class Troll : public Enemy {

    int movementTimer;

    int shootingTimer;

    bool immobile;

    sf::Sprite sprite;

    sf::Texture texture;

    std::vector<std::shared_ptr<Projectile>> projectiles;

    sf::FloatRect range;


public:

    Troll(const float x, const float y, const short health, const bool immobile);

    ~Troll();

    std::string type() const;

    bool getDead() const;

    short getHealth() const;

    void die();

    void recieveDmg(const short dmg);

    float getX() const;

    float getY() const;

    void setX(const float x);

    void setY(const float y);

    void draw(const unsigned view_x, sf::RenderWindow& window);

    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr
    );

    sf::FloatRect getHitBox() const;


};


// BOSS GREYKNIGHT

class GreyKnight : public Enemy {

    int movementTimer;

    sf::Sprite sprite;

    sf::Texture texture;

public:
    GreyKnight(const float x, const float y, const short health);

    ~GreyKnight();

    bool getDead() const;

    short getHealth() const;

    void die();

    void recieveDmg(const short dmg);

    float getX() const;

    float getY() const;

    void setX(const float x);

    void setY(const float y);

    void draw(const unsigned view_x, sf::RenderWindow& window);

    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr
    );

    sf::FloatRect getHitBox() const;

};

// BOSS BLACK KNIGHT

class BlackKnight : public Enemy {

    int movementTimer;

    int shootingTimer;

    sf::Sprite sprite;

    std::vector<std::shared_ptr<Projectile>> projectiles;

    sf::Texture texture;

    sf::FloatRect range;

public:

    BlackKnight(const float x, const float y, const short health);

    ~BlackKnight();

    std::string type() const;

    bool getDead() const;

    short getHealth() const;

    void die();

    void recieveDmg(const short dmg);

    float getX() const;

    float getY() const;

    void setX(const float x);

    void setY(const float y);

    void draw(const unsigned view_x, sf::RenderWindow& window);


    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr
    );

    sf::FloatRect getHitBox() const;

};

// BOSS WIZARD

class Wizard : public Enemy {

    int movementTimer;

    int shootingTimer;

    sf::Sprite sprite;

    std::vector<std::shared_ptr<Projectile>> projectiles;

    sf::Texture texture;

    sf::FloatRect range;


public:
    Wizard(const float x, const float y, const short health);

    ~Wizard();

    std::string type() const;

    bool getDead() const;

    short getHealth() const;

    void die();

    void recieveDmg(const short dmg);

    float getX() const;

    float getY() const;

    void setX(const float x);

    void setY(const float y);

    void draw(const unsigned view_x, sf::RenderWindow& window);

    void update(
            const unsigned view_x,
            const std::vector<std::shared_ptr<Enemy>>& enemies,
            const MapManager& mapManager,
            Satyr& satyr
    );

    sf::FloatRect getHitBox() const;

};