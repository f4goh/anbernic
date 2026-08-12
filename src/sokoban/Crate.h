#ifndef CRATE_H
#define CRATE_H

#include <cstdint>

class Crate
{
public:
    Crate(int x, int y, uint8_t tile);

    void setPosition(int x, int y);

    int x() const;
    int y() const;

    uint8_t tile() const;

private:
    int m_x;
    int m_y;

    uint8_t m_tile;
};

#endif
