#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "globals.h"
#include "structs.h"
#include "memoryFuncs.h"
#include "stringsFuncs.h"
#include "outputFuncs.h"

//Этап раскрытия скобок
void petrickDisjunctionFunc(PetrickSet petrickConjunction, PetrickSet* petrickDisjunction)
{ 
	PetrickSet nextPetrickDisjunction = { 0 };			//Временный для хранения дизъюнкции, полученной умножением
														//прошлого результата перемножения на очередную дизъюнкцию из petrickConjunction


	//Начальное значение petrickDisjunction - скопировать первую дизъюнкцию из petrickConjunction по импликантам - каждая в отдельную строку
	//["a", "b"] / [[a], [b]]
	for (int imp = 0; petrickConjunction.arr[0][imp] != '\0'; imp++)
	{
		petrickDisjunction->arr = addMemStr(petrickDisjunction->arr, &(petrickDisjunction->len));					//Выделение памяти под строку
		petrickDisjunction->arr[imp] = appendChar(petrickDisjunction->arr[imp], petrickConjunction.arr[0][imp]);		//Добавление строки - символа с \0
	}

	//По всем дизъюнкциям в petrickConjunction - ["ab", "cd"...] / [[a, b], [c, d]...]
	for (int disj = 1; disj < petrickConjunction.len; disj++)
	{

		nextPetrickDisjunction = (PetrickSet){ 0 };

		//ПЕРЕМНОЖЕНИЕ СКОБОК
		
		//По всем конъюнкциям (на первом этапе - импликантам) в petrikDisjunction
		for (int conj = 0; conj < petrickDisjunction->len; conj++)
			//По всем импликантам в дизъюнкции petrickConjunction - 'a', 'b'...
			for (int imp = 0; petrickConjunction.arr[disj][imp] != '\0'; imp++)
				//Добавляем новую конъюнкцию - конкатенируем (строка + символ) старую конъюнкцию (из petrickDisjunction) с импликантой из petrickConjunction
				nextPetrickDisjunction.arr = appendStr(nextPetrickDisjunction.arr, &(nextPetrickDisjunction.len), combineStrChar(petrickDisjunction->arr[conj], petrickConjunction.arr[disj][imp]));
	


		//ПРЕОБРАЗОВАНИЕ сформированной дизъюнкции. Состоит из 2 шагов: 
		// замена двух одинаковых букв на одну (aa -> a). от частного к общему (bdca ∨ bd -> bd)
		
		//ЗАМЕНА
		for (int elem = 0; elem < nextPetrickDisjunction.len; elem++)
			removeDuplicates(nextPetrickDisjunction.arr[elem]);

		//ИНДУКЦИЯ
		//Каждая импликанта сравнивается с каждой
		for (int i = 0; i < nextPetrickDisjunction.len; i++)
			for (int j = i + 1; j < nextPetrickDisjunction.len; j++)
				//Проверяется, покрывается ли большая импликанта меньшей (содержит ли в себе все буквы меньшей). 
				//В этом случае большая удаляется (!с освобождением памяти)
				if (strlen(nextPetrickDisjunction.arr[i]) <= strlen(nextPetrickDisjunction.arr[j]) && contains(nextPetrickDisjunction.arr[i], nextPetrickDisjunction.arr[j]))
					removeString(nextPetrickDisjunction.arr, &(nextPetrickDisjunction.len), j);
				else if (strlen(nextPetrickDisjunction.arr[i]) > strlen(nextPetrickDisjunction.arr[j]) && contains(nextPetrickDisjunction.arr[j], nextPetrickDisjunction.arr[i]))
					removeString(nextPetrickDisjunction.arr, &(nextPetrickDisjunction.len), i);


		//Вывод действий раскрытия скобок
		petrickDisjunctionOutput(nextPetrickDisjunction, petrickConjunction, disj);
		


		//Перезапись памяти из нового сформированного массива nextPetrickDisjunction в исходный - petrickDisjunction
		for (int i = 0; i < petrickDisjunction->len; i++)
			free(petrickDisjunction->arr[i]);
		free(petrickDisjunction->arr);										//ОК

		petrickDisjunction->arr = nextPetrickDisjunction.arr;
		petrickDisjunction->len = nextPetrickDisjunction.len;
	}
}