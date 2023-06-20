#include <stdint.h>
#include <stddef.h>
#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "cpu/ports.h"
#include "cpu/isr.h"
#include "libc/string.h"
#include "libc/function.h"
#include "kernel/kernel.h"
#include "libc/mem.h"

struct key_callback key_callbacks[10];
char* key_buffer;

const char ascii[] =       {'?','?','1','2','3','4','5','6','7','8','9',
                            '0','-','=','?','?','q','w','e','r','t',
                            'y','u','i','o','p','[',']','?','?','a',
                            's','d','f','g','h','j','k','l',';','\'',
                            '`','?','\\','z','x','c','v','b','n','m',
                            ',','.','/','?','?','?',' ','?','?','?'};
const char ascii_shift[] = {'?','?','!','@','#','$','%','^','&','*','(',
                            ')','_','+','?','?','Q','W','E','R','T',
                            'Y','U','I','O','P','{','}','?','?','A',
                            'S','D','F','G','H','J','K','L',':','\"',
                            '~','?','\\','Z','X','C','V','B','N','M',
                            '<','>','?','?','?','?',' ','?','?','?'};
uint8_t keys_pressed[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0};

struct keyboard_initializer *create_initializer(char* buffer_addr,
                                               uint8_t n_callbacks,
                                               uint8_t *keycodes,
                                               void (**gcallback_functions)(),
                                               void (*gcallback)()   ) {
    struct keyboard_initializer *returnvalue = malloc(sizeof(struct keyboard_initializer));
    int i = 0;
    for(; i < n_callbacks*3; i++) returnvalue->callback_keycodes[i] = keycodes[i];
    for(; i < 30; i++)            returnvalue->callback_keycodes[i] = 0x0;
    
    returnvalue->nkey_buffer        = buffer_addr;
    returnvalue->num_callbacks      = n_callbacks;
    returnvalue->callback_functions = gcallback_functions;
    returnvalue->general_callback   = gcallback;

    free(keycodes);

    return returnvalue;
}

int attempt_key_callbacks() {
    int i = 0;
    int found_callback = 0;
    for(; i < 10; i++) {
        if(key_callbacks[i].callback == 0x0) break;
        if(keys_pressed[key_callbacks[i].key_1] &&
           (keys_pressed[key_callbacks[i].key_2] || key_callbacks[i].key_2 == 0x0) &&
           (keys_pressed[key_callbacks[i].key_3] || key_callbacks[i].key_3 == 0x0)) {
            (key_callbacks[i].callback)();
            found_callback = 1;
            break;
        }
    }

    return found_callback;
}

void default_keyboard_callback(registers_t *regs) { 
    uint8_t scancode = port_byte_in(0x60);

    if(scancode < 0x81) keys_pressed[scancode] = 1;
    else                keys_pressed[scancode-0x80] = 0;

    int found_callback = attempt_key_callbacks();

    if(!found_callback && scancode < 0x81) {
        if (scancode == BACKSPACE) {
            backspace(key_buffer);
            kprint_backspace();
        } else if (scancode == LSHIFTP) {
        } else if (scancode == RSHIFTP) {
        } else {                                                    
            char letter = (keys_pressed[0x2A] || keys_pressed[0x36]) ? ascii_shift[(int) scancode] : ascii[(int) scancode];
            char str[2] = {letter, '\0'}; 
            append(key_buffer, letter); 
            kprint(str);
        }
    }

    UNUSED(regs);
}

void reset_keyboard() {
    key_buffer = 0x0;
    int i = 0;
    for(; i < 10; i++) {
        key_callbacks[i].key_1    = 0x0;
        key_callbacks[i].key_2    = 0x0;
        key_callbacks[i].key_3    = 0x0;
        key_callbacks[i].callback = 0x0;
    }
}

void init_keyboard(struct keyboard_initializer* nkey_initializer) {
    reset_keyboard();
    if(key_buffer != 0x0) free(key_buffer);
    key_buffer = nkey_initializer->nkey_buffer;

    int i = 0;
    for(; i < nkey_initializer->num_callbacks; i++) {
        key_callbacks[i].key_1 = nkey_initializer->callback_keycodes[i*3];
        key_callbacks[i].key_2 = nkey_initializer->callback_keycodes[i*3+1];
        key_callbacks[i].key_3 = nkey_initializer->callback_keycodes[i*3+2];
        key_callbacks[i].callback = nkey_initializer->callback_functions[i];
    }

    if(nkey_initializer->general_callback == 0x0) 
        register_interrupt_handler(IRQ1, default_keyboard_callback); 
    else 
        register_interrupt_handler(IRQ1, nkey_initializer->general_callback);
}