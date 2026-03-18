#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef enum {
    GAME_IDLE = 0,
    GAME_WAIT_P1,
    GAME_WAIT_P2,
    GAME_CALC_RESULT,
    GAME_SHOW_RESULT
} GameState_t;

typedef enum {
    MOVE_NONE = 0, MOVE_ROCK, MOVE_PAPER, MOVE_SCISSORS, MOVE_LIZARD, MOVE_SPOCK
} GameMove_t;

typedef enum {
    RESULT_NONE = 0, RESULT_TIE, RESULT_P1_WINS, RESULT_P2_WINS
} GameResult_t;

extern volatile GameState_t game_state;
extern volatile GameMove_t p1_move;
extern volatile GameMove_t p2_move;
extern volatile GameResult_t game_result;

void game_init(void);
void game_reset(void);
void game_fsm_update(void);

/* AQUÍ ESTÁ LA FUNCIÓN QUE FALTABA PARA EL BOTON START */
void game_button_start(void);
void game_button_pressed(GameMove_t m);

const char* game_move_str(GameMove_t m);
const char* game_result_str(GameResult_t r);

#endif
