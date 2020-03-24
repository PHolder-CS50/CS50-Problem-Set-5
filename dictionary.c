// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <ctype.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    unsigned char wordLength;
    struct node *next;
}
node;

// Number of buckets in hash table
//const unsigned int N = 1119;
//const unsigned int N = 7919;  // 1000th prime
//const unsigned int N = 65537;  // A prime just larger than 64K
const unsigned int N = 104729;  // 10 thousandth prime

// Hash table
node *table[N];
unsigned int numberOfWordsLoaded;

// Case insensitive word comarison, given word length
bool wordcmp(const char *word1, const char *word2, unsigned int wordLength)
{
    int i;
    for (i = 0; i < wordLength; i++)
    {
        if (tolower(word1[i]) != tolower(word2[i]))
        {
            return false;
        }
    }
    return true;
}

// Returns true if word is in dictionary else false
// optimized to the fact that words in the list are
// in order of length, so it can skip shorter word
// comparisons and stop when it finds a longer word
bool check(const char *word)
{
    unsigned char wordLength = strlen(word);
    node *tmp = table[hash(word)];

    while (tmp != NULL)
    {
        if (wordLength == tmp->wordLength)
        {
            // if (strcasecmp(tmp->word, word) == 0)
            if (wordcmp(tmp->word, word, wordLength))
            {
                return true;
            }
        }
        else if (wordLength < tmp->wordLength)
        {
            return false;
        }
        tmp = tmp->next;
    }

    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    unsigned long v = tolower(word[0]);
    for (int i = 1; word[i] != '\0'; i++)
    {
        unsigned char c = tolower(word[i]);
        v = ((v + i) * c) >> 3;
    }
    return v % N;
}

// Make the assumption that smallest words will be most frequent
// and keep the list in order by word length
void putWordInTable(const char *word)
{
    unsigned int wordLength = strlen(word);
    node *newNode = malloc(sizeof(node));
    if (newNode == NULL)
    {
        printf("OUT OF MEMORY!\n");
        return;
    }
    strcpy(newNode->word, word);
    newNode->wordLength = wordLength;

    unsigned int bucketNumber = hash(word);
    node *tmp = table[bucketNumber];

    // First entry in a previously empty bucket
    if (tmp == NULL)
    {
        newNode->next = NULL;
        table[bucketNumber] = newNode;
        return;
    }

    // Needs to go at head of list
    if (wordLength <= tmp->wordLength)
    {
        newNode->next = tmp;
        table[bucketNumber] = newNode;
        return;
    }

    node *prev;
    while ((tmp != NULL) && (wordLength > tmp->wordLength))
    {
        prev = tmp;
        tmp = tmp->next;
    }

    prev->next = newNode;
    newNode->next = tmp;
    return;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    numberOfWordsLoaded = 0;

    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }

    // Allow an extra char for \n from fgets(), plus room for \0
    char word[LENGTH + 2];
    while (fgets(word, LENGTH + 2, dict) != NULL)
    {
        word[strlen(word) - 1] = '\0'; // Overwrite the newline fgets() included
        putWordInTable(word);
        numberOfWordsLoaded++;
    }

    fclose(dict);

    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return numberOfWordsLoaded;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        node *tmp = table[i];
        while (tmp != NULL)
        {
            node *next = tmp->next;
            free(tmp);
            tmp = next;
        }
    }
    return true;
}


/*
// Code used to unit test with
// just clang dictionary.c and the run a.out
void dumpTable()
{
    for (int i=0; i<N; i++)
    {
        node *tmp = table[i];
        while (tmp != NULL)
        {
            node *next = tmp->next;
            printf("%i: %s(%i)\n", i, tmp->word, tmp->wordLength);
            tmp = next;
        }
    }
}
int main(void)
{
    // load("./dictionaries/large");
    // dumpTable();
    load("");
    putWordInTable("a");
    putWordInTable("b");
    putWordInTable("c");
    printf("%d\n", wordcmp("Hello","hellO",5));
    printf("%d\n", wordcmp("Bello","hellO",5));
    putWordInTable("Test");
    putWordInTable("Test1");
    putWordInTable("Test12345");
    putWordInTable("Test123");
    putWordInTable("Test12");
    putWordInTable("Test13");
    putWordInTable("Test1234");
    putWordInTable("Test14");
    putWordInTable("Test123");
    dumpTable();
    printf("%d\n", check("foo"));
    printf("%d\n", check("test"));
    printf("%d\n", check("test1"));
    printf("%d\n", check("test12"));
    printf("%d\n", check("test13"));
    printf("%d\n", check("test14"));
    printf("%d\n", check("test123"));
    printf("%d\n", check("test1234"));
    printf("%d\n", check("test12345"));
}
*/
