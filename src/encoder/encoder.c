/* encoder.c: encode and decode strings in bminor language */

#include "encoder.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>

/* Functions */

/** 
 * Takes an encoded string 'es' that contains quotes and possibly backslash codes, 
 * then converts it into a normal string 's' without quotes or codes
 * @param   es      encoded string
 * @param   s       decoded string 
 * @return  True if input string is valid and conversion in successful, otherwise 
 * False 
**/
int string_decode( const char *es, char *s ){
    if (!es || !s) {
        fprintf(stderr, "encoded string is null\n");
        return false; 
    }

    int count = 0;
    const char *encoded = es;
    char *decoded = s;

    // Check for first char to be open quotes 
    if (*encoded != '"') {
        // check for edge case with string starting with \" 
        if (encoded[0] != '\\' || encoded[1] != '"'){
            fprintf(stderr, "String does not start with opening quote\n");
            return false;
        }
    }
    encoded++;

    while(true){
        if (count > MAX_STR_LEN){
            printf("%d\n", count);
            fprintf(stderr, "Maximum string length exceeded\n");
            return false;
        }

        const char *quote_char = encoded++;
        //edge case for 
        if (*quote_char == '"' && quote_char[1] == '\0') break;
        int my_char = *quote_char;


        if (my_char == '\0') {
            fprintf(stderr, "String does not end with closing quotation\n");
            return false;
        }

        // invalid ascii no escape char
        if (my_char != '\\' && (my_char < 32 || my_char > 127)) { 
            fprintf(stderr, "unprintable ASCII, should be escaped in hex form\n");
            return false;
        }

        // valid ascii no escape character
        if (my_char != '\\' && (my_char >= 32 && my_char <= 127)) {
            *decoded++ = (char)my_char;
            count++;
            continue; 
        }

        if (my_char == '\\'){
            const char *back_char = encoded++;
            if (*back_char == '\0' ) {
                fprintf(stderr, "Terminate string without closing quote\n");
                return false; 
            }

            //escape codes
            switch(*back_char){
                case 'a':
                    *decoded++ = 7;
                    break;
                case 'b':
                    *decoded++ = 8;
                    break;
                case 'e':
                    *decoded++ = 27;
                    break;
                case 'f':
                    *decoded++ = 12;
                    break;
                case 'n':
                    *decoded++ = 10;
                    break;
                case 'r':
                    *decoded++ = 13;
                    break;
                case 't':
                    *decoded++ = 9;
                    break;
                case 'v':
                    *decoded++ = 11;
                    break;
                case '\\':
                    *decoded++ = 92;
                    break;
                case '\'':
                    *decoded++ = 39;
                    break;
                case '\"':
                    *decoded++ = 34;
                    if(back_char[1] == '\0'){
                        count++;
                        if (count > MAX_STR_LEN){
                            fprintf(stderr, "Maximum length of string should be 255\n");
                            return false;
                        }
                        goto end;
                    }
                    break;
                case '0':
                    if (back_char[1] == '\0') {
                        fprintf(stderr, "Incomplete string not closing quote\n");
                        return false;
                    }
                    if (back_char[1] != 'x'){
                        *decoded++ = '0';
                    } else if (back_char[1] == 'x' && isxdigit(back_char[2]) 
                                && isxdigit(back_char[3])){
                        encoded++;
                        int high_hex = char_to_hex(*encoded++);
                        int low_hex = char_to_hex(*encoded++);

                        int result = high_hex * 16 + low_hex;
                        *decoded++ = result;
                    } else {
                        *decoded++ = '0';
                    }
                    break;
                default:
                    *decoded++ = *back_char;
                    break;
            }
            count++;
        }
    }

    // if not end fail
    if(*encoded != '\0'){
         fprintf(stderr, "Invalid Characters after closing string\n");
         return false;
    }

    end: 
    *decoded = '\0';
    return true;
}

/** 
 * Converts normal (decoded) string into an encoded string in bminor language 
 * @param   s       normal (decoded) string
 * @param   es      encoded string
**/
void string_encode( const char *s, char *es ){
    if (!s || !es) {
        fprintf(stderr, "decoded string is null\n");
        return;
    }
    const char *decoded = s;
    char *encoded = es;

    *encoded++ = '"';

    while(*decoded){
        int my_char = *decoded++;

        switch (my_char){
            case 7:
                *encoded++ = '\\';
                *encoded++ = 'a';
                break;
            case 8:
                *encoded++ = '\\';
                *encoded++ = 'b';
                break;
            case 27:
                *encoded++ = '\\';
                *encoded++ = 'e';
                break;
            case 12:
                *encoded++ = '\\';
                *encoded++ = 'f';
                break;
            case 10:
                *encoded++ = '\\';
                *encoded++ = 'n';
                break;
            case 13:
                *encoded++ = '\\';
                *encoded++ = 'r';
                break;
            case 9:
                *encoded++ = '\\';
                *encoded++ = 't';
                break;
            case 11:
                *encoded++ = '\\';
                *encoded++ = 'v';
                break;
            case 92:
                *encoded++ = '\\';
                *encoded++ = '\\';
                break;
            case 39:
                *encoded++ = '\\';
                *encoded++ = '\'';
                break;
            case 34:
                *encoded++ = '\\';
                *encoded++ = '\"';
                break;
            default:
                if (my_char >= 32 && my_char <= 127){
                    *encoded++ = my_char;
                    continue;
                }
                *encoded++ = my_char;

        }
    }

    *encoded++ = '"';
    *encoded++ = '\0';

    return;
}

/**
* converts character into hexadecimal value
* @param    c       individual hex character
* @return   returns the hex value if valid hexadecimal e.g (0-9, A-F, a-f) else -1 
**/
int char_to_hex(char c){
    if (c >= '0' && c <='9')  return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}