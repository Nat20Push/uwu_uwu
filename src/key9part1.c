#include <stdio.h>

#define NMAX 10

int input(int* buffer, int* length);
void output(int* buffer, int length);
int sum_numbers(int* buffer, int length);
int find_numbers(int* buffer, int length, int number, int* numbers);

int main(void) {
    int length;
    int buffer[NMAX];
    int numbers[NMAX];
    int even_count = 0;

    if (input(buffer, &length)) {
        for (int i = 0; i < length; i++) {
            if (*(buffer + i) != 0 && *(buffer + i) % 2 == 0) even_count++;
        }
        int sum = sum_numbers(buffer, length);
        int numbers_count = find_numbers(buffer, length, sum, numbers);
        if (even_count > 0 && numbers_count > 0) {
            printf("%d\n", sum);
            output(numbers, numbers_count);
        } else {
            printf("n/a");
        }
    } else {
        printf("n/a");
    }

    return 0;
}
int input(int* buffer, int* length) {
    int result = scanf("%d", length) == 1 && *length > 0 && *length <= NMAX;
    for (int i = 0; result && i < *length; i++) {
        if (scanf("%d", buffer + i) != 1) result = 0;
    }
    return result;
}

void output(int* buffer, int length) {
    for (int i = 0; i < length; i++) {
        if (i > 0) printf(" ");
        printf("%d", *(buffer + i));
    }
}

int sum_numbers(int* buffer, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        if (*(buffer + i) != 0 && *(buffer + i) % 2 == 0) sum += *(buffer + i);
    }
    return sum;
}

int find_numbers(int* buffer, int length, int number, int* numbers) {
    int count = 0;
    for (int i = 0; i < length; i++) {
        if (*(buffer + i) != 0 && number % *(buffer + i) == 0) {
            *(numbers + count) = *(buffer + i);
            count++;
        }
    }
    return count;
}
