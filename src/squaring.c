#include <stdio.h>

#define NMAX 10

int input(int* a, int* n);
void output(int* a, int n);
void squaring(int* a, int n);

int main(void) {
    int n;
    int data[NMAX];

    if (input(data, &n)) {
        squaring(data, n);
        output(data, n);
    } else {
        printf("n/a");
    }

    return 0;
}
int input(int* a, int* n) {
    int result = scanf("%d", n) == 1 && *n > 0 && *n <= NMAX;

    for (int i = 0; result && i < *n; i++) {
        if (scanf("%d", a + i) != 1) result = 0;
    }

    return result;
}

void output(int* a, int n) {
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%d", *(a + i));
    }
}

void squaring(int* a, int n) {
    for (int i = 0; i < n; i++) *(a + i) *= *(a + i);
}
