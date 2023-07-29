#include <stdint.h>
#include "drivers/screen.h"
#include "stock/program_interface/popup.h"

void create_popup_msg(struct popup_msg_struct* callback) {
	int i = 0;
	for(i = 0; i < callback->x2 - callback->x1; ++i) kprint_at("-", callback->x1 + i, callback->y1);
	for(i = 0; i < callback->x2 - callback->x1; ++i) kprint_at("-", callback->x1 + i, callback->y2);
	for(i = 0; i < callback->y2 - callback->y1; ++i) kprint_at("|", callback->x1, callback->y1 + i);
	for(i = 0; i < callback->y2 - callback->y1; ++i) kprint_at("|", callback->x2, callback->y1 + i);

	kprint_at(callback->message, callback->x1+1, (callback->y1+callback->y2)/2);
}

void create_popup_str(struct popup_str_struct* callback) {
	int i = 0;
	for(i = 0; i < callback->x2 - callback->x1; ++i) kprint_at("-", callback->x1 + i, callback->y1);
	for(i = 0; i < callback->x2 - callback->x1; ++i) kprint_at("-", callback->x1 + i, callback->y2);
	for(i = 0; i < callback->y2 - callback->y1; ++i) kprint_at("|", callback->x1, callback->y1 + i);
	for(i = 0; i < callback->y2 - callback->y1; ++i) kprint_at("|", callback->x2, callback->y1 + i);

	kprint_at(callback->message, callback->x1+1, (callback->y1+callback->y2)/2);
	
}

/**
 * void create_popup
 * uint8_t type: 
 * 		0: popup with message
 * 		1: popup that returns a string
 * type, callback
 * 0     points to a struct of int, int, int, int, int,      char[]
 * 							   x1,  x2,  y1,  y2,  str_size, message
 * 
 * 1     points to a struct of int, int, int, int, int     , char[] , int         , char* 
 * 							   x1 , x2 , y1 , y2 , str_size, message, ret_str_size, ret_str
 * */
void create_popup(uint8_t type, void* callback) {
	if(type == 0)	   create_popup_msg(callback);
	else if(type == 1) create_popup_str(callback);
	else               return;
}