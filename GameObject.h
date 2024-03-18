#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_
#include <SFML/Graphics.hpp>
#include "PieceType.h"
#include "PieceModifierType.h"

struct GameObject {
    sf::Sprite sprite;
    sf::Texture texture;
};

struct GameTile : GameObject
{
    PieceType pieceType;
    PieceModifierType pieceModifierType;
    bool isMatch;
    uint8_t alpha;
};
#endif /* GAME_OBJECT_H_ */
