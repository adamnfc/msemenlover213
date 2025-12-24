#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t cumul_alloc;
    size_t cumul_desalloc;
    size_t mem_courante;
    size_t mem_peak;  //memoire allouée max simultanément (en gros le max de mem utilisée par le prog pendant une execution on incrémente mem _courante a chaque nouvelle alloc si celle ci est maximum si non on touche pas )
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

void myFree(void* ptr, InfoMem* infoMem, size_t size) {
    if (ptr == NULL){
        return; 
    }   
    if (infoMem != NULL){
        infoMem->cumul_desalloc += size;

        if (infoMem->mem_courante >= size){
            infoMem->mem_courante -= size;
        }else{
            infoMem->mem_courante = 0; 
        }        
        if (infoMem->blocs_actifs > 0){
            infoMem->blocs_actifs--;
        }
    } 
    free(ptr);
}

void* myRealloc(void* ptr, size_t new_size, InfoMem* infoMem, size_t old_size) {
    if (ptr == NULL) {
        return myMalloc(new_size, infoMem);
    }
    if (new_size == 0) {
        myFree(ptr, infoMem, old_size);
        return NULL;
    }
    void* new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) {
        if (infoMem != NULL) {
            infoMem->erreurs_alloc++;
        }
        return NULL; 
    }
    if (infoMem != NULL) {
        if (new_size > old_size) {
            size_t diff = new_size - old_size;
            infoMem->cumul_alloc += diff;
            infoMem->mem_courante += diff;
            
            if (infoMem->mem_courante > infoMem->mem_peak) {
                infoMem->mem_peak = infoMem->mem_courante;
            }
        } else if (new_size < old_size) {
            size_t diff = old_size - new_size;
            infoMem->cumul_desalloc += diff;
            
            if (infoMem->mem_courante >= diff) {
                infoMem->mem_courante -= diff;
            } else {
                infoMem->mem_courante = 0;
            }
        }
    }
    return new_ptr;
}

int main(void){
    
    InfoMem info = {0}; //init struct

    int* tab = (int*)myMalloc(10 * sizeof(int), &info);
    if (tab == NULL) {
        printf("Échec d'allocation\n");
        return 1;
    }
    
    printf("Après allocation initiale (10 ints):\n");
    printf("Mémoire courante>> %zu octets\n", info.mem_courante);
    printf("Pic de mémoire>> %zu octets\n", info.mem_peak);
    printf("Blocs actifs>> %zu\n", info.blocs_actifs);
    printf("Cumul allocations>> %zu octets\n", info.cumul_alloc);
    
    int* new_tableau = (int*)myRealloc(tab, 20 * sizeof(int), &info, 10 * sizeof(int));
    if (new_tableau == NULL) {
        printf("Échec de réallocation\n");
        myFree(tab, &info, 10 * sizeof(int));
        return 1;
    }
    tab = new_tableau;
    
    printf("\nAprès réallocation (20 ints):\n");
    printf("Mémoire courante>> %zu octets\n", info.mem_courante);
    printf("Pic de mémoire>> %zu octets\n", info.mem_peak);
    printf("Blocs actifs>> %zu\n", info.blocs_actifs);
    printf("Cumul allocations>> %zu octets\n", info.cumul_alloc);
    printf("Cumul désallocations>> %zu octets\n", info.cumul_desalloc);
    
    new_tableau = (int*)myRealloc(tab, 5 * sizeof(int), &info, 20 * sizeof(int));
    if (new_tableau == NULL) {
        printf("Échec de réallocation\n");
        myFree(tab, &info, 20 * sizeof(int));
        return 1;
    }
    tab = new_tableau;
    
    printf("\nAprès réduction (5 ints):\n");
    printf("Mémoire courante>> %zu octets\n", info.mem_courante);
    printf("Pic de mémoire>> %zu octets\n", info.mem_peak);
    printf("Blocs actifs>> %zu\n", info.blocs_actifs);
    printf("Cumul allocations>> %zu octets\n", info.cumul_alloc);
    printf("Cumul désallocations>> %zu octets\n", info.cumul_desalloc);
    
    myFree(tab, &info, 5 * sizeof(int));
    
    printf("\nAprès libération finale:\n");
    printf("Mémoire courante>> %zu octets\n", info.mem_courante);
    printf("Blocs actifs>> %zu\n", info.blocs_actifs);
    printf("Cumul allocations>> %zu octets\n", info.cumul_alloc);
    printf("Cumul désallocations>> %zu octets\n", info.cumul_desalloc);
    printf("Erreurs d'allocation>> %zu\n", info.erreurs_alloc);
}
}
