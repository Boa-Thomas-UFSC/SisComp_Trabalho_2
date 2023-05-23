#include <stdlib.h>
#include <stdio.h>

#pragma pack (push, 1)

struct mem_block {
    int is_free;
    size_t size;
    void* mem_ptr;
    struct mem_block* next;
};

#pragma pack (pop)

struct mem_block* head = NULL;
const size_t INITIAL_SIZE = 4096;  // Tamanho inicial da memória (4 kB)

void* smalloc(size_t size) {
    if (head == NULL) { // Se a lista estiver vazia, alocamos o primeiro bloco
        void* initial_memory = malloc(INITIAL_SIZE);
        if (!initial_memory) {
            return NULL;
        }
        head = (struct mem_block*)initial_memory;
        head->is_free = 1;
        head->size = INITIAL_SIZE - sizeof(struct mem_block);
        head->mem_ptr = (void*)((char*)head + sizeof(struct mem_block));
        head->next = NULL;
    }

    struct mem_block* current = head;
    struct mem_block* prev = NULL;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            current->is_free = 0;
            if (current->size > size + sizeof(struct mem_block)) {
                struct mem_block* new_block = (struct mem_block*)((char*)current->mem_ptr + size);
                new_block->is_free = 1;
                new_block->size = current->size - size - sizeof(struct mem_block);
                new_block->mem_ptr = (void*)((char*)new_block + sizeof(struct mem_block));
                new_block->next = current->next;
                current->next = new_block;
                current->size = size;
            }
            return current->mem_ptr;
        }
        prev = current;
        current = current->next;
    }

    // Se chegarmos aqui, não encontramos um bloco de memória livre grande o suficiente
    // Portanto, retornamos NULL
    return NULL;
}

void sfree(void* ptr) {
    struct mem_block* current = head;

    while (current != NULL) {
        if (current->mem_ptr == ptr) {
            current->is_free = 1;
            return;
        }
        current = current->next;
    }
}
