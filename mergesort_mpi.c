#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void merge(int *, int *, int, int, int);
void mergeSort(int *, int *, int, int);

int main(int argc, char** argv) {
	
	/********** Criando e populando o array **********/
	int n = 1024;
	int *original_array = malloc(n * sizeof(int));
	
	int c;
	srand(time(NULL));
	printf("Esse é o vetor não ordenado: ");
	for(c = 0; c < n; c++) {
		original_array[c] = rand() % n;
		printf("%d ", original_array[c]);
		}

	printf("\n");
	printf("\n");
	
	/********** Inicializando o MPI **********/
	int world_rank;
	int world_size;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
		
	/********** Dividindo o array em particoes de tamanho igual **********/
	int size = n/world_size;
	
	/********** Enviando uma partição para cada processo **********/
	int *sub_array = malloc(size * sizeof(int));
	MPI_Scatter(original_array, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);
	
	/********** Executando o mergesort em cada processo **********/
	int *tmp_array = malloc(size * sizeof(int));
	mergeSort(sub_array, tmp_array, 0, (size - 1));
	
	/********** Juntando as partições  **********/
	int *sorted = NULL;
	if(world_rank == 0) {
		sorted = malloc(n * sizeof(int));	
		}
	
	MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);
	
	/********** Fazendo a chamada final para ordenar as partições **********/
	if(world_rank == 0) {
		
		int *other_array = malloc(n * sizeof(int));
		mergeSort(sorted, other_array, 0, (n - 1));
		
		/********** Printando o array ordenado **********/
		printf("Esse é o vetor ordenado: ");
		for(c = 0; c < n; c++) {
			printf("%d ", sorted[c]);
			}
			
		printf("\n");
		printf("\n");
			
		/********** Liberando raiz **********/
		free(sorted);
		free(other_array);
			
		}
	
	/********** Liberando o resto **********/
	free(original_array);
	free(sub_array);
	free(tmp_array);
	
	/********** Finalizando o MPI **********/
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	
	}

/********** Algoritmo do Mergesort **********/
void merge(int *a, int *b, int l, int m, int r) {
	
	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;
	
	while((h <= m) && (j <= r)) {	

		if(a[h] <= a[j]) {
			b[i] = a[h];
			h++;
			}
		else {
			b[i] = a[j];
			j++;
			}
		i++;
		}
		
	if(m < h) {
		for(k = j; k <= r; k++) {
			b[i] = a[k];
			i++;
			}	
		}	
	else {	
		for(k = h; k <= m; k++) {	
			b[i] = a[k];
			i++;	
			}		
		}
		
	for(k = l; k <= r; k++) {
		a[k] = b[k];	
		}	
	}

/********** Chamada recursiva ao mergesort **********/
void mergeSort(int *a, int *b, int l, int r) {
	int m;
	
	if(l < r) {	
		m = (l + r)/2;	
		mergeSort(a, b, l, m);
		mergeSort(a, b, (m + 1), r);
		merge(a, b, l, m, r);
		}	
	}
