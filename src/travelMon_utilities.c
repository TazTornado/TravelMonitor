#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "travelMon_utilities.h"
#include "bloom_filter.h"
#include "skip_list.h"
#include "datatypes.h"


///////////////////     Basic utilities     ///////////////////

char **ArgHandler(int argc, char **argv){
/*
	Does the error checking for inline parameters of the program.
	Returns a clean vector of the 2 arguments as strings.
*/

	char **args = malloc(4 * sizeof(char*));	// argument vector to return
	
	if(args == NULL){
		perror("ArgHandler -> malloc error ");
		return NULL;
	} 		
	int mflag = 0, bflag = 0, sflag = 0, iflag = 0;       	// flags to check if all inline parameters were provided

	while(--argc){ 
		if(strcmp(*argv, "-m") == 0){		// numMonitors
			if(mflag == 0){
				args[0] = strdup(*(argv + 1));
				mflag = 1;
			} else {
				printf("Invalid argument.\n./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir\n\n");
				free(args);
				return NULL;
			}
		}
		if(strcmp(*argv, "-b") == 0){		// bufferSize
			if(bflag == 0){
				args[1] = strdup(*(argv + 1));
				bflag = 1;
			} else {
				printf("Invalid argument.\n./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir\n\n");
				free(args);
				return NULL;
			}
		}

		if(strcmp(*argv, "-s") == 0){		// sizeOfBloom
			if(sflag == 0){
				args[2] = strdup(*(argv + 1));
				sflag = 1;
			} else {
				printf("Invalid argument.\n./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir\n\n");
				free(args);
				return NULL;
			}
		}

		if(strcmp(*argv, "-i") == 0){		// input_dir
			if(iflag == 0){
				args[2] = strdup(*(argv + 1));
				iflag = 1;
			} else {
				printf("Invalid argument.\n./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir\n\n");
				free(args);
				return NULL;
			}
		}

		argv++;
	}

	if((mflag != 1) || (bflag != 1) || (sflag != 1) || (iflag != 1)){		
		printf("Usage: ./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir\nAll parameters are mandatory, flexible order.\n");
		free(args);
		return NULL;
	}

	return args;
}



// int GetSubdirs(const char *dir_path){
// 	int count = 0;
// 	DIR *directory;
// 	struct dirent *info;

// 	directory = opendir(dir_path);
// 	if(directory == NULL){
// 		perror("CountDirs failed to open directory");
// 		return -1;
// 	}

// 	while ((info = readdir(directory)) != NULL) {
// 		if (info->d_type == DT_DIR) 
// 			count++;
// 	}
// 	closedir(directory);
// 	return count;
// }


///////////////////     Main functions of the application     ///////////////////


// void vaccineStatusBloom(char *buffer, char *position, BloomFilterSet *set){
// /*
// 	command: /vaccineStatusBloom citizenID virusName

// 	Checks if there is an extra argument. 
// 	Does NOT check whether no arguments were given or if given arguments are correct.
// 	Calls BloomTest for given citizen and virus.

// */

// 	int ID = atoi(strtok_r(NULL, " ", &position));
// 	if(ID < 0){
// 		printf("Error in ID %d. Try again please..\n", ID);
// 		printf("Syntax: /vaccineStatusBloom citizenID virusName \n");
// 	}

// 	char *virus = strdup(strtok_r(NULL, " ", &position));
	
// 	// check if there are too many arguments
// 	char *extra_arg = strtok_r(NULL, " ", &position);

// 	if(extra_arg == NULL){	// command parameters were provided correctly

// 		virus[strcspn(virus, "\n")] = 0;	// get rid of the new line character in token
// 		int result = BloomTest(set, virus, ID);
// 		free(virus);
		
// 		switch(result){
// 			case 0:
// 				printf("NOT VACCINATED\n");
// 				break;
// 			case 1:
// 				printf("MAYBE\n");
// 				break;
// 			case 2:
// 				printf("Syntax: /vaccineStatusBloom citizenID virusName\n");
// 				break;
// 		}

// 	} else {				// error: extra command paratemers were given
// 		free(virus);
// 		printf("Error in command syntax.\n");
// 		printf("Syntax: /vaccineStatusBloom citizenID virusName\n");
// 	}
// }


// void VaccineStatus(char *buffer, char *position, SkipListSet *vaccinated, SkipListSet *not_vaccinated){
// /*
// 	command: /vaccineStatus citizenID virusName or /vaccineStatus citizenID

// 	Checks if there is an extra argument. 
// 	Does NOT check whether no arguments were given or if given arguments are correct.
// 	Calls BloomTest for given citizen and virus.

// */
// 	char *stringID = strtok_r(NULL, " ", &position);
// 	stringID[strcspn(stringID, "\n")] = 0;				// if only the ID was given, get rid of "\n"

// 	int ID = atoi(stringID);
// 	if(ID < 0){
// 		printf("Error in ID %d. Try again please..\n", ID);
// 		printf("Syntax: /vaccineStatusBloom citizenID virusName \n");
// 	}

// 	// check if virusName was provided
// 	char *virus; 
// 	char *extra_arg = strtok_r(NULL, " ", &position);

// 	if((extra_arg != NULL) && (strcmp(extra_arg, "\n") != 0)){		// if not provided, the token will just be "\n"

// 	/////////////////	 virusName provided		/////////////////

// 		virus = strdup(extra_arg);
// 		SkipList *virus_list = ListSearchVirus(vaccinated, virus);		// check if there is a skip list for this virus
// 		if(virus_list == NULL){
// 			printf("NOT VACCINATED\n");
// 			return;
// 		}

// 		SkipNode *entry = ListFindClosest(vaccinated, virus_list, ID);
// 		if(entry->ID == ID){											
// 			printf("VACCINATED ON %s;", entry->details->date_vaccinated);	// entry with this ID was found

// 		} else {
// 			printf("NOT VACCINATED\n");			// ID was not found	
// 		}

// 	} else {	

// 		/////////////////	 virusName not provided		/////////////////

// 		// traverse the vaccinated set
// 		SkipList *current_list = vaccinated->first;
// 		SkipNode *entry;

// 		while(current_list != NULL){
// 			entry = ListFindClosest(vaccinated, current_list, ID);
// 			if(entry->ID == ID)
// 				printf("%s YES %s\n", current_list->virus, entry->details->date_vaccinated);

// 			current_list = current_list->next;
// 		}

// 		// traverse the vaccinated set
// 		current_list = not_vaccinated->first;

// 		while(current_list != NULL){
// 			entry = ListFindClosest(not_vaccinated, current_list, ID);
// 			if(entry->ID == ID)
// 				printf("%s NO\n", current_list->virus);

// 			current_list = current_list->next;
// 		}
			
// 	}

// }


// void insertCitizenRecord(char *buffer, char *position, HashTable *table, BloomFilterSet *bloomset, SkipListSet *vaccinated, SkipListSet *not_vaccinated){
// /*
// 	command: /insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]

// 	Checks if date was provided. 
// 	Does NOT check whether no arguments were given or if given arguments are correct.
// 	Calls BloomTest for given citizen and virus.

// */	

// 	Citizen *rec = malloc(sizeof(Citizen));             // create a new struct for record
// 	if(rec == NULL){
// 		perror("insertCitizenRecord -> failed to alloc citizen struct");
// 		return;
// 	}
// 	rec->citizenID = atoi(strtok_r(NULL, " ", &position));			// get citizen ID from record
// 	rec->first_name = strdup(strtok_r(NULL, " ", &position));  		// get citizen's name from record
// 	rec->last_name = strdup(strtok_r(NULL, " ", &position));  		// get citizen's last name from record
// 	rec->country = strdup(strtok_r(NULL, " ", &position));    		// get citizen's country from record
// 	rec->age = atoi(strtok_r(NULL, " ", &position));           		// get citizen's age year from record
// 	rec->virus =  strdup(strtok_r(NULL, " ", &position));			// get virus for which citizen has/hasn't been vaccinated
// 	rec->yes_or_no = strdup(strtok_r(NULL, " ", &position));		// a string showing if citizen is vaccinated or not
// 	rec->date_vaccinated = strtok_r(NULL, " ", &position);			// get date of vaccination
	
// 	if((strcmp(rec->yes_or_no, "NO") == 0)){	
// 		if(strcmp(rec->date_vaccinated, "\n") != 0){		// "NO" should be followed by just a "\n"

// 			printf("ERROR IN SYNTAX. Only 'YES' can be followed by a date.\n");
// 			free(rec->first_name);
// 			free(rec->last_name);
// 			free(rec->country);
// 			free(rec->virus);
// 			free(rec->yes_or_no);
// 			free(rec);
// 			return;

// 		} else {
// 			rec->date_vaccinated = NULL;	// just keep the pointer NULL
// 		} 
// 	}

// 	char *temp_date;
// 	if(rec->date_vaccinated != NULL){
// 		temp_date = rec->date_vaccinated;
// 		rec->date_vaccinated = strndup(temp_date, 10);	// cut the '\n' character from the original date string
// 	}
	
// 	// check if citizen is already vaccinated
// 	if(strcmp(rec->yes_or_no, "YES") == 0){
// 		SkipList *list = ListSearchVirus(vaccinated, rec->virus);	// check if virus exists in vaccinated_persons

// 		if(list != NULL){
// 			SkipNode *entry = ListFindClosest(vaccinated, list, rec->citizenID);
// 			if(entry->ID == rec->citizenID){
// 				printf("ERROR: CITIZEN %d ALREADY VACCINATED ON %s\n", entry->details->citizenID, entry->details->date_vaccinated);
// 				free(rec->first_name);
// 				free(rec->last_name);
// 				free(rec->country);
// 				free(rec->virus);
// 				free(rec->yes_or_no);
// 				free(rec->date_vaccinated);
// 				free(rec);
// 				return;
// 			}
// 		} else {	
// 			int retval = ListAddVirus(vaccinated, rec->virus);	// add a skip list for this virus
// 			if(retval != 1){
// 				printf("Failed to add virus %s in vaccinated_person set.\n", rec->virus);
// 				free(rec->first_name);
// 				free(rec->last_name);
// 				free(rec->country);
// 				free(rec->virus);
// 				free(rec->yes_or_no);
// 				free(rec->date_vaccinated);
// 				free(rec);
// 				return;
// 			}

// 		}

// 		// insert citizen to vaccinated_skip list
// 		int retval = ListInsertCitizen(vaccinated, rec);
// 		if(retval != 1){
// 			printf("Failed to add citizen %d %s %s to vaccinated_persons skip list.\n", rec->citizenID, rec->first_name, rec->last_name);
// 			free(rec->first_name);
// 			free(rec->last_name);
// 			free(rec->country);
// 			free(rec->virus);
// 			free(rec->yes_or_no);
// 			free(rec->date_vaccinated);
// 			free(rec);
// 			return;
// 		} 
// 	} else {	
// 		// check if already registered

// 		SkipList *list = ListSearchVirus(not_vaccinated, rec->virus);	// check if virus exists in vaccinated_persons

// 		if(list != NULL){
// 			SkipNode *entry = ListFindClosest(not_vaccinated, list, rec->citizenID);
// 			if(entry->ID == rec->citizenID){
// 				printf("ERROR: CITIZEN %d ALREADY REGISTERED\n", entry->details->citizenID);
// 				free(rec->first_name);
// 				free(rec->last_name);
// 				free(rec->country);
// 				free(rec->virus);
// 				free(rec->yes_or_no);
// 				free(rec);
// 				return;
// 			}
// 		} else {	
// 			int retval = ListAddVirus(not_vaccinated, rec->virus);	// add a skip list for this virus
// 			if(retval != 1){
// 				printf("Failed to add virus %s in not_vaccinated_persons skip.\n", rec->virus);
// 				free(rec->first_name);
// 				free(rec->last_name);
// 				free(rec->country);
// 				free(rec->virus);
// 				free(rec->yes_or_no);
// 				free(rec);
// 				return;
// 			}
// 		}
		
// 		// insert citizen to vaccinated_skip list
// 		int retval = ListInsertCitizen(not_vaccinated, rec);
// 		if(retval != 1){
// 			printf("Failed to add citizen %d %s %s to not_vaccinated_persons skip list.\n", rec->citizenID, rec->first_name, rec->last_name);
// 			free(rec->first_name);
// 			free(rec->last_name);
// 			free(rec->country);
// 			free(rec->virus);
// 			free(rec->yes_or_no);
// 			free(rec);
// 			return;
// 		} 
// 	}

// 	// add citizen to the hash table
// 	int return_value = HashInsert(table, rec);
// 	if(return_value != 1){
// 		printf("Failed to add citizen %d %s %s to the hash table.\n", rec->citizenID, rec->first_name, rec->last_name);		
// 		free(rec->first_name);
// 		free(rec->last_name);
// 		free(rec->country);
// 		free(rec->virus);
// 		free(rec->yes_or_no);
// 		if(rec->date_vaccinated != NULL)
// 			free(rec->date_vaccinated);
// 		free(rec);
// 		return;
// 	}
		
		
// 	BloomFilter *entry = BloomSearch(bloomset, rec->virus);		// check if bloom filter for this virus exists
// 	if(entry == NULL){
// 		return_value = BloomAddVirus(bloomset, rec->virus);			// does not exist => add it to the set
// 		if(return_value != 1)
// 			printf("Failed to add virus %s to the bloom filter set.\n", rec->virus);

// 	}

// 	if(strcmp(rec->yes_or_no, "YES") == 0){		// vaccinated person
// 		return_value = BloomInsertElement(bloomset, rec->citizenID, rec->virus);

// 		if(return_value != 1)
// 			printf("Failed to insert citizen %d %s %s in the bloom filter.\n", rec->citizenID, rec->first_name, rec->last_name);		
// 	}
// }
