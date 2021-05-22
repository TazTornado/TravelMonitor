#pragma once

#include "datatypes.h"

// node of skip list, covers all levels
typedef struct SkipNode{   
	int ID;				// keep the id of the citizen pointed to, for easier checking
	Citizen *details;	// points to a citizen
	// doubly linked list, hence the left and right set of pointers
	struct SkipNode **right;
	struct SkipNode **left;
} SkipNode;

// there will be once such struct per virus
typedef struct SkipList{
	char *virus;		// the virus is the ID of the skip list
	SkipNode *head;
	SkipNode *nil;
	struct SkipList *next;	
} SkipList;


// a list of skip lists of all viruses (either vaccinated or not_vaccinated)
typedef struct SkipListSet{  
	int max_level; 
	SkipList *first;	// first skip list
	SkipList *last;		// last skip list
} SkipListSet;



SkipListSet *ListInit();
int FindStartingLevel(SkipListSet *, SkipList *);
SkipNode *ListFindClosest(SkipListSet *, SkipList *, int);
SkipList *ListSearchVirus(SkipListSet *, const char *);
int ListAddVirus(SkipListSet *, const char *);
int RandomLevel(SkipListSet *);
SkipNode *Backtrack(SkipNode *, SkipList *, int);
int ListInsertCitizen(SkipListSet *, Citizen *);
int ListRemoveCitizen(SkipListSet *, int , const char *);
void ListDestroy(SkipListSet *);