#ifndef _GAME_H_
#define _GAME_H_
// nu vrem warning-uri precum sprintf este unsafe, desi e o idee proasta, dam disable la warning-uri pt functii unsafe
#define _CRT_SECURE_NO_WARNINGS
// TODO: de gasit solutie cu script batch pt VS Code, desi ar fi util debugger-ul la o aplicatie de genul
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdint>
#include "GameObject.h"



constexpr int BOARD_SIZE_X = 8;
constexpr int BOARD_SIZE_Y = 9;


class Game {
private:
    sf::RenderWindow window;

    // tot ce desenam pe obiectul window retinem in memorie
    std::vector<GameTile> tiles;
    GameObject backgroundImage;
    GameObject scoreBar;
    // ar fi bine sa alocam dinamic obiecte foarte mari precum tabla de joc!!
    GameTile board[BOARD_SIZE_X][BOARD_SIZE_Y];
    // variabile interne ale clasei care tin cont de starea jocului
    bool shouldUpdateState;
    bool isMovingAnimationOn;
    bool isSwapping;
    int level;
    int numClicks;
    uint64_t score;
    sf::IntRect boardRect;
    // pozitiile unde a dat utilizatorul click-ul
    int x0, y0;
    int x1, y1;
    sf::Vector2i mousePos;
    // trebuie sa definim si ceva care tine cont de cand s-a terminat nivelul
    // un prag de scor, un numar de bomboane distruse, depinde, vedem ulterior

public:
    Game(unsigned int posX, unsigned int posY, unsigned int width, unsigned int height, std::string title);
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
    void DrawBoard(void);
    void ClearWindow(void);

    // adaugate astazi 
    bool IsCollisionOnCreation(GameTile& tile, int i, int j) const;
    bool CheckCombo(GameTile& tile, int x1, int y1, int x0, int y0) const;
    static std::string EventTypeToString(sf::Event::EventType& eventType);

    void MouseClickUpdateCallback(void);
    void MatchFinding(void);
    void MovingAnimationOnSwap(void);
    void DeletingAnimation(void);
    void CheckNoMatch(void);
    void UpdateBoard(void);
};

#endif /* _GAME_H_ */