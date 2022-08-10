#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NODOS  2000

int *matriz [MAX_NODOS] ;

int clusters [MAX_NODOS] ;

char *mi_malloc (int nbytes) {
    char *p ;
    static long int nb = 0L ;
    static int nv = 0 ;

    p = malloc (nbytes) ;
    if (p == NULL) {
         fprintf (stderr, "Error, no queda memoria disponible para %d bytes mas\n", nbytes) ;
         fprintf (stderr, "Se han reservado %ld bytes en %d llamadas\n", nb, nv) ;
         exit (0) ;
    }

    nb += (long) nbytes ;
    nv++ ;

    return p ;
}

/* crea matriz: vector de punteros a vectores en memoria din�mica */
void crear_matriz (int **matriz) {
	int i ;
	
	for (i = 0 ; i < MAX_NODOS ; i++) {
		matriz [i] = (int *) mi_malloc (sizeof (int)*MAX_NODOS) ;
	}
}

/* 0 indica que no hay arista entre los nodos i-j */
void inicializar_grafo (int **matriz, int nodos) {
	int i ;
	int j ;
	
	for (i = 0 ; i < nodos ; i++) {
		for (j = 0 ; j < nodos ; j++) {
			matriz [i][j] = 0 ;
		}
	}
}

/* crea grafo con n nodos y a arcos: no se controlan los limites */
/* 1 indica que hay arista entre los nodos i-j */
void crear_grafo (int **matriz, int nodos, int arcos) {
	int i ;
	int p ;
	int q ;
	
	inicializar_grafo (matriz, nodos) ;
		
	for (i = 0 ; i < arcos ; i++) {
		do {
			p = rand () % nodos ;
			q = rand () % nodos ;			
		} while (p == q || matriz [p][q] != 0) ; // evitar diagonal y arcos existentes 
		matriz [p][q] = 1 ; 
		matriz [q][p] = 1 ; 	// arista sim�trica
	}
}

int asignaciones [MAX_NODOS] ;

unsigned long long sin_conflictos = 0L ;
unsigned long long con_conflictos = 0L ;


/* imprime nodos del grafo en diagonal y en las intersecciones si son adyacentes */
/* imprime antes el n�mero de grafo incoexo (cluster) si est� disponible y el color asignado, si lo hay */
void imprimir_grafo (int **matriz, int nodos) {
	int i ;
	int j ;
	
	for (i = 0 ; i < nodos ; i++) {
		printf ("%4d, clust:%3d:, col:%1d ", i, clusters [i], asignaciones [i]) ;
		for (j = 0 ; j < nodos ; j++) {
			if (i == j) {			// marca diagonal con punto
				printf (".") ;
			} else if (matriz [i][j] == 1) { 	// si i-j son adyacentes
				printf ("+") ;		// marca interseccion i-j con +
			} else printf (" ") ;
		}
		printf ("\n") ;
	}
}



int proximoValor(int k, int ind, int nodos) {
	asignaciones[ind]++;
	while (asignaciones[ind] <= k) {
		int conflictos = 0;
		for (int i = 0 ; i < nodos ; i++){
			if (matriz[i][ind] == 1 && asignaciones[i] == asignaciones[ind]){
				con_conflictos++;
				conflictos++;
			}
		}
		if (conflictos == 0) return asignaciones[ind];
		else asignaciones[ind]++;
	}
	return 0;
}

/* funcioo recursiva para generar todas las asignaciones posibles.
Cuando llega al nodo hoja llama a comprueba_conflictos para 
contabilizar si se trata de una soluci�n v�lida o fallida          */
/* Se ha realizado de forma recursiva como paso previo para usar backtracking */
void asigna_colores (int ind, int nodos, int k) {
	int j ;
	int cf ;

	if (ind == nodos) {
		sin_conflictos++;		
	}else {

		do {
			asignaciones[ind] = proximoValor(k, ind, nodos);
			if (asignaciones[ind] != 0){
				asigna_colores(ind+1,nodos,k);
			}
		} while (asignaciones[ind] != 0);
	}
}


void construye_grafo (int nodos, double arcospornodo) {
	int arcos ;
	
	arcos = nodos * arcospornodo ;	
	crear_grafo (matriz, nodos, arcos) ;	// crea grafo maximo
	imprimir_grafo (matriz, nodos) ;
}

/* funci�n que construye un grafo de 100 nodos con una cantidad de arcos
determinada por arcospornodo
Posteriormente comienza a analizar la colorabilidad de subgrafos 
de tama�o 10, 11, .., hasta 100.
De esa forma la proporcion de arcos por nodo es aproximadamente la que se esperaba
aunque hay variabilidad.
Se cronometra la duracion de la generacion exhaustiva de todas las asignaciones de color poisbles
sean v�lidas o no  */
void explora_k_colorabilidad (int k, double arcospornodo) {
	int nodos ;
	double proporcion ;

	clock_t start ;
	clock_t end ;
	double cpu_time_used ;

	construye_grafo (100, arcospornodo) ;
 		
	for (nodos = 10; nodos < 11 ; nodos++) {
		sin_conflictos = 0L ;
		con_conflictos = 0L ;
		
		start = clock () ;

		asigna_colores (0, nodos, k) ;

		end = clock () ; 

//		imprimir_grafo (matriz, nodos) ;	// si se genera primera solucion

		proporcion = (double) sin_conflictos ;
		proporcion /= (double) con_conflictos ;
		
		printf ("Nodos %3d, Soluciones %llu, Fallidas %llu, total %llu, proporcion %lf\n", nodos, sin_conflictos, con_conflictos, sin_conflictos+con_conflictos, proporcion) ;

		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf ("Tiempo %3f \n", cpu_time_used) ;
	}
}

int main (void) 
{
	srand (3) ;
	
	crear_matriz (matriz) ;
/*
	analizar_grafo (matriz, 4) ;
	analizar_grafo (matriz, 16) ;
	analizar_grafo (matriz, 32) ;
	analizar_grafo (matriz, 64) ;
	analizar_grafo (matriz, 128) ;
	analizar_grafo (matriz, 256) ;
	analizar_grafo (matriz, 512) ;
//	analizar_grafo (matriz, 1024) ;
*/

	explora_k_colorabilidad (3, 4.0) ; // exploramos con k=3 y 4 arcos por nodo	

	mi_malloc (-1) ;	
	system ("PAUSE") ;
}


