#include <stdint.h>
#include <stddef.h>

size_t strlen(const char* s) {
    const char* p;
    for (p = s; *p; p++)
        ;
    return p - s;
}

char* strcpy(char* dst, const char* src) {
    char* s = dst;
    while ((*s++ = *src++) != 0)
        ;
    return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    char* s = dst;
    while (n) {
        if ((*s = *src) != 0)
            src++;
        ++s;
        --n;
    }
    return dst;
}

char* strcat(char* dst, const char* src) {
    char* s = dst;
    while (*s++)
        ;
    --s;
    while ((*s++ = *src++) != 0)
        ;
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 == *s2) {
        if (!*s1++) {
            return 0;
        }
        ++s2;
    }

    return *s1 < *s2 ? -1 : 1;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && (*s1 == *s2)) {
        if (!*s1++) {
            return 0;
        }
        ++s2;
        --n;
    }

    return (n == 0) ? 0 : (*s1 - *s2);
}

void strreverse(char* begin, char* end) {
    char aux;
    while (end > begin)
        aux = *end, *end-- = *begin, *begin++ = aux;
}

void* memset(void* s, int c, size_t n) {
    char* p = (char*)s;

    while (n) {
        *p++ = (char)c;
        --n;
    }

    return s;
}

void* memmove(void* dst, const void* src, size_t n) {
    char* s = (char*)dst;
    const char* p = (const char*)src;

    if (p >= s) {
        while (n) {
            *s++ = *p++;
            --n;
        }
    } else {
        while (n) {
            --n;
            s[n] = p[n];
        }
    }

    return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
    char* r1 = out;
    const char* r2 = in;

    while (n) {
        *r1++ = *r2++;
        --n;
    }

    return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const char* r1 = (const char*)s1;
    const char* r2 = (const char*)s2;

    while (n && (*r1 == *r2)) {
        ++r1;
        ++r2;
        --n;
    }

    return (n == 0) ? 0 : ((*r1 < *r2) ? -1 : 1);
}
