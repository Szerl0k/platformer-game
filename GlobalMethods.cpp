#include <array>
#include <vector>
#include <cmath>


#include "SFML/Graphics.hpp"

#include "Satyr.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "GlobalMethods.hpp"
#include "MapManager.hpp"
#include "Projectile.hpp"

// Kod inspirowany z https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/ConvertSketch.cpp
void convertSketch (int currentLevel, std::vector<std::shared_ptr<Enemy>>& enemies,MapManager& mapManager) {


    mapManager.loadMapSketch(currentLevel);

    mapManager.setMapSize(mapManager.getMapSketchWidth());

    auto mapHeight = floor(mapManager.getMapSketchHeight());


    for ( auto i = 0 ; i < mapManager.getMapSketchWidth() ; i++) {

        for ( auto j = 0 ; j < mapManager.getMapSketchHeight() ; j++) {

            sf::Color pixel = mapManager.getMapSketchPixel(i,j);

            if ( j < mapHeight) {

                if ( pixel == sf::Color(0,0,0) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_MIDDLE);

                else if ( pixel == sf::Color(25,25,25) ) mapManager.setMapCell(i,j, TerrainTile::GROUND_TOP_LEFT);

                else if ( pixel == sf::Color(50,50,50) ) mapManager.setMapCell(i,j, TerrainTile::GROUND_TOP);

                else if ( pixel == sf::Color(75,75,75) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_TOP_RIGHT);

                else if ( pixel == sf::Color(100,100,100) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_RIGHT);

                else if ( pixel == sf::Color(125,125,125) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_BOTTOM_RIGHT);

                else if ( pixel == sf::Color(150, 150, 150) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_BOTTOM);

                else if ( pixel == sf::Color(175,175,175) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_BOTTOM_LEFT);

                else if ( pixel == sf::Color(200, 200, 200)) mapManager.setMapCell(i, j, TerrainTile::GROUND_LEFT);

                else if ( pixel == sf::Color(0,0,200) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_PILLAR_TOP);

                else if ( pixel == sf::Color(0,0,150) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_PILLAR_MIDDLE);

                else if ( pixel == sf::Color(0, 0, 100) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_PILLAR_BOTTOM);

                else if ( pixel == sf::Color(50,50,0) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_DEEP_1);

                else if ( pixel == sf::Color(100,100,0) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_DEEP_2);

                else if ( pixel == sf::Color(200,200,0) ) mapManager.setMapCell(i, j, TerrainTile::GROUND_DEEP_3);

                else if ( pixel == sf::Color(150, 150, 0) ) mapManager.setMapCell(i, j, TerrainTile::IRREGULAR_GROUND);

                else if ( pixel == sf::Color(0,250,0) ) mapManager.setMapCell(i, j, TerrainTile::GRASS_DEFAULT);

                else if ( pixel == sf::Color(0,200,0) ) mapManager.setMapCell(i, j, TerrainTile::GRASS_DENSER);

                else if ( pixel == sf::Color(0,150,0) ) mapManager.setMapCell(i, j, TerrainTile::GRASS_ABOVE);

                else if (pixel == sf::Color(0,50,0)) mapManager.setMapCell(i,j, TerrainTile::CHECKPOINT);

                else if (pixel == sf::Color(0,100,0)) mapManager.setMapCell(i,j, TerrainTile::FINISH);

                else if (pixel == sf::Color(255, 209, 69)) mapManager.setMapCell(i,j, TerrainTile::COIN);

                else if (pixel == sf::Color(239, 58, 12)) mapManager.setMapCell(i,j, TerrainTile::HEART);

                else if (pixel == sf::Color(26,4,4)) mapManager.setMapCell(i,j, TerrainTile::BOW);

                else if (pixel == sf::Color(28,28,28)) mapManager.setMapCell(i,j, TerrainTile::ROCK);

                else if (pixel == sf::Color(27,69,108)) mapManager.setMapCell(i,j, TerrainTile::STAFF);

                // Change it, so we push_back the vector of enemies to add capybara
                else if (pixel == sf::Color(250, 0, 0)) {
                    mapManager.setMapCell(i,j, TerrainTile::CAPYBARA);
                    enemies.push_back(std::make_shared<Capybara>(i * TILE_SIZE, j * TILE_SIZE - TILE_SIZE , (SKELETON_HEALTH + currentLevel) * currentLevel));
                }

                else if (pixel == sf::Color(200, 0, 0)) {
                    mapManager.setMapCell(i,j,TerrainTile::SKELETON);
                    enemies.push_back(std::make_shared<Skeleton>(i * TILE_SIZE, j * TILE_SIZE, (SKELETON_HEALTH + currentLevel) * currentLevel, false));
                }

                else if (pixel == sf::Color(199,0,0)) {
                    mapManager.setMapCell(i,j, TerrainTile::IMMOBILE_SKELETON);
                    enemies.push_back(std::make_shared<Skeleton>(i * TILE_SIZE, j * TILE_SIZE, (SKELETON_HEALTH + currentLevel) * currentLevel, true));
                }

                else if (pixel == sf::Color(100,0,0)) {
                    mapManager.setMapCell(i,j,TerrainTile::TROLL);
                    enemies.push_back(std::make_shared<Troll>(i * TILE_SIZE, j * TILE_SIZE, (TROLL_HEALTH + currentLevel) * currentLevel,false));
                }

                else if (pixel == sf::Color(99,0,0)) {
                    mapManager.setMapCell(i,j, TerrainTile::IMMOBILE_TROLL);
                    enemies.push_back(std::make_shared<Troll>(i * TILE_SIZE, j * TILE_SIZE, (TROLL_HEALTH + currentLevel) * currentLevel, true));
                }

                else if (pixel == sf::Color(150,0,0)) {
                    mapManager.setMapCell(i,j,TerrainTile::GREY_KNIGHT);
                    enemies.push_back(std::make_shared<GreyKnight>(i * TILE_SIZE, j * TILE_SIZE, GREY_KNIGHT_HEALTH * currentLevel));
                }

                else if (pixel == sf::Color(50,0,0)) {
                    mapManager.setMapCell(i,j,TerrainTile::BLACK_KNIGHT);
                    enemies.push_back(std::make_shared<BlackKnight>(i * TILE_SIZE, j * TILE_SIZE, BLACK_KNIGHT_HEALTH * currentLevel));

                }

                else if (pixel == sf::Color(233,181,163)) {
                    mapManager.setMapCell(i,j,TerrainTile::WIZARD);
                    enemies.push_back(std::make_shared<Wizard>(i * TILE_SIZE, j * TILE_SIZE, WIZARD_HEALTH));
                }

                else mapManager.setMapCell(i, j, TerrainTile::EMPTY);
            }
            else {
                mapManager.setMapCell(i, j, TerrainTile::EMPTY);
            }

        }

    }

}
