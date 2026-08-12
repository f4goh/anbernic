#include "Player.h"

Player::Player()
    : m_x(0),
      m_y(0),
      m_direction(PlayerDirection::Down)
{
}

void Player::setPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}

void Player::setDirection(PlayerDirection direction)
{
    m_direction = direction;
}

int Player::x() const
{
    return m_x;
}

int Player::y() const
{
    return m_y;
}

PlayerDirection Player::direction() const
{
    return m_direction;
}

uint8_t Player::tile() const
{
    switch (m_direction)
    {
        case PlayerDirection::Up:
            return 24;

        case PlayerDirection::Down:
            return 17;

        case PlayerDirection::Left:
            return 25;

        case PlayerDirection::Right:
            return 16;
    }

    return 17;
}
