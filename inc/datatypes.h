#pragma once

typedef struct Date{
	int day;
	int month;
	int year;
} Date;



typedef struct Citizen{   // A Citizen's info will be stored here
	int citizenID;
	char *first_name;       
	char *last_name;
	char *country;
	int age;
	char *virus;			// virus for which citizen is vaccinated (or not)
	char *yes_or_no;		// "YES" if citizen is vaccinated for virus
	Date *date_vaccinated;	// NULL if citizen is not vaccinated
} Citizen;
