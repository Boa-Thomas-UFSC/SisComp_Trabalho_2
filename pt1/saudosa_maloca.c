#include <stdlib.h>
#include <stdio.h>

#pragma pack (push, 1)

struct mem_block {
    int is_free; // 1 se o bloco está livre, 0 caso contrário
    size_t size;   // Tamanho do bloco de memória
    void* mem_ptr; // Ponteiro para o bloco de memória
    struct mem_block* next; // Ponteiro para o próximo bloco de memória
};

#pragma pack (pop)

struct mem_block* head = NULL;
const size_t INITIAL_SIZE = 4096;  // Tamanho inicial da memória (4 kB)

void* smalloc(size_t size) {
    if (head == NULL) { // Se a lista estiver vazia, alocamos o primeiro bloco
        void* initial_memory = malloc(INITIAL_SIZE); // Alocamos a memória inicial
        if (!initial_memory) { // Se não conseguirmos alocar a memória, retornamos NULL
            return NULL;
        }
        head = (struct mem_block*)initial_memory; // Definimos o primeiro bloco como o início da memória alocada
        head->is_free = 1; // Definimos o primeiro bloco como livre
        head->size = INITIAL_SIZE - sizeof(struct mem_block); // Definimos o tamanho do primeiro bloco
        head->mem_ptr = (void*)((char*)head + sizeof(struct mem_block)); // Definimos o ponteiro para o primeiro bloco
        head->next = NULL; // Definimos o próximo bloco como NULL
    }

    struct mem_block* current = head; // Definimos o bloco atual como o primeiro bloco
    struct mem_block* prev = NULL; // Definimos o bloco anterior como NULL

    while (current != NULL) { // Percorremos a lista de blocos
        if (current->is_free && current->size >= size) { // Se o bloco atual estiver livre e tiver tamanho suficiente
            current->is_free = 0; // Definimos o bloco atual como ocupado
            if (current->size > size + sizeof(struct mem_block)) { // Se o bloco atual for maior que o tamanho necessário + o tamanho de um bloco
                struct mem_block* new_block = (struct mem_block*)((char*)current->mem_ptr + size); // Criamos um novo bloco
                new_block->is_free = 1; // Definimos o novo bloco como livre
                new_block->size = current->size - size - sizeof(struct mem_block); // Definimos o tamanho do novo bloco
                new_block->mem_ptr = (void*)((char*)new_block + sizeof(struct mem_block)); // Definimos o ponteiro para o novo bloco
                new_block->next = current->next; // Definimos o próximo bloco do novo bloco como o próximo bloco do bloco atual
                current->next = new_block; // Definimos o próximo bloco do bloco atual como o novo bloco
                current->size = size; // Definimos o tamanho do bloco atual como o tamanho necessário
            }
            return current->mem_ptr; // Retornamos o ponteiro para o bloco atual
        }
        prev = current; // Definimos o bloco anterior como o bloco atual
        current = current->next; // Definimos o bloco atual como o próximo bloco
    }

    // Se chegarmos aqui, não encontramos um bloco de memória livre grande o suficiente
    // Portanto, retornamos NULL
    return NULL;
}

void sfree(void* ptr) { // Libera o bloco de memória apontado por ptr
    struct mem_block* current = head; // Definimos o bloco atual como o primeiro bloco

    while (current != NULL) { // Percorremos a lista de blocos
        if (current->mem_ptr == ptr) { // Se o bloco atual for o bloco que queremos liberar
            current->is_free = 1; // Definimos o bloco atual como livre
            return; // Retornamos
        }
        current = current->next; // Definimos o bloco atual como o próximo bloco
    }
}
