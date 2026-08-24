#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-7

double** allocate_matrix(int n);
void free_matrix(double** matrix, int n);
int invert(double** matrix, int n, int m, double** result);
int input(double** matrix, int n, int m);
void output(double** matrix, int n, int m);

int main(void) {
    int n = 0;
    int m = 0;
    double** matrix = NULL;
    double** result = NULL;

    if (scanf("%d%d", &n, &m) == 2 && n > 0 && n == m) {
        matrix = allocate_matrix(n);
        result = allocate_matrix(n);
    }
    if (matrix != NULL && result != NULL && input(matrix, n, m) && invert(matrix, n, m, result)) {
        output(result, n, m);
    } else {
        printf("n/a");
    }
    free_matrix(matrix, n);
    free_matrix(result, n);

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

int invert(double** matrix, int n, int m, double** result) {
    int correct = n == m;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) result[i][j] = i == j;
    for (int column = 0; correct && column < n; column++) {
        int row = column;
        for (int i = column + 1; i < n; i++) {
            if (fabs(matrix[i][column]) > fabs(matrix[row][column])) row = i;
        }
        if (fabs(matrix[row][column]) < EPS) correct = 0;
        if (correct && row != column) {
            double* temp = matrix[row];
            matrix[row] = matrix[column];
            matrix[column] = temp;
            temp = result[row];
            result[row] = result[column];
            result[column] = temp;
        }
        if (correct) {
            double divisor = matrix[column][column];
            for (int j = 0; j < n; j++) {
                matrix[column][j] /= divisor;
                result[column][j] /= divisor;
            }
            for (int i = 0; i < n; i++) {
                if (i != column) {
                    double factor = matrix[i][column];
                    for (int j = 0; j < n; j++) {
                        matrix[i][j] -= factor * matrix[column][j];
                        result[i][j] -= factor * result[column][j];
                    }
                }
            }
        }
    }
    return correct;
}

void output(double** matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fabs(matrix[i][j]) < EPS) matrix[i][j] = 0.0;
            if (j > 0) printf(" ");
            printf("%.6f", matrix[i][j]);
        }
        if (i < n - 1) printf("\n");
    }
}
