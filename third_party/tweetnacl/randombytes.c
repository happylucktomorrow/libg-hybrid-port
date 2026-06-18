#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif

void randombytes(unsigned char* out, unsigned long long outlen) {
#if !defined(_WIN32)
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        unsigned long long offset = 0;
        while (offset < outlen) {
            ssize_t got = read(fd, out + offset, (size_t)(outlen - offset));
            if (got <= 0) {
                break;
            }
            offset += (unsigned long long)got;
        }
        close(fd);
        if (offset == outlen) {
            return;
        }
    }
#endif

    srand((unsigned int)(time(NULL) ^ (uintptr_t)out));
    for (unsigned long long i = 0; i < outlen; ++i) {
        out[i] = (unsigned char)(rand() & 0xff);
    }
}
