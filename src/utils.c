#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

void println(char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    printf("\n");
}
