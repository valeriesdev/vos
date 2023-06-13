#include "stock/tedit/tedit.h"

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

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFTP 0x2A
#define LSHIFTR 0xAA
#define LCTRLP 0x1D
#define LCTRLR 0x9D
#define SC_MAX 114

// Special Key Functions
#define SPECIAL_KEY_NUM 0x02
#define LSHIFTF 0x00
#define LCTRLF  0x01

struct special_key {
	int pressed;
	int scancodep;
	int scancoder;
};

struct special_key *special_keys[SPECIAL_KEY_NUM];

struct special_key* create_special_key(int scancodep, int scancoder) {
	struct special_key* new_key = malloc(sizeof(struct special_key));
	new_key->pressed = 0;
	new_key->scancodep = scancodep;
	new_key->scancoder = scancoder;
	return new_key;
}

void init_special_keys() {
	key_buffer = malloc(sizeof(char)*256);
	int i = 0;
	for(i = 0; i < SPECIAL_KEY_NUM; i++) special_keys[i] = malloc(sizeof(struct special_key));
	special_keys[0] = create_special_key(LSHIFTP, LSHIFTR);
	special_keys[1] = create_special_key(LCTRLP, LCTRLR);
}

int attempt_special_keypress(int scancode) {
	int i = 0;
	for(i = 0; i < SPECIAL_KEY_NUM; i++) {
		if(special_keys[i]->scancodep == scancode) return special_keys[i]->pressed = 1;
		if(special_keys[i]->scancoder == scancode) return (special_keys[i]->pressed = 0)+1;
	}
	return 0;
}

void get_string_hook(registers_t *regs) {
	uint8_t scancode = port_byte_in(0x60);
    
    if(attempt_special_keypress(scancode)) {
    } else if (scancode > SC_MAX && scancode != LSHIFTR) {
    	return;       // Ignore scancodes that we have not programmed in
    } else if (scancode == BACKSPACE) {
        backspace(key_buffer);
        kprint_backspace();
    } else if (scancode == ENTER) {
        kprintn("\n");
        filename_recieved();
    } else {                                                    
        char letter =  (special_keys[LSHIFTF]->pressed) ? ascii_shift[(int) scancode] : ascii[(int) scancode];
        char str[2] = {letter, '\0'}; 
        append(key_buffer, letter); 
        kprint(str);
    }
    UNUSED(regs);
}

void file_writing_hook(registers_t *regs) { 
    uint8_t scancode = port_byte_in(0x60);
    
    if(attempt_special_keypress(scancode)) {
    } else if (scancode > SC_MAX && scancode != LSHIFTR) {
    	return;       // Ignore scancodes that we have not programmed in
    } else if (scancode == BACKSPACE) {
        backspace(key_buffer);
        backspace(file);
        kprint_backspace();
    } else if (scancode == ENTER) {
        kprint("\n");
        key_buffer[0] = '\0';
    } else {                                                    
        char letter =  (special_keys[LSHIFTF]->pressed) ? ascii_shift[(int) scancode] : ascii[(int) scancode];
        if(letter == 's' && special_keys[LCTRLF]->pressed) {
        	append(file,'\0');
        	file_save();
        } else {
        	char str[2] = {letter, '\0'}; 
        	append(key_buffer, letter); 
        	append(file, letter);
        	kprint(str);
    	}
    }
    UNUSED(regs);
}