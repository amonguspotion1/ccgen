#include "types.h"

#ifdef _WIN32
#include <windows.h>
#define RtlGenRandom SystemFunction036
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);
#pragma comment(lib, "advapi32.lib")
#else
#include <stdlib.h>
#include <stdio.h>
int linux_get_random(char* buffer, int length) {
    FILE* file;
    if ((file = fopen("/dev/urandom", "rb")) == NULL) {
        return 1;
    }
    if (!fread(buffer, length, 1, file)) return 1;
    fclose(file);
    return 0;
}
#endif

void fill_buf_random(char* buffer, int len) {
    int status;
    #ifdef _WIN32
    status = !RtlGenRandom(buffer, len);
    #else
    status = linux_get_random(buffer, len);
    #endif
    if (status) {
        exit(1);
    }
}

int RND_INITIALIZED = 0;
int RND_PTR = 0;
#define RND_BUF_LEN 1024
u32 RND_BUF[RND_BUF_LEN];

u32 rnd(u32 min, u32 max) {
    if (min > max) {
        u32 tmp = min;
        min = max;
        max = tmp;
    } else if (min == max) {
        return min;
    }

    if (RND_PTR >= RND_BUF_LEN || !RND_INITIALIZED) {
        RND_PTR = 0;
        RND_INITIALIZED = 1;
        fill_buf_random((char*)RND_BUF, RND_BUF_LEN * 4);
    }
    
    u32 num = RND_BUF[RND_PTR];
    RND_PTR += 1;
    u32 range = max - min;
    
    return (num % (range + 1)) + min;
}
