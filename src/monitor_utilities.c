#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monitor_utilities.h"
#include "bloom_filter.h"
#include "skip_list.h"
#include "datatypes.h"



Date *GetDate(const char *str_date){
/*
	Converts a string date to Date type.
	Does error checking on the string format and theday/month/year ranges.
	Returns a pointer to the Date struct if the conversion succeeds
	and NULL if it fails.
*/

	if(strlen(str_date > 11 || str_date[2] != '-' || str_date[5] != '-')){
		retrun NULL;
	}

	Date *dt = malloc(sizeof(Date));
	char *temp_day, *temp_month, *temp_year;

	temp_day = strndup(str_date, 2);
	temp_month = strndup(&str_date[3], 2);
	temp_year = strndup(&str_date[6], 4);

	dt->day = atoi(temp_day);
	dt->month = atoi(temp_month);
	dt->year = atoi(temp_year);

	if( dt->day < 1 || dt->day > 30 ||
		dt->month < 1 || dt->month > 12 ||
		dt->year < 1900 || dt->year > 2021 ) {
			
			free(temp_day);
			free(temp_month);
			free(temp_year);
			free(dt);
			return NULL;
		}

	return dt;
}




int InputParser(const char *filename, BloomFilterSet *bloomset, SkipListSet *vaccinated, SkipListSet *not_vaccinated){
/*
	Parses the input file and inserts the records
	in the structures of the database: hash table, bloom filters, skip lists
	Returns 0 in case of failure and 1 for success.
*/

	FILE *inputfile = fopen(filename, "r");        // open input file
	if(inputfile == NULL){
		perror("Could not open input file ");
		return 0;		
	}

	char *recordline = NULL;    // variable to hold each line of input file
	size_t line_size = 0;      	// let getline allocate just as much space as necessary  
	int return_value = 0;
	char *temp_date = NULL;

	while(getline(&recordline, &line_size, inputfile) > 0){   // read until end of file
		char *position;
		Citizen *rec = malloc(sizeof(Citizen));             // create a new struct for record
		if(rec == NULL){
			perror("InputParser -> failed to alloc citizen struct");
			return 0;
		}
		rec->citizenID = atoi(strtok_r(recordline, " ", &position));	// get citizen ID from record
		rec->first_name = strdup(strtok_r(NULL, " ", &position));  		// get citizen's name from record
		rec->last_name = strdup(strtok_r(NULL, " ", &position));  		// get citizen's last name from record
		rec->country = strdup(strtok_r(NULL, " ", &position));    		// get citizen's country from record
		rec->age = atoi(strtok_r(NULL, " ", &position));           		// get citizen's age year from record
		rec->virus =  strdup(strtok_r(NULL, " ", &position));			// get virus for which citizen has/hasn't been vaccinated
		rec->yes_or_no = strdup(strtok_r(NULL, " ", &position));		// a string showing if citizen is vaccinated or not
		temp_date = strtok_r(NULL, " ", &position);						// get date of vaccination

		if((strcmp(rec->yes_or_no, "NO") == 0)){	
			if(strcmp(temp_date, "\n") != 0){		// "NO" should be followed by just a "\n"

				printf("ERROR IN RECORD %d %s %s %s %d %s %s %s(ERROR: vaccination date format).\n", rec->citizenID, rec->first_name, rec->last_name, rec->country, rec->age, rec->virus, rec->yes_or_no, temp_date);
				free(rec->first_name);
				free(rec->last_name);
				free(rec->country);
				free(rec->virus);
				free(rec->yes_or_no);
				free(rec);
				continue;			

			} else {

				rec->date_vaccinated = NULL;	// just keep the pointer NULL
			}
		} 


		// check for error in date format
		if(temp_date != NULL){
			rec->date_vaccinated = GetDate(temp_date);
			if(rec->date_vaccinated == NULL){
				printf("ERROR IN RECORD %d %s %s %s %d %s %s %s(ERROR: vaccination date format).\n", rec->citizenID, rec->first_name, rec->last_name, rec->country, rec->age, rec->virus, rec->yes_or_no, temp_date);
				free(rec->first_name);
				free(rec->last_name);
				free(rec->country);
				free(rec->virus);
				free(rec->yes_or_no);
				free(rec);
				continue;
			}
		}

		int retval = 0;
		BloomFilter *entry = BloomSearch(bloomset, rec->virus);		// check if bloom filter for this virus exists
		if(entry == NULL){
			// printf("Virus %s bloom filter not found \n", rec->virus);
			retval = BloomAddVirus(bloomset, rec->virus);			// does not exist => add it to the set
			if(retval != 1){
				printf("Failed to add virus %s in set.\n", rec->virus);
			}
		}
		
		if(strcmp(rec->yes_or_no, "YES") == 0){		// vaccinated person

			SkipNode *citizen_entry;
			SkipList *list = ListSearchVirus(vaccinated, rec->virus);
			if(list == NULL){
				// printf("Vaccinated_persons Skip List for %s not found \n", rec->virus);
				retval = ListAddVirus(vaccinated, rec->virus);			// does not exist => add it to the set
				if(retval != 1){
					printf("Failed to add virus %s in vaccinated_persons set.\n", rec->virus);
				}

			} else {	// found list, check if citizen has already registered
				citizen_entry = ListFindClosest(vaccinated, list, rec->ID);

				if(citizen_entry->ID == rec->ID){	// already registered for this virus
					printf("ERROR IN RECORD %d %s %s %s %d %s %s %s(ERROR: inconsistent record).\n", rec->citizenID, rec->first_name, rec->last_name, rec->country, rec->age, rec->virus, rec->yes_or_no, temp_date);
					free(rec->first_name);
					free(rec->last_name);
					free(rec->country);
					free(rec->virus);
					free(rec->yes_or_no);
					free(rec->date_vaccinated);
					free(rec); 
					continue;
				}
			}

			// insert citizen to vaccinated_skip list
			retval = ListInsertCitizen(vaccinated, rec);
			if(retval != 1){
				printf("Failed to add citizen %d %s %s to vaccinated_persons skip list.\n", rec->citizenID, rec->first_name, rec->last_name);
			} 

			// insert citizen to the bloom filter
			retval = BloomInsertElement(bloomset, rec->citizenID, rec->virus);
			if(retval != 1){
				printf("Initialization for virus %s is required.\n", rec->virus);
			}

		} else {	// not vaccinated person

			SkipNode *citizen_entry;
			SkipList *list = ListSearchVirus(not_vaccinated, rec->virus);
			if(list == NULL){
				// printf("Not_vaccinated_persons Skip List for %s not found \n", rec->virus);
				retval = ListAddVirus(not_vaccinated, rec->virus);			// does not exist => add it to the set
				if(retval != 1){
					printf("Failed to add virus %s in not vaccinated_person set.\n", rec->virus);
				}
			} else {	// found list, check if citizen has already registered
				citizen_entry = ListFindClosest(not_vaccinated, list, rec->ID);

				if(citizen_entry->ID == rec->ID){	// already registered for this virus
					printf("ERROR IN RECORD %d %s %s %s %d %s %s %s(ERROR: inconsistent record).\n", rec->citizenID, rec->first_name, rec->last_name, rec->country, rec->age, rec->virus, rec->yes_or_no, temp_date);
					free(rec->first_name);
					free(rec->last_name);
					free(rec->country);
					free(rec->virus);
					free(rec->yes_or_no);
					free(rec); 
					continue;
				}
			}

			// insert citizen to not_vaccinated_skip list
			retval = ListInsertCitizen(not_vaccinated, rec);
			if(retval != 1){
				printf("Failed to add citizen %d %s %s to not_vaccinated_persons skip list.\n", rec->citizenID, rec->first_name, rec->last_name);
			}
		}
	}

	if(recordline != NULL)
		free(recordline);   // recordline space not useful anymore
	fclose(inputfile);
	return 1;
}
