#!/bin/bash
rm -f hash_table
gcc -w -o hash_table hash_table.c main.c
./hash_table