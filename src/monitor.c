#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>


#include "datatypes.h"
#include "utilities.h"
#include "skip_list.h"
#include "bloom_filter.h"


/*
	Run executable: ./Monitor fifoPath1 fifoPath2 fifoPath3 fifoPath4

*/


int main(int argc, char **argv){


	if(argc != 5){
		printf("Error in arguments! Try again\n");
		printf("Usage: ./Monitor fifoPath1 fifoPath2 fifoPath3 fifoPath4\n");
		exit(EXIT_FAILURE);
	}

	
	FILE *read_request_fifo = fopen(argv[1], "r");
	FILE *read_countries_fifo = fopen(argv[2], "r");
	FILE *write_bloomfilters = fopen(argv[3], "w");
	FILE *write_response = fopen(argv[4], "w");

	// int bloom_size = 0;
	// if(fread(&bloom_size, sizeof(int), 1, read_request_fifo)){
	// 	perror("Failed to read bloom filter size");
	// }


	// BloomFilterSet *bloomset = BloomInit(bloom_size);
	// SkipListSet *not_vaccinated = ListInit();

	// int message_size;
	// if(fread(&message_size, sizeof(int), 1, read_countries_fifo)){
	// 	perror("Failed to read countries message size");
	// }

	// char country[100];
	// while(fscanf("%s\n", country) > 0){
		
	// }






	fclose(read_request_fifo);
	fclose(read_countries_fifo);
	fclose(write_bloomfilters);
	fclose(write_response);


	return 0;
}