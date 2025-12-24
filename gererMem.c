#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t cumul_alloc;
    size_t cumul_desalloc;
    size_t mem_courante;
    size_t mem_peak; //memoire allouée max simultanément (en gros le max de mem utilisée par le prog pendant une execution on incrémente mem _courante a chaque nouvelle alloc si celle ci est maximum si non on touche pas )
    size_t blocs_actifs; 
    size_t erreurs_alloc; 
} InfoMem;

void* myMalloc(size_t size, InfoMem* infoMem) {
    if (infoMem == NULL){
        return malloc(size);
    }
    void* ptr = malloc(size);
    if (ptr == NULL){
        infoMem->erreurs_alloc++;
        return NULL;
    }
    infoMem->cumul_alloc += size;
    infoMem->mem_courante += size;
    infoMem->blocs_actifs++;
    
    if (infoMem->mem_courante > infoMem->mem_peak){
        infoMem->mem_peak = infoMem->mem_courante;
    }
    return ptr;
}

int main(void){
    
    InfoMem info = {0}; //init struct

    int* tableau = (int*)myMalloc(100 * sizeof(int), &info);
    if (tableau == NULL){
        printf("Échec d'allocation\n");
        return 1;
    }
    printf("Mémoire courante>> %zu octets\n", info.mem_courante); //formatage %zu pour les size_t
    printf("Pic de mémoire>> %zu octets\n", info.mem_peak);
    printf("Blocs actifs>> %zu\n", info.blocs_actifs);
    return 0;
}
