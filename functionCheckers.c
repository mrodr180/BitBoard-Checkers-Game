#include <stdio.h>


unsigned long long setBits(unsigned long long value, int position) {
    if (position < 0 || position >= 64)
        return value;
    return value | (1ull << position);

}

unsigned long long setBitZero(unsigned long long value, int position) {
    if (position < 0 || position >= 64)
        return value;
    return value & ~(1ull << position);
}

unsigned long long toggleBit(unsigned long long value, int position) {
    if (position < 0 || position >= 64)
        return value;
    return value ^ (1ull << position);
}

unsigned int getBitValue(unsigned long long value, int position) {
    if (position < 0 || position >= 64)
        return 0;
    return (unsigned int ) ((value >> position) & 1ull);
}

int countBits(unsigned long long value) {
    int count = 0;
    while (value) {
        count+= value & 1;
        value >>= 1;
    }
    return count;
}

unsigned long long shiftLeft(unsigned long long value, int positions) {
    return value << positions;
}

unsigned long long shiftRight(unsigned long long value, int positions) {
    return value >> positions;
}

void printBinary(unsigned long long value) {
    for (int i = 63; i >= 0; i--) {
        printf("%llu", (value >> i) & 1ull);
        if (i % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

void printHex(unsigned int value) {
    printf("0x%016llx", value);
}