/**
 * This file may be included by any other *.c or *.h file.
 * it provide functions and preprocessor macros that could be used most commonly.
 *
 * Author: RuiXiao <xrfind@gmail.com>
 */
#ifndef CNRT_BASE_H
#define CNRT_BASE_H

/****************Configuration Section*******************************************************************/
//#define NDEBUG
//VERBOSE_LEVEL has 7 values now, 0, 1, 2, 3, 4, 5, 6.
//we have eight different print functions: 
//		printm: only be used in main function.
//		printgfb&printgfe: only be used at the beginning and end of the global functions.
//		printgf: used in the global functions.
//		printsfb&printsfe: only be used at the beginning and end of the static functions.
//		printsf: used in the static functions.
//		printlp: used in loop, e.g. for, while.
//if VERBOSE_LEVEL == 1, only printm will output.
//if VERBOSE_LEVEL == 2, printgfb&printgfe will output, too.
//if VERBOSE_LEVEL == 3, printgf will output, too.
//if VERBOSE_LEVEL == 4, printsfb&printsfe will output, too.
//if VERBOSE_LEVEL == 5, printsf will output, too.
//if VERBOSE_LEVEL == 6, printlp will output, too.
//if VERBOSE_LEVEL == 0, all printxxx stop output.
#define VERBOSE_LEVEL 0
/********************************************************************************************************/


/********assert should be used very offen.***************************************************************/
#include <assert.h>
/********************************************************************************************************/

/********use everywhere.*********************************************************************************/
void print_time(void);
void print_label(int i);
/********************************************************************************************************/

/*******use everywhere.**********************************************************************************/
#include <stdio.h> //for FILE, perror, fprintf, stderr
void fileError(FILE *fp, char* format, ...);
void iserror(char *format, ...);
#define isError(format, ...) do {\
		fprintf(stderr, "[ERROR]:\n\t%s ==> ", __func__);\
		iserror(format, ##__VA_ARGS__);\
		fprintf(stderr, "\n");\
} while(0)
/********************************************************************************************************/

/*******some time, I use*********************************************************************************/
double dmin(double a, double b);
double dmax(double a, double b);
int imin(int a, int b);
int imax(int a, int b);
long lmin(long a, long b);
long lmax(long a, long b);
/********************************************************************************************************/

/*********use everywhere.********************************************************************************/
void *malloc_safe(size_t size, const char *funcname, const char *filename, const int lineNum);
void *calloc_safe(size_t num, size_t size, const char *funcname, const char *filename, const int lineNum);
void *realloc_safe(void *p, size_t size, const char *funcname, const char *filename, const int lineNum);
#define smalloc(n) malloc_safe(n, __func__, __FILE__, __LINE__)
#define scalloc(n, s) calloc_safe(n, s, __func__, __FILE__, __LINE__)
#define srealloc(p, n) realloc_safe(p, n, __func__, __FILE__, __LINE__)
/********************************************************************************************************/

/********check some prerequisite*************************************************************************/
void prerequisite(void);
/********************************************************************************************************/

/********************************************************************************************************/
//print1l will be used in main function. and some special situation.
//print2l will be used in each global functions' begin and end position.
//print3l will be used to describ the logic in global function 
//print4l will be used in the static functions' begin&end position. but if static function is used in a loop, use printnl instead.
//print5l will be used in describe the logic in static function
//printnl will be used in loop.
#define printm(format, ...) ((void)0)
#define printgfb(format, ...) ((void)0)
#define printgfe(format, ...) ((void)0)
#define printgf(format, ...) ((void)0)
#define printsfb(format, ...) ((void)0)
#define printsfe(format, ...) ((void)0)
#define printsf(format, ...) ((void)0)
#define printlp(format, ...) ((void)0)

#if VERBOSE_LEVEL >= 1
#undef printm
#define printm(format, ...) do {\
		printf("[level 1] ");\
		printf("%s ==> ", __func__);\
		printf(format, ##__VA_ARGS__);\
} while(0)
#if VERBOSE_LEVEL >= 2
#undef printgfb
#define printgfb() do {\
		printf("[level 2] %s =>> begin......\n", __func__);\
} while(0)
#undef printgfe
#define printgfe() do {\
		printf("[level 2] %s =>> ........end\n", __func__);\
} while(0)
#if VERBOSE_LEVEL >= 3
#undef printgf
#define printgf(format, ...) do {\
		printf("[level 3] %s =>> ", __func__);\
		printf(format, ##__VA_ARGS__);\
} while(0)
#if VERBOSE_LEVEL >= 4
#undef printsfb
#define printsfb() do {\
		printf("[level 4] %s =>> begin......\n", __func__);\
} while(0)
#undef printsfe
#define printsfe() do {\
		printf("[level 4] %s =>> ........end\n", __func__);\
} while(0)
#if VERBOSE_LEVEL >= 5
#undef printsf
#define printsf(format, ...) do {\
		printf("[level 5] %s =>> ", __func__);\
		printf(format, ##__VA_ARGS__);\
} while(0)
#if VERBOSE_LEVEL >= 6
#undef printlp
#define printlp(format, ...) do {\
		printf("[level 6] %s =>> ", __func__);\
		printf(format, ##__VA_ARGS__);\
} while(0)
#endif
#endif
#endif
#endif
#endif
#endif
/********************************************************************************************************/

#endif
