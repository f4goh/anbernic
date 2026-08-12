#include "Level.h"


Level::Level()
    : m_width(0),
      m_height(0),
      m_originX(0),
      m_originY(0),
      m_startPlayerX(0),
      m_startPlayerY(0)
{
}


/*
 * ============================================================
 * CHARGEMENT DU NIVEAU
 * ============================================================
 */

bool Level::load(
    const LevelDefinition& definition)
{
    if (definition.tiles == nullptr)
        return false;

    if (definition.width <= 0 ||
        definition.height <= 0)
    {
        return false;
    }


    m_width = definition.width;
    m_height = definition.height;

    m_originX = definition.originX;
    m_originY = definition.originY;

    m_startPlayerX = definition.playerX;
    m_startPlayerY = definition.playerY;


    /*
     * Copie du tableau.
     */
    m_tiles.assign(
        definition.tiles,
        definition.tiles +
        m_width * m_height
    );


    /*
     * Suppression des anciennes caisses.
     */
    m_crates.clear();

    m_startCratePositions.clear();


    /*
     * Position initiale du joueur.
     */
    m_player.setPosition(
        m_startPlayerX,
        m_startPlayerY
    );

    m_player.setDirection(
        PlayerDirection::Down
    );


    /*
     * Recherche des caisses.
     */
    createCratesFromTiles();


    return true;
}


/*
 * ============================================================
 * RESET
 * ============================================================
 */

void Level::reset()
{
    /*
     * Joueur.
     */
    m_player.setPosition(
        m_startPlayerX,
        m_startPlayerY
    );

    m_player.setDirection(
        PlayerDirection::Down
    );


    /*
     * Caisses.
     */
    m_crates.clear();


    for (const auto& position :
         m_startCratePositions)
    {
        m_crates.emplace_back(
            position.first,
            position.second,
            20
        );
    }
}


/*
 * ============================================================
 * DIMENSIONS
 * ============================================================
 */

int Level::width() const
{
    return m_width;
}


int Level::height() const
{
    return m_height;
}


/*
 * ============================================================
 * ORIGINE
 * ============================================================
 */

int Level::originX() const
{
    return m_originX;
}


int Level::originY() const
{
    return m_originY;
}


/*
 * ============================================================
 * VERIFICATION DES LIMITES
 * ============================================================
 */

bool Level::inside(
    int x,
    int y) const
{
    return
        x >= 0 &&
        x < m_width &&
        y >= 0 &&
        y < m_height;
}


/*
 * ============================================================
 * TILE
 * ============================================================
 */

uint8_t Level::tile(
    int x,
    int y) const
{
    if (!inside(x, y))
        return 255;

    return m_tiles[
        y * m_width + x
    ];
}


/*
 * ============================================================
 * MUR
 * ============================================================
 */

bool Level::isWall(
    int x,
    int y) const
{
    const uint8_t t =
        tile(x, y);


    switch (t)
    {
        case 0:
        case 1:
        case 2:
        case 7:
        case 8:
        case 10:
        case 11:
        case 13:
        case 15:
        case 16:
        case 18:
        case 24:

            return true;

        default:

            return false;
    }
}


/*
 * ============================================================
 * DESTINATION
 * ============================================================
 */

bool Level::isTarget(
    int x,
    int y) const
{
    const uint8_t t =
        tile(x, y);

    return
        t == 3 ||
        t == 4;
}


/*
 * ============================================================
 * CASE ACCESSIBLE
 * ============================================================
 */

bool Level::isWalkable(
    int x,
    int y) const
{
    if (!inside(x, y))
        return false;


    /*
     * 255 = extérieur du niveau.
     */
    if (tile(x, y) == 255)
        return false;


    if (isWall(x, y))
        return false;


    return true;
}


/*
 * ============================================================
 * CAISSES
 * ============================================================
 */

bool Level::hasCrate(
    int x,
    int y) const
{
    return crateAt(x, y) != nullptr;
}


Crate* Level::crateAt(
    int x,
    int y)
{
    for (Crate& crate : m_crates)
    {
        if (crate.x() == x &&
            crate.y() == y)
        {
            return &crate;
        }
    }

    return nullptr;
}


const Crate* Level::crateAt(
    int x,
    int y) const
{
    for (const Crate& crate : m_crates)
    {
        if (crate.x() == x &&
            crate.y() == y)
        {
            return &crate;
        }
    }

    return nullptr;
}


std::vector<Crate>& Level::crates()
{
    return m_crates;
}


const std::vector<Crate>& Level::crates() const
{
    return m_crates;
}


/*
 * ============================================================
 * JOUEUR
 * ============================================================
 */

Player& Level::player()
{
    return m_player;
}


const Player& Level::player() const
{
    return m_player;
}


/*
 * ============================================================
 * CREATION DES CAISSES
 * ============================================================
 */

void Level::createCratesFromTiles()
{
    for (int y = 0;
         y < m_height;
         ++y)
    {
        for (int x = 0;
             x < m_width;
             ++x)
        {
            uint8_t t =
                m_tiles[
                    y * m_width + x
                ];


            /*
             * 20 à 23 = caisses.
             */

            if (t >= 20 &&
                t <= 23)
            {
                /*
                 * Mémorise la position
                 * initiale.
                 */

                m_startCratePositions.emplace_back(
                    x,
                    y
                );


                /*
                 * Création de la caisse.
                 *
                 * On conserve le tile d'origine
                 * 20/21/22/23.
                 */

                m_crates.emplace_back(
                    x,
                    y,
                    t
                );


                /*
                 * Sous la caisse :
                 *
                 * herbe = tile 5
                 */

                m_tiles[
                    y * m_width + x
                ] = 5;
            }
        }
    }
}


/*
 * ============================================================
 * DEPLACEMENT DU JOUEUR
 * ============================================================
 */

MoveResult Level::movePlayer(
    int dx,
    int dy)
{
    if (dx == 0 &&
        dy == 0)
    {
        return MoveResult::Blocked;
    }


    /*
     * Direction graphique.
     */

    if (dx > 0)
    {
        m_player.setDirection(
            PlayerDirection::Right
        );
    }
    else if (dx < 0)
    {
        m_player.setDirection(
            PlayerDirection::Left
        );
    }
    else if (dy > 0)
    {
        m_player.setDirection(
            PlayerDirection::Down
        );
    }
    else
    {
        m_player.setDirection(
            PlayerDirection::Up
        );
    }


    /*
     * Nouvelle position du joueur.
     */

    const int newX =
        m_player.x() + dx;

    const int newY =
        m_player.y() + dy;


    /*
     * Mur ou extérieur.
     */

    if (!isWalkable(newX, newY))
    {
        return MoveResult::Blocked;
    }


    /*
     * Recherche d'une caisse.
     */

    Crate* crate =
        crateAt(newX, newY);


    /*
     * Pas de caisse :
     * déplacement normal.
     */

    if (crate == nullptr)
    {
        m_player.setPosition(
            newX,
            newY
        );

        return MoveResult::Moved;
    }


    /*
     * Une caisse est devant le joueur.
     */

    const int crateX =
        newX + dx;

    const int crateY =
        newY + dy;


    /*
     * La caisse ne peut pas entrer
     * dans un mur.
     */

    if (!isWalkable(
            crateX,
            crateY))
    {
        return MoveResult::Blocked;
    }


    /*
     * Deux caisses ne peuvent pas
     * être poussées l'une contre l'autre.
     */

    if (hasCrate(
            crateX,
            crateY))
    {
        return MoveResult::Blocked;
    }


    /*
     * Pousse la caisse.
     */

    crate->setPosition(
        crateX,
        crateY
    );


    /*
     * Déplace le joueur.
     */

    m_player.setPosition(
        newX,
        newY
    );


    return MoveResult::Pushed;
}


/*
 * ============================================================
 * NIVEAU TERMINE
 * ============================================================
 */

bool Level::isCompleted() const
{
    if (m_crates.empty())
        return false;


    for (const Crate& crate :
         m_crates)
    {
        if (!isTarget(
                crate.x(),
                crate.y()))
        {
            return false;
        }
    }


    return true;
}
