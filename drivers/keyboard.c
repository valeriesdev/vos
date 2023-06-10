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
    
    if (scancode > SC_MAX && scancode != LSHIFTR) return;       // Ignore scancodes that we have not programmed in      | プログラムしていないスキャンコードを無視する
    if (scancode == BACKSPACE) {
        backspace(key_buffer);
        kprint_backspace();
    } else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer); /* kernel-controlled function */
        key_buffer[0] = '\0';
    } else if (scancode == LSHIFTP) {                           // If the user presses shift, disable caps              | ユーザーがShiftキーを押した場合は、キャップを無効にします
        caps = 0;                                               // The caps variable works backwards                    | caps変数は逆方向に機能します
    } else if (scancode == LSHIFTR) {                           // If the user releases shift, enable caps              | ユーザーがシフトを解放した場合は、上限を有効にします
        caps = 1;                                               // The caps variable works backwards                    | caps変数は逆方向に機能します
    } else {                                                    // If the pressed letter is normal                      | 押されたレターが正常な場合
        char letter = sc_ascii[(int)scancode + ((int)caps)*42]; // Get letter by accessing ascii indexed by scancode    | scancodeでインデックス付けされたasciiにアクセスしてレターを取得
        char str[2] = {letter, '\0'};                           // Then, we add '\0' to the end to terminate the string | 次に、末尾に「\ 0」を追加して、文字列を終了します
        append(key_buffer, letter);                             // Then, we place the letter in the key buffer          | 次に、その文字をキーバッファに配置します
        kprint(str);                                            // Then, we print the string                            | 次に、文字列を印刷します **/ 
    }
    UNUSED(regs);
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}
