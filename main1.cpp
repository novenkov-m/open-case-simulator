#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <string>

struct Skin {
    std::string name;
    std::string texturePath;
    sf::Color fallbackColor;
    float chance;
};

std::vector<sf::Texture> textures;
std::random_device rd;
std::mt19937 gen(rd());

std::vector<int> generateTape(const std::vector<Skin>& skins, int& winnerIndex, int& winnerSkinIndex) {
    std::vector<int> tape;
    const int totalItems = 100;
    
    std::uniform_real_distribution<float> chanceDist(0.f, 100.f);
    float roll = chanceDist(gen);
    float cumulative = 0.f;
    
    winnerSkinIndex = 0;
    for (size_t i = 0; i < skins.size(); ++i) {
        cumulative += skins[i].chance;
        if (roll <= cumulative) {
            winnerSkinIndex = static_cast<int>(i);
            break;
        }
    }
    
    std::uniform_int_distribution<> winnerPosDist(70, 90);
    winnerIndex = winnerPosDist(gen);
    
    std::uniform_int_distribution<> skinDist(0, skins.size() - 1);
    for (int i = 0; i < totalItems; ++i) {
        if (i == winnerIndex) {
            tape.push_back(winnerSkinIndex);
        } else {
            tape.push_back(skinDist(gen));
        }
    }
    return tape;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Case Opener SFML");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Шрифт не загружен!\n";
        return -1;
    }

    std::vector<Skin> skins = {
        {"Blue Skin 1", "assets/blue1.png",  sf::Color(0, 150, 255),  16.0f},
        {"Blue Skin 2", "assets/blue2.png",  sf::Color(0, 150, 255),  16.0f},
        {"Blue Skin 3", "assets/blue3.png",  sf::Color(0, 150, 255),  16.0f},
        
        {"Purple Skin 1", "assets/purple1.png", sf::Color(150, 50, 200), 10.0f},
        {"Purple Skin 2", "assets/purple2.png", sf::Color(150, 50, 200), 10.0f},
        {"Purple Skin 3", "assets/purple3.png", sf::Color(150, 50, 200), 10.0f},
        
        {"Pink Skin 1", "assets/pink1.png", sf::Color(255, 100, 180),  6.0f},
        {"Pink Skin 2", "assets/pink2.png", sf::Color(255, 100, 180),  6.0f},
        
        {"Gold Skin 1", "assets/gold1.png", sf::Color(255, 215, 0),   2.5f},
        {"Gold Skin 2", "assets/gold2.png", sf::Color(255, 215, 0),   2.5f},
        {"Gold Skin 3", "assets/gold3.png", sf::Color(255, 215, 0),   2.5f},
        {"Gold Skin 4", "assets/gold4.png", sf::Color(255, 215, 0),   2.5f}
    };

    // Загрузка текстур
    for (const auto& skin : skins) {
        sf::Texture tex;
        if (!tex.loadFromFile(skin.texturePath)) {
            std::cerr << "Не загружено: " << skin.texturePath << "\n";
        }
        textures.push_back(tex);
    }

    const float itemWidth = 100.f;
    const float itemGap = 10.f;
    const float itemTotal = itemWidth + itemGap;
    const int totalItems = 100;

    std::vector<int> tape;
    int winnerIndex = 0, winnerSkinIndex = 0;
    float scrollPosition = 0.f, scrollSpeed = 0.f;
    bool isSpinning = false;
    sf::Clock clock;

    // Маркер
    sf::RectangleShape marker(sf::Vector2f(4, 120));
    marker.setFillColor(sf::Color::Yellow);
    marker.setPosition(400, 240);

    // Кнопка
    sf::RectangleShape button(sf::Vector2f(200, 50));
    button.setPosition(300, 500);
    button.setFillColor(sf::Color::Green);

    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString("Open");
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setPosition(370, 510);

    sf::Text text;
    text.setFont(font);              
    text.setString("Hello, ludoman!"); 
    text.setCharacterSize(50);       
    text.setFillColor(sf::Color::Black);
    text.setPosition(200, 100); 


    auto startSpin = [&]() {
        tape = generateTape(skins, winnerIndex, winnerSkinIndex);
        isSpinning = true;
        scrollSpeed = 8000.f;
        scrollPosition = 0.f;
    };

    tape = generateTape(skins, winnerIndex, winnerSkinIndex);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && 
                event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i clickPos = sf::Mouse::getPosition(window);
                if (button.getGlobalBounds().contains(clickPos.x, clickPos.y)) {
                    if (!isSpinning) startSpin();
                }
            }
        }

        float dt = clock.restart().asSeconds();

        if (isSpinning) {
            scrollPosition += scrollSpeed * dt;
            scrollSpeed *= 0.985f;

            if (scrollSpeed < 30.f) {
                isSpinning = false;
            }
        }

        window.clear(sf::Color(255, 225, 255));

        float startX = 400 - scrollPosition;
        for (int i = 0; i < totalItems; ++i) {
            float x = startX + i * itemTotal;
            if (x < -itemWidth || x > 800) continue;

            int skinIdx = tape[i];
            
            if (textures[skinIdx].getSize().x > 0) {
                sf::Sprite sprite(textures[skinIdx]);
                sprite.setPosition(x, 240);
                sprite.setScale(itemWidth / textures[skinIdx].getSize().x, 
                               100.f / textures[skinIdx].getSize().y);
                window.draw(sprite);
            } else {
                sf::RectangleShape item(sf::Vector2f(itemWidth, 100));
                item.setPosition(x, 240);
                item.setFillColor(skins[skinIdx].fallbackColor);
                window.draw(item);
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (!isSpinning) {
            button.setFillColor(button.getGlobalBounds().contains(mousePos.x, mousePos.y) 
                               ? sf::Color(0, 220, 0) : sf::Color::Green);
        } else {
            button.setFillColor(sf::Color(100, 100, 100));
        }

        window.draw(text);
        window.draw(button);
        window.draw(buttonText);
        window.draw(marker);
        
        window.display();
    }

    return 0;
}