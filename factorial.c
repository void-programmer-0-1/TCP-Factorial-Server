#include "factorial.h"
#include <string.h>
#include <stdlib.h>


void bigint_init(bigint_t *b){
    memset(b->digits, 0, sizeof(b->digits));
    b->digits[0] = 1;
    b->nd = 1;
}

int bigint_mul_small(bigint_t *b, int m) {
    if (m <= 1) return 1;
    long carry = 0;
    for (int i = 0; i < b->nd; ++i) {
        long prod = (long)b->digits[i] * m + carry;
        b->digits[i] = prod % 10;
        carry = prod / 10;
    }
    while (carry) {
        if (b->nd >= MAX_DIGITS) return 0;
        b->digits[b->nd++] = carry % 10;
        carry /= 10;
    }
    return 1;
}

int compute_factorial_big(int n, bigint_t *out) {
    bigint_init(out);
    for (int i = 2; i <= n; ++i)
        if (!bigint_mul_small(out, i)) return 0;
    return 1;
}

char *bigint_to_str(const bigint_t *b) {
    int len = b->nd;
    char *s = malloc(len + 2);
    for (int i = 0; i < len; ++i)
        s[len - 1 - i] = '0' + b->digits[i];
    s[len] = '\n';
    s[len + 1] = '\0';
    return s;
}
