#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bloom_filter.h"
#define K 16		// hash_i will always be called 16 times


/*
This algorithm (k=33) was first reported by dan bernstein many years 
ago in comp.lang.c. 
The magic of number 33 (why it works better than many other constants, 
prime or not) has never been adequately explained.
*/
unsigned long djb2(unsigned char *str) {
	unsigned long hash = 5381;
	int c; 
	while (c = *str++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}


/*
This algorithm was created for sdbm (a public-domain reimplementation of ndbm) 
database library. it was found to do well in scrambling bits, causing better 
distribution of the keys and fewer splits. it also happens to be a good 
general hashing function with good distribution. The actual function 
is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below 
is the faster version used in gawk. There is even a faster, duff-device 
version. The magic constant 65599 was picked out of thin air while experimenting 
with different constants, and turns out to be a prime. this is one of the 
algorithms used in berkeley db (see sleepycat) and elsewhere.
*/
unsigned long sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}


/* 
Return the result of the Kth hash funcation. This function uses djb2 and sdbm.
None of the functions used here is strong for cryptography purposes but they
are good enough for the purpose of the class.

The approach in this function is based on the paper:
https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf
*/
unsigned long hash_i(unsigned char *str, unsigned int i) {
	return djb2(str) + i*sdbm(str) + i*i;
}




/////////////////////////// Initialize Set of Bloom Filters ///////////////////////////

BloomFilterSet *BloomInit(int mapsize){
// creates the structure for the bloom filter set
// returns NULL in case of malloc failure
// and a pointer to the set in case of success

	BloomFilterSet *set = malloc(sizeof(BloomFilterSet));
	if(set == NULL){
		printf("Allocation of Bloom Filter Set struct has failed.\n");
		return NULL;
	}

	// initialize data members
	set->size = mapsize;
	set->first = NULL;
	set->last = NULL;

	return set;
}



///////////////////////////		Search  (on virus name)		///////////////////////////

BloomFilter *BloomSearch(BloomFilterSet *set, const char *virus){
// traverses the list of bloom filters in the set
// if a bloom filter for given virus is found, a pointer to that is returned
// if not found, it returns NULL

	if((set->last != NULL) && (strcmp(set->last->virus, virus) == 0)){		// save some time in case the virus was just added to the set
		return set->last;							// by not traversing the list
	}
	
	BloomFilter *entry = set->first;

	while(entry != NULL){
		if(strcmp(entry->virus, virus) == 0)
			return entry;
		
		entry = entry->next;
	} 

	return NULL;        // filter on given virus was not found
}



/////////////////////////////////		Insertion		/////////////////////////////////

void SetBits(BloomFilter *virusNode, const char *ID, int mapsize){
// for a given ID, the function sets 
// the bitmap bits to 1 according
// to the result of hash_i


	unsigned int i;
	unsigned long index;
	for( i = 0; i < K; i++){		// call hash_i, K times
		index = (int) (hash_i((unsigned char *)(ID), i) % mapsize);
		virusNode->bitmap[index] = '1';
	}
}


void CombineFilters(BloomFilterSet *set, BloomFilter *virusNode, int mapsize){
	BloomFilter *existing_node = BloomSearch(set, virusNode->virus);

	if(existing_node == NULL){
		set->last->next = virusNode;
		virusNode->next = NULL;
		set->last = virusNode;
	} else {
		for(int i = 0; i < mapsize; i++){
			if((existing_node->bitmap[i] == '0') && (virusNode->bitmap[i] == '1'))
				existing_node->bitmap[i] = '1';

			free(virusNode->bitmap);
			free(virusNode->virus);
			free(virusNode);
		}
	}
}




int BloomAddVirus(BloomFilterSet *set, const char *virus){
// adds a bloom filter for given virus to the set
// it does NOT check whether a bloom filter for the virus already exists
// returns 0 for malloc failure
// and 1 for success



	BloomFilter *current_last = set->last;		// get last filter in chain

	if(set->first == NULL){			// case 1: this is the 1st bloom filter in the set

		current_last = malloc(sizeof(BloomFilter));				// alloc space for last->next
		if(current_last == NULL){
			perror("Malloc failure in BloomInsert");
			return 0;
		}

		current_last->next = NULL;
		current_last->virus = strdup(virus);						// set ID of new bloom filter
		current_last->bitmap = malloc((set->size + 1) * sizeof(char));
		if(current_last->bitmap == NULL){
			perror("Malloc failure in BloomAddVirus (bitmap)");
			return 0;
		}	
		memset(current_last->bitmap, '0', set->size);				// initialize bits to 0
		memset(current_last->bitmap + set->size, '\0', 1);		// must be null-terminated to display
		set->first = current_last;
		set->last = current_last;
		
	} else {			// case 2: there are alrady bloom filters in chain
			
		current_last->next = malloc(sizeof(BloomFilter));				// alloc space for last->next
		if(current_last->next == NULL){
			perror("Malloc failure in BloomInsert (next)");
			return 0;
		}

		current_last->next->next = NULL;
		current_last->next->virus = strdup(virus);						// set ID of new bloom filter
		current_last->next->bitmap = malloc((set->size + 1) * sizeof(char));	
		if(current_last->next->bitmap == NULL){
			perror("Malloc failure in BloomAddVirus (bitmap)");
			return 0;
		}
		memset(current_last->next->bitmap, '0', set->size);				// initialize bits to 0
		memset(current_last->next->bitmap + set->size, '\0', 1);		// must be null-terminated to display
		set->last = current_last->next;									// update set->last 
	}

		return 1;
}




int BloomInsertElement(BloomFilterSet *set , int ID, const char *virus){
// checks for existance of a bloom filter for the virus
// if found, the function sets the bits for given ID according to hash_i
// returns 0 if bloom filter was not found
// and 1 for success

	BloomFilter *filter = BloomSearch(set, virus);
	
	if(filter == NULL){
		printf("Bloom Filter for %s has not been found\n", virus);
		return 0;
	} else {
		char *stringID = NULL;
		int length = snprintf(stringID, 0, "%u", ID);		// find out the length of the string ID
		stringID = malloc((length + 1) * sizeof(char));		// allocate the necessary space for this ID
		if(stringID == NULL){
			perror("Malloc failure in BloomInsertElement (stringID)");
			return 0;
		}
		sprintf(stringID, "%u", ID);
		SetBits(filter, stringID, set->size);				// use hash_i to set bitmap
		free(stringID);
	}
	return 1;
}




///////////////////		Check Existance of element in a filter		///////////////////

int BloomTest(BloomFilterSet *set, const char *virus, int ID){
// the filtering happens here
// if the bloom filter for virus was found, 
// hash_i is used to check whether the ID is in the set
// returns 1 if all bits are 1
// or 0 if a 0-bit is found

	BloomFilter *filter = BloomSearch(set, virus);
	if(filter == NULL){
		printf("Bloom Filter for %s has not been found\n", virus);
		return 2;
	} else {
		char *stringID = NULL;
		int length = snprintf(stringID, 0, "%u", ID);		// find out the length of the string ID
		stringID = malloc((length + 1) * sizeof(char));		// allocate the necessary space for this ID
		if(stringID == NULL){
			perror("Malloc failure in BloomTest (stringID)");
			return 0;
		}
		sprintf(stringID, "%u", ID);

		unsigned int i;
		unsigned long index;
		for( i = 0; i < K; i++){
			index = (int) (hash_i((unsigned char *)(stringID), i) % set->size);
			if(filter->bitmap[index] == '0'){
				free(stringID);
				return 0;		// definitely not in the set 
			}
		}
		free(stringID);
		return 1; 			// maybe in the set

	}
}




////////////////	Print All Bloom Filter structures in the Set 	////////////////

void BloomDisplay(BloomFilterSet *set){
// diplays the bitmap of ech bloom filter in the set

	BloomFilter *filter = set->first;
	
	while(filter != NULL){
		printf("\nBloom Filter: %s\n", filter->virus);
		printf("-------------------------------------------------\n");
		printf("%s\n", filter->bitmap);
		printf("-------------------------------------------------\n");
		filter = filter->next;
	}
}



////////////////	Free all allocated space for the Bloom FIlters		////////////////

void BloomDestroy(BloomFilterSet *set){
// frees alloc'd memory for the bitmap of each bloom filter
// frees all bloom filter structures and the set

	BloomFilter *current = set->first, *next = NULL;

	while(current != NULL){		// free space of attributes in each bloom filter struct
		next = current->next;
		free(current->virus);
		free(current->bitmap);
		free(current);
		current = next;
	}

	free(set);			// free space of the whole set struct
}