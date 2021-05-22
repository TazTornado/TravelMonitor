#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "datatypes.h"
#include "skip_list.h"

#define POS_INF INT_MAX
#define NEG_INF INT_MIN
#define MAX_LEVEL 10


////////////////////////////////	 Initialization		////////////////////////////////

SkipListSet *ListInit(){
	SkipListSet *set = malloc(sizeof(SkipListSet));			// allocating space for the skip list set struct
	if(set == NULL){
		perror("Allocation of SkipListSet struct has failed");
		return NULL;
	}

	// initialize as empty (for now)
	set->first = NULL;		
	set->last = NULL;
	set->max_level = MAX_LEVEL;		// maximum level for the skip list

	return set;
}


//////////////////////////////////		Search		/////////////////////////////////

int FindStartingLevel(SkipListSet *set, SkipList *list){
// Traverses the head's set of forward pointers top to bottom
// to find one pointing to a citizen node.
// Returns the index/level of the first such pointer.

	SkipNode *node = list->head;
	if(node == list->nil){		// the skip list has no citizens in yet
		return 0;				// so start at the bottom
	}

	for(int i = set->max_level-1; i >= 0; i--){
		if(node->right[i] != list->nil){
			return i;
		}
	}
	return 0;
}

SkipNode *ListFindClosest(SkipListSet *set, SkipList *list, int ID){
// Traverses the list, starting from the top useful level
// and going forward or downward
// returns a pointer to the exact or the previous node with ID = given ID

	int level = FindStartingLevel(set, list);

	SkipNode *node = list->head;
	while(level > 0){
		if(node->right[level]->ID < ID){
			// go forward
			node = node->right[level];

		} else if(node->right[level]->ID > ID){
			// go downward
			level--;
		} else {
			// the ID of next node in this level is equal to given ID
			node = node->right[level];
			level = 0;
		}
	}
	// at bottom level now, only go forward
	while(node->right[0]->ID <= ID){
		node = node->right[0];
		if(node->ID == ID)
			break;
	}

	// node should now hold either the exact citizen we are looking for, or the one that will be its previous
	return node;	
}

SkipList *ListSearchVirus(SkipListSet *set, const char *virus){
// Traverses the list of skip lists until it finds the one of given virus
// Returns a pointer to the skiplist struct,
// or NULL if there is no skip list for this virus

	SkipList *list = set->first;		// start at the beginning of the chain
	if(list == NULL){
		// printf("The set of skip lists is empty.\n");
		return NULL;
	}

	if((set->last != NULL) && (strcmp(set->last->virus, virus) == 0)){	// save some time in case the virus was just added to the set
		return set->last;					// by not traversing the list
	}

	while(list != NULL){			// traverse the list of skiplist nodes
		if(strcmp(list->virus, virus) == 0){
			break;					// if the virus is found, stop searching
		}
		list = list->next;
	}

	return list;	// NULL if virus was not found
}



////////////////////////////////	 Insertion		////////////////////////////////

////////////////	Virus Insertion	////////////////
int ListAddVirus(SkipListSet *set, const char *virus){
// adds a new SkipList struct in the set
// creates the head and nil nodes of the new skip list
// returns 0 for malloc failure
// and 1 for success 

	SkipList *list = malloc(sizeof(SkipList));		// allocate space for a skiplist struct
	if(list == NULL){
		perror("Allocation of new skip list struct has failed");
		return 0;
	}

	list->next = NULL;
	list->virus = strdup(virus);			// initialize ID of skip list
	int i;

	if(set->first == NULL){		// this is the first list in the set
		set->first = list;
	} else {					// not the first list => next to last 
		set->last->next = list;
	}
	set->last = list;		// list is now the new last in chain

	/////////////	sentinel/head node logistics	/////////////
	list->head = malloc(sizeof(SkipNode));		// allocate space for sentinel/head node
	if(list->head == NULL){
		perror("Allocation of new skip list sentinel has failed");
		return 0;
	}
	list->head->ID = NEG_INF;					// initialize attributes of sentinel node
	list->head->details = NULL;					// sentinel does not point to a citizen

	list->head->left = malloc(set->max_level * sizeof(SkipNode *));	// allocate space for backward pointers
	if(list->head->left == NULL){
		perror("Allocation of head->left pointers has failed");
		return 0;
	}
	list->head->right = malloc(set->max_level * sizeof(SkipNode *));	// allocate space for forward pointers
	if(list->head->right == NULL){
		perror("Allocation of head->right pointers has failed");
		return 0;
	}
	for(i = 0; i < set->max_level; i++){
		list->head->left[i] = NULL;		// head will only point forward
	}

	//////////////////	nil node logistics	//////////////////
	list->nil = malloc(sizeof(SkipNode));		// allocate space for nil node
	if(list->nil == NULL){
		perror("Allocation of new skip list nil has failed");
		return 0;
	}
	list->nil->ID = POS_INF;					// initialize attributes of nil node
	list->nil->details = NULL;					// nil does not point to a citizen
	list->nil->left = malloc(set->max_level * sizeof(SkipNode *));	// allocate space for backward pointers
	if(list->nil->left == NULL){
		perror("Allocation of nil->left pointers has failed");
		return 0;
	}
	list->nil->right = malloc(set->max_level * sizeof(SkipNode *));	// allocate space for forward pointers
	if(list->nil->right == NULL){
		perror("Allocation of nil->right pointers has failed");
		return 0;
	}
	for(i = 0; i < set->max_level; i++){
		list->nil->right[i] = NULL;		// nil will only point backward
	}

	// now link the head to nil both ways
	for(i = 0; i < set->max_level; i++){
		list->nil->left[i] = list->head;
		list->head->right[i] = list->nil;		
	}	

	return 1;

}

////////////////	Citizen Insertion	////////////////

int RandomLevel(SkipListSet *set){
// Decides randomly if a node will be promoted 
// to higher levels in skip list.
// 50% chance for 1st level, 25% chance for 2nd level etc
// Returns the level to which the node will be promoted (range: 0..MAX_LEVEL-1)

	srand(time(0));

	for(int i = 0; i < set->max_level; i++){
		if((rand() % 100) <= 50){			// (1/2) ^ i probability of promoting to level i
			return i;
		}
	}
	return set->max_level - 1;		// max level
}

SkipNode *Backtrack(SkipNode *node, SkipList *list, int current_level){
// This routine is used when a new citizen node in the skip list has been promoted.
// It finds the previous node that has a forward pointer in level 'current_level + 1'.
// It backtracks on the current_level and
// returns the first node it finds, that has a forward pointer on the above level.

	SkipNode *current_node = node->left[current_level];		// start from the exact previous node of given
	
	while(current_node->right[current_level + 1] == NULL){
		if(current_node == list->head){	// reached the head,
			break;							// can't backtrack any more
		} else {
			// backtrack move
			current_node = current_node->left[current_level];
		}
	}
	return current_node;
}

int ListInsertCitizen(SkipListSet *set, Citizen *citizen){
// If there is a skip list for given virus and the citizen is not in that list
// the function finds the right place to insert the node.
// Then, it promotes randomly the node to higher levels of the list
// Returns 0 for malloc failure,
// 1 for success and
// 2 if the skip list is not found or the citizen is already in the list.
	
	SkipList *list = ListSearchVirus(set, citizen->virus);
	if(list == NULL){
		printf("Skip List for %s has not been initialized!", citizen->virus);
		return 2;
	}
	
	SkipNode *closestnode = ListFindClosest(set, list, citizen->citizenID);
	if(closestnode->ID == citizen->citizenID){		// citizen is already in this skip list
		printf("Citizen with ID %d has already registered for %s\n", citizen->citizenID, citizen->virus);
		return 2;
	}

	// citizen is not in this skip list => new node logistics
	SkipNode *newnode = malloc(sizeof(SkipNode));
	if(newnode == NULL){
		perror("Allocation of new node in skip list has failed");
		return 0;
	}
	newnode->details = citizen;			// initialize attributes of new node
	newnode->ID = citizen->citizenID;
	newnode->right = malloc(set->max_level * sizeof(SkipNode *));	// allocate space for forward pointers
	if(newnode->right == NULL){
		perror("Allocation of new node forward pointers has failed");
		return 0;
	}
	newnode->left = malloc(set->max_level * sizeof(SkipNode *));	// allocate space for backward pointers
	if(newnode->left == NULL){
		perror("Allocation of new node backward pointers has failed");
		return 0;
	}
	newnode->right[0] = closestnode->right[0];	// link new node to its next
	newnode->left[0] = closestnode;				// link new node to its previous
	closestnode->right[0] = newnode;			// link previous to newnode
	newnode->right[0]->left[0] = newnode;		// link next to new node

	//////////////	promote node to higher levels	//////////////

	int prom_level = RandomLevel(set);
	SkipNode *anchor;			// the anchor to the level above
	for(int i = 0; i < prom_level; i++){		// for every level up to prom_level, 
		anchor = Backtrack(newnode, list, i);		// find the anchor to the level above and
		
		newnode->right[i+1] = anchor->right[i+1];	// link new node to its next,
		newnode->left[i+1] = anchor;				// link new node to its previous,
		anchor->right[i+1] = newnode;				// link previous to newnode,
		newnode->right[i+1]->left[i+1] = newnode;	// link next to new node

	}

	// initialize the pointers in higher levels to NULL
	for(int i = prom_level+1; i < set->max_level; i++){
		newnode->right[i] = NULL;
		newnode->left[i] = NULL;
	}

	return 1;
}



////////////////////////////////	 Deletion		////////////////////////////////

int ListRemoveCitizen(SkipListSet *set, int ID, const char *virus){
// If the skipnode for given ID exists,
// the function rehooks the node's previous with its next
// and frees any allocated memory for it.
// Returns 0 if citizen is not in the list
// and 1 for success

	SkipNode *entry = ListFindClosest(set, ListSearchVirus(set, virus), ID);	// check if citizen exists in list
	if(entry->ID != ID){
		printf("Citizen with ID %d has not been registered for virus %s\n", ID, virus);
		return 0;
	}

	int i = 0;
	while((i < set->max_level)){
		if(entry->right[i] != NULL){
			
			entry->left[i]->right[i] = entry->right[i];				// unhook node from its previous
			entry->right[i]->left[i] = entry->left[i];				// unhook node from its next
		}
		i++;
	}

	free(entry->right);
	free(entry->left);
	free(entry);
	return 1;
}


void ListDestroy(SkipListSet *set){
// Destroys the whole set of skip lists, list-by-list, node-by-node.

	SkipList *current_list = set->first, *next_list;
	SkipNode *current_node, *next_node;

	while(current_list != NULL){	// for every list in the set
		current_node = current_list->head;	
		
		while(current_node != NULL){	// for every node in the skip list
			next_node = current_node->right[0];		// hold next
			// free malloc'd members
			free(current_node->right);
			free(current_node->left);
			free(current_node);
			current_node = next_node;	// go to next node
		}

		next_list = current_list->next;	// hold next list
		free(current_list->virus); 
		free(current_list);
		current_list = next_list;
	}

	free(set);
}