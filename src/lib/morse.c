#include "morse.h"
#include "../hw.h"

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
    switch (c) {
#define DEFINE_MORSE_CHAR(c, c_codes, c_codes_size)\
        case c:                     \
            _morse_blink_for(c_codes, c_codes_size);  \
            break;
        DEFINE_MORSE_CHAR('a', { 0, 1       }, 2);
        DEFINE_MORSE_CHAR('b', { 1, 0, 0, 0 }, 4);
        DEFINE_MORSE_CHAR('c', { 1, 0, 1, 0 }, 4);
        DEFINE_MORSE_CHAR('d', { 1, 0, 0    }, 3);
        DEFINE_MORSE_CHAR('e', { 0          }, 1);
        DEFINE_MORSE_CHAR('f', { 0, 0, 1, 0 }, 4);
        DEFINE_MORSE_CHAR('g', { 1, 1, 0    }, 3);
        DEFINE_MORSE_CHAR('h', { 0, 0, 0, 0 }, 4);
        DEFINE_MORSE_CHAR('i', { 0, 0       }, 2);
        DEFINE_MORSE_CHAR('j', { 0, 1, 1, 1 }, 4);
        DEFINE_MORSE_CHAR('k', { 1, 0, 1    }, 3);
        DEFINE_MORSE_CHAR('l', { 0, 1, 0, 0 }, 4);
        DEFINE_MORSE_CHAR('m', { 1, 1       }, 2);
        DEFINE_MORSE_CHAR('n', { 1, 0       }, 2);
        DEFINE_MORSE_CHAR('o', { 1, 1, 1    }, 3);
        DEFINE_MORSE_CHAR('p', { 0, 1, 1, 0 }, 4);
        DEFINE_MORSE_CHAR('q', { 1, 1, 0, 1 }, 4);
        DEFINE_MORSE_CHAR('r', { 0, 1, 0    }, 3);
        DEFINE_MORSE_CHAR('s', { 0, 0, 0    }, 3);
        DEFINE_MORSE_CHAR('t', { 1          }, 1);
        DEFINE_MORSE_CHAR('u', { 0, 0, 1    }, 3);
        DEFINE_MORSE_CHAR('v', { 0, 0, 0, 1 }, 4);
        DEFINE_MORSE_CHAR('w', { 0, 1, 1    }, 3);
        DEFINE_MORSE_CHAR('x', { 0, 1, 1, 0 }, 4);
        DEFINE_MORSE_CHAR('y', { 1, 0, 1, 1 }, 4);
        DEFINE_MORSE_CHAR('z', { 1, 1, 0, 0 }, 4);
#undef DEFINE_MORSE_CHAR
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
