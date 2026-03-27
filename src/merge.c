#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memoryFuncs.h"
#include "stringsFuncs.h"
#include "outputFuncs.h"
#include "globals.h"
#include "structs.h"

void merge(ImplicantSet* funcMin, ImplicantSet* notMerged)
{
	int mergeFlag = 1;													//Количество склеиваний за итерацию - флаг
	int countStageOutput = 2;											//Для вывода в консоль

	while (mergeFlag)
	{
		mergeFlag = 0;
		int* used = (int*)calloc(funcMin->len, sizeof(int));				//Помечает использовавшиеся в группе

		ImplicantSet nextFuncMin = { 0 };									//Массив ещё раз склеившихся импликант

		printf("\n%d этап:\n\n", countStageOutput);

		//Работаем в funcMin, формируем группы по положению x
		for (int i = 0; i < funcMin->len; i++)
		{
			if (used[i])
				continue;

			ImplicantSet arrOfImp = { 0 };						//Временный массив для импликант с одним положением x. 
																//Наполняется из funcMin
			
			used[i] = 1;
			arrOfImp.arr = append(arrOfImp.arr, &(arrOfImp.len), funcMin->arr[i]);

			for (int k = i + 1; k < funcMin->len; k++)
			{
				if (!used[k] && createMask(funcMin->arr[i], impLen) == createMask(funcMin->arr[k], impLen))
				{
					used[k] = 1;
					arrOfImp.arr = append(arrOfImp.arr, &(arrOfImp.len), funcMin->arr[k]);
				}
			}

			//Вывод временного массива
			stdOutput(arrOfImp);
			printf("\n");

			int* mergedInGroup = (int*)calloc(arrOfImp.len, sizeof(int));
			if (!mergedInGroup)
				exit(1);

			//Склеивание во временном массиве
			for (int baseElem = 0; baseElem < arrOfImp.len; baseElem++)
			{
				for (int countElem = baseElem + 1; countElem < arrOfImp.len; countElem++)
				{
					int changeAmount = 0;
					int changeIndex = 0;
					//Бежим по цифрам сравниваемой пары
					for (int compareNum = 0; compareNum < impLen; compareNum++)
					{
						if (arrOfImp.arr[baseElem][compareNum] != arrOfImp.arr[countElem][compareNum])
						{
							changeAmount++;
							changeIndex = compareNum;
						}
					}
					if (changeAmount < 2)								//Склеивание > 2, т.к. при 0 удаляются дубликаты
					{
						mergeFlag = 1;

						int* newImpTemp = (int*)calloc(impLen, sizeof(int));
						if (!newImpTemp)
							exit(1);
						memcpy(newImpTemp, arrOfImp.arr[baseElem], impLen * sizeof(int));

						if (changeAmount == 1)
							newImpTemp[changeIndex] = 2;

						//Новый массив
						nextFuncMin.arr = append(nextFuncMin.arr, &(nextFuncMin.len), newImpTemp);

						//Помечаем склеившиеся импликанты для формирования массива несклеившихся
						mergedInGroup[baseElem] = 1;
						mergedInGroup[countElem] = 1;
					}
				}
			}

			//Сохраняем несклеившиеся импликанты - добавление в notMerged из arrOfImp
			for (int i = 0; i < arrOfImp.len; i++)
				if (!mergedInGroup[i])
					notMerged->arr = appendValueCopy(notMerged->arr, &(notMerged->len), arrOfImp.arr[i], impLen);

			free(mergedInGroup);
			free(arrOfImp.arr);
		}


		//Вывод после очередного склеивания
		printf("Результат %d этапа:\n\n", countStageOutput);
		stdOutput(nextFuncMin);
		printf("\nНе склеившиеся импликанты:\n\n");
		stdOutput(*notMerged);

		printf("\n--------------------------------\n");

		//Перезапись памяти из нового сформированного массива nextFuncMin в исходный - funcMin
		if (mergeFlag)
		{
			for (int i = 0; i < funcMin->len; i++)
				free(funcMin->arr[i]);
			free(funcMin->arr);

			funcMin->arr = nextFuncMin.arr;
			funcMin->len = nextFuncMin.len;
		}

		countStageOutput++;
		free(used);
	}
}