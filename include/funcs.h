#include "structs.h"

void initialTransform(int*, ImplicantSet*);

void stage1Grouping(ImplicantSet, ImplicantSets*);

void stage1(ImplicantSet, ImplicantSets, ImplicantSet*, ImplicantSet*);

void merge(ImplicantSet*, ImplicantSet*);

void implicantTableFunc(ImplicantSet, ImplicantSet, ImplicantSet*, IntVector*, int***);

void petrickConjunctionFunc(int, int, IntVector, int**, PetrickSet*, IntVector*);

void petrickDisjunctionFunc(PetrickSet, PetrickSet*);

void irredundantFormsTransform(ImplicantSet, int**, char**, ImplicantSets*);

int complexityAssessment(ImplicantSet, ImplicantSets, int**);

void minimalIrredundantForms(ImplicantSets, int*, int, ImplicantSets*);