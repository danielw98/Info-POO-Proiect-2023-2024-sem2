#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_
#include <SFML/Graphics.hpp>
#include "PieceType.h"
#include "PieceModifierType.h"

struct GameObject {
    sf::Sprite sprite;
    sf::Texture* texture;
    
    GameObject() : texture{ nullptr } {}
    GameObject(sf::Sprite& sprite_, sf::Texture *texture_) 
        : sprite{ sprite_ }, texture { texture_ }{}
};

struct GameTile : GameObject
{
    PieceType pieceType;
    PieceModifierType pieceModifierType;
    bool isMatch;
    bool shouldSwap;
    uint8_t alpha;
    
    GameTile(PieceType pieceType_ = PieceType::PIECE_NONE) 
        : pieceType{ pieceType_ }, pieceModifierType{PieceModifierType::PIECEMODIFIER_NONE },
        isMatch{ false }, shouldSwap{ false }, alpha{ 255 } {}

    GameTile(sf::Sprite& sprite, sf::Texture *texture, PieceType pieceType, PieceModifierType pieceModifierType, 
        bool isMatch = false, bool shouldMove = false, uint8_t alpha = 255)
        : GameObject(sprite, texture), pieceType{ pieceType }, pieceModifierType{ pieceModifierType }, 
        isMatch{ isMatch }, shouldSwap{ shouldMove }, alpha { alpha } {}
};


struct GameTilePosition {
    int x;
    int y;
    sf::Vector2f position;
    GameTilePosition(int x=0, int y=0, sf::Vector2f position=sf::Vector2f()) : x{ x }, y{ y }, position{ position } {}
};
#endif /* GAME_OBJECT_H_ */
