#include <stdlib.h>
#include <string.h>
#include "memoryFuncs.h"
#include "globals.h"
#include "structs.h"


//Первое склеивание
void stage1(ImplicantSet func, ImplicantSets stage1Arr, ImplicantSet* funcMin, ImplicantSet* notMerged)
{
	//Берём каждый элемент из i-го массива и сравниваем с каждым из i+1-го

	ImplicantSet isMerged = { 0 };											//Массив использованных в склеивании элементов

	for (int first = 0; first < impLen; first++)							//до impLen, так как длина stage1Arr - impLen + 1
	{
		for (int firstElem = 0; firstElem < stage1Arr.set[first].len; firstElem++)
		{
			//Бежим по i+1-му массиву
			for (int secondElem = 0; secondElem < stage1Arr.set[first + 1].len; secondElem++)
			{

				int changeAmount = 0;
				int changeIndex = 0;

				//Бежим по цифрам сравниваемой пары
				for (int compareNum = 0; compareNum < impLen; compareNum++)
				{
					if (stage1Arr.set[first].arr[firstElem][compareNum] != stage1Arr.set[first + 1].arr[secondElem][compareNum])
					{
						changeAmount++;
						changeIndex = compareNum;
					}
				}

				if (changeAmount == 1)
				{
					isMerged.arr = append(isMerged.arr, &(isMerged.len), stage1Arr.set[first].arr[firstElem]);												//Использованы в склеивании
					isMerged.arr = append(isMerged.arr, &(isMerged.len), stage1Arr.set[first + 1].arr[secondElem]);
					int* newImpTemp = (int*)calloc(impLen, sizeof(int));
					if (!newImpTemp)
						exit(1);
					memcpy(newImpTemp, stage1Arr.set[first].arr[firstElem], impLen * sizeof(int));
					newImpTemp[changeIndex] = 2;
					funcMin->arr = append(funcMin->arr, &(funcMin->len), newImpTemp);
				}
			}
		}
	}

	//Формирование массива не склеившихся импликант после первого склеивания - все элементы, содержащиеся в func, но не в isMerged
	for (int i = 0; i < func.len; i++)
	{
		int found = 0;
		for (int j = 0; j < isMerged.len; j++)
			if (memcmp(func.arr[i], isMerged.arr[j], impLen * sizeof(int)) == 0)
			{
				found = 1;
				break;
			}

		if (!found)
			notMerged->arr = appendValueCopy(notMerged->arr, &(notMerged->len), func.arr[i], impLen);
	}

	free(isMerged.arr);			//ОК
}