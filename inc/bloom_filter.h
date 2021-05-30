#pragma once


typedef struct BloomFilter{	
	struct BloomFilter *next;
	char *virus;		// the virus name is also it's unique ID 
	char *bitmap;		// array of 'BloomFilterSet.size' chars 
}BloomFilter;


// a list of bloom filters of all viruses
typedef struct BloomFilterSet{ 	
	int size;		// number of bits in bitmap
	BloomFilter *first;		// first bloom filter struct
	BloomFilter *last;		// last bloom filter struct
} BloomFilterSet;


unsigned long djb2(unsigned char *);
unsigned long sdbm(unsigned char *);
unsigned long hash_i(unsigned char *,unsigned int);
BloomFilterSet *BloomInit(int);
BloomFilter *BloomSearch(BloomFilterSet *, const char *);
void SetBits(BloomFilter *, const char *, int);
void CombineFilters(BloomFilterSet *, BloomFilter *, int);
int BloomAddVirus(BloomFilterSet *, const char *);
int BloomInsertElement(BloomFilterSet *, int , const char *);
int BloomTest(BloomFilterSet *, const char *, int );
void BloomDisplay(BloomFilterSet *);
void BloomDestroy(BloomFilterSet *);