#ifndef _GAME_H_
#define _GAME_H_

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

constexpr int FRAMES_PER_SECOND = 60;
constexpr int WINDOW_POS_X = 400;
constexpr int WINDOW_POS_Y = 100;

class Game
{
private:
    struct GameObject {
        sf::Sprite sprite;
        sf::Texture texture;
    };

private:
    sf::RenderWindow window;
    GameObject background;
    std::vector<GameObject> tiles;

public:
    Game(unsigned int width, unsigned int height, std::string title);
    void Play(void);

private:
    void HandleEvents(void);
    void UpdateGameState(void);
    void Render(void);
    void DrawBackground(void);
    void LoadAssets(void);
    void LoadGameObjectFromFile(const std::string& filePath, GameObject& gameObject);
    void DisplayWindow(void);
    void ClearWindow(void);
    static std::string EventTypeToString(sf::Event::EventType& eventType);
};

#endif /* _GAME_H_ */