#ifndef HASH_TABLE_H
#define HASH_TABLE_H

struct Node {
    const char *key;
    void *value;
    struct Node *next;
};

struct Hash_table {
    struct Node **table;
    unsigned int table_length;
    unsigned int size;
    unsigned int threshold;
    float load_factor;
};

unsigned int table_size_for(unsigned int cap);
struct Hash_table *create_hash_table_c_lf(unsigned int initial_capacity, float load_factor);
struct Hash_table *create_hash_table_c(unsigned int initial_capacity);
struct Hash_table *create_hash_table();
unsigned long hash(const char *str);
void hash_table_resize(struct Hash_table *hash_table);
struct Node *get_node(struct Hash_table *hash_table, const char *key);
void print_node(struct Node *node);
void insert(struct Hash_table *hash_table, char *key, void *value);
void print_hash_table(struct Hash_table *hash_table);
void delete(struct Hash_table *hash_table, char *key);
void print_hash_table_metadata(struct Hash_table *hash_table);
void clear_hash_table(struct Hash_table *hash_table);
void clear_node(struct Node *node);

#endif //HASH_TABLE_H
