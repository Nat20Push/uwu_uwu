#include <stdio.h>

#define N 15
#define M 13

void transform(int* buf, int** matr, int n, int m);
void make_picture(int** picture, int n, int m);
void reset_picture(int** picture, int n, int m);
void output(int** picture, int n, int m);

int main(void) {
    int picture_data[N][M];
    int* picture[N];
    transform(*picture_data, picture, N, M);
    make_picture(picture, N, M);
    output(picture, N, M);

    return 0;
}

void make_picture(int** picture, int n, int m) {
    int frame_w[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int frame_h[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int tree_trunk[] = {7, 7, 7, 7};
    int tree_foliage[] = {3, 3, 3, 3};
    int sun_data[6][5] = {{0, 6, 6, 6, 6}, {0, 0, 6, 6, 6}, {0, 0, 6, 6, 6},
                          {0, 6, 0, 0, 6}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};

    reset_picture(picture, n, m);
    for (int j = 0; j < m; j++) {
        picture[0][j] = frame_w[j];
        picture[7][j] = frame_w[j];
        picture[14][j] = frame_w[j];
    }
    for (int i = 0; i < n; i++) {
        picture[i][0] = frame_h[i];
        picture[i][6] = frame_h[i];
        picture[i][12] = frame_h[i];
    }
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 5; j++) picture[i + 1][j + 7] = sun_data[i][j];
    for (int j = 0; j < 4; j++) {
        picture[3][j + 2] = tree_foliage[j];
        picture[4][j + 2] = tree_foliage[j];
        picture[10][j + 2] = tree_trunk[j];
    }
    for (int j = 0; j < 2; j++) {
        picture[2][j + 3] = tree_foliage[j];
        picture[5][j + 3] = tree_foliage[j];
        picture[6][j + 3] = tree_trunk[j];
        picture[8][j + 3] = tree_trunk[j];
        picture[9][j + 3] = tree_trunk[j];
    }
}

void reset_picture(int** picture, int n, int m) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) picture[i][j] = 0;
}

void transform(int* buf, int** matr, int n, int m) {
    for (int i = 0; i < n; i++) matr[i] = buf + i * m;
}

void output(int** picture, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (j > 0) printf(" ");
            printf("%d", picture[i][j]);
        }
        if (i < n - 1) printf("\n");
    }
}
