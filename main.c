#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void assert(bool value, const char *fail_message);

void assert_not(bool value, const char *fail_message);

void test_fill_100_elements(void);

void test_resize_filling_100_elements(void);


void assert(bool value, const char *fail_message) {
    if (!value) {
        printf("Assert was failed: %s", fail_message);
        exit(-1);
    }
}

void assert_not(bool value, const char *fail_message) {
    if (value) {
        printf("Assert was failed: %s", fail_message);
        exit(-1);
    }
}


void test_fill_100_elements() {
    struct Hash_table *map = create_hash_table_c_lf(100, 0.75f);
    for (int i = 0; i < 100; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        int *value = (int *) malloc(sizeof(int));
        *value = i * 2;
        insert(map, key, value);
    }

    assert_not(get_node(map, "key98") == NULL,
               "There's no node with key = 'key198' in table!");

    assert_not(get_node(map, "key97") == NULL,
               "There's no node with key = 'key197' in table!");

    assert_not(get_node(map, "key96") == NULL,
               "There's no node with key = 'key196' in table!");

    assert_not(get_node(map, "key95") == NULL,
               "There's no node with key = 'key195' in table!");

    assert(get_node(map, "key200") == NULL,
           "There's node with key = 'key200' in table!");

    assert(map->size == 100,
           "Map's size is not 100!"); // <- не отработает, потому что часто встречаются коллизии

    clear_hash_table(map);
}

void test_resize_filling_100_elements() {
    struct Hash_table *map = create_hash_table_c_lf(16, 0.75f);
    for (int i = 0; i < 100; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        int *value = (int *) malloc(sizeof(int));
        *value = i * 2;
        insert(map, key, value);
    }

    assert_not(map->table_length == 16,"Table's length is 16!"); // произошел ресайз..

    clear_hash_table(map);
}

void test_insert_delete_get() {
    struct Hash_table *map = create_hash_table_c_lf(16, 0.75f);
    assert(map->size == 0, "Hash table is not empty!");
    char key[5] = "key1";
    insert(map, key, "value");
    assert(map->size == 1, "Element was not inserted!");
    assert(get_node(map, "key1") != NULL, "Gotten node is null!");
    delete(map, "key1");
    assert(get_node(map, "key1") == NULL, "Gotten node is not null!");

    clear_hash_table(map);
}

int main() {
    test_fill_100_elements();
    test_resize_filling_100_elements();
    test_insert_delete_get();
    return 0;
}