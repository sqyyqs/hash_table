#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMUM_CAPACITY 1 << 31
#define DEFAULT_CAPACITY 1 << 4 //aka 16
#define DEFAULT_LOAD_FACTOR 0.75f

unsigned int table_size_for(unsigned int cap) {
    return 1u << (32 - __builtin_clz(cap - 1));
}

struct Hash_table *create_hash_table_c_lf(unsigned int initial_capacity, float load_factor) {
    if (initial_capacity > MAXIMUM_CAPACITY) {
        initial_capacity = MAXIMUM_CAPACITY;
    }
    if (load_factor <= 0 || load_factor >= 1) {
        load_factor = DEFAULT_LOAD_FACTOR;
    }
    if (initial_capacity == 0) {
        initial_capacity = DEFAULT_CAPACITY;
    }

    struct Hash_table *hash_table = (struct Hash_table *) malloc(sizeof(struct Hash_table));

    hash_table->threshold = table_size_for(initial_capacity);
    hash_table->load_factor = load_factor;
    hash_table->size = 0;
    hash_table->table_length = hash_table->threshold;
    hash_table->table = (struct Node **) malloc(sizeof(struct Node *) * hash_table->threshold);

    for (int i = 0; i < hash_table->threshold; i++) {
        hash_table->table[i] = NULL;
    }

    hash_table->threshold = hash_table->threshold * hash_table->load_factor;
    return hash_table;
}

struct Hash_table *create_hash_table_c(unsigned int initial_capacity) {
    return create_hash_table_c_lf(initial_capacity, DEFAULT_LOAD_FACTOR);
}

struct Hash_table *create_hash_table() {
    return create_hash_table_c(DEFAULT_CAPACITY);
}

unsigned long hash(const char *str) {
    if (str == NULL) {
        return 0;
    }
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash ^ (hash >> 16); // уменьшает количество коллизий, в случаях когда отличаются только старшие биты
}


void hash_table_resize(struct Hash_table *hash_table) {
    unsigned int new_capacity = 2 * (hash_table->table_length);
    if (new_capacity != 0) {
        hash_table->table_length = new_capacity;
        //используем не realloc, потому что надо пересчитать хеши
        struct Node **new_table = (struct Node **) malloc(sizeof(struct Node *) * new_capacity);
        for (unsigned int i = 0; i < new_capacity; i++) {
            new_table[i] = NULL;
        }
        for (unsigned int i = 0; i < hash_table->table_length; i++) {
            struct Node *current = hash_table->table[i];
            while (current != NULL) {
                struct Node *next = current->next;
                unsigned int new_index = (new_capacity - 1) & hash(current->key);
                current->next = new_table[new_index];
                new_table[new_index] = current;
                current = next;
            }
        }
        free(hash_table->table);
        hash_table->table = new_table;
    }
    hash_table->threshold = (int) (new_capacity * hash_table->load_factor);
}

struct Node *get_node(struct Hash_table *hash_table, const char *key) {
    if (hash_table == NULL
        || hash_table->table == NULL
        || hash_table->size == 0
        || key == NULL) {
        return NULL;
    }
    unsigned int node_index = (hash_table->table_length - 1) & hash(key);
//    printf("(delete)index = %u\n", node_index); //отладка
    struct Node *current = hash_table->table[node_index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void print_node(struct Node *node) {
    if (node == NULL) {
        printf("node is NULL.\n");
        return;
    }
    printf("key = %s, value = %p\n", node->key, node->value);
}

void print_hash_table_metadata(struct Hash_table *hash_table) {
    printf("Hash_table{size = %u, table_length = %u, threshold = %u, load_factor = %.2f}\n"
           , hash_table->size
           , hash_table->table_length
           , hash_table->threshold
           , hash_table->load_factor
    );
}

void insert(struct Hash_table *hash_table, char *key, void *value) {
    if (hash_table == NULL) {
        return;
    }
    if (key == NULL) {
        printf("Ключ не может быть NULL.\n");
        return;
    }
    unsigned int index = (hash_table->table_length - 1) & hash(key);

    struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = NULL;
    struct Node* node = hash_table->table[index];
//    printf("index for %s is %u(hash = %lu)\n", key, index, hash(key)); отладка
    if (node == NULL) {
        hash_table->table[index] = new_node;
        hash_table->size++;
    } else if (strcmp(node->key, key) == 0) { // замена
        free(hash_table->table[index]);
        hash_table->table[index] = new_node;
    } else { // коллизия
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = new_node;
    }
    if (hash_table->size > hash_table->threshold) {
        hash_table_resize(hash_table);
    }
}

void delete(struct Hash_table *hash_table, char *key) {
    if (hash_table == NULL) {
        printf("Таблица не может быть NULL.\n");
        return;
    }
    if (key == NULL) {
        printf("Ключ не может быть NULL.\n");
        return;
    }

    unsigned int index = (hash_table->table_length - 1) & hash(key);
    struct Node *node = hash_table->table[index];
    struct Node *prev = NULL;

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                hash_table->table[index] = node->next;
                hash_table->size--;
            }
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

void print_hash_table(struct Hash_table *hash_table) {
    if (hash_table == NULL) {
        printf("HashTable is NULL.\n");
        return;
    }
    if (hash_table->size == 0) {
        printf("HashTable is empty.\n");
        return;
    }
    printf("\nHashTable Contents:\n");
    for (unsigned int i = 0; i < hash_table->table_length; i++) {
        struct Node *current = hash_table->table[i];
        while (current != NULL) {
            print_node(current);
            current = current->next;
        }
    }
    printf("\n");
}

void clear_hash_table(struct Hash_table *hash_table) {
    if (hash_table == NULL) {
        return;
    }
    for (unsigned int i = 0; i < hash_table->table_length; i++) {
        struct Node *current = hash_table->table[i];
        while (current != NULL) {
            struct Node *next = current->next;
            clear_node(current);
            current = next;
        }
    }
    free(hash_table->table);
    free(hash_table);
}

void clear_node(struct Node *node) {
    if (node == NULL) {
        return;
    }
    free(node->key);
    free(node->value);
    free(node);
}


