#define BACKSPACE 0x0E
#define ENTER 0x1C
#define SC_MAX 114

// Special Key Functions
#define SPECIAL_KEY_NUM 0x0F

#define LSHIFTF 0x00
#define LSHIFTP 0x2A
#define LSHIFTR 0xAA

#define RSHIFTF 0x01
#define RSHIFTP 0x36
#define RHISFTR 0xB6

struct keyboard_initializer {
	char* nkey_buffer;
    uint8_t num_callbacks;
    uint8_t callback_keycodes[30]; // 0-2 is callback 1, 3-5 is 2, 6-8 is 3, etc
    void (**callback_functions)();    
    void (*general_callback)();
};

struct key_callback {
	uint8_t key_1;
	uint8_t key_2;
	uint8_t key_3;

	void (*callback)();
};

extern char *key_buffer;

/** Example to initialize the keyboard with one callback, for the enter key.
 *  uint8_t *keycodes = malloc(sizeof(uint8_t)*3);
 *  keycodes[0] = 0x1C; keycodes[1] = NULL; keycodes[2] = NULL;
 *  void (**gcallback_functions)() = malloc(sizeof(void*)*10);
 *  *gcallback_functions = user_input;
 *  struct keyboard_initializer* keyboardi = create_initializer(malloc(sizeof(char)*256),
 *                                                              1,
 *                                                              keycodes,
 *                                                              gcallback_functions,
 *                                                              NULL);
 *  init_keyboard(keyboardi);
  **/
void init_keyboard(struct keyboard_initializer* nkey_initializer);
int attempt_key_callbacks();
struct keyboard_initializer *create_initializer(char* buffer_addr,
                                               uint8_t n_callbacks,
                                               uint8_t *keycodes,
                                               void (**gcallback_functions)(),
                                               void (*gcallback)());
char* read_line();