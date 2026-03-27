#include <stdlib.h>
#include "memoryFuncs.h"
#include "globals.h"
#include "structs.h"

//Группируем на impLen массивов
void stage1Grouping(ImplicantSet func, ImplicantSets* stage1Arr)
{

	stage1Arr->set = (ImplicantSet*)calloc(impLen + 1, sizeof(ImplicantSet));
	if (!stage1Arr->set)
		exit(1);

	/*stage1Arr->lens = (int*)calloc(impLen + 1, sizeof(int));
	if (!stage1Arr->lens)
		exit(1);*/

	for (int i = 0; i < impLen + 1; i++)
		stage1Arr->set[i].arr = NULL;

	//Считаем единицы для каждой импликанты
	int oneSum;
	for (int i = 0; i < func.len; i++)					//Заходим в каждую импликанту
	{
		oneSum = 0;
		for (int j = 0; j < impLen; j++)				//Заходим в каждую переменную каждой импликанты
			if (func.arr[i][j] == 1)
				oneSum++;

		stage1Arr->set[oneSum].arr = append(stage1Arr->set[oneSum].arr, &(stage1Arr->set[oneSum].len), func.arr[i]);
	}

}