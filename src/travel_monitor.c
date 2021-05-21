#include <stdio.h>
#include <stdlib.h>


#include "datatypes.h"
#include "travelMon_utilities.h"

/*
	Run executable: ./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir

*/


int main(int argc, char **argv){

	int num_monitors, buffer_size, bloom_size;
	char *input_dir;
	char **my_args = ArgHandler(argc, argv);

	if(my_args != NULL){
		num_monitors = atoi(my_args[0]);
		buffer_size = atoi(my_args[1]);
		bloom_size = 8 * (atoi(my_args[2]));	// size was given in bytes => convert to bits
		input_dir = strdup(my_args[3]);
		free(my_args[0]);
		free(my_args[1]);
		free(my_args[2]);
		free(my_args[3]);
		free(my_args);

	} else {
		printf("TravelMonitor failed to acquire parameters. Exiting now..\n");
		exit(EXIT_FAILURE);
	}

	int num_countries = CountSubdirs(input_dir);

	if(num_countries < num_monitors)		// too many monitors => only keep as many as necessary
		num_monitors = num_countries;

	if(buffer_size < 128){
		// error
	}

/////////////////////////////*		Create Monitors		*/////////////////////////////

	pid_t chpid;
	MonitorInfo *monitors_map = malloc(num_monitors * sizeof(MonitorInfo));










free(input_dir);

}