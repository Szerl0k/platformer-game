#include <cmath>

#include "fmt/core.h"
#include <iostream>

#include "Satyr.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "GlobalMethods.hpp"
#include "MapManager.hpp"
#include "Projectile.hpp"


// Klasa jest inspirowana kodem z https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/MapManager.cpp
// Główną inspiracją jest tutaj logika kolizji mapy i częściowo logika rysowania mapy.
// Pozostałe pola i metody, są jedynie elementami niezbędnymi do poprawnego działania tej logiki.


MapManager::MapManager(){

    mapTexture.loadFromFile(R"(..\Resources\MapTextures.png)");

    tileSprite.setTexture(mapTexture);

}

int MapManager::getMapSketchHeight() { return mapSketch.getSize().y;}

int MapManager::getMapSketchWidth() {return mapSketch.getSize().x;}

int MapManager::getMapWidth() const {return map.size();}

sf::Color MapManager::getMapSketchPixel(int x, int y) {return mapSketch.getPixel(x,y);}

void MapManager::loadMapSketch(const int currentLevel) {
    if (mapSketch.loadFromFile(R"(..\Resources\MapSketch)" + std::to_string(currentLevel) + ".png")) {
        fmt::println("Sucesfully loaded Sketch file");
    }
}

void MapManager::update() {


}

void MapManager::setMapCell(int i, int j, const TerrainTile& tile) {map[i][j] = tile;}

void MapManager::setMapSize(const int newSize) {
    map.clear();
    map.resize(newSize);
}




/*
 * Obie metody mapCollision są zaciągnięte z: https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/MapManager.hpp
 * Logika przestawiona w tych metodach jest taka sama jak u źródła.
 * Zwraca ona wektor liczb typu int. Każda z tych liczb jest binarną reprezentacją kolizji w kolejnym rzędzie hitboxa
 * Np. hitbox 4x4:
 * 0110
 * 0000
 * 0100
 * 1111
 * 0 - oznaczają brak kolizji; 1 - oznaczają kolizję
 * Wobec tego zwrócony wektor będzie miał w sobie: {6, 0, 4, 15}
 * W innych klasach sprawdzane jest czy zwrócny wektor ma w sobie same 0, czy jakąś inną reprezentacje binarną.
 * Na podstawie tej informacji zostanie wydedukowana istniejaca (bądź nie) kolizja z potencjalnymi blokami wokół.
 */

std::vector<int>
MapManager::mapCollision(const std::vector<TerrainTile>& tilesToCheck, const sf::FloatRect& hitbox) const {

    std::vector<int> retVec;

    for (short a = floor(hitbox.top / TILE_SIZE); a <= floor((ceil(hitbox.height + hitbox.top) - 1) /  TILE_SIZE); a++) {

        retVec.push_back(0);

        for (short b = floor(hitbox.left / TILE_SIZE); b <= floor((ceil(hitbox.width + hitbox.left) - 1) / TILE_SIZE); b++) {

            if (b >= 0 and b < map.size()) {

                if (a >= 0 and a < map[0].size()) {

                    if (tilesToCheck.end() != std::find(tilesToCheck.begin(), tilesToCheck.end(), map[b][a])) {

                        retVec[a - floor(hitbox.top / TILE_SIZE)] += pow(2, floor((ceil(hitbox.left + hitbox.width) - 1 ) / TILE_SIZE) - b);


                    }
                }

            }
            else if (tilesToCheck.end() != std::find(tilesToCheck.begin(), tilesToCheck.end(), TerrainTile::GROUND_MIDDLE)) {

                retVec[a - floor(hitbox.top / TILE_SIZE)] += pow(2, floor((ceil(hitbox.width + hitbox.left) - 1 ) / TILE_SIZE) - b);

            }

        }

    }

    return retVec;

}


/* czyt. poprzedni komentarz */

// Taka sama metoda jak wyżej, z wyjątkiem, że dostarczamy do niej wektor, do którego dodawane są koordynaty elementów mapy, których hitbox dotyka
std::vector<int>
MapManager::mapCollision(const std::vector<TerrainTile>& tilesToCheck, std::vector<sf::Vector2i>& tilesToDelete, const sf::FloatRect& hitbox) const {

    std::vector<int> retVec;

    tilesToDelete.clear();

    for (int a = floor(hitbox.top / TILE_SIZE); a <= floor((ceil(hitbox.height + hitbox.top) - 1) / TILE_SIZE); a++) {

        retVec.push_back(0);

        for (int b = floor(hitbox.left / TILE_SIZE); b <= floor((ceil(hitbox.width + hitbox.left) - 1) / TILE_SIZE); b++) {

            if (b >= 0 and b < map.size()) {

                if (a >= 0 and a < map[0].size()) {

                    if (tilesToCheck.end() != std::find(tilesToCheck.begin(), tilesToCheck.end(), map[b][a])) {

                        tilesToDelete.push_back(sf::Vector2i(b,a));

                        retVec[a - floor(hitbox.top / TILE_SIZE)] += pow(2, floor((ceil(hitbox.left + hitbox.width) - 1) / TILE_SIZE) - b);

                    }

                }

            }
            else if (tilesToCheck.end() != std::find(tilesToCheck.begin(), tilesToCheck.end(), TerrainTile::GROUND_MIDDLE)) {

                retVec[a - floor(hitbox.top / TILE_SIZE)] += pow(2, floor((ceil(hitbox.left + hitbox.width) - 1 ) / TILE_SIZE) - b);

            }

        }


    }

    return retVec;

}

/*
  * Metoda drawMap jest inspirowana z: https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/MapManager.cpp
  * Mimo, że kod w niej zawarty różni się od tego co u źródła, to jest on inspirowany.
  * Został on zmieniony pod kątem tego co jest potrzebne i wykorzystywane w moim projekcie.
  */

void MapManager::drawMap(sf::RenderTexture& renderTexture, const unsigned view_x) {

    unsigned int mapEnd = ceil( (SCREEN_WIDTH + view_x + ENEMY_UPDATE_AREA) / static_cast<float>(TILE_SIZE));
    unsigned int mapHeight = floor(mapSketch.getSize().y);
    unsigned int mapStart = floor(view_x / static_cast<float>(TILE_SIZE));

    for ( auto i = mapStart ; i < mapEnd ; i++ ) {

        for ( auto j = 0 ; j < mapHeight ; j++ ) {

            short sprite_x = 0;
            short sprite_y = 0;

            tileSprite.setPosition(TILE_SIZE * i, TILE_SIZE * j);


            if ( map[i][j] != TerrainTile::EMPTY ) {

                // TerrainTile::GROUND_TOP_LEFT jest rysowany domyślnie, jeśli poniższe warunki się nie spełnią,
                // ponieważ sprite_x i sprite_y jest zainicjalizowane jako (0,0),
                // a jest to też pozycją tego bloku w teksturach
                if (TerrainTile::GROUND_MIDDLE == map[i][j]) { sprite_x = 1; sprite_y = 1; }

                else if (map[i][j] == TerrainTile::GROUND_TOP) { sprite_x = 1; sprite_y = 0; }

                else if (map[i][j] == TerrainTile::GROUND_LEFT) { sprite_x = 0, sprite_y = 1; }

                else if (map[i][j] == TerrainTile::GROUND_TOP_RIGHT) { sprite_x = 2, sprite_y = 0; }

                else if (map[i][j] == TerrainTile::GROUND_RIGHT) { sprite_x = 2, sprite_y = 1; }

                else if (map[i][j] == TerrainTile::GROUND_BOTTOM_RIGHT) { sprite_x = 2, sprite_y = 2; }

                else if (map[i][j] == TerrainTile::GROUND_BOTTOM) { sprite_x = 1, sprite_y = 2; }

                else if (map[i][j] == TerrainTile::GROUND_BOTTOM_LEFT) { sprite_x = 0, sprite_y = 2; }

                else if (map[i][j] == TerrainTile::GROUND_PILLAR_TOP) { sprite_x = 4, sprite_y = 0; }

                else if (map[i][j] == TerrainTile::GROUND_PILLAR_MIDDLE) { sprite_x = 4, sprite_y = 1; }

                else if (map[i][j] == TerrainTile::GROUND_PILLAR_BOTTOM) {sprite_x = 4, sprite_y = 2; }

                else if (map[i][j] == TerrainTile::IRREGULAR_GROUND) { sprite_x = 3, sprite_y = 2; }

                else if (map[i][j] == TerrainTile::GROUND_DEEP_1) { sprite_x = 5, sprite_y = 0; }

                else if (map[i][j] == TerrainTile::GROUND_DEEP_2) { sprite_x = 6, sprite_y = 0; }

                else if (map[i][j] == TerrainTile::GROUND_DEEP_3) { sprite_x = 7, sprite_y = 0; }

                else if (map[i][j] == TerrainTile::GRASS_DENSER) {sprite_x = 5, sprite_y = 2;}

                else if (map[i][j] == TerrainTile::GRASS_DEFAULT) {sprite_x = 6, sprite_y = 2;}

                else if (map[i][j] == TerrainTile::GRASS_ABOVE) {sprite_x  = 7, sprite_y = 2;}

                else if (map[i][j] == TerrainTile::CHECKPOINT) {sprite_x = 5, sprite_y = 1;}

                else if (map[i][j] == TerrainTile::FINISH) {sprite_x = 6, sprite_y = 1;}

                else if (map[i][j] == TerrainTile::COIN) {sprite_x = 3, sprite_y = 0;}

                else if (map[i][j] == TerrainTile::HEART) {sprite_x = 3, sprite_y = 1;}

                else if (map[i][j] == TerrainTile::BOW) {sprite_x = 0, sprite_y = 3;}

                else if (map[i][j] == TerrainTile::ROCK) {sprite_x = 1, sprite_y = 3;}

                else if (map[i][j] == TerrainTile::STAFF) {sprite_x = 2, sprite_y = 3;}

                else if (map[i][j] == TerrainTile::COLLECTED) {
                    sprite_x = 7, sprite_y = 1;

                }

                else if (
                        map[i][j] == TerrainTile::CAPYBARA or
                        map[i][j] == TerrainTile::SKELETON or
                        map[i][j] == TerrainTile::IMMOBILE_SKELETON or
                        map[i][j] == TerrainTile::GREY_KNIGHT or
                        map[i][j] == TerrainTile::BLACK_KNIGHT or
                        map[i][j] == TerrainTile::TROLL or
                        map[i][j] == TerrainTile::IMMOBILE_TROLL or
                        map[i][j] == TerrainTile::WIZARD or
                        map[i][j] == TerrainTile::COLLECTED
                ) { continue; }


                tileSprite.setTextureRect(sf::IntRect(sprite_x * TILE_SIZE, sprite_y * TILE_SIZE, TILE_SIZE, TILE_SIZE));
                renderTexture.draw(tileSprite);


            } else {
                tileSprite.setTextureRect(sf::IntRect(7 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
                renderTexture.draw(tileSprite);
            }

        }

    }
    renderTexture.display();

}
