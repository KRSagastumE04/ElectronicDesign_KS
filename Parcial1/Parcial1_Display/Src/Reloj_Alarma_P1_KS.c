#include <stdint.h>

/* ================= BASE ADDRESSES ================= */
#define PERIPHERAL_BASE_ADDRESS  0x40000000U
#define AHB_BASE_ADDRESS         (PERIPHERAL_BASE_ADDRESS + 0x00020000U)
#define RCC_BASE_ADDRESS         (AHB_BASE_ADDRESS + 0x00001000U)
#define IOPORT_ADDRESS           (PERIPHERAL_BASE_ADDRESS + 0x10000000U)

#define GPIOA_BASE_ADDRESS       (IOPORT_ADDRESS + 0x00000000U)
#define GPIOB_BASE_ADDRESS       (IOPORT_ADDRESS + 0x00000400U)
#define GPIOC_BASE_ADDRESS       (IOPORT_ADDRESS + 0x00000800U)

/* ================= STRUCTS ================= */
typedef struct {
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t OSPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFR[2];
    uint32_t BRR;
} GPIO_RegDef_t;

typedef struct {
    uint32_t CR;
    uint32_t ICSCR;
    uint32_t CRRCR;
    uint32_t CFGR;
    uint32_t CIER;
    uint32_t CIFR;
    uint32_t CICR;
    uint32_t IOPRSTR;
    uint32_t AHBRSTR;
    uint32_t APB2RSTR;
    uint32_t APB1RSTR;
    uint32_t IOPENR;
} RCC_RegDef_t;

#define GPIOA ((GPIO_RegDef_t*)GPIOA_BASE_ADDRESS)
#define GPIOB ((GPIO_RegDef_t*)GPIOB_BASE_ADDRESS)
#define GPIOC ((GPIO_RegDef_t*)GPIOC_BASE_ADDRESS)
#define RCC   ((RCC_RegDef_t*)RCC_BASE_ADDRESS)

/* ================= BUZZER PIN ================= */
#define BUZZER_PIN   8u   // PB8
#define BUZZER_ON()  (GPIOB->ODR |=  (1u<<BUZZER_PIN))
#define BUZZER_OFF() (GPIOB->ODR &= ~(1u<<BUZZER_PIN))

/* ================= PROTOTYPES ================= */
static void delay_ms(uint16_t n);
static uint8_t decoder(uint8_t v);

static uint8_t button_pressed_event(void);
static char keypad_getkey_press_release_4x4(void);
static void buzzer_tone_1s_2400Hz(void);

/* ================= MENU FSM ================= */
typedef enum {
    MODE_RUN = 0,
    MODE_SET_TIME_HOUR,
    MODE_SET_TIME_MIN,
    MODE_SET_ALARM_HOUR,
    MODE_SET_ALARM_MIN
} Mode_t;

/* ================= KEYPAD 4x4 PINOUT =================
   Filas (inputs pull-up): PA1, PA2, PA3, PA4
   Cols  (outputs):        PA5, PA6, PA7, PA8
*/
static const uint8_t row_pins[4] = {1,2,3,4}; // PA1..PA4
static const uint8_t col_pins[4] = {5,6,7,8}; // PA5..PA8

static const char keymap_4x4[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

/* ================= DECODER ================= */
static uint8_t decoder(uint8_t v)
{
    switch(v){
        case 0: return 0x3F;
        case 1: return 0x06;
        case 2: return 0x5B;
        case 3: return 0x4F;
        case 4: return 0x66;
        case 5: return 0x6D;
        case 6: return 0x7D;
        case 7: return 0x07;
        case 8: return 0x7F;
        case 9: return 0x6F;
        default:return 0x00;   // vacío
    }
}

/* ================= DELAY ================= */
static void delay_ms(uint16_t n)
{
    volatile uint16_t i;
    for(;n>0;n--)
        for(i=0;i<150;i++);
}

/* ================= BUTTON (PA0) ================= */
static uint8_t button_pressed_event(void)
{
    static uint8_t last = 1; // reposo=1
    uint8_t now = (GPIOA->IDR & (1u<<0)) ? 1 : 0;

    if(last==1 && now==0){
        delay_ms(20);
        now = (GPIOA->IDR & (1u<<0)) ? 1 : 0;
        if(now==0){
            last = 0;
            return 1;
        }
    }
    if(now==1) last = 1;
    return 0;
}

/* ===== helper: alguna fila presionada? ===== */
static uint8_t keypad_any_row_pressed(void)
{
    for(int r=0;r<4;r++){
        if( (GPIOA->IDR & (1u<<row_pins[r])) == 0 ) return 1;
    }
    return 0;
}

/* KEYPAD 4x4  */
static char keypad_getkey_press_release_4x4(void)
{
    for(int c=0;c<4;c++)
    {
        for(int k=0;k<4;k++) GPIOA->ODR |= (1u<<col_pins[k]);   // HIGH
        GPIOA->ODR &= ~(1u<<col_pins[c]);                      // col LOW
        delay_ms(0.1);

        for(int r=0;r<4;r++)
        {
            if( (GPIOA->IDR & (1u<<row_pins[r])) == 0 )
            {
                delay_ms(12); // debounceR & (1u<<row_pins[r])) == 0 )
                {
                    char key = keymap_4x4[r][c];

                    // esperar que suelte
                    while(keypad_any_row_pressed()){
                        delay_ms(2);
                    }
                    delay_ms(10);
                    return key;
                }
            }
        }
    }
    return 0;
}

/* Tono buzzer 2.4 kHz
*/
static void buzzer_tone_1s_2400Hz(void)
{


    for(uint16_t i = 0; i < 4800; i++)
    {
        GPIOB->ODR ^= (1u<<BUZZER_PIN);

        // delay corto (ajustable).

        for(volatile uint16_t d = 0; d < 120; d++);
    }
    BUZZER_OFF();
}

/* FUNCIÓN RELOJ + ALARMA  */
void Reloj_Alarma_P1_KS(void)
{
    /* Habilitar GPIOA + GPIOB + GPIOC */
    RCC->IOPENR |= (1<<0) | (1<<1) | (1<<2);

    /* PB0–PB7 como salida (segmentos) */
    for(int i=0;i<8;i++){
        GPIOB->MODER &= ~(3u<<(i*2));
        GPIOB->MODER |=  (1u<<(i*2));
    }

    /* PB8 como salida (BUZZER) */
    GPIOB->MODER &= ~(3u<<(BUZZER_PIN*2));
    GPIOB->MODER |=  (1u<<(BUZZER_PIN*2));
    BUZZER_OFF();

    /* Pines de los 8 dígitos (GPIOC) */
    uint8_t digit_pins[8] = {5,6,8,9,0,1,2,3};
    for(int i=0;i<8;i++){
        GPIOC->MODER &= ~(3u<<(digit_pins[i]*2));
        GPIOC->MODER |=  (1u<<(digit_pins[i]*2));
    }

    /* Botón PA0 input + pull-up */
    GPIOA->MODER &= ~(3u<<(0*2));
    GPIOA->PUPDR &= ~(3u<<(0*2));
    GPIOA->PUPDR |=  (1u<<(0*2)); // pull-up

    /* Keypad filas */
    for(int i=0;i<4;i++){
        uint8_t p = row_pins[i];
        GPIOA->MODER &= ~(3u<<(p*2));
        GPIOA->PUPDR &= ~(3u<<(p*2));
        GPIOA->PUPDR |=  (1u<<(p*2));
    }

    /*  Keypad columnas output */
    for(int i=0;i<4;i++){
        uint8_t p = col_pins[i];
        GPIOA->MODER &= ~(3u<<(p*2));
        GPIOA->MODER |=  (1u<<(p*2));
        GPIOA->ODR   |=  (1u<<p); // HIGH
    }

    /* ===== reloj ===== */
    uint8_t horas=12, minutos=0, segundos=0;

    /* ===== alarma ===== */
    uint8_t alarm_h = 6, alarm_m = 30;
    uint8_t alarm_enabled = 1;

    // latch para sonar en coimcidencia 1
    uint8_t alarm_latched = 0;

    uint8_t digits[8];
    uint8_t fsm=0;
    uint16_t refresh_counter=0;

    Mode_t mode = MODE_RUN;

    while(1)
    {
        /* Menú de hora con botón */
        if(button_pressed_event()){
            if(mode == MODE_RUN) mode = MODE_SET_TIME_HOUR;
            else if(mode == MODE_SET_TIME_HOUR) mode = MODE_SET_TIME_MIN;
            else if(mode == MODE_SET_TIME_MIN) mode = MODE_SET_TIME_HOUR;
        }

        /* ===== Keypad ===== */
        char k = keypad_getkey_press_release_4x4();

        /* ===== Entrar al menú de alarma con '1' ===== */
        if(mode == MODE_RUN && k=='1'){
            mode = MODE_SET_ALARM_HOUR;
            k = 0;
        }

        /* ===== Menús ===== */
        if(mode != MODE_RUN && k != 0)
        {
            if(k=='#'){
                if(mode==MODE_SET_TIME_HOUR) mode = MODE_SET_TIME_MIN;
                else if(mode==MODE_SET_TIME_MIN) mode = MODE_SET_TIME_HOUR;
                else if(mode==MODE_SET_ALARM_HOUR) mode = MODE_SET_ALARM_MIN;
                else if(mode==MODE_SET_ALARM_MIN) mode = MODE_SET_ALARM_HOUR;
            }
            else if(k=='*'){
                mode = MODE_RUN;   // guardar/salir
                segundos = 0;
            }
            else if(k=='2'){ // SUBIR
                if(mode==MODE_SET_TIME_HOUR) horas = (horas + 1) % 24;
                else if(mode==MODE_SET_TIME_MIN) minutos = (minutos + 1) % 60;
                else if(mode==MODE_SET_ALARM_HOUR) alarm_h = (alarm_h + 1) % 24;
                else if(mode==MODE_SET_ALARM_MIN) alarm_m = (alarm_m + 1) % 60;
            }
            else if(k=='8' || k=='0'){ // BAJAR (8 pedido; 0 extra)
                if(mode==MODE_SET_TIME_HOUR) horas = (horas==0)? 23 : (horas-1);
                else if(mode==MODE_SET_TIME_MIN) minutos = (minutos==0)? 59 : (minutos-1);
                else if(mode==MODE_SET_ALARM_HOUR) alarm_h = (alarm_h==0)? 23 : (alarm_h-1);
                else if(mode==MODE_SET_ALARM_MIN) alarm_m = (alarm_m==0)? 59 : (alarm_m-1);
            }
        }

        /* Disparo alarma
           - Suena 1 vez cuando coincide HH:MM
        */
        if(alarm_enabled)
        {
            if(horas == alarm_h && minutos == alarm_m)
            {
                if(!alarm_latched)
                {
                    alarm_latched = 1;
                    buzzer_tone_1s_2400Hz(); // <-- 1 segundo audible
                }
            }
            else
            {
                alarm_latched = 0;
            }
        }

        /* ===== Display =====
           En menú de alarma mostramos la ALARMA para confirmar cambios.
        */
        uint8_t show_h = horas, show_m = minutos;
        if(mode==MODE_SET_ALARM_HOUR || mode==MODE_SET_ALARM_MIN){
            show_h = alarm_h;
            show_m = alarm_m;
        }

        digits[0] = show_m/10;
        digits[1] = show_m%10;

        digits[2] = segundos/10;
        digits[3] = segundos%10;

        digits[4] = 10;
        digits[5] = 10;

        digits[6] = show_h/10;
        digits[7] = show_h%10;

        /* multiplexado */
        for(int i=0;i<8;i++)
            GPIOC->ODR |= (1u<<digit_pins[i]); // apagar todos

        GPIOB->ODR &= ~0xFFu;               // limpiar segmentos PB0..PB7
        GPIOB->ODR |= decoder(digits[fsm]); // escribir segmentos

        /* parpadeo */
        uint8_t blink = 0;
        if(mode != MODE_RUN){
            blink = ((refresh_counter % 20) < 10) ? 1 : 0;
        }

        if(mode == MODE_RUN){
            GPIOC->ODR &= ~(1u<<digit_pins[fsm]);
        } else {
            if((mode==MODE_SET_TIME_HOUR || mode==MODE_SET_ALARM_HOUR) && (fsm==6 || fsm==7)){
                if(!blink) GPIOC->ODR &= ~(1u<<digit_pins[fsm]);
            }
            else if((mode==MODE_SET_TIME_MIN || mode==MODE_SET_ALARM_MIN) && (fsm==0 || fsm==1)){
                if(!blink) GPIOC->ODR &= ~(1u<<digit_pins[fsm]);
            }
            else {
                GPIOC->ODR &= ~(1u<<digit_pins[fsm]);
            }
        }

        delay_ms(2);

        fsm++;
        if(fsm>=8){
            fsm=0;
            refresh_counter++;
        }

        /* ===== 1 segundo (aprox) ===== */
        if(refresh_counter >= 42){
            refresh_counter=0;

            segundos++;
            if(segundos>=60){
                segundos=0;
                minutos++;
                if(minutos>=60){
                    minutos=0;
                    horas++;
                    if(horas>=24) horas=0;
                }
            }
        }
    }
}
