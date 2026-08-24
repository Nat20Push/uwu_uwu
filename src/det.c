#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-7

double** allocate_matrix(int n);
void free_matrix(double** matrix, int n);
double det(double** matrix, int n, int m);
int input(double** matrix, int n, int m);
void output(double value);

int main(void) {
    int n = 0;
    int m = 0;
    double** matrix = NULL;

    if (scanf("%d%d", &n, &m) == 2 && n > 0 && n == m) matrix = allocate_matrix(n);
    if (matrix != NULL && input(matrix, n, m)) {
        output(det(matrix, n, m));
    } else {
        printf("n/a");
    }
    free_matrix(matrix, n);

    return 0;
}

double** allocate_matrix(int n) {
    double** matrix = calloc(n, sizeof(double*));

    for (int i = 0; matrix != NULL && i < n; i++) {
        matrix[i] = malloc(n * sizeof(double));
        if (matrix[i] == NULL) {
            free_matrix(matrix, n);
            matrix = NULL;
        }
    }
    return matrix;
}

void free_matrix(double** matrix, int n) {
    if (matrix != NULL) {
        for (int i = 0; i < n; i++) free(matrix[i]);
        free(matrix);
    }
}

int input(double** matrix, int n, int m) {
    int result = 1;
    for (int i = 0; result && i < n; i++) {
        for (int j = 0; result && j < m; j++) {
            if (scanf("%lf", &matrix[i][j]) != 1) result = 0;
        }
    }
    return result;
}

double det(double** matrix, int n, int m) {
    double result = 1.0;
    int sign = 1;

    if (n != m) result = 0.0;
    for (int column = 0; column < n && fabs(result) > EPS; column++) {
        int row = column;
        for (int i = column + 1; i < n; i++) {
            if (fabs(matrix[i][column]) > fabs(matrix[row][column])) row = i;
        }
        if (fabs(matrix[row][column]) < EPS) {
            result = 0.0;
        } else {
            if (row != column) {
                double* temp = matrix[row];
                matrix[row] = matrix[column];
                matrix[column] = temp;
                sign = -sign;
            }
            for (int i = column + 1; i < n; i++) {
                double factor = matrix[i][column] / matrix[column][column];
                for (int j = column; j < n; j++) matrix[i][j] -= factor * matrix[column][j];
            }
            result *= matrix[column][column];
        }
    }
    return result * sign;
}

void output(double value) {
    if (fabs(value) < EPS) value = 0.0;
    printf("%.6f", value);
}
