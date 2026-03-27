#include <stdio.h>
#include "globals.h"
#include "structs.h"


void stdImpOutput(int* arr, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (arr[i] == 2)
			printf("x");
		else
			printf("%d", arr[i]);
	}
}

void stdOutput(ImplicantSet set)
{
	for (int i = 0; i < set.len; i++)
	{
		if (!set.arr[i])
			continue;

		stdImpOutput(set.arr[i], impLen);
		printf("\n");
	}
}


//Отрисовка результата первого склеивания
void stage1Output(ImplicantSets stage1Arr)
{
	printf("\nРезльтат группировки: \n\n");
	for (int i = 0; i < impLen + 1; i++)
	{
		printf("%d единиц:\n\n", i);
		stdOutput(stage1Arr.set[i]);
		printf("\n");
	}
}


//Отрисовка импликантной таблицы
void implicantTableOutput(ImplicantSet func, ImplicantSet notMerged, int** implicantTable)
{
	int indent = 2;						//Отступ
	int gapSize = impLen + indent;		//Рамзер ячейки
	int middle = (int)(gapSize / 2);	//Середина ячейки для проставления крестиков

	//Горизонтальный отступ
	printf("%*s%c%*s", impLen, "", '|', indent, "");

	//Верхняя строка
	for (int i = 0; i < func.len; i++)
	{
		stdImpOutput(func.arr[i], impLen);
		printf("%c%*s", '|', indent, "");
	}

	//Последующие строки
	for (int i = 0; i < notMerged.len; i++)
	{
		printf("\n");

		//Горизонтальные линии
		for (int j = 0; j < func.len * (gapSize + 1) + impLen + 1; j++)
			printf("-");

		printf("\n");

		//Импликанты в стобцах
		stdImpOutput(notMerged.arr[i], impLen);
		printf("|");

		//Заполнение крестиками
		for (int j = 0; j < func.len; j++)
			if (implicantTable[j][i])
				printf("%*s%c%*s", middle, "", 'x', gapSize - middle, "|");
			else
				printf("%*s%c", gapSize, "", '|');
	}
}


//Отрисовка таблицы после вычеркивания
void crossedImplicantTableOutput(
	int** func, ImplicantSet notMerged, 
	IntVector kernelIndex, int** implicantTable, 
	IntVector notCoveredByKernelIndex
)
{
	int indent = 2;						//Отступ
	int gapSize = impLen + indent;		//Рамзер ячейки
	int middle = (int)(gapSize / 2);	//Середина ячейки для проставления крестиков
	int letterIndent = 2;				//Отступ между буквенным индексом импликанты и самой импликантой

	//Горизонтальный отступ
	printf("%*s%c%*s", 1 + letterIndent + impLen, "", '|', indent, "");

	//Верхняя строка
	for (int minterm = 0; minterm < notCoveredByKernelIndex.len; minterm++)
	{
		stdImpOutput(func[minterm], impLen);
		printf("%c%*s", '|', indent, "");
	}


	int isKernel;												//Флаг вхождения импликанты в ядро
	for (int i = 0; i < notMerged.len; i++)
	{

		//Пропуск импликант, входящих в ЯДРО
		isKernel = 0;
		for (int j = 0; j < kernelIndex.len; j++)
			if (i == kernelIndex.arr[j])
			{ 
				isKernel = 1;
				break;
			}
		if (isKernel)
			continue;

		printf("\n");
		//Горизонтальная линия
		for (int j = 0; j < notCoveredByKernelIndex.len * (gapSize + 1) + impLen + letterIndent + 2; j++)
			printf("-");
		printf("\n");

		//Буквенное имя импликанты
		printf("%c%*s", 'a' + i, letterIndent, "");
		//Импликанта
		stdImpOutput(notMerged.arr[i], impLen);
		printf("|");

		//Заполнение крестиками
		for (int j = 0; j < notCoveredByKernelIndex.len; j++)
		{
			if (implicantTable[notCoveredByKernelIndex.arr[j]][i])
				printf("%*s%c%*s", middle, "", 'x', gapSize - middle, "|");
			else
				printf("%*s%c", gapSize, "", '|');
		}
	}
}

void petrickСonjunctionOutput(PetrickSet petrickConjunction, int startDisjIndex)
{

	for (int i = startDisjIndex; i < petrickConjunction.len; i++)
	{
		printf("(");
		for (int j = 0; petrickConjunction.arr[i][j] != '\0'; j++)
		{ 
			printf("%c", petrickConjunction.arr[i][j]);
			if (petrickConjunction.arr[i][j + 1] != '\0')
				printf(" ∨ ");
		}
		printf(")");

		if (i < petrickConjunction.len - 1)
			printf(" ∧ ");
	}
}

void petrickDisjunctionOutput(
	PetrickSet nextPetrickDisjunction,
	PetrickSet petrickConjunction, 
	int startDisjIndex
)
{
	printf("\n\n");
	if (startDisjIndex < petrickConjunction.len - 1)
		printf("(");
	for (int i = 0; i < nextPetrickDisjunction.len; i++)
	{
		fputs(nextPetrickDisjunction.arr[i], stdout);
		if (i < nextPetrickDisjunction.len - 1)
			printf(" ∨ ");
	}
	if (startDisjIndex < petrickConjunction.len - 1)
		printf(")");

	if (startDisjIndex < petrickConjunction.len - 1)
		printf(" ∧ ");

	petrickСonjunctionOutput(petrickConjunction, startDisjIndex + 1);
}

//Вывод ядра как мднф
void kernelMinOutput(ImplicantSet kernel)
{
	for (int i = 0; i < kernel.len; i++)
	{
		stdImpOutput(kernel.arr[i], impLen);
		if (i < kernel.len - 1)
			printf(" ∨ ");
	}
}

//Вывод тупиковых форм со сложностью
void irredundantFormsOutput(ImplicantSets irredundantForms, int* complexity)
{
	printf("\nТупиковые формы:\n\n");

	for (int form = 0; form < irredundantForms.len; form++)
	{
		printf("f = ");
		kernelMinOutput(irredundantForms.set[form]);

		printf("\tсложность по Квайну: %d", complexity[form]);
		printf("\n\n");
	}
	printf("*инверсии переменной учитываются один раз\n");
}


//Вывод минимальных тупиковых форм
void minimalIrredundantFormsOutput(ImplicantSets output)
{
	printf("\nМинимальные тупиковые формы:\n\n");

	for (int form = 0; form < output.len; form++)
	{
		printf("f = ");
		kernelMinOutput(output.set[form]);
		printf("\n\n");
	}
}