#pragma once

#include "bloom_filter.h"
#include "skip_list.h"
#include "datatypes.h"

typedef struct SubdirsInfo{
	int count;					// number of subdirectories
	char **subdirectories;		// names of subdirectories
}


typedef struct MonitorInfo{
	pid_t monitor_id;
	char **fifo_names;
	FILE **fifo_streams;
} MonitorInfo;



char **ArgHandler(int, char **);
