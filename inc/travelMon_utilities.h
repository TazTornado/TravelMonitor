#pragma once

#include "bloom_filter.h"
#include "skip_list.h"

char **ArgHandler(int, char **);
int CountSubdirs(const char *);
int InputParser(const char *, HashTable *, BloomFilterSet *, SkipListSet *, SkipListSet *);

void vaccineStatusBloom(char *, char *, BloomFilterSet *);
void VaccineStatus(char *, char *, SkipListSet *, SkipListSet *);
void insertCitizenRecord(char *, char *, HashTable *, BloomFilterSet *, SkipListSet *, SkipListSet *);
