#include "structs.h"

void stdImpOutput(int*, int);

void stdOutput(ImplicantSet);

void stage1Output(ImplicantSets);

void implicantTableOutput(ImplicantSet, ImplicantSet, int**);

void crossedImplicantTableOutput(int**, ImplicantSet, IntVector, int**, IntVector);

void petrickСonjunctionOutput(PetrickSet, int);

void petrickDisjunctionOutput(PetrickSet, PetrickSet, int);

void kernelMinOutput(ImplicantSet);

void irredundantFormsOutput(ImplicantSets, int*);

void minimalIrredundantFormsOutput(ImplicantSets);