//#include <stdlib.h>
#pragma once

int** maze_create(int max_x, int max_y) {
    srand(12);
    int y;
    int x;
    int r;

    int** field = new int* [max_y];
    for (int i = 0; i < max_y; i++) {
        field[i] = new int[max_x];
    }
    for (int i = 0; i < max_y; i++) {
        for (int j = 0; j < max_x; j++) {
            field[i][j] = 0;
        }
    }

    for (x = 0; x < max_x; x++) {
        field[0][x] = 1;
        field[max_y - 1][x] = 1;
    }

    for (y = 0; y < max_y; y++) {
        field[y][0] = 1;
        field[y][max_x - 1] = 1;
    }

    y = 2;
    for (x = 2; x < max_x - 1; x = x + 2) {
        r = (rand() % 12) + 1;
        field[y][x] = 1;
        if (r >= 1 && r <= 3) {
            if (field[y - 1][x] == 0) {
                field[y - 1][x] = 1;
            }
            else if (field[y - 1][x] == 1) {
                x = x - 2;
            }
        }
        if (r >= 4 && r <= 6) {
            if (field[y + 1][x] == 0) {
                field[y + 1][x] = 1;
            }
            else if (field[y + 1][x] == 1) {
                x = x - 2;
            }
        }
        if (r >= 7 && r <= 9) {
            if (field[y][x - 1] == 0) {
                field[y][x - 1] = 1;
            }
            else if (field[y][x - 1] == 1) {
                x = x - 2;
            }
        }
        if (r >= 10 && r <= 12) {
            if (field[y][x + 1] == 0) {
                field[y][x + 1] = 1;
            }
            else if (field[y][x + 1] == 1) {
                x = x - 2;
            }
        }
    }

    for (y = 4; y < max_y - 1; y = y + 2) {
        for (x = 2; x < max_x - 1; x = x + 2) {
            r = (rand() % 12) + 1;
            field[y][x] = 1;
            if (r >= 1 && r <= 4) {
                if (field[y + 1][x] == 0) {
                    field[y + 1][x] = 1;
                }
                else if (field[y + 1][x] == 1) {
                    x = x - 2;
                }
            }
            if (r >= 5 && r <= 8) {
                if (field[y][x - 1] == 0) {
                    field[y][x - 1] = 1;
                }
                else if (field[y][x - 1] == 1) {
                    x = x - 2;
                }
            }
            if (r >= 9 && r <= 12) {
                if (field[y][x + 1] == 0) {
                    field[y][x + 1] = 1;
                }
                else if (field[y][x + 1] == 1) {
                    x = x - 2;
                }
            }
        }
    }
    return field;
}