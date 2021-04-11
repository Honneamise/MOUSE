#ifndef MOUSE_H
#define MOUSE_H

#define _CRT_SECURE_NO_WARNINGS

#include "stdlib.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"

#define STACK_SIZE 256
#define PROG_SIZE 4096
#define DATA_SIZE 260

#define ASCII_SIZE 26
#define ASCII_UPPER_OFFSET 65
#define ASCII_NUMBER_OFFSET 48

#define ERROR_PROG_END          "REACHED END OF PROGRAM"
#define ERROR_STACK_FULL        "STACK FULL"
#define ERROR_STACK_EMPTY       "STACK EMPTY"
#define ERROR_UNKNOW_SYMBOL     "UNKNOW SYMBOL FOUND"
#define ERROR_INPUT             "INPUT ERROR"
#define ERROR_PROG_SIZE         "FILE SIZE EXCEED LIMIT"
#define ERROR_TAG               "INVALID TAG FOUND"
#define ERROR_FILE_NOT_FOUND    "FILE NOT FOUND"
#define ERROR_FILE_READING      "ERROR READING FILE"

typedef enum tagtype
{
    MACRO,
    PARAM,
    LOOP

}tagtype;

typedef struct frame
{
   tagtype tag;
   int pos;
   int off;

}frame;

typedef struct mouse
{
    char PROG[PROG_SIZE];
    frame STACK[STACK_SIZE];
    int CALSTACK[STACK_SIZE];
    int DEFS[ASCII_SIZE];
    int DATA[DATA_SIZE];

    char CH;
    int CHPOS;

    int CAL;
    int LEVEL;
    int OFFSET;

    int LINE;

} mouse;

/**********/
void ERROR(char *s);

int NUM(char c);

int VAL(char c);

void GETCHAR(mouse *m);

void PUSHCAL(mouse *m, int d);

int POPCAL(mouse *m);

void PUSH(mouse *m, tagtype tag);

void POP(mouse *m);

void SKIP(mouse *m, char lch, char rch);

void LOAD(mouse *m, char *file);

void INIT(mouse *m);

void RUN(mouse *m);

#endif