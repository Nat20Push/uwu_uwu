#include <stdio.h>

#define NMAX 10

int input(int* a, int* n);
void output(int* a, int n);
int max(int* a, int n);
int min(int* a, int n);
double mean(int* a, int n);
double variance(int* a, int n);
void output_result(int max_v, int min_v, double mean_v, double variance_v);

int main(void) {
    int n;
    int data[NMAX];

    if (input(data, &n)) {
        output(data, n);
        printf("\n");
        output_result(max(data, n), min(data, n), mean(data, n), variance(data, n));
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

int max(int* a, int n) {
    int result = *a;
    for (int i = 1; i < n; i++) {
        if (*(a + i) > result) result = *(a + i);
    }
    return result;
}

int min(int* a, int n) {
    int result = *a;
    for (int i = 1; i < n; i++) {
        if (*(a + i) < result) result = *(a + i);
    }
    return result;
}

double mean(int* a, int n) {
    double result = 0.0;
    for (int i = 0; i < n; i++) result += *(a + i);
    return result / n;
}

double variance(int* a, int n) {
    double result = 0.0;
    double mean_value = mean(a, n);

    for (int i = 0; i < n; i++) {
        double difference = *(a + i) - mean_value;
        result += difference * difference;
    }
    return result / n;
}

void output_result(int max_v, int min_v, double mean_v, double variance_v) {
    printf("%d %d %.6f %.6f", max_v, min_v, mean_v, variance_v);
}
