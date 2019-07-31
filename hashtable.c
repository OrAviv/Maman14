#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "data.h"

/*********************
 Hash code generator.
**********************/
unsigned int hash_code_gen(char *s)
{
    unsigned int hashnum;

    for (hashnum = 0; *s != '\0'; s++)
        hashnum = *s + 31 * hashnum;

    return hashnum % HASHSIZE;
}

/*****************************************************
 Helper function for searching node in the hashtable.
******************************************************/
hash_node *search_node(char *s, hash_node *hashtab[])
{
    hash_node *tmp_node;

    for (tmp_node = hashtab[hash_code_gen(s)]; tmp_node != NULL; tmp_node = tmp_node->next)
        if (strcmp(s, tmp_node->name) == 0)
            return tmp_node;

    return NULL;
}

/*********************************************************
 Helper function to add new node to the hashtable struct.
*********************************************************/
hash_node *add_node(char *s, int defn, int num, hash_node *hash_table[])
{
    hash_node *new_node;
    unsigned int hashval;

    if ((new_node = search_node(s, hash_table)) == NULL)
    {
        new_node = (hash_node *) malloc(sizeof(*new_node));

        if (new_node == NULL || (new_node->name = duplicate_str(s)) == NULL)
            return NULL;

        else
        {
            hashval = hash_code_gen(s);
            new_node->next = hash_table[hashval];
            hash_table[hashval] = new_node;
        }
    }

    new_node->num = defn;
    new_node->mem_words = num;

    /*Returns pointer to a new created node.*/
    return new_node;
}

/************************************
 Clear hashtable for a next os file.
*************************************/
void reset_hash_table(void)
{
    int i;

    for (i = 0; i < HASHSIZE; i++)
    {
        data_list[i] = NULL;
        instructions_list[i] = NULL;
        extern_list[i] = NULL;
    }
}