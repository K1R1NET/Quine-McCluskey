#include <stdlib.h>
#include <math.h>
#include "globals.h"
#include "structs.h"

//Формируем func [[0, 1, 0, 1 ...], [1, 0, 1, 0 ...]]
void initialTransform(int* input, ImplicantSet* func)
{

	func->arr = (int**)calloc(func->len, sizeof(int*));

	if (!func->arr)
		exit(1);
	for (int i = 0; i < func->len; i++)
	{
		func->arr[i] = (int*)calloc(impLen, sizeof(int));
		if (!func->arr[i])
			exit(1);
		for (int j = 0; j < impLen; j++)
			func->arr[i][j] = (input[i] / (int)pow(10, impLen - j - 1)) % 10;
	}

}