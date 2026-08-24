#include <stdio.h>
#include <stdlib.h>

#define NMAX 100

int** allocate_matrix(int type, int n, int m, int** static_rows);
void free_matrix(int** matrix, int type, int n);
int input(int** matrix, int n, int m);
void output(int** matrix, int n, int m);
void find_values(int** matrix, int n, int m, int* maximums, int* minimums);
void output_array(int* array, int n);

int main(void) {
    int type = 0;
    int n = 0;
    int m = 0;
    int static_data[NMAX][NMAX];
    int* static_rows[NMAX];
    int** matrix = NULL;
    int* maximums = NULL;
    int* minimums = NULL;

    if (scanf("%d%d%d", &type, &n, &m) == 3 && type >= 1 && type <= 4 && n > 0 && m > 0 &&
        (type != 1 || (n <= NMAX && m <= NMAX))) {
        for (int i = 0; i < n && type == 1; i++) static_rows[i] = static_data[i];
        matrix = allocate_matrix(type, n, m, static_rows);
        maximums = malloc(n * sizeof(int));
        minimums = malloc(m * sizeof(int));
    }
    if (matrix != NULL && maximums != NULL && minimums != NULL && input(matrix, n, m)) {
        find_values(matrix, n, m, maximums, minimums);
        output(matrix, n, m);
        printf("\n");
        output_array(maximums, n);
        printf("\n");
        output_array(minimums, m);
    } else {
        printf("n/a");
    }
    free(maximums);
    free(minimums);
    free_matrix(matrix, type, n);
    return 0;
}

int** allocate_matrix(int type, int n, int m, int** static_rows) {
    int** matrix = NULL;
    if (type == 1) {
        matrix = static_rows;
    } else if (type == 2) {
        matrix = calloc(n, sizeof(int*));
        for (int i = 0; matrix != NULL && i < n; i++) {
            matrix[i] = malloc(m * sizeof(int));
            if (matrix[i] == NULL) {
                free_matrix(matrix, type, n);
                matrix = NULL;
            }
        }
    } else if (type == 3) {
        matrix = malloc(n * sizeof(int*));
        int* data = malloc(n * m * sizeof(int));
        if (matrix != NULL && data != NULL) {
            for (int i = 0; i < n; i++) matrix[i] = data + i * m;
        } else {
            free(matrix);
            free(data);
            matrix = NULL;
        }
    } else if (type == 4) {
        matrix = malloc(n * sizeof(int*) + n * m * sizeof(int));
        if (matrix != NULL) {
            int* data = (int*)(matrix + n);
            for (int i = 0; i < n; i++) matrix[i] = data + i * m;
        }
    }
    return matrix;
}

void free_matrix(int** matrix, int type, int n) {
    if (matrix != NULL && type == 2) {
        for (int i = 0; i < n; i++) free(matrix[i]);
        free(matrix);
    } else if (matrix != NULL && type == 3) {
        free(matrix[0]);
        free(matrix);
    } else if (matrix != NULL && type == 4) {
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

void find_values(int** matrix, int n, int m, int* maximums, int* minimums) {
    for (int i = 0; i < n; i++) {
        maximums[i] = matrix[i][0];
        for (int j = 1; j < m; j++) {
            if (matrix[i][j] > maximums[i]) maximums[i] = matrix[i][j];
        }
    }
    for (int j = 0; j < m; j++) {
        minimums[j] = matrix[0][j];
        for (int i = 1; i < n; i++) {
            if (matrix[i][j] < minimums[j]) minimums[j] = matrix[i][j];
        }
    }
}

void output_array(int* array, int n) {
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%d", array[i]);
    }
}
