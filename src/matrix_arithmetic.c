#include <stdio.h>
#include <stdlib.h>

int** allocate_matrix(int n, int m);
void free_matrix(int** matrix);
int input(int** matrix, int n, int m);
void output(int** matrix, int n, int m);
int sum(int** first, int n1, int m1, int** second, int n2, int m2, int** result);
int mul(int** first, int n1, int m1, int** second, int n2, int m2, int** result);
void transpose(int** matrix, int n, int m, int** result);

int main(void) {
    int operation = 0;
    int n1 = 0;
    int m1 = 0;
    int n2 = 0;
    int m2 = 0;
    int** first = NULL;
    int** second = NULL;
    int** result = NULL;
    int correct =
        scanf("%d%d%d", &operation, &n1, &m1) == 3 && operation >= 1 && operation <= 3 && n1 > 0 && m1 > 0;

    if (correct) first = allocate_matrix(n1, m1);
    if (first == NULL || !input(first, n1, m1)) correct = 0;
    if (correct && operation != 3) {
        correct = scanf("%d%d", &n2, &m2) == 2 && n2 > 0 && m2 > 0;
        if (correct) second = allocate_matrix(n2, m2);
        if (second == NULL || !input(second, n2, m2)) correct = 0;
    }
    if (correct && operation == 1 && n1 == n2 && m1 == m2) result = allocate_matrix(n1, m1);
    if (correct && operation == 2 && m1 == n2) result = allocate_matrix(n1, m2);
    if (correct && operation == 3) result = allocate_matrix(m1, n1);
    if (result == NULL) correct = 0;
    if (correct && operation == 1) correct = sum(first, n1, m1, second, n2, m2, result);
    if (correct && operation == 2) correct = mul(first, n1, m1, second, n2, m2, result);
    if (correct && operation == 3) transpose(first, n1, m1, result);
    if (correct) {
        output(result, operation == 3 ? m1 : n1, operation == 2 ? m2 : m1);
    } else {
        printf("n/a");
    }
    free_matrix(first);
    free_matrix(second);
    free_matrix(result);
    return 0;
}

int** allocate_matrix(int n, int m) {
    int** matrix = malloc(n * sizeof(int*));
    int* data = malloc(n * m * sizeof(int));

    if (matrix != NULL && data != NULL) {
        for (int i = 0; i < n; i++) matrix[i] = data + i * m;
    } else {
        free(matrix);
        free(data);
        matrix = NULL;
    }
    return matrix;
}

void free_matrix(int** matrix) {
    if (matrix != NULL) {
        free(matrix[0]);
        free(matrix);
    }
}

int input(int** matrix, int n, int m) {
    int result = 1;
    for (int i = 0; result && i < n; i++) {
        for (int j = 0; result && j < m; j++) {
            if (scanf("%d", &matrix[i][j]) != 1) result = 0;
        }
    }
    return result;
}

void output(int** matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (j > 0) printf(" ");
            printf("%d", matrix[i][j]);
        }
        if (i < n - 1) printf("\n");
    }
}

int sum(int** first, int n1, int m1, int** second, int n2, int m2, int** result) {
    int correct = n1 == n2 && m1 == m2;
    for (int i = 0; correct && i < n1; i++)
        for (int j = 0; j < m1; j++) result[i][j] = first[i][j] + second[i][j];
    return correct;
}

int mul(int** first, int n1, int m1, int** second, int n2, int m2, int** result) {
    int correct = m1 == n2;
    for (int i = 0; correct && i < n1; i++) {
        for (int j = 0; j < m2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < m1; k++) result[i][j] += first[i][k] * second[k][j];
        }
    }
    return correct;
}

void transpose(int** matrix, int n, int m, int** result) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) result[j][i] = matrix[i][j];
}
