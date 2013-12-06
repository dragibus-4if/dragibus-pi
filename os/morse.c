#include "morse.h"
#include "hw.h"

/* Code morse représenté par un booléen: 0 si court, 1 si long */
typedef unsigned short int _morse_code_t;

static void _morse_pause(_morse_code_t code) {
    time_t speed = (code) ? 1 : MORSE_LONG_SHORT_RATIO;
    for (time_t i = 0; i < MORSE_PAUSE_TIME; i += speed);
}

/* TODO rendre ça atomique */
static void _morse_blink(_morse_code_t code) {
    led_on();
    _morse_pause(code);
    led_off();
}

static void _morse_blink_for(const _morse_code_t * codes, size_t codes_size) {
    for (size_t i = 0; i < codes_size; i++) {
        _morse_blink(codes[i]);
        _morse_pause(0);
    }
}

void morse_blink_char(char c) {
    if (c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
    }

    switch (c) {
#define PROTECT_P99(...) __VA_ARGS__
#define DEFINE_MORSE_CHAR(c, c_codes, c_codes_size)     \
        case c:                                         \
            _morse_blink_for(c_codes, c_codes_size);    \
            break;
        DEFINE_MORSE_CHAR('a', PROTECT_P99((_morse_code_t[2]){ 0, 1       }), 2);
        DEFINE_MORSE_CHAR('b', PROTECT_P99((_morse_code_t[4]){ 1, 0, 0, 0 }), 4);
        DEFINE_MORSE_CHAR('c', PROTECT_P99((_morse_code_t[4]){ 1, 0, 1, 0 }), 4);
        DEFINE_MORSE_CHAR('d', PROTECT_P99((_morse_code_t[3]){ 1, 0, 0    }), 3);
        DEFINE_MORSE_CHAR('e', PROTECT_P99((_morse_code_t[1]){ 0          }), 1);
        DEFINE_MORSE_CHAR('f', PROTECT_P99((_morse_code_t[4]){ 0, 0, 1, 0 }), 4);
        DEFINE_MORSE_CHAR('g', PROTECT_P99((_morse_code_t[3]){ 1, 1, 0    }), 3);
        DEFINE_MORSE_CHAR('h', PROTECT_P99((_morse_code_t[4]){ 0, 0, 0, 0 }), 4);
        DEFINE_MORSE_CHAR('i', PROTECT_P99((_morse_code_t[2]){ 0, 0       }), 2);
        DEFINE_MORSE_CHAR('j', PROTECT_P99((_morse_code_t[4]){ 0, 1, 1, 1 }), 4);
        DEFINE_MORSE_CHAR('k', PROTECT_P99((_morse_code_t[3]){ 1, 0, 1    }), 3);
        DEFINE_MORSE_CHAR('l', PROTECT_P99((_morse_code_t[4]){ 0, 1, 0, 0 }), 4);
        DEFINE_MORSE_CHAR('m', PROTECT_P99((_morse_code_t[2]){ 1, 1       }), 2);
        DEFINE_MORSE_CHAR('n', PROTECT_P99((_morse_code_t[2]){ 1, 0       }), 2);
        DEFINE_MORSE_CHAR('o', PROTECT_P99((_morse_code_t[3]){ 1, 1, 1    }), 3);
        DEFINE_MORSE_CHAR('p', PROTECT_P99((_morse_code_t[4]){ 0, 1, 1, 0 }), 4);
        DEFINE_MORSE_CHAR('q', PROTECT_P99((_morse_code_t[4]){ 1, 1, 0, 1 }), 4);
        DEFINE_MORSE_CHAR('r', PROTECT_P99((_morse_code_t[3]){ 0, 1, 0    }), 3);
        DEFINE_MORSE_CHAR('s', PROTECT_P99((_morse_code_t[3]){ 0, 0, 0    }), 3);
        DEFINE_MORSE_CHAR('t', PROTECT_P99((_morse_code_t[1]){ 1          }), 1);
        DEFINE_MORSE_CHAR('u', PROTECT_P99((_morse_code_t[3]){ 0, 0, 1    }), 3);
        DEFINE_MORSE_CHAR('v', PROTECT_P99((_morse_code_t[4]){ 0, 0, 0, 1 }), 4);
        DEFINE_MORSE_CHAR('w', PROTECT_P99((_morse_code_t[3]){ 0, 1, 1    }), 3);
        DEFINE_MORSE_CHAR('x', PROTECT_P99((_morse_code_t[4]){ 0, 1, 1, 0 }), 4);
        DEFINE_MORSE_CHAR('y', PROTECT_P99((_morse_code_t[4]){ 1, 0, 1, 1 }), 4);
        DEFINE_MORSE_CHAR('z', PROTECT_P99((_morse_code_t[4]){ 1, 1, 0, 0 }), 4);
#undef DEFINE_MORSE_CHAR
#undef PROTECT_P99
        default:
            break;
    }
}

void morse_blink_buffer(const char * buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        morse_blink_char(buffer[i]);
        _morse_pause(1);
    }
}
