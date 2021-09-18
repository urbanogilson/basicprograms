#include <stdio.h>

#define COMMENT(x) fprintf(stdout, "----" x "----\n");

#define TEST(x)                                                                             \
    if (!(x))                                                                               \
    {                                                                                       \
        fprintf(stderr, "\033[31;1mFAILED:\033[22;39m %s:%d %s\n", __FILE__, __LINE__, #x); \
    }                                                                                       \
    else                                                                                    \
    {                                                                                       \
        fprintf(stdout, "\033[32;1mOK:\033[22;39m %s\n", #x);                               \
    }
