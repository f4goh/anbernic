#ifndef INPUT_H
#define INPUT_H

#include <cstdint>

class Input
{
public:
    Input();
    ~Input();

    bool init();
    void update();
    void shutdown();

    bool quitRequested() const;

    int moveX() const;
    int moveY() const;

    bool movePressed();

    // Nouveau : bouton A Anbernic
    bool buttonA() const;
    bool buttonB() const;

    // Redémarrage du niveau 
    // PC : RETURN 
    // Anbernic : B 
   bool restartRequested() const;
   bool confirmPressed() const;
   bool levelSelectRequested() const;
   bool buttonY() const;
   bool nextMusicRequested() const;
   bool previousMusicRequested() const;


private:
    bool m_quit;

    int m_moveX;
    int m_moveY;

    bool m_previousMove;

    bool m_buttonA;
    bool m_buttonB;

    bool m_restart;
    bool m_confirm;
    bool m_levelSelect;
    bool m_buttonY;
    bool m_nextMusic;
    bool m_previousMusic;

};

#endif
