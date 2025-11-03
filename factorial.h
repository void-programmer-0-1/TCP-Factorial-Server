#ifndef _FACTORIAL_H_
#define _FACTORIAL_H_

#define MAX_DIGITS 20000

typedef struct{
    int digits[MAX_DIGITS];
    int nd;
} bigint_t;

void bigint_init(bigint_t *b);
int bigint_mul_small(bigint_t *b, int m);
int compute_factorial_big(int n, bigint_t* out);
char *bigint_to_str(const bigint_t *b);

#endif