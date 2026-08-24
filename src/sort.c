#include <stdio.h>
#include <stdlib.h>

int input(int* array, int n);
void sort(int* array, int n);
void output(int* array, int n);

int main(void) {
    int n;
    int* array = NULL;

    if (scanf("%d", &n) == 1 && n > 0) array = malloc(n * sizeof(int));
    if (array != NULL && input(array, n)) {
        sort(array, n);
        output(array, n);
    } else {
        printf("n/a");
    }
    free(array);

    return 0;
}

int input(int* array, int n) {
    int result = 1;
    for (int i = 0; result && i < n; i++) {
        if (scanf("%d", array + i) != 1) result = 0;
    }
    return result;
}

void sort(int* array, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (*(array + j) > *(array + j + 1)) {
                int temp = *(array + j);
                *(array + j) = *(array + j + 1);
                *(array + j + 1) = temp;
            }
        }
    }
}

void output(int* array, int n) {
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%d", *(array + i));
    }
}
