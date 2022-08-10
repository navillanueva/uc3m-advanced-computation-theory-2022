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

/* imprime nodos del grafo en diagonal y en las intersecciones si son adyacentes */
/* imprime antes el n�mero de grafo incoexo (cluster) si est� disponible */
void imprimir_grafo (int **matriz, int nodos) {
	int i ;
	int j ;
	
	for (i = 0 ; i < nodos ; i++) {
		printf ("%4d, c:%3d: ", i, clusters [i]) ;
		for (j = 0 ; j < nodos ; j++) {
			if (i == j) {			// marca diagonal con punto
				printf (".", i) ;
			} else if (matriz [i][j] == 1) { 	// si i-j son adyacentes
				printf ("+") ;		// marca interseccion i-j con +
			} else printf (" ") ;
		}
		printf ("\n") ;
	}
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
	
	arcos = nodos*(nodos-1)/4 ;

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
	printf ("TIempo %3f \n", cpu_time_used) ;
}


int main (void) 
{
	srand (3) ;
	
	crear_matriz (matriz) ;

	analizar_grafo (matriz, 4) ;
	analizar_grafo (matriz, 8) ;
	analizar_grafo (matriz, 12) ;
	analizar_grafo (matriz, 16) ;
	analizar_grafo (matriz, 24) ;
	analizar_grafo (matriz, 32) ;
	analizar_grafo (matriz, 48) ;
	analizar_grafo (matriz, 64) ;
	analizar_grafo (matriz, 80) ;
	analizar_grafo (matriz, 112) ;
	analizar_grafo (matriz, 144) ;
	analizar_grafo (matriz, 176) ;
	analizar_grafo (matriz, 208) ;
	analizar_grafo (matriz, 256) ;
	analizar_grafo (matriz, 288) ;
	analizar_grafo (matriz, 320) ;
	analizar_grafo (matriz, 352) ;
	analizar_grafo (matriz, 384) ;
	analizar_grafo (matriz, 416) ;
	analizar_grafo (matriz, 448) ;
	analizar_grafo (matriz, 480) ;
	analizar_grafo (matriz, 512) ;

/*	analizar_grafo (matriz, 16) ;
	analizar_grafo (matriz, 32) ;
	analizar_grafo (matriz, 64) ;
	analizar_grafo (matriz, 128) ;
	analizar_grafo (matriz, 256) ;
	analizar_grafo (matriz, 512) ;
//	analizar_grafo (matriz, 1024) ; */

	mi_malloc (-1) ;	
	system ("PAUSE") ;
}