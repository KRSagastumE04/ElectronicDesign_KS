#include <stdint.h>
#include <stdio.h>
#include "stm32l053xx.h"
#include "game.h"

/* =========================================================
   ESTRUCTURAS Y VARIABLES
   ========================================================= */
struct time {
    uint8_t seg_u, seg_d;
    uint8_t min_u, min_d;
    uint8_t hor_u, hor_d;
};

struct time mihora = {0, 0, 0, 0, 2, 1};
struct time alarma = {0, 0, 0, 3, 6, 0};

volatile uint8_t seven_segs_sequence = 0;
uint8_t digit_pins[8] = {5, 6, 8, 9, 0, 1, 2, 3}; // GPIOC

typedef enum {
    MODE_RUN = 0, MODE_SET_TIME_HOUR, MODE_SET_TIME_MIN,
    MODE_SET_ALARM_HOUR, MODE_SET_ALARM_MIN, MODE_GAME
} Mode_t;

volatile Mode_t mode = MODE_RUN;

volatile uint8_t alarm_enabled      = 1;
volatile uint8_t alarm_latched      = 0;
volatile uint8_t alarm_ringing      = 0;
volatile uint8_t alarm_ring_seconds = 0;

volatile uint32_t blink_div = 0;
volatile uint8_t update_display_flag = 0;
volatile uint32_t terminal_refresh_counter = 0;

/* =========================================================
   PROTOTIPOS
   ========================================================= */
uint8_t decoder(uint8_t v);
void    buzzer_on(void);
void    buzzer_off(void);
void    update_time_increment(struct time *t);
uint8_t get_hours(struct time *t);
uint8_t get_minutes(struct time *t);
void    set_hours(struct time *t, uint8_t h);
void    set_minutes(struct time *t, uint8_t m);
void    inc_hours(struct time *t);
void    dec_hours(struct time *t);
void    inc_minutes(struct time *t);
void    dec_minutes(struct time *t);
uint8_t times_equal_hm(struct time *a, struct time *b);
void    process_serial_key(char k);
void    terminal_app_update(void);

/* =========================================================
   SOLUCIÓN DE PRINTF PARA STM32 (FORZAR SALIDA USART)
   ========================================================= */
int _write(int file, char *ptr, int len) {
    int i;
    for (i = 0; i < len; i++) {
        while (!(USART2->ISR & (1 << 7))); // Esperar a que el buffer TX este vacio
        USART2->TDR = (ptr[i] & 0xFF);
    }
    return len;
}

/* =========================================================
   DECODIFICADOR Y ZUMBADOR
   ========================================================= */
uint8_t decoder(uint8_t v) {
    uint8_t table[] = { 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, 0b01000000 };
    return (v <= 10) ? table[v] : 0x00;
}

void buzzer_on(void)  { GPIOB->ODR |=  (1u << 8); }
void buzzer_off(void) { GPIOB->ODR &= ~(1u << 8); }

/* =========================================================
   MANEJO DE TIEMPO
   ========================================================= */
void update_time_increment(struct time *t) {
    t->seg_u++;
    if (t->seg_u > 9)  { t->seg_u = 0; t->seg_d++; }
    if (t->seg_d > 5)  { t->seg_d = 0; t->min_u++; }
    if (t->min_u > 9)  { t->min_u = 0; t->min_d++; }
    if (t->min_d > 5)  { t->min_d = 0; t->hor_u++; }
    if (t->hor_u > 9)  { t->hor_u = 0; t->hor_d++; }
    if (t->hor_d == 2 && t->hor_u == 4) { t->hor_d = 0; t->hor_u = 0; }
}

uint8_t get_hours(struct time *t)   { return (uint8_t)(t->hor_d * 10 + t->hor_u); }
uint8_t get_minutes(struct time *t) { return (uint8_t)(t->min_d * 10 + t->min_u); }
void set_hours(struct time *t, uint8_t h)   { if (h > 23) h = 0; t->hor_d = h / 10; t->hor_u = h % 10; }
void set_minutes(struct time *t, uint8_t m) { if (m > 59) m = 0; t->min_d = m / 10; t->min_u = m % 10; }
void inc_hours(struct time *t) { set_hours(t, (get_hours(t) + 1) % 24); }
void dec_hours(struct time *t) { uint8_t h = get_hours(t); set_hours(t, (h == 0) ? 23 : (h - 1)); }
void inc_minutes(struct time *t) { set_minutes(t, (get_minutes(t) + 1) % 60); t->seg_d = 0; t->seg_u = 0; }
void dec_minutes(struct time *t) { uint8_t m = get_minutes(t); set_minutes(t, (m == 0) ? 59 : (m - 1)); t->seg_d = 0; t->seg_u = 0; }

uint8_t times_equal_hm(struct time *a, struct time *b) {
    return (a->hor_d == b->hor_d && a->hor_u == b->hor_u && a->min_d == b->min_d && a->min_u == b->min_u);
}

/* =========================================================
   TERMINAL APP UPDATE (PANTALLA SERIAL)
   ========================================================= */
void terminal_app_update(void)
{
    printf("\r\n\r\n========================================\r\n");

    if (mode == MODE_RUN) {
        printf(" [ RELOJ PRINCIPAL ]\r\n");
        printf(" HORA: %d%d:%d%d:%d%d\r\n", mihora.hor_d, mihora.hor_u, mihora.min_d, mihora.min_u, mihora.seg_d, mihora.seg_u);
        printf(" ALARMA: %d%d:%d%d\r\n", alarma.hor_d, alarma.hor_u, alarma.min_d, alarma.min_u);
        printf(" -> Presiona START (PA9) para JUGAR\r\n");
        printf(" -> Envia [1] o [2] por teclado PC para configurar reloj/alarma\r\n");
    }
    else if (mode == MODE_SET_TIME_HOUR || mode == MODE_SET_TIME_MIN) {
        printf(" [ CONFIGURAR RELOJ ]\r\n");
        printf(" Editando: %s\r\n", (mode == MODE_SET_TIME_HOUR) ? "HORAS" : "MINUTOS");
        printf(" HORA ACTUAL: %d%d:%d%d\r\n", mihora.hor_d, mihora.hor_u, mihora.min_d, mihora.min_u);
        printf(" Envia: [2]=Subir | [3]=Bajar | [A]=Siguiente/Guardar\r\n");
    }
    else if (mode == MODE_SET_ALARM_HOUR || mode == MODE_SET_ALARM_MIN) {
        printf(" [ CONFIGURAR ALARMA ]\r\n");
        printf(" Editando: %s\r\n", (mode == MODE_SET_ALARM_HOUR) ? "HORAS" : "MINUTOS");
        printf(" ALARMA ACTUAL: %d%d:%d%d\r\n", alarma.hor_d, alarma.hor_u, alarma.min_d, alarma.min_u);
        printf(" Envia: [2]=Subir | [3]=Bajar | [A]=Siguiente/Guardar\r\n");
    }
    else if (mode == MODE_GAME) {
        printf(" [ JUEGO POR TURNOS: P-P-T-L-S ]\r\n");
        if (game_state == GAME_IDLE) {
            printf(" ESTADO: Listo para iniciar\r\n");
            printf(" -> PULSA START (PA9) PARA EMPEZAR\r\n");
            printf(" -> Envia [D] por teclado PC para salir\r\n");
        }
        else if (game_state == GAME_WAIT_P1) {
            printf(" -> TURNO JUGADOR 1 <-\r\n");
            printf(" Por favor, presiona tu movimiento...\r\n");
        }
        else if (game_state == GAME_WAIT_P2) {
            printf(" -> TURNO JUGADOR 2 <-\r\n");
            printf(" Por favor, presiona tu movimiento...\r\n");
        }
        else if (game_state == GAME_SHOW_RESULT) {
            printf(" -------- RESULTADOS --------\r\n");
            printf(" Jugador 1 eligio: %s\r\n", game_move_str(p1_move));
            printf(" Jugador 2 eligio: %s\r\n", game_move_str(p2_move));
            printf(" ---------> GANADOR: %s <---------\r\n", game_result_str(game_result));
            printf("\r\n Pulsa START para jugar otra vez.\r\n");
        }
    }
    printf("========================================\r\n");
}

/* =========================================================
   PROCESAMIENTO TECLAS DESDE LA TERMINAL DE ARDUINO (RESPALDO)
   ========================================================= */
void process_serial_key(char k)
{
    if (k >= 'a' && k <= 'z') k -= 32; /* Convertir a mayusculas */

    if (mode == MODE_RUN) {
        if      (k == '1') { mode = MODE_SET_ALARM_HOUR; update_display_flag = 1; }
        else if (k == '2') { mode = MODE_SET_TIME_HOUR;  update_display_flag = 1; }
        else if (k == 'D') { mode = MODE_GAME; game_reset(); update_display_flag = 1; }
    }
    else if (mode == MODE_SET_TIME_HOUR) {
        if      (k == '2') inc_hours(&mihora);
        else if (k == '3') dec_hours(&mihora);
        else if (k == 'A') mode = MODE_SET_TIME_MIN;
        update_display_flag = 1;
    }
    else if (mode == MODE_SET_TIME_MIN) {
        if      (k == '2') inc_minutes(&mihora);
        else if (k == '3') dec_minutes(&mihora);
        else if (k == 'A') { mihora.seg_d = 0; mihora.seg_u = 0; mode = MODE_RUN; }
        update_display_flag = 1;
    }
    else if (mode == MODE_SET_ALARM_HOUR) {
        if      (k == '2') inc_hours(&alarma);
        else if (k == '3') dec_hours(&alarma);
        else if (k == 'A') mode = MODE_SET_ALARM_MIN;
        update_display_flag = 1;
    }
    else if (mode == MODE_SET_ALARM_MIN) {
        if      (k == '2') inc_minutes(&alarma);
        else if (k == '3') dec_minutes(&alarma);
        else if (k == 'A') mode = MODE_RUN;
        update_display_flag = 1;
    }
    else if (mode == MODE_GAME) {
        if (k == 'D') { mode = MODE_RUN; game_reset(); update_display_flag = 1; }
    }
}

/* =========================================================
   INICIALIZAR USART2 (PA2 TX, PA3 RX)
   ========================================================= */
void uart2_init(void) {
    RCC->IOPENR |= (1 << 0);       // Reloj GPIOA
    RCC->APB1ENR |= (1 << 17);     // Reloj USART2

    GPIOA->MODER &= ~((3u << (2 * 2)) | (3u << (3 * 2)));
    GPIOA->MODER |=  ((2u << (2 * 2)) | (2u << (3 * 2)));

    GPIOA->AFR[0] &= ~((0xFu << (2 * 4)) | (0xFu << (3 * 4)));
    GPIOA->AFR[0] |=  ((4u << (2 * 4)) | (4u << (3 * 4)));

    USART2->BRR = 1667; // 9600 baudios
    USART2->CR1 = (1 << 0) | (1 << 3) | (1 << 2) | (1 << 5);

    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void) {
    if (USART2->ISR & (1 << 5)) {
        char c = (char)(USART2->RDR & 0xFF);
        process_serial_key(c);
    }
}

/* =========================================================
   MAIN
   ========================================================= */
int main(void)
{
    __disable_irq();

    RCC->CR   |= (1 << 0);
    RCC->CFGR |= (1 << 0);
    RCC->IOPENR  |= (1 << 0) | (1 << 1) | (1 << 2);
    RCC->APB2ENR |= (1 << 0);

    /* Inicializar UART */
    uart2_init();

    /* Desactivar el buffering de printf (¡MUY IMPORTANTE PARA VER TEXTO!) */
    setvbuf(stdout, NULL, _IONBF, 0);

    /* ---- GPIOB: PB0-PB7 (segmentos) ---- */
    GPIOB->MODER &= ~0x0000FFFF;
    GPIOB->MODER |=  0x00005555;

    GPIOB->MODER &= ~(3u << (8 * 2)); // Buzzer
    GPIOB->MODER |=  (1u << (8 * 2));
    buzzer_off();

    /* ---- GPIOC: digitos ---- */
    for (int i = 0; i < 8; i++) {
        GPIOC->MODER &= ~(3u << (digit_pins[i] * 2));
        GPIOC->MODER |=  (1u << (digit_pins[i] * 2));
        GPIOC->ODR   |=  (1u << digit_pins[i]);
    }

    /* ---- 6 BOTONES FISICOS (PA9, PA10, PA11, PA12, PC7, PC13) ---- */
    for (int p = 9; p <= 12; p++) {
        GPIOA->MODER &= ~(3u << (p * 2));
        GPIOA->PUPDR &= ~(3u << (p * 2));
        GPIOA->PUPDR |=  (1u << (p * 2)); // Resistencias Pull-Up internas
    }

    GPIOC->MODER &= ~(3u << (7 * 2));
    GPIOC->PUPDR &= ~(3u << (7 * 2));
    GPIOC->PUPDR |=  (1u << (7 * 2));

    GPIOC->MODER &= ~(3u << (13 * 2));
    GPIOC->PUPDR &= ~(3u << (13 * 2));
    GPIOC->PUPDR |=  (1u << (13 * 2));

    /* =========================================================
       EXTI - Enrutamiento para los 6 botones
       ========================================================= */
    // PA9 a PA12
    SYSCFG->EXTICR[2] &= ~(0xFu << 4);
    SYSCFG->EXTICR[2] &= ~(0xFu << 8);
    SYSCFG->EXTICR[2] &= ~(0xFu << 12);
    SYSCFG->EXTICR[3] &= ~(0xFu << 0);

    // PC7
    SYSCFG->EXTICR[1] &= ~(0xFu << 12);
    SYSCFG->EXTICR[1] |=  (0x2u << 12);

    // PC13
    SYSCFG->EXTICR[3] &= ~(0xFu << 4);
    SYSCFG->EXTICR[3] |=  (0x2u << 4);

    /* Habilitar las mascaras EXTI 7, 9, 10, 11, 12, 13 */
    EXTI->IMR  |= (1 << 7) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13);
    EXTI->FTSR |= (1 << 7) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13);

    /* =========================================================
       TIMERS Y SYSTICK
       ========================================================= */
    SysTick->LOAD = 16000000 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 7;

    RCC->APB2ENR |= (1 << 2);
    TIM21->PSC   = 40 - 1;
    TIM21->ARR   = 100 - 1;
    TIM21->DIER |= (1 << 0);
    TIM21->CR1  |= (1 << 0);

    NVIC_SetPriority(TIM21_IRQn,    0);
    NVIC_SetPriority(SysTick_IRQn,  1);
    NVIC_SetPriority(USART2_IRQn,   2);
    NVIC_SetPriority(EXTI4_15_IRQn, 3);

    NVIC_EnableIRQ(TIM21_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn);

    game_init();
    __enable_irq();

    printf("\r\n\r\n--- SISTEMA REINICIADO Y LISTO ---\r\n");
    update_display_flag = 1;

    while (1) {
        if (update_display_flag) {
            update_display_flag = 0;
            terminal_app_update();
        }
    }
}

/* =========================================================
   RUTINAS DE INTERRUPCIÓN
   ========================================================= */
void TIM21_IRQHandler(void)
{
    if (!(TIM21->SR & (1 << 0))) return;
    TIM21->SR &= ~(1 << 0);

    if (alarm_ringing) GPIOB->ODR ^= (1u << 8);
    else buzzer_off();

    struct time *tshow = &mihora;
    if (mode == MODE_SET_ALARM_HOUR || mode == MODE_SET_ALARM_MIN) { tshow = &alarma; }

    for (int i = 0; i < 8; i++) GPIOC->ODR |= (1u << digit_pins[i]);

    uint8_t blink_off = 0;
    if (mode != MODE_RUN && mode != MODE_GAME) { blink_off = ((blink_div / 500) & 1u); }

    switch (seven_segs_sequence) {
        case 0: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(tshow->min_d); if (!((mode == MODE_SET_TIME_MIN || mode == MODE_SET_ALARM_MIN) && blink_off)) GPIOC->ODR &= ~(1u << digit_pins[0]); seven_segs_sequence++; break;
        case 1: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(tshow->min_u); if (!((mode == MODE_SET_TIME_MIN || mode == MODE_SET_ALARM_MIN) && blink_off)) GPIOC->ODR &= ~(1u << digit_pins[1]); seven_segs_sequence++; break;
        case 2: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(mihora.seg_d); GPIOC->ODR &= ~(1u << digit_pins[2]); seven_segs_sequence++; break;
        case 3: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(mihora.seg_u); GPIOC->ODR &= ~(1u << digit_pins[3]); seven_segs_sequence++; break;
        case 4: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(10); GPIOC->ODR &= ~(1u << digit_pins[4]); seven_segs_sequence++; break;
        case 5: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(10); GPIOC->ODR &= ~(1u << digit_pins[5]); seven_segs_sequence++; break;
        case 6: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(tshow->hor_d); if (!((mode == MODE_SET_TIME_HOUR || mode == MODE_SET_ALARM_HOUR) && blink_off)) GPIOC->ODR &= ~(1u << digit_pins[6]); seven_segs_sequence++; break;
        case 7: GPIOB->ODR = (GPIOB->ODR & ~0xFFu) | decoder(tshow->hor_u); if (!((mode == MODE_SET_TIME_HOUR || mode == MODE_SET_ALARM_HOUR) && blink_off)) GPIOC->ODR &= ~(1u << digit_pins[7]); seven_segs_sequence = 0; break;
        default: seven_segs_sequence = 0; break;
    }
    blink_div++;
}

void SysTick_Handler(void)
{
    if (mode != MODE_SET_TIME_HOUR && mode != MODE_SET_TIME_MIN) update_time_increment(&mihora);
    if (mode == MODE_GAME) game_fsm_update();

    if (alarm_enabled) {
        if (times_equal_hm(&mihora, &alarma) && mihora.seg_d == 0 && mihora.seg_u == 0) {
            if (!alarm_latched) { alarm_latched = 1; alarm_ringing = 1; alarm_ring_seconds = 5; }
        } else if (!times_equal_hm(&mihora, &alarma)) { alarm_latched = 0; }
        if (alarm_ringing) { if (alarm_ring_seconds > 0) alarm_ring_seconds--; else { alarm_ringing = 0; buzzer_off(); } }
    } else { alarm_latched = 0; alarm_ringing = 0; alarm_ring_seconds = 0; buzzer_off(); }

    terminal_refresh_counter++;
    if (terminal_refresh_counter >= 1000) {
        terminal_refresh_counter = 0;
        if (mode == MODE_RUN) update_display_flag = 1;
    }
}

/* =========================================================
   INTERRUPCIONES DE LOS 6 BOTONES FISICOS
   ========================================================= */
void EXTI4_15_IRQHandler(void)
{
    /* Boton START en PA9 */
    if (EXTI->PR & (1 << 9)) {
        EXTI->PR |= (1 << 9);
        if (mode == MODE_RUN) { mode = MODE_GAME; game_reset(); }
        else if (mode == MODE_GAME) { game_button_start(); } // Llama a la funcion start
        update_display_flag = 1;
    }

    GameMove_t move_pressed = MOVE_NONE;
    if (EXTI->PR & (1 << 10)) { EXTI->PR |= (1 << 10); move_pressed = MOVE_ROCK; }
    if (EXTI->PR & (1 << 11)) { EXTI->PR |= (1 << 11); move_pressed = MOVE_PAPER; }
    if (EXTI->PR & (1 << 12)) { EXTI->PR |= (1 << 12); move_pressed = MOVE_SCISSORS; }
    if (EXTI->PR & (1 << 7))  { EXTI->PR |= (1 << 7);  move_pressed = MOVE_LIZARD; }
    if (EXTI->PR & (1 << 13)) { EXTI->PR |= (1 << 13); move_pressed = MOVE_SPOCK; }

    if (move_pressed != MOVE_NONE && mode == MODE_GAME) {
        game_button_pressed(move_pressed);
        update_display_flag = 1;
    }
}
