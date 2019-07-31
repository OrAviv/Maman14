#ifndef HASHTABLE_H

#define HASHSIZE 200

/*************************
 Struct for the hashtable
**************************/
typedef struct int_list
{
    struct int_list *next;
    struct int_list *previous;
    char *name;
    int num;
    int mem_words;
} hash_node;

/****************************************
 Create new node for the hashtable list.
****************************************/
hash_node *add_node(char *name, int defn, int mem_w, hash_node *hashtab[]);

/*************
 Search node.
**************/
hash_node *search_node(char *s, hash_node *hashtab[]);

/*******************************
 Reset hashtable fileds values.
********************************/
void reset_hash_table(void);

/*******************
 Data symbols list.
********************/
static hash_node *data_list[HASHSIZE];

/**********************
 Instustructions list.
***********************/
static hash_node *instructions_list[HASHSIZE];

/****************
 Externals list.
*****************/
static hash_node *extern_list[HASHSIZE];

#endif