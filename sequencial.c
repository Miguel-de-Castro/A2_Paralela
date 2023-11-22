// Arquivo: sequencial.c
// Autor    Roland Teodorowitsch

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

#define MESTREID 0

// DADOS COMPARTILHADOS
int m1[SIZE][SIZE], m2[SIZE][SIZE], mres[SIZE][SIZE];
int l1, c1, l2, c2, lres, cres;

int main(int argc, char *argv[])
{
    int i, j, k, id, p;
    double elapsed_time;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // INICIALIZA OS ARRAYS A SEREM MULTIPLICADOS
    l1 = c1 = SIZE;
    l2 = c2 = SIZE;
    if (c1 != l2)
    {
        fprintf(stderr, "Impossivel multiplicar matrizes: parametros invalidos.\n");
        return 1;
    }
    lres = l1;
    cres = c2;

    if (id == MESTREID)
    {
        k = 1;
        for (i = 0; i < SIZE; i++)
        {
            for (j = 0; j < SIZE; j++)
            {
                if (k % 2 == 0)
                    m1[i][j] = -k;
                else
                    m1[i][j] = k;
            }
            k++;
        }
        k = 1;
        for (j = 0; j < SIZE; j++)
        {
            for (i = 0; i < SIZE; i++)
            {
                if (k % 2 == 0)
                    m2[i][j] = -k;
                else
                    m2[i][j] = k;
            }
            k++;
        }
        

        // PREPARA PARA MEDIR TEMPO
        elapsed_time = -MPI_Wtime();

        // Envia a M2
        omp_set_num_threads(1);
        MPI_Bcast(&m2, SIZE * SIZE, MPI_INT, MESTREID, MPI_COMM_WORLD);

        int chunkSize = SIZE / (p - 1);

        for (int i = 0; i < p - 1; ++i)
        {
            int offset = i * chunkSize;
            // Ajuste para enviar linhas faltantes
            int chunkSizeToSend = chunkSize;
            if (i == p - 2)
            {
                chunkSizeToSend += SIZE % (p - 2);
            }
            // Envia dados
            MPI_Send(&offset, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&chunkSizeToSend, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&m1[offset][0], chunkSizeToSend * SIZE, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < p - 1; ++i)
        {
            int offset, senderID;
            MPI_Recv(&offset, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            senderID = status.MPI_SOURCE;
            MPI_Recv(&chunkSize, 1, MPI_INT, senderID, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&mres[offset][0], chunkSize * SIZE, MPI_INT, senderID, 0, MPI_COMM_WORLD, &status);
        }

        // OBTEM O TEMPO
        elapsed_time += MPI_Wtime();
        // MOSTRA O TEMPO DE EXECUCAO
        // printf("\nNodos: %d - Size: %d - Tempo: %lf \n", p - 1, SIZE, elapsed_time);

        // printf("Matriz Resultante Mestre (mres):\n");
        // for (int i = 0; i < SIZE; i++) {
        //     for (int j = 0; j < SIZE; j++) {
        //         printf("%d ", mres[i][j]);
        //     }
        //     printf("\n");
        // }

        // printf("\n");
        // printf("\n");

        // VERIFICA SE O RESULTADO DA MULTIPLICACAO ESTA CORRETO
        // printf("Matriz Verifica Mestre :\n");
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
                        // printf("%d ", k_col);
                        
                        if (mres[i][j] != k_col)
                            return 1;
                    }
                    else
                    {
                        // printf("%d ", -k_col);

                        if (mres[i][j] != -k_col)
                            return 1;
                    }
                }
                else
                {
                    if (j % 2 == 0)
                    {
                        // printf("%d ", -k_col);

                        if (mres[i][j] != -k_col)
                            return 1;
                    }
                    else
                    {
                        // printf("%d ", k_col);

                        if (mres[i][j] != k_col)
                            return 1;
                    }
                }
            }
            // printf("\n");
        }
        printf("\nMultiplicou certo!\nNodos: %d - Size: %d - Tempo: %lf \n", p - 1, SIZE, elapsed_time);
    }
    else
    {
        int numThreads = 16;
        if (id == p - 1)
        {
            numThreads--;
        }
        omp_set_num_threads(numThreads);

        MPI_Bcast(&m2, SIZE * SIZE, MPI_INT, MESTREID, MPI_COMM_WORLD);

        // printf("%d - Recebeu m2:\n", id);
        // for (int i = 0; i < SIZE; i++) {
        //     for (int j = 0; j < SIZE; j++) {
        //         printf("%d ", m2[i][j]);
        //     }
        //     printf("\n");
        // }

        // printf("\n");
        // printf("\n");

        int offset2, chunkSize2;
        MPI_Recv(&offset2, 1, MPI_INT, MESTREID, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&chunkSize2, 1, MPI_INT, MESTREID, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&m1[offset2][0], chunkSize2 * SIZE, MPI_INT, MESTREID, 0, MPI_COMM_WORLD, &status);

        // printf("%d - Recebeu m1:\n", id);
        // for (int i = 0; i < SIZE; i++) {
        //     for (int j = 0; j < SIZE; j++) {
        //         printf("%d ", m1[i][j]);
        //     }
        //     printf("\n");
        // }

        // printf("\n");
        // printf("\n");

        printf("Sou o %d, offset: %d, chunkSize: %d\n", id, offset2, chunkSize2);

#pragma omp parallel for
        for (i = offset2; i < chunkSize2 + offset2; i++)
        {

            for (j = 0; j < SIZE; j++)
            {
                mres[i][j] = 0;
                for (k = 0; k < SIZE; k++)
                {
                    mres[i][j] += m1[i][k] * m2[k][j];
                }
                // printf("i:, %d, j: %d, mres: %d\n", i, j, mres[i][j]);
            }
        }

#pragma omp barrier

        // printf("Matriz Resultante (mres):\n");
        // for (int i = 0; i < SIZE; i++) {
        //     for (int j = 0; j < SIZE; j++) {
        //         printf("%d - %d ", id, mres[i][j]);
        //     }
        //     printf("\n");
        // }

        printf("\n");

        MPI_Send(&offset2, 1, MPI_INT, MESTREID, 0, MPI_COMM_WORLD);
        MPI_Send(&chunkSize2, 1, MPI_INT, MESTREID, 0, MPI_COMM_WORLD);
        MPI_Send(&mres[offset2][0], chunkSize2 * SIZE, MPI_INT, MESTREID, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}