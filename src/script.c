#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "globals.h"
#include "structs.h"
#include "memoryFuncs.h"
#include "stringsFuncs.h"
#include "funcs.h"
#include "outputFuncs.h"

#include <emscripten/emscripten.h>



int impLen = 0;
int *input = NULL;
int funcLen = 0;


void EMSCRIPTEN_KEEPALIVE setImpLen(int val)
{
    impLen = val;
    printf("\n\nКоличество переменных: %d\n", impLen);
}


void EMSCRIPTEN_KEEPALIVE setArray(int *arr, int len) 
{
    funcLen = len;
    input = (int*)calloc(len, sizeof(int));
    for (int i = 0; i < len; i++)
        input[i] = arr[i];
}



void EMSCRIPTEN_KEEPALIVE runAlgorithm()
{

	ImplicantSet func = {NULL, funcLen};

	//Вывод введённой функции
	printf("\nВведённая функция:\n\n");
	stdOutput(func);
	printf("--------------------------------\n");


	/*------------------------------------ПРЕОБРАЗОВАНИЕ В МАССИВ-----------------------------------------*/
	
	//Формируем func [[0, 1, 0, 1 ...], [1, 0, 1, 0 ...]]

	initialTransform(input, &func);

	free(input);

	/*-------------------------------------------ПЕРВЫЙ ЭТАП----------------------------------------------*/

	//Группируем на impLen массивов по количеству единиц
	ImplicantSets stage1Arr = { 0 };									//Массив групп, объеденённых по количеству единиц

	stage1Grouping(func, &stage1Arr);

	//Вывод stage1Arr
	stage1Output(stage1Arr);

	printf("--------------------------------\n");
	

	//Массив импликант, участвующих в склеивании
	ImplicantSet funcMin = { 0 };
	//Массив не склеившихся импликант
	ImplicantSet notMerged = { 0 };
	
	stage1(func, stage1Arr, &funcMin, &notMerged);

	//Вывод массива после первого этапа
	printf("\n1 этап: \n\n");
	stdOutput(funcMin);
	//Вывод массива не склеившихся импликант после первого этапа
	printf("\nНе склеившиеся импликанты:\n\n");
	stdOutput(notMerged);

	printf("\n--------------------------------\n");

	for (int i = 0; i < impLen + 1; i++)
		free(stage1Arr.set[i].arr);
	free(stage1Arr.set);

	/*----------------------------------------ПОСЛЕДУЮЩИЕ ЭТАПЫ-------------------------------------------*/
	
	merge(&funcMin, &notMerged);

	for (int i = 0; i < funcMin.len; i++)
		free(funcMin.arr[i]);
	free(funcMin.arr);

	/*----------------------------------------ИМПЛИКАНТНАЯ ТАБЛИЦА----------------------------------------*/

	//Массив импликант, составляющих ядро функции
	ImplicantSet kernel = { 0 };
	//Массив индексов импликант, составляющих ядро функции, в notMerged
	IntVector kernelIndex = { 0 };
	//Импликантная таблица: строка - минтерм; столбец - простая импликанта
	int** implicantTable = NULL;

	implicantTableFunc(
		func, notMerged, 
		&kernel, &kernelIndex, 
		&implicantTable
	);
	
	//Вывод таблицы
	printf("\nИмпликантная таблица:\n\n");
	implicantTableOutput(func, notMerged, implicantTable);

	//Вывод ядра функции
	printf("\n\nЯдро функции:\n\n");
	stdOutput(kernel);

	printf("\n--------------------------------\n");


	/*----------------------------------------МЕТОД ПЕТРИКА-----------------------------------------------*/

	//Создать массив (конъюнкция дизъюнкций) вида [[a, b], [c, d], [d, e], [b, e, f]]
	
	//Массив (конъюнкция дизъюнкций) вида [[a, b], [c, d], [d, e], [b, e, f]]
	PetrickSet petrickConjunction = { 0 };
	//Массив индексов не покрытых минтермов в func - далее используется 1 раз только для вывода таблицы после вычёркивания
	IntVector notCoveredByKernelIndex = { 0 };
	
	petrickConjunctionFunc(
		func.len, notMerged.len, 
		kernelIndex, implicantTable, 
		&petrickConjunction, &notCoveredByKernelIndex
	);
	
	if (!notCoveredByKernelIndex.len)				//Нет минтермов, не покрытых ядром -> мднф - ядро -> ВЫХОД
	{
		printf("\nМинимальная тупиковая форма:\n\n");
		kernelMinOutput(kernel);
		printf("\n\n");
	}
	else
	{

		//Вывод таблицы после вычёркивания
		printf("\nИмпликантная таблица после вычёркивания:\n\n");
		crossedImplicantTableOutput(
			func.arr, notMerged,
			kernelIndex, implicantTable,
			notCoveredByKernelIndex
		);
		//Вывод конъюнкций дизъюнкций по методу Петрика
		printf("\n\nМетод Петрика:\n\n");
		petrickСonjunctionOutput(petrickConjunction, 0);


		/*----------------------РАСКРЫТИЕ СКОБОК - ФОРМИРОВАНИЕ ДИЗЪЮНКЦИИ В МЕТОДЕ ПЕТРИКА-------------------*/

		PetrickSet petrickDisjunction = { 0 };				//В нём формируется результат раскрытия скобок - [abc, bcd, ...]
	
		//Функция включает вывод
		petrickDisjunctionFunc(petrickConjunction, &petrickDisjunction);

		printf("\n\n--------------------------------\n");


		/*---------------------------ФОРМИРОВАНИЕ ТУПИКОВЫХ ФОРМ ИЗ ДИЗЪЮНКЦИИ ПЕТРИКА------------------------*/

		ImplicantSets irredundantForms;													//Все тупиковые формы
		irredundantForms.set = NULL; irredundantForms.len = petrickDisjunction.len;		//Количество тупиковых форм = количеству конъюнкций в дизъюнкции Петрика
			
		irredundantFormsTransform(
			kernel, notMerged.arr, petrickDisjunction.arr,
			&irredundantForms
		);

		for (int i = 0; i < petrickDisjunction.len; i++)				//ОК	
			free(petrickDisjunction.arr[i]);
		free(petrickDisjunction.arr);
	

		/*------------------------------------ОЦЕНКА СЛОЖНОСТИ ТУПИКОВЫХ ФОРМ---------------------------------*/

		int* complexity = NULL;							//Массив значений сложностей по Квайну тупиковых форм
		
		int minComplexity = complexityAssessment(kernel, irredundantForms, &complexity);
		
		//Вывод тупиковых форм со сложностью по Квайну
		irredundantFormsOutput(irredundantForms, complexity);


		printf("\n--------------------------------\n");


		/*--------------------------------------МИНИМАЛЬНЫЕ ТУПИКОВЫЕ ФОРМЫ-----------------------------------*/

		ImplicantSets output;
		output.set = NULL; output.len = 0;

		minimalIrredundantForms(
			irredundantForms,
			complexity, minComplexity,
			&output
		);

		//Вывод МДНФ
		minimalIrredundantFormsOutput(output);

		/*----------------------------------------------------------------------------------------------------*/


		for (int i = 0; i < irredundantForms.len; i++)				
			free(irredundantForms.set[i].arr);
		free(irredundantForms.set);										
										
		free(complexity);												

		free(output.set);

	}


	for (int i = 0; i < func.len; i++)
		free(func.arr[i]);
	free(func.arr);		

	for (int i = 0; i < notMerged.len; i++)
		free(notMerged.arr[i]);
	free(notMerged.arr);

	free(kernel.arr);

	free(kernelIndex.arr);

	for (int i = 0; i < func.len; i++)
		free(implicantTable[i]);
	free(implicantTable);


	for (int i = 0; i < petrickConjunction.len; i++)
		free(petrickConjunction.arr[i]);
	free(petrickConjunction.arr);

	free(notCoveredByKernelIndex.arr);


}