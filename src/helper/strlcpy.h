#include <string.h>

size_t strlcpy(char *dst, const char *src, size_t siz) {
    const char *s = src;
    size_t left = siz;

    if (left) {
        while (--left != 0) {
            if ((*dst++ = *s++) == '\0')
                break;
        }
    }

    if (left == 0) {
        if (siz != 0)
            *dst = '\0';  // Null terminate if size is not zero
        while (*s++)
            ;
    }

    return(s - src - 1);
}
