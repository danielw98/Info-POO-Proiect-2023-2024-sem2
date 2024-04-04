#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_
#include <SFML/Graphics.hpp>
#include "PieceType.h"
#include "PieceModifierType.h"

struct GameObject {
    sf::Sprite sprite;
    sf::Texture texture;
    
    GameObject() = default;
    GameObject(sf::Sprite& sprite, sf::Texture& texture) : sprite{ sprite }, texture{ texture } {}
};

struct GameTile : GameObject
{
    PieceType pieceType;
    PieceModifierType pieceModifierType;
    bool isMatch;
    bool shouldMove;
    uint8_t alpha;
    
    GameTile() = default;
    GameTile(sf::Sprite& sprite, sf::Texture& texture, PieceType pieceType, PieceModifierType pieceModifierType, bool isMatch = false, bool shouldMove = false, uint8_t alpha = 255)
        : GameObject(sprite, texture), pieceType{ pieceType }, pieceModifierType{ pieceModifierType }, isMatch{ isMatch }, shouldMove{ shouldMove }, alpha { alpha } {}
};


struct GameTilePosition {
    int x;
    int y;
    sf::Vector2f position;
    GameTilePosition() = default;
    GameTilePosition(int x, int y, sf::Vector2f position) : x{x}, y{y}, position{position}{}
};
#endif /* GAME_OBJECT_H_ */
