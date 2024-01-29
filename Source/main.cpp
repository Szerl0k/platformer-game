#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <chrono>
#include "SFML/Graphics.hpp"

#include "fmt/core.h"

#include "Satyr.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "GlobalMethods.hpp"
#include "MapManager.hpp"
#include "Projectile.hpp"


/*
 *                  UWAGI OGÓLNE
 *
 * Duża część logiki kolizji, generowania mapy i innych rzeczy zostało napisane na podstawie kodu użytkownika Kofybrek na Github
 * oraz jego mini serii viedo na Youtube:
 *
 * https://github.com/Kofybrek/Super-Mario-Bros/tree/Main/Source
 *
 * https://youtu.be/7D4uoSoQsjw?si=6j-JqzONcHyFM8D8
 *
 * https://youtu.be/6SnGgsgV_GY?si=qebK0Erx8z5fPnRw
 *
 *
 * We wszystkich miejscach, gdzie używam inspirowanego kodu, pojawiają się stosowne przypisy.
 * Są wyjątki typu logika kolizji, gdzie dla każdej klasy jest ona inna (zmieniona pod konkretne zachowania).
 * Wtedy bezpośrednia adnotacja do źrodła nie będzie podana, tylko komentarz typu:  //Źródło w Enemy.hpp .
 *
 *
 */

auto main() -> int {

    //-------------------------------------------- INITIALIZE --------------------------------------------

    auto event = sf::Event();

    sf::Font courier_new;
    courier_new.loadFromFile(R"(..\Resources\cour.ttf)");


    auto textAbove = sf::Text("Hearts: 1\tCoins: 0\tMap: 1\tExp:", courier_new, 20);
    textAbove.setStyle(sf::Text::Style::Bold);
    textAbove.setPosition(0,0);


    // Używany do wyświetlania tekstu pomocnicznego, np. "Press N to shoot an arrow"
    auto helpText = sf::Text("", courier_new, 15);
    helpText.setStyle(sf::Text::Style::Bold);


    auto background = sf::Texture();
    background.loadFromFile(R"(..\Resources\Background.jpg)");
    auto backgroundSprite = sf::Sprite();
    backgroundSprite.setTexture(background);



    auto window = sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Gra");

    // Mapę przesyłam przez RenderTexture - wyjaśnienie na dole (w pętli okna - DRAW)
    auto renderTexture = sf::RenderTexture();
    
    renderTexture.create(10000, SCREEN_HEIGHT);
    renderTexture.display();
    auto mapSprite = sf::Sprite();

    window.setSize(sf::Vector2u(1920, 1080));
    window.setPosition(sf::Vector2i(0,0));
    window.setFramerateLimit(60);

    auto view = sf::View(sf::FloatRect(0,0, SCREEN_WIDTH, SCREEN_HEIGHT));

    auto satyr = Satyr(0);

    auto mapManager = MapManager();

    // Try moving it to Satyr.cpp / .hpp
    auto satyrProjectiles = std::vector<std::shared_ptr<Projectile>>();
    auto projectileCount = 0;
    auto ONE_PROJECTILE_ALLOWED = true;

    auto enemies = std::vector<std::shared_ptr<Enemy>>();

    /* W wektorze przeciwnikow umieszczamy zawsze jednego przeciwnika, tak aby przynajmniej jeden tam zawsze był
     * Jest to potrzebne później do określenia czy generowane pociski są "przyjazne" lub "nieprzyjazne"
     */

    enemies.push_back(std::make_shared<Capybara>(-40,0,1));

    auto currentLevel = 1;

    auto endgame = false;



    convertSketch(currentLevel, enemies, mapManager);


    //-------------------------------------------- INITIALIZE --------------------------------------------

    // main game loop
    while ( window.isOpen() )  {
        //-------------------------------------------- UPDATE --------------------------------------------

        unsigned view_x;


        window.clear();

        // Przechodzenie między poziomami, lub restart poziomu gdy skończą się serduszka
        if (satyr.getNextLevel() or satyr.resetLevel) {

            if (satyr.getNextLevel())
                currentLevel++;

            auto tmpCoins = int();
            auto tmpExp = int();

            if (currentLevel != 1) {
                tmpCoins = satyr.getCoins();
                tmpExp = satyr.experience;
                satyr.canUseBow = true;

                if(currentLevel == 3){
                    satyr.canUseRock = true;
                }
                if(currentLevel == 4) {
                    satyr.canUseRock = true;
                    satyr.canUseStaff = true;
                }


            } else
                tmpCoins = 0;


            // Wypełniamy ekran kolorem tła - bez tego grafika poprzedniego poziomu byłaby widoczna. Wtedy jeśli zadana nowa
            // tekstura (lub nowa tekstura z kanałem alfa = 0) by nie zakryła starej tekstury, to stara tekstura byłaby nadal widoczna.
            // Dlatego skoro nasze tło jest koloru 42;66;92, to wypełniam całe tło nim, aby w miejscach z kanałem alfa widniał ten kolor
            // a nie tekstury z poprzedniego poziomu
            renderTexture.clear(sf::Color(42, 66, 92));

            // Reset mapy i postaci
            enemies.erase(enemies.begin(), enemies.end());
            enemies.push_back(std::make_shared<Capybara>(-40,0,1));
            satyrProjectiles.erase(satyrProjectiles.begin(), satyrProjectiles.end());
            convertSketch(currentLevel, enemies, mapManager);
            satyr.reset();
            textAbove.setString("Hearts: " + std::to_string(satyr.getHearts()) + "\tCoins: " + std::to_string(satyr.getCoins()) + "\tMap: " + std::to_string(currentLevel) + "\tExp: " + std::to_string(satyr.experience));
            satyr.setCoins(tmpCoins);
            satyr.experience = tmpExp;


            fmt::println("Level successfully loaded");

            satyr.resetLevel = false;
        }


        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) window.close();

            if (satyr.canUseBow and sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {

                /* Musiałem to dodać, żeby jedno przyciśnięcie klawisza faktycznie tworzyło dokładnie jeden pocisk
                 * Bez tego, gdyby "tapnąć" klawisz, to mimo, że na ekranie widać jeden posisk, to w rzeczywistości tworzą się
                 * dwa obiekty, o tej samej pozycji na ekranie.
                 * Zauważyłem to przy debugowaniu strzał poprzez printowanie "Destroyed" gdy strzała była usuwana z wektora pocisków
                 * Wówczas printowały się dwa wiersze zamiast jednego.
                 */
                if (ONE_PROJECTILE_ALLOWED) {

                    satyrProjectiles.push_back(std::make_shared<Arrow>(ARROW_DMG * currentLevel, satyr));
                    ONE_PROJECTILE_ALLOWED = false;

                }


            }


            if (satyr.canUseRock and sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {

                if (ONE_PROJECTILE_ALLOWED) {

                    satyrProjectiles.push_back(std::make_shared<Rock>(ROCK_DMG * currentLevel, satyr));
                    ONE_PROJECTILE_ALLOWED = false;

                }

            }


            if (satyr.canUseStaff and sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {

                if (ONE_PROJECTILE_ALLOWED) {

                    satyrProjectiles.push_back(std::make_shared<StaffProj>(STAFF_DMG * currentLevel, satyr));
                    ONE_PROJECTILE_ALLOWED = false;

                }

            }

        }

        ONE_PROJECTILE_ALLOWED = true;



        view_x = std::clamp<int>(round(satyr.getX()) - 0.5f * (SCREEN_WIDTH - TILE_SIZE), 0, TILE_SIZE * mapManager.getMapWidth() - SCREEN_WIDTH);

        // Skoro view podąża za naszą postacią, tak długo jak Satyr sie znajduje się przy krańcu mapy
        // Z racji, że część tła jest teksturą wyśletlaną na ekranie, to ono (dosłownie) przesuwa się razem z poruszaniem się Satyra
        // Kod poniżej sprawia, że gdy Satyr zbliży się do określonego koordynatu, i kamera przestanie za nim podążać, to tło się zatrzyma

        backgroundSprite.setPosition(satyr.getX() - SCREEN_WIDTH /2 , 0);


        /* Tutaj tak samo jak wyżej, tylko dla tekstu na górze */
        if (satyr.getX() >= 500) {
            textAbove.setPosition(15 + satyr.getX() - SCREEN_WIDTH / 2, 0);
        }

        else {
            textAbove.setPosition(0,0);

        }


        satyr.update(mapManager, enemies);

        if (currentLevel != 5) {

            textAbove.setString(
                    "Hearts: " + std::to_string(satyr.getHearts()) +
                    "\tCoins: " + std::to_string(satyr.getCoins()) +
                    "\tMap: " + std::to_string(currentLevel) +
                    "\tExp: " + std::to_string(satyr.experience)
            );

        } else {

            textAbove.setString("\n\n\n\n\t\t\tThe End. Thank you for playing!!!");

        }

        // Aktualizowanie pocisków Satyra

        for (auto i = 0 ; i < satyrProjectiles.size() ; i++) {

            satyrProjectiles[i]->update(view_x, satyrProjectiles,enemies, mapManager, satyr, satyr);

        }

        // Aktualizowanie pocisków przeciwników
        for (auto i = 0 ; i < enemies.size() ; i++) {

            enemies[i]->update(view_x, enemies, mapManager, satyr);

        }

        // Usuwamy pociski, które mają być usunięte
        for (auto i = 0 ; i < satyrProjectiles.size() ; i++) {

            if (satyrProjectiles[i]->getDelete()) {

                satyrProjectiles.erase(satyrProjectiles.begin() + i);

            }

        }

        // Usuwanie zabitych przeciwników + experience
        for (auto i = 0 ; i < enemies.size() ; i++) {

            if (enemies[i]->getDead()) {

                enemies.erase(enemies.begin() + i);
                satyr.experience += 5 * currentLevel;

            }

        }

        //-------------------------------------------- UPDATE --------------------------------------------

        //--------------------------------------------- DRAW ---------------------------------------------

        view.reset(sf::FloatRect(view_x, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
        window.setView(view);
        window.draw(backgroundSprite);

        /* Tutaj spokałem się z uciążliwym bugiem
         * Jak mapa była wrzucona bezpośrednio do okna (RenderWindow), a nie za pośrednictwem renderTexture,
         * wówczas pokazywały się dziwne pionowe kreski gdy view podążał za Satyrem
         * Problem bardzo podobny do tego jak poniżej (Stack Overflow), ale zastosowałem rozwiązanie z dokumentacji SFML:
         * https://stackoverflow.com/questions/55997965/how-to-fix-vertical-artifact-lines-in-a-vertex-array-in-sfml-with-pixel-perfect
         *
         * https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#off-screen-drawing
         *
         * Niestety duży minus tego rozwiązania jest to, (chyba) nie da się dynamicznie przesuwać renderTexture,
         * przez co parę linijek wyżej musiałem ustawić ręcznie rozdzielczość tej tekstury.
         * Wobec tego jestem mocno ogarniczony pod względem optymalizacji generowania mapy
         * Nie wiem też, czy nie będzie później problemu z generowaniem nowych poziomów.
         *
         *      // Komentarz na koniec pisania projektu -> TAK BYŁY :)
         */

        mapManager.drawMap(renderTexture, view_x);
        mapSprite.setTexture(renderTexture.getTexture());
        window.draw(mapSprite);

        satyr.draw(window);

        for (auto& enemy : enemies) {

            enemy->draw(view_x, window);

        }

        // Wyświetlamy zdrowie przeciwników
        for (auto& enemy : enemies) {

            auto hpText = sf::Text("HP " + std::to_string(enemy->getHealth()), courier_new, 15);
            hpText.setPosition(enemy->getX()  - 5, enemy->getY() - 20);
            hpText.setStyle(sf::Text::Style::Italic);
            hpText.setOutlineThickness(.5f);
            hpText.setOutlineColor(sf::Color::Red);
            window.draw(hpText);

        }

        for (auto& proj : satyrProjectiles) {
            proj->draw(view_x, window);
        }

        window.draw(textAbove);


        // Wyświetlanie tekstów pomocniczych
        if (currentLevel == 2) {

            helpText.setString("TAP M TO THROW ROCKS!");
            helpText.setPosition(2850,200);
            window.draw(helpText);

        } else if (currentLevel == 1) {

            helpText.setString("TAP N TO USE THE BOW");
            helpText.setPosition(1900,200);
            window.draw(helpText);

        } else if (currentLevel == 4) {

            helpText.setString("TAP B TO USE THE STAFF");
            helpText.setPosition(SATYR_SPAWN_X, SATYR_SPAWN_Y - 300);
            window.draw(helpText);

        }

        window.display();

        //--------------------------------------------- DRAW ---------------------------------------------

    }

}


