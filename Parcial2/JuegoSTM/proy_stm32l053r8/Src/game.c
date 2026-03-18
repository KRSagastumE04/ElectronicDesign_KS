#include "game.h"

volatile GameState_t game_state = GAME_IDLE;
volatile GameMove_t p1_move = MOVE_NONE;
volatile GameMove_t p2_move = MOVE_NONE;
volatile GameResult_t game_result = RESULT_NONE;

static uint32_t show_result_timer = 0;

void game_init(void) { game_reset(); }

void game_reset(void) {
    game_state = GAME_IDLE;
    p1_move = MOVE_NONE;
    p2_move = MOVE_NONE;
    game_result = RESULT_NONE;
}

/* AQUÍ ESTÁ LA LÓGICA QUE EL COMPILADOR NO ENCONTRABA */
void game_button_start(void) {
    if (game_state == GAME_IDLE || game_state == GAME_SHOW_RESULT) {
        game_state = GAME_WAIT_P1;
        p1_move = MOVE_NONE;
        p2_move = MOVE_NONE;
        game_result = RESULT_NONE;
    }
}

void game_button_pressed(GameMove_t m) {
    if (game_state == GAME_WAIT_P1) {
        p1_move = m;
        game_state = GAME_WAIT_P2;
    }
    else if (game_state == GAME_WAIT_P2) {
        p2_move = m;
        game_state = GAME_CALC_RESULT;
    }
}

void game_fsm_update(void) {
    if (game_state == GAME_CALC_RESULT) {
        if (p1_move == p2_move) {
            game_result = RESULT_TIE;
        } else {
            uint8_t p1_wins = 0;
            switch(p1_move) {
                case MOVE_ROCK:     if (p2_move == MOVE_SCISSORS || p2_move == MOVE_LIZARD) p1_wins = 1; break;
                case MOVE_PAPER:    if (p2_move == MOVE_ROCK || p2_move == MOVE_SPOCK) p1_wins = 1; break;
                case MOVE_SCISSORS: if (p2_move == MOVE_PAPER || p2_move == MOVE_LIZARD) p1_wins = 1; break;
                case MOVE_LIZARD:   if (p2_move == MOVE_SPOCK || p2_move == MOVE_PAPER) p1_wins = 1; break;
                case MOVE_SPOCK:    if (p2_move == MOVE_SCISSORS || p2_move == MOVE_ROCK) p1_wins = 1; break;
                default: break;
            }
            game_result = p1_wins ? RESULT_P1_WINS : RESULT_P2_WINS;
        }
        game_state = GAME_SHOW_RESULT;
        show_result_timer = 0;
    }
    else if (game_state == GAME_SHOW_RESULT) {
        show_result_timer++;
        if (show_result_timer > 3000) {
            // Se queda mostrando el resultado
        }
    }
}

const char* game_move_str(GameMove_t m) {
    switch(m) {
        case MOVE_ROCK: return "RO"; case MOVE_PAPER: return "PA";
        case MOVE_SCISSORS: return "SC"; case MOVE_LIZARD: return "LI";
        case MOVE_SPOCK: return "SP"; default: return "--";
    }
}

const char* game_result_str(GameResult_t r) {
    switch(r) {
        case RESULT_TIE: return "EMP"; case RESULT_P1_WINS: return "P1!";
        case RESULT_P2_WINS: return "P2!"; default: return "---";
    }
}
