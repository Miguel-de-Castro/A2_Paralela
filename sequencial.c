// Arquivo: sequencial.c
// Autor    Roland Teodorowitsch

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MESTREID 0

// DADOS COMPARTILHADOS
int m1[SIZE][SIZE],m2[SIZE][SIZE],mres[SIZE][SIZE];
int l1, c1, l2, c2, lres, cres;

int main(int argc, char *argv[]) {
    int    i, j, k, id, p;
    double elapsed_time;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // PREPARA PARA MEDIR TEMPO
    elapsed_time = - MPI_Wtime ();

    if (id == MESTREID) {
        // INICIALIZA OS ARRAYS A SEREM MULTIPLICADOS   
        l1 = c1 = SIZE;
        l2 = c2 = SIZE;
        if (c1 != l2) {
            fprintf(stderr,"Impossivel multiplicar matrizes: parametros invalidos.\n");
            return 1;
        }
        lres = l1;
        cres = c2;
        k=1;
        for (i=0 ; i<SIZE; i++) {
            for (j=0 ; j<SIZE; j++) {
                if (k%2==0)
                m1[i][j] = -k;
                else
                m1[i][j] = k;
            }
            k++;
        }
        k=1;
        for (j=0 ; j<SIZE; j++) {
            for (i=0 ; i<SIZE; i++) {
                if (k%2==0)
                m2[i][j] = -k;
                else
                m2[i][j] = k;
            }
            k++;
        }

        // Envia a M2 
        MPI_Bcast(&m2, SIZE * SIZE, MPI_INT, MESTREID, MPI_COMM_WORLD);
    
    } else {
        MPI_Bcast(&m2, SIZE * SIZE, MPI_INT, MESTREID, MPI_COMM_WORLD);
        for (j = 0; j < SIZE; j++) {
            printf("id: %d - %d ", id, m2[i][j]);
        }
    }

/*
  // Enviar linhas atraves do MPI

  // REALIZA A MULTIPLICACAO

  // i = var1 / lres = var2 (MPI)
  for (i=0 ; i<lres; i++) { // OpenMP
      for (j=0 ; j<cres; j++) { 
          mres[i][j] = 0;
          for (k=0 ; k<c1; k++) { // Escravo
              mres[i][j] += m1[i][k] * m2[k][j];
          }
      }
  }
*/
  // OBTEM O TEMPO
  elapsed_time += MPI_Wtime ();
/*
  // VERIFICA SE O RESULTADO DA MULTIPLICACAO ESTA CORRETO
  for (i=0 ; i<SIZE; i++) {
      k = SIZE*(i+1);
      for (j=0 ; j<SIZE; j++) {
          int k_col = k*(j+1);
          if (i % 2 ==0) {
             if (j % 2 == 0) {
                if (mres[i][j]!=k_col)
                   return 1;
             }
             else {
                if (mres[i][j]!=-k_col)
                   return 1;
             }
          }
          else {
             if (j % 2 == 0) {
                if (mres[i][j]!=-k_col)
                   return 1;
             }
             else {
                if (mres[i][j]!=k_col)
                   return 1;
             }
          }
      } 
  }

*/
  // MOSTRA O TEMPO DE EXECUCAO
  printf("%lf",elapsed_time);
  MPI_Finalize();
  return 0;

}
