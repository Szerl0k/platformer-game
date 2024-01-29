#pragma once
#include <iostream>
#include <vector>


#include "Global.hpp"
#include "MapManager.hpp"

#include "fmt/core.h"
#include "SFML/Graphics.hpp"

// Klasa jest inspirowana kodem z https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/MapManager.hpp
// Główną inspiracją jest tutaj logika kolizji mapy i częściowo logika rysowania mapy.
// Pozostałe pola i metody, są wyłącznie elementami niezbędnymi do poprawnego działania tej logiki.
class MapManager{

    Map map;

    sf::Image mapSketch;

    sf::Sprite tileSprite;

    sf::Texture mapTexture;

public:
    MapManager();

    int getMapSketchHeight();

    int getMapSketchWidth();

    int getMapWidth() const;

    sf::Color getMapSketchPixel(int x, int y);

    void loadMapSketch(const int currentLevel);

    void update();

    void setMapCell(int i, int j, const TerrainTile& tile);

    void setMapSize(const int newSize);

    /*
     * Obie metody mapCollision są zaciągnięte z: https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/MapManager.hpp
     * Logika przestawiona w tych metodach jest taka sama jak u źródła.
     * Krótkie wyjaśnienie logiki jest w komentarzu w pliku MapManager.cpp
     */

    std::vector<int> mapCollision(const std::vector<TerrainTile>& tilesToCheck, const sf::FloatRect& hitbox) const;

    std::vector<int> mapCollision(const std::vector<TerrainTile>& tilesToCheck, std::vector<sf::Vector2i>& tilesToDelete, const sf::FloatRect& hitbox) const;


    /*
     * Metoda drawMap jest inspirowana z: https://github.com/Kofybrek/Super-Mario-Bros/blob/Main/Source/Headers/MapManager.hpp
     * Mimo, że kod w niej zawarty różni się od tego co u źródła, to jest on inspirowany.
     * Został on zmieniony pod kątem tego co jest wykorzystywane w moim projekcie.
     */
    void drawMap(sf::RenderTexture& renderTexture, const unsigned view_x);



};


