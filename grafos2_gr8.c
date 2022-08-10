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


int borrar_arco_aleatorio (int **matriz, int nodos, int arcos) {
	int p ;
	int q ;

	if (arcos == 0) {
		return 0 ;
	}
		
	do {
		p = rand () % nodos ;
		q = rand () % nodos ;			
	} while (matriz [p][q] == 0 || p == q) ;  // p==q es redundante

	matriz [p][q] = 0 ; 
	matriz [q][p] = 0 ;
	
	return arcos-1 ; 
}

/* para propagar recursivamente una marca (n�mero de cluster) a los nodos adyacentes del nodo n */
void propagar_marcaR (int **matriz, int nodos, int n, int n_cluster) {
	int j ;
	
	for (j = 0 ; j < nodos ; j++) {
		if (n != j && matriz [n][j] != 0 && clusters [j] == -1) {
			clusters [j] = n_cluster ;
			propagar_marcaR (matriz, nodos, j, n_cluster) ;
		}	
	}
}

/* wrapper para propagar una marca (n�mero de cluster) a los nodos adyacentes  */
int contar_clusters (int **matriz, int nodos) {
	int i ;
	int j ;
	int n_cluster ;
	
	for (i = 0 ; i < nodos ; i++) {	
		clusters [i] = -1 ;				// todos los nodos sin marcar para empezar
	}
	n_cluster = 0 ;
	
	for (i = 0 ; i < nodos ; i++) {
		if (clusters [i] == -1) {   	// nodo i no est� maecado ==> es nodo de grafo inconexo
			clusters [i] = n_cluster ;  // nuevo cluster y propagar a sus adyacentes
			propagar_marcaR (matriz, nodos, i, n_cluster) ;
			n_cluster++ ;
		}
	}
	
	return n_cluster ;
}


void analizar_grafo (int **matriz, int nodos) {
	int arcos ;
	int clust0 ;
	int clust1 ;

	clock_t start ;
	clock_t end ;
	double cpu_time_used ;
	
	arcos = nodos*(nodos-1)/2 ;

	crear_grafo (matriz, nodos, arcos) ;	// crea grafo completo

	start = clock () ;
	clust0 = 0 ;
	
	do {
		clust1 = contar_clusters (matriz, nodos) ;
		if (clust1 > clust0) { 	// imprime solo si aparece nuevo cluster
	 		printf ("Nodos\t %4d\t Arcos\t %4d\t Clusters\t %3d\n", nodos, arcos, clust1) ;
	 		clust0 = clust1 ;
		}						// elimina un arco al azar:
		arcos = borrar_arco_aleatorio (matriz, nodos, arcos) ;		
	} while (arcos > 0) ;

	clust1 = contar_clusters (matriz, nodos) ;
	if (clust1 > clust0) { 	// imprime solo si aparece nuevo cluster
 		printf ("Nodos\t %4d\t Arcos\t %4d\t Clusters\t %3d\n", nodos, arcos, clust1) ;
 		clust0 = clust1 ;
	}

	end = clock () ; 
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf ("Tiempo %3f \n", cpu_time_used) ;
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


/* dada una asignacion de colores en asignaciones comprueba si dos nodos i, j 
 estan conectados y en tal caso no coinciden en colores.                        */
int comprueba_conflictos (int nodos) 
{
	int i ;
	int j ;
	long int cf = 0L ;
	
	for (i = 0 ; i < nodos-1 ; i++) {
		for (j = i+1 ; j < nodos ; j++) {
			if (matriz [i][j] == 1) {
				if (asignaciones [i] == asignaciones [j]) {
					cf++ ;
				}
			}
		}
	}
	return cf ;
}


/* funcioo recursiva para generar todas las asignaciones posibles.
Cuando llega al nodo hoja llama a comprueba_conflictos para 
contabilizar si se trata de una soluci�n v�lida o fallida          */
/* Se ha realizado de forma recursiva como paso previo para usar backtracking */
void asigna_colores (int ind, int nodos, int k) {
	int j ;
	int cf ;
	
	if (ind >= nodos) {
		cf = comprueba_conflictos (nodos) ;
		if (cf == 0) {
			sin_conflictos++ ;
		} else {
			con_conflictos++ ;
		}
	} else {
		for (j = 0 ; j < k ; j++) {
			asignaciones [ind] = j ;
			asigna_colores (ind+1, nodos, k) ;
 
//			if (sin_conflictos >= 1) return ;	// �apa para obtener primera solucion

 		}	
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
 		
	for (nodos = 5 ; nodos < 20 ; nodos++) {
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