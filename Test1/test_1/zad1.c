#include "zad1.h"

void memory(unsigned int rows, unsigned int cols)
{
    double **matrix;

    matrix = malloc(rows * sizeof(double*));

    for (size_t i = 0; i < rows; i++)
        matrix[i] = malloc(cols * sizeof(double));

    check_matrix(matrix, rows, cols);

    for (size_t i = 0; i < rows; i++)
        free(matrix[i]);

    free(matrix);
}

int main(int arc, char **argv)
{
    srand(42);
    unsigned int cols = rand() % 20;
    unsigned int rows = rand() % 20;
    memory(cols, rows);
    check_memory(cols, rows);
}
