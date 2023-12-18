#ifndef __PARSER_H
#define __PARSER_H
typedef struct file_info
{
    FILE *fp;
    FILE *fout;
    char *fname;
    struct yy_buffer_state *saved_state;
} FILE_INFO;

typedef struct ifdef_stack
{
    int value;
    struct ifdef_stack *next;
} IFDEF_STACK;
#endif
