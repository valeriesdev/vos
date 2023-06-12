#include "keyboard.h"
#include "../cpu/ports.h"
#include "../cpu/isr.h"
#include "screen.h"
#include "../libc/string.h"
#include "../libc/function.h"
#include "../kernel/kernel.h"
#include <stdint.h>

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFTP 0x2A
#define LSHIFTR 0xAA

static char key_buffer[256];

int caps;
#define SC_MAX 114
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar", "q", "w", "e",
        "r", "t", "y", "u", "i", "o", "p", "", "[", "]", "Enter", "Lctrl",
        "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`",
        "LShift", "\\", "z", "x", "c", "v", "b", "n", "m", ",", "."};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', 0x04, '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' ', 'q', 'w', 'e',
        'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '?', '?',
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0x04, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.',
        '/', 0x04, 0x04, 0x04, ' '};

static void keyboard_callback(registers_t *regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);
    
    if (scancode > SC_MAX && scancode != LSHIFTR) return;       // Ignore scancodes that we have not programmed in
    if (scancode == BACKSPACE) {
        backspace(key_buffer);
        kprint_backspace();
    } else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer); /* kernel-controlled function */
        key_buffer[0] = '\0';
    } else if (scancode == LSHIFTP) {                           // If the user presses shift, disable caps
        caps = 0;                                               // The caps variable works backwards
    } else if (scancode == LSHIFTR) {                           // If the user releases shift, enable caps
        caps = 1;                                               // The caps variable works backwards
    } else {                                                    // If the pressed letter is normal
        char letter = sc_ascii[(int)scancode + ((int)caps)*42]; // Get letter by accessing ascii indexed by scancode
        char str[2] = {letter, '\0'};                           // Then, we add '\0' to the end to terminate the string
        append(key_buffer, letter);                             // Then, we place the letter in the key buffer
        kprint(str);                                            // Then, we print the string
    }
    UNUSED(regs);
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}

void rehook_keyboard(void (*new_function)()) {
    register_interrupt_handler(IRQ1, new_function);
}
