D           [0-9]
L           [a-zA-Z_]
H           [a-fA-F0-9]
E           [Ee][+-]?{D}+
FS          (f|F|l|L)
IS          (u|U|l|L)*

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include "parser.h"
#include "y.tab.h"

#define MAX_BUF_SIZE 1 << 15 //32K

typedef enum {
    CONFIG_CMD_NONE,
    SRC_ROOT_DIR,
    OUTPUT_DIR,
    UNDEF_FLAGS,
    DEFINE_FLAGS,
    IFDEF_REMOVE,
    EVALUATE_NUM,
	ASSIGN_VALUE,
	EXCLUDE_FILES,
	BYPASS_FILES,
	FILTER_SUFFIX,
} CONFIG_CMD;

extern YYSTYPE yylval;
FILE_INFO *g_include_files = NULL;
void count();
int column = 0;
int lineno = 1;
int ifdef_remove=0;
int evaluate_num=0;
static int ignore(int);
static int comment(int);
static char *save_to_NL();
static IFDEF_STACK *ifdef_top = NULL;
typedef struct stringstack
{
    char *string;
    struct stringstack *next;
} StringStack;

static StringStack *ifdef_string = 0, *else_string = 0, *endif_string = 0;
static StringStack *exclude_files = 0;
static StringStack *bypass_files = 0;
static StringStack *filter_suffix = 0;
static void push_ifdef_string(char *s);
static void push_else_string(char *s);
static void push_endif_string(char *s);
static void add_exclude_files(char *fname);
static void add_bypass_files(char *fname);
static void add_filter_suffix(char *suffix);
static int in_exclude_files(char *fname);
static int in_bypass_files(char *fname);
static int in_filter_suffix(char *suffix);
static int create_dir_stack(char *src_dir_name, char *dst_dir_name, char bAll); 
static int config_stage = 1;
static int config_eq    = 0;
static CONFIG_CMD config_cmd = CONFIG_CMD_NONE;
static char *assign_variable = 0;
typedef struct symbol
{
    char *name;
    struct symbol *next;
}SYMBOL;

static SYMBOL *g_remove_flags = 0;
static char *g_src_root_dir = 0, *g_output_dir = 0;

typedef struct flist
{
    FILE *src;
    FILE *dst;
    char *src_name;
    char *dst_name;
	char isBypass;
    struct flist *next;
} FLIST;

static FLIST *g_file_list = 0;
%}

%s PARSER PLAINTEXT
%%
<INITIAL>"SRC_ROOT_DIR" { count(); config_cmd = SRC_ROOT_DIR; }
<INITIAL>"OUTPUT_DIR"   { count(); config_cmd = OUTPUT_DIR;   }
<INITIAL>"UNDEF_FLAGS"  { count(); config_cmd = UNDEF_FLAGS; }
<INITIAL>"DEFINE_FLAGS" { count(); config_cmd = DEFINE_FLAGS; }
<INITIAL>"IFDEF_REMOVE" { count(); config_cmd = IFDEF_REMOVE; }
<INITIAL>"EVALUATE_NUM" { count(); config_cmd = EVALUATE_NUM; }
<INITIAL>"EXCLUDE_FILES" { count(); config_cmd = EXCLUDE_FILES; }
<INITIAL>"BYPASS_FILES"  { count(); config_cmd = BYPASS_FILES; }
<INITIAL>"FILTER_SUFFIX" { count(); config_cmd = FILTER_SUFFIX; }
<INITIAL>"$"{L}({L}|{D})* { count(); config_cmd = ASSIGN_VALUE; assign_variable=strdup(yytext+1); }
<INITIAL>"="            { count(); config_eq = 1; }
<INITIAL>[/]?({L}|{D}|[-]|[/]|[\.])*  { 
                          count();  
                          if (!config_eq)
                          {
                              printf("Skip token: %s\n", yytext);
                              return;
                          }
                          switch(config_cmd)
                          {
                              case SRC_ROOT_DIR:
                                  g_src_root_dir = strdup(yytext);
                                  printf("SRC ROOT=%s\n", g_src_root_dir);
                                  break;
                              case OUTPUT_DIR:
                                  g_output_dir = strdup(yytext);
                                  printf("OUTPUT =%s\n", g_output_dir);
                                  break;
                              case UNDEF_FLAGS:
                              {
                                  create_symbol(strdup(yytext), 0);
                                  printf("UNDEF FLAGS=%s\n", yytext);
                                  break;
                              }
                              case DEFINE_FLAGS:
                              {
                                  create_symbol(strdup(yytext), 1);
                                  printf("DEFINE FLAGS=%s\n", yytext);
                                  break;
                              }
                              case IFDEF_REMOVE:
                              {
								  if (!strcmp(yytext, "y") || !strcmp(yytext, "Y"))
									  ifdef_remove = 1;

                                  printf("IFDEF_REMOVE=%s\n", yytext);
                                  break;
                              }
                              case EVALUATE_NUM:
                              {
								  if (!strcmp(yytext, "y") || !strcmp(yytext, "Y"))
									  evaluate_num = 1;

                                  printf("EVALUATE_NUM=%s\n", yytext);
                                  break;
                              }
                              case EXCLUDE_FILES:
                              {
								  add_exclude_files(yytext);
                                  printf("Add EXCLUDE_FILES=%s\n", yytext);
                                  break;
                              }
                              case BYPASS_FILES:
                              {
								  add_bypass_files(yytext);
                                  printf("Add BYPASS_FILES=%s\n", yytext);
                                  break;
                              }
                              case FILTER_SUFFIX:
                              {
								  add_filter_suffix(yytext);
                                  printf("Add FILTER_SUFFIX=%s\n", yytext);
                                  break;
                              }

                              case ASSIGN_VALUE:
                              {
							      int value;
								  if (!assign_variable)
								  {
									  printf("ERROR! no assign variable at L%d\n", lineno);
								  } 
								  else
								  {
								      value = strtol(yytext, NULL, 10); 
                                      assign_symbol(assign_variable, create_scalar_expr(value));
                                      printf("ASSIGN_VALUE:%s=%d\n", assign_variable, value);
									  assign_variable = 0;
								  }
                                  break;
                              }
                              default:
                                  printf("Unknown token: %s\n", yytext);
                                  break;
                          }
                        }
<INITIAL>"#"            { count(); ignore(0); }
<INITIAL>"\n"           { count(); }
<INITIAL>[ \t\v\f]      { count(); }
<INITIAL>.              { count(); }

<PLAINTEXT>"/*"            { return comment(1); }
<PLAINTEXT>"//"            { return ignore(1); }
<PLAINTEXT>[ \t\v\f\n]*    { count(); yylval.string = strdup(yytext); return(STRING); }
<PLAINTEXT>"#ifdef"        { count(); BEGIN PARSER; push_ifdef_string(save_to_NL()); return(IFDEF); }
<PLAINTEXT>"#ifndef"       { count(); BEGIN PARSER; push_ifdef_string(save_to_NL()); return(IFNDEF); }
<PLAINTEXT>"#if"           { count(); BEGIN PARSER; push_ifdef_string(save_to_NL()); return(IF2); }
<PLAINTEXT>"#endif"        { count(); BEGIN PARSER; push_endif_string(save_to_NL()); return(ENDIF); }
<PLAINTEXT>"#else"         { count(); BEGIN PARSER; push_else_string(save_to_NL()); return(ELSE2); }
<PLAINTEXT>.            { count(); return ignore(1); }
<PARSER>"/*"            { comment(0); }
<PARSER>"//"            { ignore(0); }
<PARSER>"defined"       { count();  return(DEFINED); }
<PARSER>0[xX]{H}+{IS}?  { count(); yylval.num = strtol(yytext, NULL, 16); return(CONSTANT); }
<PARSER>0{D}+{IS}?      { count(); yylval.num = strtol(yytext, NULL, 8);  return(CONSTANT); }
<PARSER>{D}+{IS}?       { count(); yylval.num = strtol(yytext, NULL, 10); return(CONSTANT); }
<PARSER>L?'(\\.|[^\\'])+'   { count(); yylval.num = 0; return(CONSTANT); }
<PARSER>{D}+{E}{FS}?        { count(); yylval.num = strtol(yytext, NULL, 10);  return(CONSTANT); }
<PARSER>{D}*"."{D}+({E})?{FS}?  { count(); yylval.real = strtod(yytext, NULL); return(CONSTANT); }
<PARSER>{D}+"."{D}*({E})?{FS}?  { count(); yylval.real = strtod(yytext, NULL); return(CONSTANT); }
<PARSER>{L}({L}|{D})*         { count();  yylval.string = strdup(yytext); return(IDENTIFIER);}
<PARSER>"("         { count(); return('('); }
<PARSER>")"         { count(); return(')'); }
<PARSER>"<"         { count(); return('<'); }
<PARSER>">"         { count(); return('>'); }
<PARSER>"!"         { count(); return('!'); }
<PARSER>"&&"            { count(); return(AND_OP); }
<PARSER>"||"            { count(); return(OR_OP); }
<PARSER>"..."           { count(); return(ELLIPSIS); }
<PARSER>">>="           { count(); return(RIGHT_ASSIGN); }
<PARSER>"<<="           { count(); return(LEFT_ASSIGN); }
<PARSER>"+="            { count(); return(ADD_ASSIGN); }
<PARSER>"-="            { count(); return(SUB_ASSIGN); }
<PARSER>"*="            { count(); return(MUL_ASSIGN); }
<PARSER>"/="            { count(); return(DIV_ASSIGN); }
<PARSER>"%="            { count(); return(MOD_ASSIGN); }
<PARSER>"&="            { count(); return(AND_ASSIGN); }
<PARSER>"^="            { count(); return(XOR_ASSIGN); }
<PARSER>"|="            { count(); return(OR_ASSIGN); }
<PARSER>">>"            { count(); return(RIGHT_OP); }
<PARSER>"<<"            { count(); return(LEFT_OP); }
<PARSER>"++"            { count(); return(INC_OP); }
<PARSER>"--"            { count(); return(DEC_OP); }
<PARSER>"->"            { count(); return(PTR_OP); }
<PARSER>"<="            { count(); return(LE_OP); }
<PARSER>">="            { count(); return(GE_OP); }
<PARSER>"=="            { count(); return(EQ_OP); }
<PARSER>"!="            { count(); return(NE_OP); }
<PARSER>";"             { count(); return(';'); }
<PARSER>("{"|"<%")      { count(); return('{'); }
<PARSER>("}"|"%>")      { count(); return('}'); }
<PARSER>","             { count(); return(','); }
<PARSER>":"             { count(); return(':'); }
<PARSER>"="             { count(); return('='); }
<PARSER>("["|"<:")      { count(); return('['); }
<PARSER>("]"|":>")      { count(); return(']'); }
<PARSER>"."         { count(); return('.'); }
<PARSER>"&"         { count(); return('&'); }
<PARSER>"~"         { count(); return('~'); }
<PARSER>"-"         { count(); return('-'); }
<PARSER>"+"         { count(); return('+'); }
<PARSER>"*"         { count(); return('*'); }
<PARSER>"/"         { count(); return('/'); }
<PARSER>"%"         { count(); return('%'); }
<PARSER>"^"         { count(); return('^'); }
<PARSER>"|"         { count(); return('|'); }
<PARSER>"?"         { count(); return('?'); }
<PARSER>"\n"        { count(); BEGIN PLAINTEXT; }
<PARSER>.  { count(); }
<<EOF>>    { return EOF; }

%%
yywrap()
{
    return(1);
}

void change_file()
{
	//printf("change file: g_file_list=%p\n", g_file_list);
    // output the parsed file
    if (g_file_list)
    {
        output_file(g_file_list->dst);
        //printf("src fp=%p, dst fp=%p\n", g_file_list->src, g_file_list->dst);
        fclose(g_file_list->src);
        fclose(g_file_list->dst);
        g_file_list = g_file_list->next;
    }

    if (config_stage)
    {
        config_stage = 0;
        BEGIN PLAINTEXT;
        create_dir_stack(g_src_root_dir, g_output_dir, 0);
    }

    // switch to the new file
    yy_delete_buffer(YY_CURRENT_BUFFER);

	while(g_file_list && g_file_list->isBypass)
	{
		char buf[520];
		sprintf(buf, "\\cp -f %s %s", 
				g_file_list->src_name, 
				g_file_list->dst_name);
		system(buf);
		g_file_list = g_file_list->next;
        printf("\nWARN: bypass %sn", g_file_list->src_name);
	}

    if (g_file_list)
    {

        FILE *fsrc;
        FILE *fdst;
		fsrc = fopen(g_file_list->src_name, "r");
		fdst = fopen(g_file_list->dst_name, "w");
		g_file_list->src = fsrc;
		g_file_list->dst = fdst;
        yyin = g_file_list->src;
        printf("\n====BEGIN parsing %s =====\n", g_file_list->src_name);
        lineno = 0;
        yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
		BEGIN PLAINTEXT;
        return;
    }
	yyin = 0;
}

char *current_file_name()
{
    if (!g_file_list) 
        return 0;
    return g_file_list->src_name;
}
//#define DBG_PUSH_POP
char *pop_ifdef_string()
{
    char *s;
    if (!ifdef_string)
    {
        printf("ERROR! no ifdef string to pop, at L(%d)\n", lineno);
        return 0;
    }
    s = ifdef_string->string;
    ifdef_string = ifdef_string->next;
#ifdef DBG_PUSH_POP
    printf("pop ifdef string:%s\n", s);
#endif
    return s;
}
char *pop_else_string()
{
    char *s;
    if (!else_string)
    {
        printf("ERROR! no else string to pop, at L(%d)\n", lineno);
        return 0;
    }
    s = else_string->string;

    else_string = else_string->next;
#ifdef DBG_PUSH_POP
    printf("pop else string:%s\n", s);
#endif
    return s;
}
char *pop_endif_string()
{
    char *s;
    if (!endif_string)
    {
        printf("ERROR! no endif string to pop, at L(%d)\n", lineno);
        return 0;
    }
    s = endif_string->string;
    endif_string = endif_string->next;
#ifdef DBG_PUSH_POP
    printf("pop endif string:%s\n", s);
#endif
    return s;
}
void push_ifdef_string(char *s)
{
    StringStack *ss = (StringStack *)malloc(sizeof(StringStack));
    ss->next = ifdef_string;
    ss->string = s;
#ifdef DBG_PUSH_POP
    printf("push ifdef string %s\n", s);
#endif
    ifdef_string = ss;
}
void push_else_string(char *s)
{
    StringStack *ss = (StringStack *)malloc(sizeof(StringStack));
    ss->next = else_string;
    ss->string = s;
#ifdef DBG_PUSH_POP
    printf("push else string %s\n", s);
#endif
    else_string = ss;
}
void push_endif_string(char *s)
{
    StringStack *ss = (StringStack *)malloc(sizeof(StringStack));
    ss->next = endif_string;
    ss->string = s;
#ifdef DBG_PUSH_POP
    printf("push endif string:%s\n", s);
#endif
    endif_string = ss;
}

static void add_exclude_files(char *fname)
{
	StringStack *ss = (StringStack *)malloc(sizeof(StringStack));
	ss->string = strdup(fname);
	ss->next = exclude_files;
	exclude_files = ss;
}

static void add_bypass_files(char *fname)
{
	StringStack *ss = (StringStack *)malloc(sizeof(StringStack));
	ss->string = strdup(fname);
	ss->next = bypass_files;
	bypass_files = ss;
}

static void add_filter_suffix(char *suffix)
{
	StringStack *ss = (StringStack *)malloc(sizeof(StringStack));
	ss->string = strdup(suffix);
	ss->next = filter_suffix;
	filter_suffix = ss;
}

static int in_exclude_files(char *fname)
{
	StringStack *ss = exclude_files;
	while(ss)
	{
		if (!strcmp(ss->string, fname))
			return 1;
				
		ss = ss->next;
	}
	return 0;
}

static int in_bypass_files(char *fname)
{
	StringStack *ss = bypass_files;
	while(ss)
	{
		if (!strcmp(ss->string, fname))
			return 1;
				
		ss = ss->next;
	}
	return 0;
}

static int in_filter_suffix(char *fname)
{
	StringStack *ss = filter_suffix;
	int flen = strlen(fname);
	char *suffix = (char *)(fname+flen);

	while(suffix > fname)
	{
		if (*(--suffix) == '.')
			break;
	}

	if (suffix == fname) 
		return 0; 

	while(ss)
	{
	    
		if (!strcmp(ss->string, suffix))
		{
	        //printf("suffix===>%s\n", suffix);
			return 1;
		}
				
		ss = ss->next;
	}
	return 0;
}

#if 0
int process_file(char *fname)
{
    FILE_INFO *node = malloc(sizeof(FILE_INFO));
    char out_name[128];
    FILE *fp = NULL, *fout = NULL;
    if (!fname)
    {
        printf("\nERROR! L%d: include NULL file name.\n", lineno);
        return 0;
    }
    if (!strcmp(fname, "STDIN"))
    {
        fp = stdin;
    } else {
        fp = fopen(fname, "r"); 
        if (!fp)
        {
            printf("ERROR, can't open %s for read\n", fname);
            return 0;
        }
    }
    sprintf(out_name, "%s.out", fname);
    fout = fopen(out_name, "w"); 
    if (!fout)
    {
        printf("ERROR, can't open %s for write\n", out_name);
        return 0;
    }

    yyin = fp;
    node->fp = fp;
    node->fout = fout;
    node->fname = strdup(fname);
    //node->saved_state = YY_CURRENT_BUFFER;

    g_include_files = node;
    lineno=1;
    
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
    return 1;
}
#endif
static int
comment(int retString)
{
    char c, c1;
    char *buf, *p;

	buf = malloc(MAX_BUF_SIZE);
	memset(buf, 0, MAX_BUF_SIZE);
	p = buf;

    strcpy(p, yytext);
    p += yyleng;
loop:

    while ((c = input()) != '*' && c != 0)
    {
        if (c == '\n') lineno++;

        *(p++) = c;
        //putchar(c);
    }
    if (c == '*')
        *(p++) = c;

    if ((c1 = input()) != '/' && c != 0)
    {
        unput(c1);
        goto loop;
    }

    if (c1 == '/')
        *(p++) = c1;
        //putchar(c1);

    *p = 0x00;

	if (p-buf > MAX_BUF_SIZE)
	{
		printf("ERROR, internal buffer overflow:"
				"%d > %d, at %s,L(%d)\n", (p-buf), MAX_BUF_SIZE,
				current_file_name(), lineno);
		assert(p-buf <= MAX_BUF_SIZE);
	}

    if (retString)
    {
        yylval.string = strdup(buf);
		free(buf);
        return STRING;
    }
	free(buf);
    return 0;
}

static int ignore(int retString)
{
    char c;
    char *buf, *p;

	buf = malloc(MAX_BUF_SIZE);
    memset(buf, 0, MAX_BUF_SIZE);
	p = buf;
    strcpy(p, yytext);
    p += yyleng;

loop:
    while ((c = input()) != '\n' && c != 0 && c != EOF)
    {
        *(p++) = c;
        //putchar(c);
    }


	// don't unput EOF, which is identified as any char 
	// "." => ignore(1) => infinite loop
	if (c != EOF) 
		unput(c);

    *p = 0x00;

	if (p-buf > MAX_BUF_SIZE)
	{
		printf("ERROR, internal buffer overflow:"
				"%d > %d, at %s,L(%d)\n", (p-buf), MAX_BUF_SIZE,
				current_file_name(), lineno);
		assert(p-buf <= MAX_BUF_SIZE);
	}

    if (retString)
    {
        yylval.string = strdup(buf);
        //printf("\nignore: [%s]\n", yylval.string);
		free(buf);
        return (STRING);
    }
	free(buf);
    return 0;

}

static char *save_to_NL()
{
    char c, prev;
    char *buf, *p, *start;
	int unput_NL = 0;

	buf = malloc(MAX_BUF_SIZE);
    memset(buf, 0, MAX_BUF_SIZE);
	p = buf;
    strcpy(p, yytext);
    p += yyleng;
    start = p;

loop:
    while ((c = input()) != '\n' && c != 0 && c != EOF )
    {
        *(p++) = c;
        //putchar(c);
		prev = c;
    }
	if (c != EOF)
		*(p++) = c;
	if (c == '\n' && prev == '\\')
		goto loop;

    *p = 0x00;

	if (p-buf > MAX_BUF_SIZE)
	{
		printf("ERROR, internal buffer overflow:"
				"%d > %d, at %s,L(%d)\n", (p-buf), MAX_BUF_SIZE,
				current_file_name(), lineno);
		assert(p-buf <= MAX_BUF_SIZE);
	}


	unput_NL=0;
    while(p > start)
	{
		char uc = *--p;

		// only unput 1st NL
		if (uc == '\n' && !unput_NL)
		{
			unput(uc);
			//printf("unput [%c:%x]\n", uc, uc);
			unput_NL = 1;
		}
		else if (uc != '\n' && uc != '\\')
		{
			unput(uc);
			//printf("unput [%c:%x]\n", uc, uc);
		}

	}

	//printf("===========\n");
	p = strdup(buf);
	free(buf);

    return p;
}



void count()
{
    int i;

    for (i = 0; yytext[i] != '\0'; i++)
        if (yytext[i] == '\n')
        {
            //printf("\nline:%d\n", lineno);
            lineno++;
            column = 0;
        }
        else if (yytext[i] == '\t')
            column += 8 - (column % 8);
        else
            column++;

    //ECHO;
}

static 
int create_dir_stack(char *src_dir_name, char *dst_dir_name, char bypassAll) {
    DIR *pDIR;
    struct dirent *pDirEnt;

    /* Open the current directory */
    pDIR = opendir(src_dir_name);

    if ( pDIR == NULL ) {
        fprintf( stderr, "%s %d: opendir(%s) failed (%s)\n",
                 __FILE__, __LINE__, src_dir_name, strerror( errno ));
        exit( -1 );
    }

    /* Get each directory entry from pDIR and print its name */

    pDirEnt = readdir( pDIR );
    //printf("====== SRC src ====\n");
    while ( pDirEnt != NULL ) {
		char isBypass = bypassAll;
		if (in_exclude_files(pDirEnt->d_name))
		{
			printf("WARN: exclude file/dir=%s\n", pDirEnt->d_name);
			pDirEnt = readdir( pDIR );
			continue;
		}
		if (in_bypass_files(pDirEnt->d_name))
		{
			printf("WARN: bypass file/dir=%s\n", pDirEnt->d_name);
			isBypass = 1;
		}
        if (strcmp(pDirEnt->d_name, ".") && strcmp(pDirEnt->d_name, ".."))
        {
            char src_path[256], dst_path[256];
            int  slen = strlen(src_dir_name);
            int  dlen = strlen(dst_dir_name);
            if (src_dir_name[slen-1] == '/')
                sprintf(src_path, "%s%s", src_dir_name, pDirEnt->d_name);
            else
                sprintf(src_path, "%s/%s", src_dir_name, pDirEnt->d_name);

            if (dst_dir_name[dlen-1] == '/')
                sprintf(dst_path, "%s%s", dst_dir_name, pDirEnt->d_name);
            else
                sprintf(dst_path, "%s/%s", dst_dir_name, pDirEnt->d_name);
            //printf("src_path=%s\n", src_path);
            //if (flen > 2)
            //    printf("suffix=%s\n", &pDirEnt->d_name[flen-2]);
            if(is_dir(src_path, dst_path))
            {
                char buf[275]; 
                sprintf(buf, "mkdir -p %s", dst_path);
                system(buf);

                create_dir_stack(src_path, dst_path, isBypass);
                printf("src dir=> %s\n", src_path);
                printf("dst dir=> %s\n", dst_path);
            }
            else if (in_filter_suffix(pDirEnt->d_name))
				/*
                (!strcmp(&pDirEnt->d_name[flen-2], ".c") ||
                 !strcmp(&pDirEnt->d_name[flen-2], ".h")))
				 */
            {
                //printf("file=>%s\n", pDirEnt->d_name);
                FILE *fsrc = fopen(src_path, "r");
                FILE *fdst = fopen(dst_path, "w");
                if (!fsrc)
                    printf("ERROR! can't open %s for read\n", src_path);
                if (!fdst)
                    printf("ERROR! can't open %s for write\n", dst_path);
                if (fsrc && fdst)
                {
                    FLIST *flist = malloc(sizeof(FLIST));
					fclose(fsrc);
					fclose(fdst);
                    flist->src   = 0; //fsrc;
                    flist->dst   = 0; //fdst;
                    flist->src_name = strdup(src_path);
                    flist->dst_name = strdup(dst_path);
                    flist->isBypass = isBypass;
                    flist->next  = g_file_list;
                    g_file_list  = flist;
                }
            }
			else
			{
                char buf[520]; 

				// copy file from src to dst directly
                sprintf(buf, "\\cp -f %s %s", src_path, dst_path);
				system(buf);
                //printf("%s\n", buf);
			}
        }
        pDirEnt = readdir( pDIR );
    }

    /* Release the open directory */

    closedir( pDIR );

    return 0;
}

int is_dir(const char * dirname)    
{    
    struct stat sDir; 
    if (stat(dirname ,&sDir) < 0 )    
        return 0; 
    if (S_IFDIR == (sDir.st_mode & S_IFMT))    
        return 1; 
    return 0; 
} 

