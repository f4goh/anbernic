#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef enum
{
    INPUT_A,
    INPUT_B,
    INPUT_X,
    INPUT_Y,

    INPUT_L1,
    INPUT_R1,
    INPUT_L2,
    INPUT_R2,

    INPUT_START,
    INPUT_SELECT,
    INPUT_MENU,

    INPUT_MAX
} InputButton;

bool input_init(void);
void input_update(void);
void input_close(void);

bool input_down(InputButton button);

int input_dpad_x(void);
int input_dpad_y(void);

int input_left_x(void);
int input_left_y(void);

int input_right_x(void);
int input_right_y(void);

#endif
