#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <dirent.h>



#include "datatypes.h"
#include "utilities.h"

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

	if(buffer_size < 256){
		printf("ERROR: buffer size should be >= %ld\n", sizeof(int));
		exit(EXIT_FAILURE);
	}



/////////////////////////////*		Prepare local data		*/////////////////////////////

	MonitorInfo *monitors_map = malloc(num_monitors * sizeof(MonitorInfo));
	struct dirent **subdirs;
    int num_countries = scandir(input_dir, &subdirs, NULL, alphasort);
    
    if(num_countries <= 0){
        perror("ERROR! scandir failure:\n");
        exit(EXIT_FAILURE);
    }
    
	if(num_countries < num_monitors)		// too many monitors => only keep as many as necessary
		num_monitors = num_countries;

	int i;
  
  
	//	populate the monitor map and create fifos	//

  	char *fifo_name = malloc(40 * sizeof(char)); 
	for(i = 0; i < num_monitors; i++){
		monitors_map[i].fifo_names = malloc(4 * sizeof(char *));
		monitors_map[i].fifo_streams = malloc(4 * sizeof(FILE *));

		sprintf(fifo_name, "../request_toMonitor%d", i);
		monitors_map[i].fifo_names[0] = strdup(fifo_name);
		errno = 0;
		if((mkfifo(monitors_map[i].fifo_names[0], 0666) < 0) && (errno != EEXIST))
			perror("Failed to create fifo 1");

		sprintf(fifo_name, "../countries_toMonitor%d", i);
		monitors_map[i].fifo_names[1] = strdup(fifo_name);
		errno = 0;
		if((mkfifo(monitors_map[i].fifo_names[1], 0666) < 0) && (errno != EEXIST))
			perror("Failed to create fifo 2");

		sprintf(fifo_name, "../response_fromMonitor%d", i);
		monitors_map[i].fifo_names[2] = strdup(fifo_name);
		errno = 0;
		if((mkfifo(monitors_map[i].fifo_names[2], 0666) < 0) && (errno != EEXIST))
			perror("Failed to create fifo 3");

		sprintf(fifo_name, "../bloomfilter_fromMonitor%d", i);
		monitors_map[i].fifo_names[3] = strdup(fifo_name);
		errno = 0;
		if((mkfifo(monitors_map[i].fifo_names[3], 0666) < 0) && (errno != EEXIST))
			perror("Failed to create fifo 4");

	} 


	free(fifo_name);



/////////////////////////////*		Create Monitors		*/////////////////////////////

	pid_t chpid;
	// char *buffer; 

	for(int i = 0; i < num_monitors; i++){
		chpid = fork();
		switch(chpid){
			case -1:	// fork failure

				perror("Failed to fork");
				exit(1);

			case 0:		// child clause

				if(execvp("./bin/Monitor", monitors_map[i].fifo_names) == -1){
					perror("Failed to exec Monitor");
					exit(1);
				}
				break;

			default:
				monitors_map[i].monitor_id = chpid;

				//	open fifo streams //

				monitors_map[i].fifo_streams[0] = fopen(monitors_map[i].fifo_names[0], "w");
				if(monitors_map[i].fifo_streams[0] == NULL){
					perror("TravelMonitor failed to open fifo 1");
					exit(EXIT_FAILURE);
				}
				
				// if (fcntl(fileno(monitors_map[i]->fifo_streams[0]), F_SETPIPE_SZ, buffer_size) < 0) {
				// 	perror("TravelMonitor set buffersize failed");
				// 	exit(EXIT_FAILURE);
				// }

				monitors_map[i].fifo_streams[1] = fopen(monitors_map[i].fifo_names[1], "w");
				if(monitors_map[i].fifo_streams[1] == NULL)
					perror("TravelMonitor failed to open fifo 2");

				// if (fcntl(fileno(monitors_map[i]->fifo_streams[1]), F_SETPIPE_SZ, buffer_size) < 0) {
				// 	perror("TravelMonitor set buffersize failed");
				// 	exit(EXIT_FAILURE);
				// }

				monitors_map[i].fifo_streams[2] = fopen(monitors_map[i].fifo_names[2], "r");
				if(monitors_map[i].fifo_streams[2] == NULL)
					perror("TravelMonitor failed to open fifo 3");

				// if (fcntl(fileno(monitors_map[i]->fifo_streams[2]), F_SETPIPE_SZ, buffer_size) < 0) {
				// 	perror("TravelMonitor set buffersize failed");
				// 	exit(EXIT_FAILURE);
				// }

				monitors_map[i].fifo_streams[3] = fopen(monitors_map[i].fifo_names[3], "r");
				if(monitors_map[i].fifo_streams[3] == NULL)
					perror("TravelMonitor failed to open fifo 4");

				// if (fcntl(fileno(monitors_map[i]->fifo_streams[3]), F_SETPIPE_SZ, buffer_size) < 0) {
				// 	perror("TravelMonitor set buffersize failed");
				// 	exit(EXIT_FAILURE);
				// }

		}

	}


/////////////////////////////*		Bloom Filter Set logistics		*/////////////////////////////


	BloomFilterSet *bloomset = BloomInit(bloom_size);		// initialize bloom filter set
	if(bloomset == NULL){
		printf("TravelMonitor failed to initialize bloom filter set. Please try again...\n");
		exit(EXIT_FAILURE);
	}

	// for(i = 0; i < num_monitors; i++){
	// 	if(fwrite(sizeOfBloom, sizeof(int), 1, monitors_map[i].fifo_streams[0] != 1){		// write sizeOfBloom to monitors
	// 		perror("Write to request_toMonitor failed");
	// 	}
	// }





	for(i = 0; i < num_monitors; i++){
		for (int k = 0; k < 4; k++){
			fclose(monitors_map[i].fifo_streams[k]);
			unlink(monitors_map[i].fifo_names[k]);
			free(monitors_map[i].fifo_streams[k]);
			free(monitors_map[i].fifo_names[k]);
		}
	}
	free(monitors_map);
	free(input_dir);

}