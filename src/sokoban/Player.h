#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>

enum class PlayerDirection
{
    Up,
    Down,
    Left,
    Right
};

class Player
{
public:
    Player();

    void setPosition(int x, int y);
    void setDirection(PlayerDirection direction);

    int x() const;
    int y() const;

    PlayerDirection direction() const;

    uint8_t tile() const;

private:
    int m_x;
    int m_y;

    PlayerDirection m_direction;
};

#endif
