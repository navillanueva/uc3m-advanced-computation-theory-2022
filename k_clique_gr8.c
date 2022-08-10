#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NODOS  200

int *matriz [MAX_NODOS] ;
int con[MAX_NODOS];
int orden[MAX_NODOS];

int listaClique [MAX_NODOS];  // Lista donde van los nodos candidatos a formar un k-clique

unsigned long long int posibles_asignaciones;

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
void crear_matriz () {
    int i ;

    for (i = 0 ; i < MAX_NODOS ; i++) {
        matriz [i] = (int *) mi_malloc (sizeof (int)*MAX_NODOS) ;
    }
}

void liberar_matriz () {
    for (int i = 0; i < MAX_NODOS; i++) {
        free(matriz[i]);
    }
}

/* 0 indica que no hay arista entre los nodos i-j */
void inicializar_grafo (int nodos) {
    int i ;
    int j ;

    for (i = 0 ; i < nodos ; i++) {
        for (j = 0 ; j < nodos ; j++) {
            matriz [i][j] = 0 ;
        }
    }
}

void resetear_clique(int nodos) {
    // Resetea lista de cliques
    for(int i = 0; i < nodos; i++) {
        listaClique[i] = -1;  // Resetea nodos de posibles cliques
    }
}

/* crea grafo con n nodos y a arcos: no se controlan los limites */
/* 1 indica que hay arista entre los nodos i-j */
void crear_grafo (int nodos, int arcos) {
    int i ;
    int p ;
    int q ;

    inicializar_grafo (nodos) ;

    for (i = 0 ; i < arcos ; i++) {
        do {
            p = rand () % nodos ;
            q = rand () % nodos ;
        } while (p == q || matriz [p][q] != 0) ; // evitar diagonal y arcos existentes
        matriz [p][q] = 1 ;
        matriz [q][p] = 1 ; 	// arista sim�trica
    }
}

void imprimir_grafo (int nodos) {
    for (int i=0; i<nodos; i++) {
        printf("Nodo %d: ", i) ;
        for (int j=0; j<nodos; j++){
            if (matriz[i][j])
                printf("%d, ", j) ;
        }
        printf("\n ");
    }
}

void construye_grafo (int nodos, double arcospornodo) {
    int arcos ;

    arcos = nodos * arcospornodo ;
    crear_grafo (nodos, arcos) ;	// crea grafo maximo
}


void array_orden(int orden[], int *num_conex, int nodos){
	for (int i = 0 ; i < nodos +1; i++){
		orden[i] = 0;
	}

	for (int j = 0; j < nodos + 1; j ++){
		int indice = -1;
		int aux = -1;
		for (int i = 0 ; i < nodos ; i++){
			if (aux < num_conex[i]) {
				aux = num_conex[i];
				indice = i;
			}
		}
		num_conex[indice] = -1;
		orden[j] = indice;
	}
}



void num_conexiones(int num_conex[], int nodos) {
	for (int i = 0 ; i < nodos ; i++){
		num_conex[i] = 0;
	}
	for (int i = 0; i < nodos ; i++){
		for (int j = 0 ; j < nodos ; j++){
			if (matriz[i][j] == 1) num_conex[i]++;
		}
	}
}




/*
 * Para cada nodo de la lista de candidatos a clique,
 * comprueba que esté unido al resto de elementos.
 */
int esClique(int iListaClique) { 
    for (int i = 0; i < iListaClique; i++) { 
        for (int j = i + 1; j < iListaClique; j++) 
            if (matriz[listaClique[i]][ listaClique[j]] == 0)
                return 0;
    }
    return 1;
}

/*
 * Este método recursivo busca un k-clique.
 * Devuelve 1 si hay al menos 1 k-clique.
 * nodos: numero de nodos.
 * n: nodo actual (empieza en 0)
 * iListaClique: posición de la lista de clique
 * k: numero de colores
 * return: 1 si hay k-clique, 0 si no lo hay
 */
int encontrarK_clique(int nodos, int n, int iListaClique, int k) { 
    for (int j = n; j < nodos; j++){
        
        if(1) { 
            //num_conexiones(con, nodos);
		    //array_orden(orden, con, nodos);
            listaClique[iListaClique] = j;
            if (esClique(iListaClique + 1)){
                if (iListaClique < k - 1){
                    int clique = encontrarK_clique(nodos, j + 1, iListaClique + 1, k);
                    if (clique)
                        return 1 ;
                } else{
                    printf("Encontrado %d-clique: ", k);
                    for (int kk=0; kk<k; kk++){
                        printf("%d, ", listaClique[kk]);
                    }
                    printf("\n");
                    return 1 ;
                }
            }
        }
    }
    return 0 ;
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

    clock_t start ;
	clock_t end ;
	double cpu_time_used ;

    construye_grafo (300, arcospornodo) ;



    for (nodos = 10 ; nodos < 300 ; nodos++) {
        start = clock () ;

        resetear_clique(nodos);
        printf("Nodos %d:\n", nodos) ;
        //num_conexiones(con, nodos);
		//array_orden(orden, con, nodos);
        encontrarK_clique(nodos, 0, 0, k + 1) ;

        end = clock () ;

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf ("Tiempo %3f \n", cpu_time_used) ;
    }
}


int main (void)
{
    srand (3) ;

    crear_matriz (matriz) ;

    explora_k_colorabilidad (3, 4.0) ; // exploramos con k=3 y 4 arcos por nodo

    liberar_matriz() ;
    mi_malloc (-1) ;
    system ("PAUSE") ;
}