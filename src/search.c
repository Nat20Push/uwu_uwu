#include <math.h>
#include <stdio.h>

#define NMAX 30

int input(int* a, int* n);
double mean(int* a, int n);
double variance(int* a, int n);
int search(int* a, int n);

int main(void) {
    int n;
    int data[NMAX];

    if (input(data, &n)) {
        printf("%d", search(data, n));
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

int search(int* a, int n) {
    int result = 0;
    double mean_value = mean(a, n);
    double limit = mean_value + 3.0 * sqrt(variance(a, n));

    for (int i = 0; i < n && result == 0; i++) {
        int value = *(a + i);
        if (value != 0 && value % 2 == 0 && value >= mean_value && value <= limit) result = value;
    }
    return result;
}
