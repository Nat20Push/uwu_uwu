#include <stdio.h>

#define SIZE 10

int input(int* a);
void sort(int* a);
void output(int* a);

int main(void) {
    int data[SIZE];

    if (input(data)) {
        sort(data);
        output(data);
    } else {
        printf("n/a");
    }

    return 0;
}
int input(int* a) {
    int result = 1;
    for (int i = 0; i < SIZE && result; i++) {
        if (scanf("%d", a + i) != 1) result = 0;
    }
    return result;
}

void sort(int* a) {
    for (int i = 0; i < SIZE - 1; i++) {
        for (int j = 0; j < SIZE - i - 1; j++) {
            if (*(a + j) > *(a + j + 1)) {
                int temp = *(a + j);
                *(a + j) = *(a + j + 1);
                *(a + j + 1) = temp;
            }
        }
    }
}

void output(int* a) {
    for (int i = 0; i < SIZE; i++) {
        if (i > 0) printf(" ");
        printf("%d", *(a + i));
    }
}
