#ifndef _GAME_H_
#define _GAME_H_

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

constexpr int WINDOW_POS_X = 360;
constexpr int WINDOW_POS_Y = 50;

constexpr int BOARD_SIZE_X = 8;
constexpr int BOARD_SIZE_Y = 9;

class Game {
private:
    struct GameObject {
            sf::Sprite sprite;
            sf::Texture texture;
            sf::Vector2f position;
            sf::Vector2f scale;
    };

private:
    sf::RenderWindow window;

    std::vector<GameObject> tiles;

    GameObject backgroundImage;
    GameObject scoreBar;
    GameObject board[BOARD_SIZE_X][BOARD_SIZE_Y];

    bool shouldUpdateState;
    int level;
public:
    Game(unsigned int width, unsigned int height, std::string title);
    void Play(void);
private:
    void HandleEvents(void);
    void UpdateGameState(void);
    void SetupBoard(void);
    void Render(void);
    void DrawBackground(void);
    void LoadAssets(void);
    void LoadGameObjectFromFile(const std::string& filePath, GameObject& gameObject);
    void DisplayWindow(void);
    void DisplayBoard(void);
    void ClearWindow(void);
    static std::string EventTypeToString(sf::Event::EventType& eventType);
};

#endif /* _GAME_H_ */