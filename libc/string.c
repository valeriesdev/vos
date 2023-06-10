#include "string.h"
#include <stdint.h>
#include <stdbool.h>
#include "../drivers/screen.h"
#include "mem.h"

/**
 * K&R implementation
 */
char* int_to_ascii(int n) {
    char *str = "";
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
    return str;
}

char* hex_to_ascii(int n) {
    char *str = "";
    str[0] = '\0';
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
    return str;
}

/* K&R */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len-1] = '\0';
}

/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

char** str_split(char* a_str, const char a_delim) {
    int count = 0;
    char current = a_str[0];
    int i = 0;
    while(current != '\0') {
        if(current == a_delim) count++;
        
        i++;
        current = a_str[i];
    }

    char** output = malloc((count+1)*sizeof(char*));
    if(count == 0) { 
        output[0] = a_str;
        return output; 
    }
    

    current = a_str[0];
    count = 0;
    i = 0;
    int last = 0;
    while(current != '\0') {
        if(current == a_delim) {
            output[count] = malloc(sizeof(char)*(i-last));
            int j = 0;
            while(j < (i-last)) {
                output[count][j] =  a_str[last+j];
                j++;
            }
            output[count][j-1] = '\0';
            last = i;
            count++;
            current = a_str[i++];
        } else {
            current = a_str[i++];
        }
    }

    output[count] = malloc(sizeof(char)*(i-last));
    int j = 0;
    while(j < (i-last)) {
        output[count][j] =  a_str[last+j];
        j++;
    }
    output[count][j] = '\0';
    return output;
}