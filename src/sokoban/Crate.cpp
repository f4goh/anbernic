#include "Crate.h"

Crate::Crate(int x, int y, uint8_t tile)
    : m_x(x),
      m_y(y),
      m_tile(tile)
{
}

void Crate::setPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}

int Crate::x() const
{
    return m_x;
}

int Crate::y() const
{
    return m_y;
}

uint8_t Crate::tile() const
{
    return m_tile;
}
