#include <stdlib.h>
#include <stdio.h>
#include "memoryFuncs.h"
#include "structs.h"

//Создать массив вида [[a, b], [c, d], [d, e], [b, e, f]], где каждый подмассив формируется минтермом
void petrickConjunctionFunc(
	int funcLen, int notMergedLen,
	IntVector kernelIndex, int** implicantTable, 
	PetrickSet* petrickConjunction, IntVector* notCoveredByKernelIndex
)
{
	int covered;											//Флаг покрытия							

	for (int minterm = 0; minterm < funcLen; minterm++)
	{
		//Проверка покрытия минтерма ядром
		covered = 0;
		for (int kIndex = 0; kIndex < kernelIndex.len; kIndex++)
			if (implicantTable[minterm][kernelIndex.arr[kIndex]])
			{
				covered = 1;
				break;
			}
		//Если минтерм покрывается импликантой из ядра, то пропускаем его
		if (covered)
			continue;

		//Массив индексов не покрытых минтермов в func - далее используется только для вывода
		notCoveredByKernelIndex->arr = appendNum(notCoveredByKernelIndex->arr, &(notCoveredByKernelIndex->len), minterm);

		//Для каждого минтерма формируем дизъюнкцию [a, b, c, ...], при чем известно, что ядро не будет покрывать этот минтерм
		petrickConjunction->arr = addMemStr(petrickConjunction->arr, &(petrickConjunction->len));

		//Отбираем импликанты, покрывающие минтерм
		for (int imp = 0; imp < notMergedLen; imp++)
			if (implicantTable[minterm][imp])
				petrickConjunction->arr[petrickConjunction->len - 1] = appendChar(petrickConjunction->arr[petrickConjunction->len - 1], 'a' + imp);

	}
}