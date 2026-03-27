#include "stdlib.h"
#include "memoryFuncs.h"
#include "globals.h"
#include "structs.h"


void implicantTableFunc(
	ImplicantSet func, ImplicantSet notMerged, 
	ImplicantSet* kernel, IntVector* kernelIndex,
	int*** implicantTable
)
{ 
	int changeAmount;
	int coveringImpCount;																													
	int kernelImpIndex;
	int alreadyInKernel = 0;

	(*implicantTable) = (int**)calloc(func.len, sizeof(int*));						//Массив, показывающий какими простыми импликантами покрыт исходный минтерм
	if (!(*implicantTable))
		exit(1);
	
	for (int minterm = 0; minterm < func.len; minterm++)
	{
		(*implicantTable)[minterm] = (int*)calloc(notMerged.len, sizeof(int));
		if (!((*implicantTable)[minterm]))
			exit(1);

		coveringImpCount = 0;
		kernelImpIndex = 0;

		for (int imp = 0; imp < notMerged.len; imp++)
		{
			//Проверка покрытия
			changeAmount = 0;
			for (int compareNum = 0; compareNum < impLen; compareNum++)
				if (func.arr[minterm][compareNum] != notMerged.arr[imp][compareNum] && notMerged.arr[imp][compareNum] != 2)
					changeAmount++;

			if (!changeAmount)											//Отработка покрытия
			{
				(*implicantTable)[minterm][imp] = 1;					//Покрывается
				coveringImpCount++;										//Количество импликант, покрывающих столбец
				kernelImpIndex = imp;									//Индекс уникальной импликанты (потенциально)
			}
		}

		if (coveringImpCount == 1)										//Ряд покрывает одна импликанта
		{
			alreadyInKernel = 0;
			//по массиву kernelIndex
			for (int inKernelIndex = 0; inKernelIndex < kernelIndex->len; inKernelIndex++)
			{
				if (kernelImpIndex == kernelIndex->arr[inKernelIndex])
				{
					alreadyInKernel = 1;
					break;
				}
			}

			if (!alreadyInKernel)																//Импликанты разные - добавляем в ядро
			{
				kernel->arr = append(kernel->arr, &(kernel->len), notMerged.arr[kernelImpIndex]);				//Наполняется из notMerged
				kernelIndex->arr = appendNum(kernelIndex->arr, &(kernelIndex->len), kernelImpIndex);
			}
		}
	}
}