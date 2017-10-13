#ifndef __LOGER__H__
#define __LOGER__H__
#include <stdio.h>
#define DEBUG_ERROR(...) printf( __VA_ARGS__); printf("\n")
#define DEBUG_LOG(...) printf( __VA_ARGS__); printf("\n")
#endif