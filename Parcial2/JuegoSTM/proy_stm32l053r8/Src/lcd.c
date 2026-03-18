#include "stm32l053xx.h"
#include "lcd.h"

/* Mapeo de pines para el proyecto actual (Todo en GPIOB) */
#define LCD_RS_PIN 9u
#define LCD_EN_PIN 11u
#define LCD_D4_PIN 12u
#define LCD_D5_PIN 13u
#define LCD_D6_PIN 14u
#define LCD_D7_PIN 15u

volatile char lcd_buf[32];
volatile uint8_t lcd_pending = 0;
static uint8_t lcd_idx = 0;

/* Delay bloqueante seguro basado en ciclos (aprox 16MHz) */
static void delayMs(uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * 4000); i++);
}

static void lcd_set_rs(uint8_t v) {
    if (v) GPIOB->BSRR = (1u << LCD_RS_PIN);
    else   GPIOB->BSRR = (1u << (LCD_RS_PIN + 16u));
}

static void lcd_set_en(uint8_t v) {
    if (v) GPIOB->BSRR = (1u << LCD_EN_PIN);
    else   GPIOB->BSRR = (1u << (LCD_EN_PIN + 16u));
}

static void lcd_pulse_en(void) {
    lcd_set_en(1);
    for (volatile uint16_t i = 0; i < 40; i++);
    lcd_set_en(0);
    for (volatile uint16_t i = 0; i < 40; i++);
}

static void lcd_write_nibble(uint8_t nibble) {
    if (nibble & 0x01) GPIOB->BSRR = (1u << LCD_D4_PIN);
    else GPIOB->BSRR = (1u << (LCD_D4_PIN + 16u));

    if (nibble & 0x02) GPIOB->BSRR = (1u << LCD_D5_PIN);
    else GPIOB->BSRR = (1u << (LCD_D5_PIN + 16u));

    if (nibble & 0x04) GPIOB->BSRR = (1u << LCD_D6_PIN);
    else GPIOB->BSRR = (1u << (LCD_D6_PIN + 16u));

    if (nibble & 0x08) GPIOB->BSRR = (1u << LCD_D7_PIN);
    else GPIOB->BSRR = (1u << (LCD_D7_PIN + 16u));

    lcd_pulse_en();
}

static void lcd_send(uint8_t value, uint8_t rs) {
    lcd_set_rs(rs);
    lcd_write_nibble(value >> 4);
    for (volatile uint16_t i = 0; i < 40; i++);
    lcd_write_nibble(value & 0x0F);
    for (volatile uint16_t i = 0; i < 40; i++);
}

static void lcd_cmd(uint8_t cmd) {
    lcd_send(cmd, 0);
}

static void lcd_data(uint8_t ch) {
    lcd_send(ch, 1);
}

void lcd_init(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

    uint8_t pins[] = {LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN};
    for (int i = 0; i < 6; i++) {
        uint8_t p = pins[i];
        GPIOB->MODER   &= ~(3u << (p * 2));
        GPIOB->MODER   |=  (1u << (p * 2));
        GPIOB->OTYPER  &= ~(1u << p);
        GPIOB->OSPEEDR &= ~(3u << (p * 2));
        GPIOB->PUPDR   &= ~(3u << (p * 2));
    }

    /* Secuencia exacta basada en el driver del PDF */
    lcd_set_rs(0);
    lcd_set_en(0);
    delayMs(100);

    lcd_write_nibble(0x03); delayMs(10);
    lcd_write_nibble(0x03); delayMs(5);
    lcd_write_nibble(0x03); delayMs(1);
    lcd_write_nibble(0x02); delayMs(1);

    lcd_cmd(0x28); delayMs(1);
    lcd_cmd(0x08); delayMs(1);
    lcd_cmd(0x01); delayMs(5);
    lcd_cmd(0x06); delayMs(1);
    lcd_cmd(0x0C); delayMs(1);

    for (int i = 0; i < 32; i++) lcd_buf[i] = ' ';
    lcd_idx = 0;
    lcd_pending = 0;
}

void lcd_clear(void) {
    lcd_cmd(0x01);
    delayMs(5);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? 0x80 + col : 0xC0 + col;
    lcd_cmd(addr);
}

void lcd_print(const char *s) {
    while (*s) {
        lcd_data((uint8_t)*s++);
    }
}

void lcd_print_line(uint8_t row, const char *s) {
    lcd_set_cursor(row, 0);
    uint8_t i = 0;
    while (*s && i < 16) {
        lcd_data((uint8_t)*s++);
        i++;
    }
    while (i < 16) {
        lcd_data(' ');
        i++;
    }
}

/* Manejo no bloqueante corregido para evitar saltarse la primera letra */
void lcd_queue_update(const char *line1, const char *line2) {
    uint8_t i;
    for (i = 0; i < 16; i++) {
        if (line1 && line1[i] != '\0') lcd_buf[i] = line1[i];
        else                           lcd_buf[i] = ' ';
    }
    for (i = 0; i < 16; i++) {
        if (line2 && line2[i] != '\0') lcd_buf[16 + i] = line2[i];
        else                           lcd_buf[16 + i] = ' ';
    }
    lcd_idx = 0;
    lcd_pending = 1;
}

void lcd_tick(void) {
    if (!lcd_pending) return;

    if (lcd_idx == 0) {
        lcd_set_cursor(0, 0);
    }
    else if (lcd_idx <= 16) {
        lcd_data((uint8_t)lcd_buf[lcd_idx - 1]);
    }
    else if (lcd_idx == 17) {
        lcd_set_cursor(1, 0);
    }
    else if (lcd_idx <= 33) {
        lcd_data((uint8_t)lcd_buf[lcd_idx - 2]);
    }
    else {
        lcd_pending = 0;
        return;
    }

    lcd_idx++;
    if (lcd_idx > 33) {
        lcd_pending = 0;
    }
}
