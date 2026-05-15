//bidimensional array interpreter
#include "bda.h"

static inline void skip_comment(uint8_t **src){
    (*src)++;
    if(**src == '*'){
        (*src)++;
        while(**src != 0){
            if(**src == '*'){
                (*src)++;
                if(**src == '/'){
                    (*src)++;
                    return;
                }
            } else{
                (*src)++;
            }
        }
    }
}

static inline void flush_x(bda_descriptor *bda, uint64_t *x, uint64_t *y, uint64_t *n, uint64_t *number, int64_t val) {
    if(*x >= (*bda).x){
        (*bda).n = *n;
        return;
    }
    *(uint64_t *)((uint8_t *)(*bda).dest + ((*y * (*bda).x + *x) * 8)) = val;
    (*x)++;
    (*n)++;
    *number = 0;
}

static inline void flush_y(bda_descriptor *bda, uint64_t *x, uint64_t *y, uint64_t *n, uint64_t *number, int64_t val) {
    if(*y >= (*bda).y){
        (*bda).n = *n;
        return;
    }
    *(uint64_t *)((uint8_t *)(*bda).dest + ((*y * (*bda).x + *x) * 8)) = val;
    (*y)++;
    *x = 0;
    (*n)++;
    *number = 0;
}

void bda_parse(bda_descriptor *bda){

    if((*bda).x == 0 || (*bda).y == 0 || (*bda).n == 0){
        (*bda).x = 0;
        (*bda).y = 0;
        (*bda).n = 0;
        return;
    }

    uint8_t *src = (uint8_t *)(*bda).src;
    uint64_t x = 0, y = 0, n = 0;
    uint64_t number = 0;

    while(n<(*bda).n){
        if(*src==0){
            break;
        }
        else if(*src=='/'){
            skip_comment(&src);
        }
        else if(*src=='0'){
            src++;
            if(*src==0) break;
            if(*src == 'x'){
                src++;
                number = 0;
                while(*src != 0) {
                    if(*src == '/') {
                        skip_comment(&src);
                    }
                    else if(*src >= '0' && *src <= '9') {
                        number = (number << 4) | (*src - '0');
                        src++;
                    }
                    else if(*src >= 'a' && *src <= 'f') {
                        number = (number << 4) | (*src - 'a' + 10);
                        src++;
                    }
                    else if(*src >= 'A' && *src <= 'F') {
                        number = (number << 4) | (*src - 'A' + 10);
                        src++;
                    }
                    else if(*src == ',') {
                        flush_x(bda, &x, &y, &n, &number, (int64_t)number);
                        src++;
                        break;
                    }
                    else if(*src == ';') {
                        flush_y(bda, &x, &y, &n, &number, (int64_t)number);
                        src++;
                        break;
                    }
                    else {
                        src++;
                    }
                }
            }
        }
        else if(*src == '-') {
            number = 0;
            src++;
            if(*src == 0) break;
            while(*src != 0) {
                if(*src == '/') {
                    skip_comment(&src);
                }
                else if(*src >= '0' && *src <= '9') {
                    number = (number * 10) + (*src - '0');
                    src++;
                }
                else if(*src == ',') {
                    flush_x(bda, &x, &y, &n, &number, -(int64_t)number);
                    src++;
                    break;
                }
                else if(*src == ';') {
                    flush_y(bda, &x, &y, &n, &number, -(int64_t)number);
                    src++;
                    break;
                }
                else {
                    src++;
                }
            }
            if(number != 0){
                number = -number;
            }
        }
        else if(*src >= '0' && *src <= '9') {
            number = 0;
            while(*src != 0) {
                if(*src == '/') {
                    skip_comment(&src);
                }
                else if(*src >= '0' && *src <= '9') {
                    number = (number * 10) + (*src - '0');
                    src++;
                }
                else if(*src == ',') {
                    flush_x(bda, &x, &y, &n, &number, (int64_t)number);
                    src++;
                    break;
                }
                else if(*src == ';') {
                    flush_y(bda, &x, &y, &n, &number, (int64_t)number);
                    src++;
                    break;
                }
                else {
                    src++;
                }
            }
        }
        else if(*src == ',') {
            flush_x(bda, &x, &y, &n, &number, (int64_t)number);
            src++;
        }
        else if (*src == ';') {
            flush_y(bda, &x, &y, &n, &number, (int64_t)number);
            src++;
        }
        else{
            src++;
        }
    }

    n = number;
    (*bda).n = n;
    (*bda).x = x;
    (*bda).y = y;
    (*bda).src = (uint64_t)src;

    return;
}