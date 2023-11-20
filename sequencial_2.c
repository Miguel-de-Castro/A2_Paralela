#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

int m1[SIZE][SIZE], m2[SIZE][SIZE], mres[SIZE][SIZE];
int l1, c1, l2, c2, lres, cres;

int main(int argc, char *argv[]) {
    int i, j, k, id, p;
    double elapsed_time;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    elapsed_time = -MPI_Wtime();

    if (p < 2) {
        fprintf(stderr, "Este programa requer pelo menos 2 processos MPI.\n");
        MPI_Finalize();
        exit(1);
    }

    // INICIALIZA OS ARRAYS A SEREM MULTIPLICADOS
    l1 = c1 = SIZE;
    l2 = c2 = SIZE;
    if (c1 != l2) {
        fprintf(stderr, "Impossivel multiplicar matrizes: parametros invalidos.\n");
        MPI_Finalize();
        return 1;
    }
    lres = l1;
    cres = c2;
    k = 1;

    if (id == 0) {
        // Código do Mestre
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                if (k % 2 == 0)
                    m1[i][j] = -k;
                else
                    m1[i][j] = k;
            }
            k++;
        }
        k = 1;
        for (j = 0; j < SIZE; j++) {
            for (i = 0; i < SIZE; i++) {
                if (k % 2 == 0)
                    m2[i][j] = -k;
                else
                    m2[i][j] = k;
            }
            k++;
        }

        // Envie a matriz m2 para os escravos
        MPI_Bcast(m2, SIZE * SIZE, MPI_INT, 0, MPI_COMM_WORLD);

        // Envie partes da matriz m1 para os escravos
        int chunk_size = l1 / (p - 1);
        for (i = 1; i < p; i++) {
            int start_row = (i - 1) * chunk_size;
            int end_row = (i == p - 1) ? l1 : start_row + chunk_size;
            MPI_Send(&start_row, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end_row, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&(m1[start_row][0]), (end_row - start_row) * SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Receba as linhas calculadas pelos escravos
        for (i = 1; i < p; i++) {
            int start_row = (i - 1) * chunk_size;
            int end_row = (i == p - 1) ? l1 : start_row + chunk_size;
            MPI_Recv(&(mres[start_row][0]), (end_row - start_row) * SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // VERIFICA SE O RESULTADO DA MULTIPLICACAO ESTA CORRETO
        for (i = 0; i < SIZE; i++)
        {
            k = SIZE * (i + 1);
            for (j = 0; j < SIZE; j++)
            {
                int k_col = k * (j + 1);
                if (i % 2 == 0)
                {
                    if (j % 2 == 0)
                    {
                        if (mres[i][j] != k_col){
                            printf("Erro 1 - i: %d, j: %d\n", i, j);
                            printf("%d",mres[i][j]);
                            printf("\n");
                            printf("%d",k_col);
                            printf("\n");
                            return 1;
                        }
                    }
                    else
                    {
                        if (mres[i][j] != -k_col){
                            printf("Erro 2 - i: %d, j: %d\n", i, j);
                            printf("%d",mres[i][j]);
                            printf("\n");
                            printf("%d",-k_col);
                            printf("\n");
                            return 1;
                        }
                    }
                }
                else
                {
                    if (j % 2 == 0)
                    {
                        if (mres[i][j] != -k_col){
                            printf("Erro 3 - i: %d, j: %d\n", i, j);
                            printf("%d",mres[i][j]);
                            printf("\n");
                            printf("%d",-k_col);
                            printf("\n");
                            return 1;
                        }
                    }
                    else
                    {
                        if (mres[i][j] != k_col){
                            printf("Erro 4 - i: %d, j: %d\n", i, j);
                            printf("%d",mres[i][j]);
                            printf("\n");
                            printf("%d",k_col);
                            printf("\n");
                            return 1;
                        }
                    }
                }
            }
        }
        printf("\nNodos: %d - Size: %d - Multiplicou certo!\n", p - 1, SIZE);

        // MOSTRA O TEMPO DE EXECUCAO
        printf("Tempo de execucao: %lf segundos\n", elapsed_time);
    } else {
        // Código do Escravo
        // Receba a matriz m2 do mestre
        MPI_Bcast(m2, SIZE * SIZE, MPI_INT, 0, MPI_COMM_WORLD);

        // Receba a parte da matriz m1 do mestre
        int start_row, end_row;
        MPI_Recv(&start_row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end_row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&(m1[start_row][0]), (end_row - start_row) * SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Realize a multiplicação parcial
        int chunk_size = end_row - start_row;
#pragma omp parallel for collapse(2)
        for (i = 0; i < chunk_size; i++) {
            for (j = 0; j < c2; j++) {
                mres[start_row + i][j] = 0;
                for (k = 0; k < c1; k++) {
                    mres[start_row + i][j] += m1[i][k] * m2[k][j];
                }
            }
        }

        // Envie as linhas calculadas de volta para o mestre
        MPI_Send(&(mres[start_row][0]), chunk_size * SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
