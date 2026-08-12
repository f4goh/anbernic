#ifndef LEVEL_H
#define LEVEL_H

#include <cstdint>
#include <vector>
#include <utility>

#include "Player.h"
#include "Crate.h"


/*
 * Résultat d'une tentative de déplacement.
 */
enum class MoveResult
{
    Blocked,
    Moved,
    Pushed
};


/*
 * Définition d'un niveau.
 *
 * width / height :
 * taille réelle du tableau
 *
 * tiles :
 * tableau des tiles
 *
 * originX / originY :
 * position du niveau à l'écran, en pixels
 *
 * playerX / playerY :
 * position initiale du joueur dans le tableau
 */
struct LevelDefinition
{
    int width;
    int height;

    const uint8_t* tiles;

    int originX;
    int originY;

    int playerX;
    int playerY;
};


class Level
{
public:

    Level();

    bool load(
        const LevelDefinition& definition
    );

    void reset();


    // -------------------------------------------------
    // Dimensions
    // -------------------------------------------------

    int width() const;
    int height() const;


    // -------------------------------------------------
    // Position du niveau
    // -------------------------------------------------

    int originX() const;
    int originY() const;


    // -------------------------------------------------
    // Tiles
    // -------------------------------------------------

    uint8_t tile(
        int x,
        int y
    ) const;

    bool isWall(
        int x,
        int y
    ) const;

    bool isWalkable(
        int x,
        int y
    ) const;

    bool isTarget(
        int x,
        int y
    ) const;


    // -------------------------------------------------
    // Caisses
    // -------------------------------------------------

    bool hasCrate(
        int x,
        int y
    ) const;

    Crate* crateAt(
        int x,
        int y
    );

    const Crate* crateAt(
        int x,
        int y
    ) const;

    std::vector<Crate>& crates();

    const std::vector<Crate>& crates() const;


    // -------------------------------------------------
    // Joueur
    // -------------------------------------------------

    Player& player();

    const Player& player() const;


    // -------------------------------------------------
    // Déplacement
    // -------------------------------------------------

    MoveResult movePlayer(
        int dx,
        int dy
    );


    // -------------------------------------------------
    // Fin de niveau
    // -------------------------------------------------

    bool isCompleted() const;


private:

    int m_width;
    int m_height;

    int m_originX;
    int m_originY;


    /*
     * Décor du niveau.
     *
     * Les caisses ne sont pas conservées
     * dans ce tableau : elles deviennent
     * des objets Crate.
     */
    std::vector<uint8_t> m_tiles;


    /*
     * Caisses dynamiques.
     */
    std::vector<Crate> m_crates;


    /*
     * Joueur.
     */
    Player m_player;


    /*
     * Position initiale du joueur.
     */
    int m_startPlayerX;
    int m_startPlayerY;


    /*
     * Positions initiales des caisses.
     */
    std::vector<
        std::pair<int, int>
    > m_startCratePositions;


    bool inside(
        int x,
        int y
    ) const;

    void createCratesFromTiles();
};

#endif
