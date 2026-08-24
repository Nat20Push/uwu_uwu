#include <stdio.h>

#define LEN 100

int input_number(int* number, int* length);
void output(int* number, int length);
int compare(int* number1, int length1, int* number2, int length2);
void sum(int* buff1, int len1, int* buff2, int len2, int* result, int* result_length);
void sub(int* buff1, int len1, int* buff2, int len2, int* result, int* result_length);

int main(void) {
    int number1[LEN];
    int number2[LEN];
    int result[LEN + 1];
    int length1;
    int length2;
    int result_length;

    if (input_number(number1, &length1) && input_number(number2, &length2)) {
        sum(number1, length1, number2, length2, result, &result_length);
        output(result, result_length);
        printf("\n");
        if (compare(number1, length1, number2, length2) >= 0) {
            sub(number1, length1, number2, length2, result, &result_length);
            output(result, result_length);
        } else {
            printf("n/a");
        }
    } else {
        printf("n/a");
    }

    return 0;
}
int input_number(int* number, int* length) {
    int character;
    int result = 1;
    int waiting_digit = 1;
    *length = 0;

    while ((character = getchar()) != '\n' && character != EOF) {
        if (waiting_digit && character >= '0' && character <= '9' && *length < LEN) {
            *(number + *length) = character - '0';
            (*length)++;
            waiting_digit = 0;
        } else if (!waiting_digit && character == ' ') {
            waiting_digit = 1;
        } else {
            result = 0;
        }
    }
    if (*length == 0 || waiting_digit) result = 0;
    return result;
}

void output(int* number, int length) {
    int start = 0;
    while (start < length - 1 && *(number + start) == 0) start++;

    for (int i = start; i < length; i++) {
        if (i > start) printf(" ");
        printf("%d", *(number + i));
    }
}

int compare(int* number1, int length1, int* number2, int length2) {
    int start1 = 0;
    int start2 = 0;
    int result = 0;

    while (start1 < length1 - 1 && *(number1 + start1) == 0) start1++;
    while (start2 < length2 - 1 && *(number2 + start2) == 0) start2++;
    if (length1 - start1 > length2 - start2) result = 1;
    if (length1 - start1 < length2 - start2) result = -1;
    for (; start1 < length1 && result == 0; start1++, start2++) {
        if (*(number1 + start1) > *(number2 + start2)) result = 1;
        if (*(number1 + start1) < *(number2 + start2)) result = -1;
    }
    return result;
}

void sum(int* buff1, int len1, int* buff2, int len2, int* result, int* result_length) {
    int i = len1 - 1;
    int j = len2 - 1;
    int position = LEN;
    int carry = 0;

    while (i >= 0 || j >= 0 || carry > 0) {
        int value = carry;
        if (i >= 0) value += *(buff1 + i--);
        if (j >= 0) value += *(buff2 + j--);
        *(result + position--) = value % 10;
        carry = value / 10;
    }
    *result_length = LEN - position;
    for (int k = 0; k < *result_length; k++) *(result + k) = *(result + position + 1 + k);
}

void sub(int* buff1, int len1, int* buff2, int len2, int* result, int* result_length) {
    int i = len1 - 1;
    int j = len2 - 1;
    int position = LEN - 1;
    int borrow = 0;

    while (i >= 0) {
        int value = *(buff1 + i--) - borrow;
        if (j >= 0) value -= *(buff2 + j--);
        if (value < 0) {
            value += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        *(result + position--) = value;
    }
    *result_length = LEN - position - 1;
    for (int k = 0; k < *result_length; k++) *(result + k) = *(result + position + 1 + k);
}
