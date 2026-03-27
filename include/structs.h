#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct
{
	int* arr;
	int len;

} IntVector;

typedef struct
{
	int** arr;
	int len;

} ImplicantSet;

typedef struct
{
	ImplicantSet* set;
	int len;

} ImplicantSets;

typedef struct
{
	char** arr;
	int len;

} PetrickSet;

#endif