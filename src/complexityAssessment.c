#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "structs.h"

int findMin(int*, int);

//Оценка сложности
int complexityAssessment(
	ImplicantSet kernel, 
	ImplicantSets irredundantForms,
	int** complexity
)
{ 

	//Оценка сложности ядра
	int kernelComplexity = 0;
	int* wasVarInvertedInKernel = (int*)calloc(impLen, sizeof(int));			//Сохраняет, встречалась ли переменная со знаком инверсии в ядре
	if (!wasVarInvertedInKernel)												//Используется далее при оценки сложности тупиковых форм
		exit(1);																//Чтобы считать инверсии не более 1 раза

	//По всем импликантам в ядре
	for (int imp = 0; imp < kernel.len; imp++)
	{
		//По всем переменным в импликанте ядра
		for (int var = 0; var < impLen; var++)
		{ 
			//Конъюнкции + инверсии
			if (kernel.arr[imp][var] == 1)
				kernelComplexity++;

			else if (kernel.arr[imp][var] == 0)
				if (wasVarInvertedInKernel[var])
				{ 
					kernelComplexity++;
				} 
				else
				{
					kernelComplexity += 2;
					wasVarInvertedInKernel[var] = 1;
				}
		}
	}
	//Дизъюнкции не учитываются здесь, так как далее они учтены для всей тупиковой формы		


	//Оценка сложности тупиковых форм
	*complexity = (int*)calloc(irredundantForms.len, sizeof(int));
	if (!(*complexity))
		exit(1);
	int formComplexity;														//Сложность очередной тупиковой формы
	 
	//По всем тупиковым формам
	for (int form = 0; form < irredundantForms.len; form++)
	{
		formComplexity = kernelComplexity;									//Сложность тупиковой формы включает в себя сложность ядра

		int* wasVarInvertedInForm = (int*)calloc(impLen, sizeof(int));		//Массив флагов инверсий переменных для очередной тупиковой формы
		if (!wasVarInvertedInForm)
			exit(1);
		memcpy(wasVarInvertedInForm, wasVarInvertedInKernel, impLen * sizeof(int));		//Инициализируется из массива флагов инверсий ядра


		//По всем импликантам - "a", "b", "c" ...
		int imp;
		for (imp = kernel.len; imp < irredundantForms.set[form].len; imp++)
		{
			//По всем переменным в импликанте
			for (int var = 0; var < impLen; var++)
			{
				//Конъюнкции + инверсии
				if (irredundantForms.set[form].arr[imp][var] == 1)
					formComplexity++;

				else if (irredundantForms.set[form].arr[imp][var] == 0)
					if (wasVarInvertedInForm[var])
					{
						formComplexity++;
					}
					else
					{
						formComplexity += 2;
						wasVarInvertedInForm[var] = 1;
					}
			}
		}
		//Дизъюнкции
		formComplexity += imp + 1;					//1, т.к. на последней итерации imp = кол-во импликант в тупиковой форме - 1
													//для учёта дизъюнкций в ДНФ необходимо добавить количество конъюнкций (либо кол-во знаков ∨ + 1)
		(*complexity)[form] = formComplexity;

		free(wasVarInvertedInForm);
	}

	free(wasVarInvertedInKernel);

	return findMin(*complexity, irredundantForms.len);
}


int findMin(int* arr, int len) 
{
	int min = arr[0];
	for (int i = 1; i < len; i++)
		if (arr[i] < min)
			min = arr[i];
	return min;
}