#pragma once

#include "bloom_filter.h"
#include "skip_list.h"
#include "datatypes.h"

Date *GetDate(const char *);
int InputParser(const char *, BloomFilterSet *, SkipListSet *, SkipListSet *);
