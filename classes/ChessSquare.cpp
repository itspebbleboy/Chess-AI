#include "ChessSquare.h"
#include "Chess.h"
#include <string>

void ChessSquare::initHolder(const ImVec2 &position, const char *spriteName, const int column, const int row)
{
    _column = column;
    _row = row;
    int odd = (column + row) % 2;
    ImVec4 color = odd ? ImVec4(0.48,0.58,0.36,1.0) : ImVec4(0.93,0.93,0.84,1.0);
    BitHolder::initHolder(position, color, spriteName);
}

bool ChessSquare::canDropBitAtPoint(Bit *newbit, const ImVec2& point)
{
    if (bit() == nullptr) {
        //std::cout <<"canDropBitAtPoint returned true (empty) for row: "<<getRow()<<" col: "<<getColumn()<<std::endl;
        return true;
    }
    //
    // xor the gametags to see if we have opposing colors
    //
    if ((bit()->gameTag()^newbit->gameTag()) >= 128) {

        return true;
    }
    //std::cout <<"canDropBitAtPoint returned false for row: "<<getRow()<<" col: "<<getColumn()<<std::endl;
    return false;
}

bool ChessSquare::dropBitAtPoint(Bit *newbit, const ImVec2& point)
{
    if (bit() == nullptr) {
        setBit(newbit);
        newbit->setParent(this);
        newbit->setPosition(getPosition());
        return true;
    }
    // we're taking a piece!
    if ((bit()->gameTag()^newbit->gameTag()) >= 128) {
        
        setBit(newbit);
        newbit->setParent(this);
        newbit->setPosition(getPosition());
        return true;
    }
    return false;
}

void ChessSquare::setMoveHighlighted(bool highlighted) {
    int odd = (_column + _row) % 2;
    _color = odd ? ImVec4(0.48,0.58,0.36,1.0) : ImVec4(0.93,0.93,0.84,1.0);
    if (highlighted) {
        _color = odd ? ImVec4(0.48,0.58,0.36,1.0) : ImVec4(0.93,0.93,0.84,1.0);
        _color = Lerp(_color, ImVec4(0.75,0.79,0.30,1.0), 0.75);
        return;
    }
    else{
        _color = Lerp(_color, odd ? ImVec4(0.48,0.58,0.36,1.0) : ImVec4(0.93,0.93,0.84,1.0), 0.75);
    }
}
