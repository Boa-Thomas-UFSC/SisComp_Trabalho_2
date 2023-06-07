#include <stdlib.h>
#include <stdio.h>

#pragma pack (push, 1) // garante que não haverá padding entre os membros da struct

struct mem_block { // estrutura que representa um bloco de memória
    int is_free; // flag que indica se o bloco está livre ou não
    size_t size; // tamanho do bloco
    void* mem_ptr; // ponteiro para o bloco de memória
    struct mem_block* next; // ponteiro para o próximo bloco de memória
};

#pragma pack (pop) // restaura o alinhamento padrão

struct mem_block* head = NULL; // ponteiro para o primeiro bloco de memória
const size_t INITIAL_SIZE = 4096;  // Tamanho inicial da memória (4 kB)

void* smalloc(size_t size) { // size é o tamanho do bloco de memória a ser alocado
    if (head == NULL) { // se a lista de blocos de memória estiver vazia
        void* initial_memory = malloc(INITIAL_SIZE); // aloca memória inicial
        if (!initial_memory) { // se não foi possível alocar memória
            return NULL; // retorna NULL
        }
        head = (struct mem_block*)initial_memory; // head aponta para o bloco de memória inicial
        head->is_free = 1; // marca o bloco de memória como livre
        head->size = INITIAL_SIZE - sizeof(struct mem_block); // define o tamanho do bloco de memória
        head->mem_ptr = (void*)((char*)head + sizeof(struct mem_block)); // define o ponteiro para o bloco de memória
        head->next = NULL; // define o ponteiro para o próximo bloco de memória como NULL
    }

    struct mem_block* current = head; // current é o ponteiro para o bloco de memória atual

    while (current != NULL) { // percorre a lista de blocos de memória
        if (current->is_free && current->size >= size) { // encontrou um bloco de memória livre com tamanho suficiente
            current->is_free = 0; // marca o bloco de memória como ocupado
            if (current->size > size + sizeof(struct mem_block)) { // se o bloco de memória for maior que o necessário
                struct mem_block* new_block = (struct mem_block*)((char*)current->mem_ptr + size); // cria um novo bloco de memória
                new_block->is_free = 1; // marca o novo bloco de memória como livre
                new_block->size = current->size - size - sizeof(struct mem_block); // define o tamanho do novo bloco de memória
                new_block->mem_ptr = (void*)((char*)new_block + sizeof(struct mem_block)); // define o ponteiro para o novo bloco de memória
                new_block->next = current->next; // define o ponteiro para o próximo bloco de memória
                current->next = new_block; // atualiza o ponteiro para o próximo bloco de memória
                current->size = size; // atualiza o tamanho do bloco de memória
            }
            return current->mem_ptr; // retorna o ponteiro para o bloco de memória
        }
        current = current->next; // atualiza o ponteiro para o bloco atual
    }

    return NULL;
}

void sfree(void* ptr) { // ptr é o ponteiro retornado por smalloc
    struct mem_block* current = head; // current é o ponteiro para o bloco de memória que contém ptr
    struct mem_block* prev = NULL; // prev é o ponteiro para o bloco de memória anterior a current

    while (current != NULL) { // percorre a lista de blocos de memória
        if (current->mem_ptr == ptr) { // encontrou o bloco de memória que contém ptr
            current->is_free = 1; // marca o bloco de memória como livre
            // coalesce free blocks (merge adjacent free blocks)
            if (prev && prev->is_free) { // bloco anterior está livre
                prev->size += sizeof(struct mem_block) + current->size; // junta os blocos
                prev->next = current->next; // atualiza o ponteiro para o próximo bloco
                current = prev; // atualiza o ponteiro para o bloco atual
            }
            if (current->next && current->next->is_free) { // próximo bloco está livre
                current->size += sizeof(struct mem_block) + current->next->size; // junta os blocos
                current->next = current->next->next; // atualiza o ponteiro para o próximo bloco
            }
            return; // retorna
        }
        prev = current; // atualiza o ponteiro para o bloco anterior
        current = current->next; // atualiza o ponteiro para o bloco atual
    }
}



void print_mem_block(struct mem_block *m){
    printf("-----------------------------\n");    
    while(m){

        printf("is_free: %d\n", m->is_free);
        printf("size: %ld\n", m->size);
        printf("mem_ptr: %p\n", m->mem_ptr);
        printf("next: %p\n", m->next);
        printf("-----------------------------\n");
        m = m->next;
    }
};


int main(){
    /* Exemplo de uso do smalloc(), sfree() e print_mem_block() */
    char *p1 = (char*) smalloc(200);
    char *p2 = (char*) smalloc(100);
    char *p3 = (char*) smalloc(300);

    print_mem_block(head);        

    sfree(p3);
    sfree(p2);

    print_mem_block(head);            

    return 0;
}