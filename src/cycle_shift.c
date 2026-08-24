#include <stdio.h>

#define NMAX 10

int input(int* a, int* n, int* shift);
void cycle_shift(int* a, int n, int shift);
void output(int* a, int n);

int main(void) {
    int n;
    int shift;
    int data[NMAX];

    if (input(data, &n, &shift)) {
        cycle_shift(data, n, shift);
        output(data, n);
    } else {
        printf("n/a");
    }

    return 0;
}
int input(int* a, int* n, int* shift) {
    int result = scanf("%d", n) == 1 && *n > 0 && *n <= NMAX;
    for (int i = 0; result && i < *n; i++) {
        if (scanf("%d", a + i) != 1) result = 0;
    }
    if (result && scanf("%d", shift) != 1) result = 0;
    return result;
}

void cycle_shift(int* a, int n, int shift) {
    shift %= n;
    if (shift < 0) shift += n;

    for (int step = 0; step < shift; step++) {
        int first = *a;
        for (int i = 0; i < n - 1; i++) *(a + i) = *(a + i + 1);
        *(a + n - 1) = first;
    }
}

void output(int* a, int n) {
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%d", *(a + i));
    }
}
