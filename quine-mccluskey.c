#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <emscripten/emscripten.h>
//#define impLen 5

int impLen = 0;

int* override(int* arr, int len)
{
	int* buffer = (int*)realloc(arr, len * sizeof(int));
	if (buffer)
		return buffer;
	exit(1);
}

int** override2D(int** arr, int len)
{
	int** buffer = (int**)realloc(arr, len * sizeof(int*));
	if (buffer)
	{
		buffer[len - 1] = (int*)calloc(impLen, sizeof(int));
		return buffer;
	}
	exit(1);
}

char* overrideChar(char* arr, int len)
{
	char* buffer = (char*)realloc(arr, len * sizeof(char));
	if (buffer)
		return buffer;
	exit(1);
}

char** override2DChar(char** arr, int len)
{
	char** buffer = (char**)realloc(arr, len * sizeof(char*));
	if (buffer)
		return buffer;
	exit(1);
}

int*** override3D(int*** arr, int len)
{
	int*** buffer = (int***)realloc(arr, len * sizeof(int**));
	if (buffer)
		return buffer;
	exit(1);
}

char* combineStrChar(char* str, char ch) 
{
	int len = strlen(str);
	//Новый размер: длина строки + 1 символ + '\0'
	char* new_str = (char*)calloc(len + 2, sizeof(char));
	if (!new_str)
		exit(1);
	// Копируем исходную строку
	strcpy(new_str, str);
	// Добавляем символ и завершающий нуль
	new_str[len] = ch;
	new_str[len + 1] = '\0';
	return new_str;
}

//smaller покрывает larger -> 1, иначе -> 0
int contains(char* smaller, char* larger) 
{
	for (int i = 0; i < strlen(smaller); i++)
		if (!strchr(larger, smaller[i]))
			return 0;
	return 1;
}

void removeString(char** arr, int* size, int index)
{
	free(arr[index]);
	for (int i = index; i < *size - 1; i++)
		arr[i] = arr[i + 1];
	(*size)--;
}


int createMask(int* arr)
{
	int mask = 0;
	for (int i = 0; i < impLen; i++)
		if (arr[i] == 2)
			mask += arr[i] * pow(10, impLen - i - 1);
	return mask;
}

char* removeDuplicates(const char* str) {
	int len = strlen(str);
	char* result = (char*)calloc(len + 1, sizeof(char));
	if (!result)
		exit(1);
	int pos = 0;
	for (int i = 0; i < len; i++) {
		char c = str[i];
		int exists = 0;
		for (int j = 0; j < pos; j++)
			if (result[j] == c) 
			{
				exists = 1;
				break;
			}
		if (!exists)
			result[pos++] = c;
	}

	result[pos] = '\0';
	return result;
}


int *input = NULL;
int funcLen = 0;


void EMSCRIPTEN_KEEPALIVE setImpLen(int val)
{
    impLen = val;
    printf("\n\nКоличество переменных: %d\n", impLen);
}

//Передаем массив импликант

void EMSCRIPTEN_KEEPALIVE setArray(int *arr, int len) {
    funcLen = len;
    input = (int*)malloc(len * sizeof(int));
    for (int i = 0; i < len; i++)
        input[i] = arr[i];
}

// Запускаем алгоритм минимизации

void EMSCRIPTEN_KEEPALIVE runAlgorithm()
{
	//Формируем func [[0, 1, 0, 1 ...], [1, 0, 1, 0 ...]]
	int** func = (int**)calloc(funcLen, sizeof(int*));
	if (func)
		for (int i = 0; i < funcLen; i++)
		{
			func[i] = (int*)calloc(impLen, sizeof(int));
			for (int j = 0; j < impLen; j++)
				func[i][j] = (input[i] / (int)pow(10, impLen - j - 1)) % 10;
		}

	//Вывод
	printf("\n\nВведённая функция:\n\n");
	for (int i = 0; i < funcLen; i++)
	{ 
		for (int j = 0; j < impLen; j++)
			printf("%d", func[i][j]);
		printf("\n");
	}
	printf("\n--------------------------------\n");

	//ПЕРВЫЙ ЭТАП: группируем на impLen массивов
	int*** arr1Stage = (int***)calloc(impLen + 1, sizeof(int**));
	if (arr1Stage)
		for (int i = 0; i < impLen + 1; i++)
			arr1Stage[i] = (int**)calloc(funcLen, sizeof(int*));

	//Считаем единицы для каждой импликанты
	int oneSum;
	for (int i = 0; i < funcLen; i++)					//Заходим в каждую импликанту
	{
		oneSum = 0;
		for (int j = 0; j < impLen; j++)				//Заходим в каждую переменную каждой импликанты
			if (func[i][j] == 1)
				oneSum++;
	
		arr1Stage[oneSum][i] = func[i];
	}

	//Вывод arr1Stage
	printf("\nРезльтат 1 этапа: \n\n");
	for (int i = 0; i < impLen + 1; i++)
	{ 
		printf("%d единиц:\n", i);
		for (int j = 0; j < funcLen; j++)
		{ 
			if (arr1Stage[i][j])
			{ 
				for (int k = 0; k < impLen; k++)
					printf("%d", arr1Stage[i][j][k]);
				printf("\n");
			}
		}	
		printf("\n");
	}
	printf("\n--------------------------------\n");


	//Массив после первого этапа
	int** funcMin = NULL;
	int funcMinLen = 0;


	//Берём каждый элемент из i-го массива и сравниваем с каждым из i+1-го
	int changeAmount;
	int changeIndex;
	int* isMerged = (int*)calloc(funcLen, sizeof(int));						//Массив флагов на использованный в склеивании элемент
	for (int first = 0; first < impLen + 1; first++)
	{
		for (int firstElem = 0; firstElem < funcLen; firstElem++)
		{ 
			if (!arr1Stage[first][firstElem])								//Выход, если пусто
				continue;
			
			//Бежим по i+1-му массиву
			for (int secondElem = 0; secondElem < funcLen; secondElem++)
			{
				if (!arr1Stage[first + 1][secondElem])						//Выход, если пусто
					continue;
				changeAmount = 0;
				changeIndex = 0;

				//Бежим по цифрам сравниваемой пары
				for (int compareNum = 0; compareNum < impLen; compareNum++)
				{ 
					if (arr1Stage[first][firstElem][compareNum] != arr1Stage[first + 1][secondElem][compareNum])
					{
						changeAmount++;
						changeIndex = compareNum;
					}
				}

				if (changeAmount == 1)
				{	
					isMerged[firstElem] = 1;												//Использованы в склеивании
					isMerged[secondElem] = 1;
					int* newImpTemp = (int*)calloc(impLen, sizeof(int));
					memcpy(newImpTemp, arr1Stage[first][firstElem], impLen * sizeof(int));
					newImpTemp[changeIndex] = 2;
					funcMinLen++;
					funcMin = override2D(funcMin, funcMinLen);
					funcMin[funcMinLen - 1] = newImpTemp;
				}
			}
		}
	}
	
	//Выод массива после первого этапа
	printf("1 этап: \n\n");
	for (int i = 0; i < funcMinLen; i++)
	{
		for (int j = 0; j < impLen; j++)
			printf("%d", funcMin[i][j]);
		printf("\n");
	}

	//Массив не склеившихся импликант
	int** notMerged = NULL;
	int notMergedLen = 0;

	//Формирование массива не склеившихся импликант после первого этапа
	for (int i = 0; i < funcLen; i++)
	{
		if (!isMerged[i])
		{
			notMergedLen++;
			notMerged = override2D(notMerged, notMergedLen);
			notMerged[notMergedLen - 1] = func[i];
		}
	}
	
	free(isMerged);

	//Вывод массива не склеившихся импликант после первого этапа
	printf("\nНе склеившиеся импликанты:\n\n");
	for (int i = 0; i < notMergedLen; i++)
	{
		for (int j = 0; j < impLen; j++)
			printf("%d", notMerged[i][j]);
		printf("\n");
	}

	printf("\n--------------------------------\n");

	//ПОСЛЕДУЮЩИЕ этапы
	int merge = 1;													//Количество склеиваний за итерацию
	int count = 2;
	while (merge)
	{
		merge = 0;
		int* used = (int*)calloc(funcMinLen, sizeof(int));				//Для формирования групп импликант
		int** nextFuncMin = NULL;										//Следующий массив (в строку)
		int nextFuncMinLen = 0;
		printf("\n%d этап:\n\n", count);
		for (int i = 0; i < funcMinLen; i++)
		{
			if (used[i])
				continue;

			int** arrOfImp = (int**)calloc(funcMinLen, sizeof(int*));																//Временный массив для импликант с одним положением x
			arrOfImp[i] = funcMin[i];

			for (int k = i + 1; k < funcMinLen; k++)
			{
				if (!used[k] && createMask(funcMin[i]) == createMask(funcMin[k]))		
				{
					used[k] = 1;
					//Формирование временного массива импликант
					arrOfImp[k] = funcMin[k];
				}
			}
			used[i] = 1;

			//Вывод временного массива
			for (int g = 0; g < funcMinLen; g++)
			{
				if (!arrOfImp[g])
					continue;
				for (int s = 0; s < impLen; s++)
					printf("%d", arrOfImp[g][s]);
				printf("\n");
			}
			printf("\n");

			//Склеивание во временном массиве
			for (int baseElem = 0; baseElem < funcMinLen; baseElem++)
			{
				if (!arrOfImp[baseElem])
					continue;

				for (int countElem = baseElem + 1; countElem < funcMinLen; countElem++)
				{
					if (!arrOfImp[countElem])
						continue;

					int changeAmount = 0;
					int changeIndex = 0;
					//Бежим по цифрам сравниваемой пары
					for (int compareNum = 0; compareNum < impLen; compareNum++)
					{
						if (arrOfImp[baseElem][compareNum] != arrOfImp[countElem][compareNum])
						{
							changeAmount++;
							changeIndex = compareNum;
						}
					}
					if (changeAmount < 2)												//Склеивание
					{
						merge++;
						//Помечаем склеившиеся импликанты
						funcMin[baseElem] = NULL;
						funcMin[countElem] = NULL;
						//
						int* newImpTemp = (int*)calloc(impLen, sizeof(int));
						memcpy(newImpTemp, arrOfImp[baseElem], impLen * sizeof(int));
						if (changeAmount == 1)
							newImpTemp[changeIndex] = 2;
						//Новый массив
						nextFuncMinLen++;
						nextFuncMin = override2D(nextFuncMin, nextFuncMinLen);
						nextFuncMin[nextFuncMinLen - 1] = newImpTemp;
					}
				}
			}
			free(arrOfImp);
		}

		printf("Результат этапа:\n\n");
		for (int i = 0; i < nextFuncMinLen; i++)
		{
			for (int j = 0; j < impLen; j++)
				printf("%d", nextFuncMin[i][j]);
			printf("\n");
		}

		//Сохраняем несклеившиеся импликанты
		for (int i = 0; i < funcMinLen; i++)
			if (funcMin[i])
			{
				notMergedLen++;
				notMerged = override2D(notMerged, notMergedLen);
				notMerged[notMergedLen - 1] = funcMin[i];
			}

		printf("\nНе склеившиеся импликанты:\n\n");
		for (int i = 0; i < notMergedLen; i++)
		{
			for (int j = 0; j < impLen; j++)
				printf("%d", notMerged[i][j]);
			printf("\n");
		}

		printf("\n--------------------------------\n");

		//Перезапись памяти из свеже сформированного массива
		if (merge)
		{
			free(funcMin);
			funcMinLen = nextFuncMinLen;
			funcMin = (int**)calloc(funcMinLen, sizeof(int*));
			for (int i = 0; i < nextFuncMinLen; i++)
			{
				funcMin[i] = (int*)calloc(impLen, sizeof(int));
				memcpy(funcMin[i], nextFuncMin[i], impLen * sizeof(int));					//из nextFuncMin в funcMin
			}
			
		}
		
		count++;
		free(nextFuncMin);
		free(used);
	}


	//ИМПЛИКАНТНАЯ ТАБЛИЦА
	int** kernel = NULL;
	int kernelLen = 0;
	int* kernelIndex = NULL;
	int kernelIndexLen = 0;
	int** sets = (int**)calloc(funcLen, sizeof(int*));						//Массив, показывающий какими простыми импликантами покрыт исходный набор
	int isSameImp = 0;
	for (int set = 0; set < funcLen; set++)
	{
		sets[set] = (int*)calloc(notMergedLen, sizeof(int));
		oneSum = 0;
		changeIndex = 0;
		for (int imp = 0; imp < notMergedLen; imp++)
		{
			changeAmount = 0;
			for (int compareNum = 0; compareNum < impLen; compareNum++)
				if (func[set][compareNum] != notMerged[imp][compareNum] && notMerged[imp][compareNum] != 2)
					changeAmount++;
			sets[set][imp] = (changeAmount) ? 0 : 1;			//0 - не покрывается, 1 - покрывается
			if (!changeAmount)
			{ 
				oneSum++;										//Количество импликант, покрывающих столбец
				changeIndex = imp;								//Индекс уникальной импликанты (потенциально)
			}
		}
		if (oneSum == 1)										//Ряд покрывает одна импликанта
		{
			for (int kernelImp = 0; kernelImp < kernelLen; kernelImp++)
			{
				changeAmount = 0;
				for (int sameImpNum = 0; sameImpNum < impLen; sameImpNum++)
					if (notMerged[changeIndex][sameImpNum] != kernel[kernelImp][sameImpNum])
						changeAmount++;
				isSameImp = (changeAmount) ? 0 : 1;				//Импликанты разные - 1, одинаковые - 0
			}
			if (!isSameImp)										//Импликанты разные
			{
				kernelLen++;
				kernel = override2D(kernel, kernelLen);
				kernel[kernelLen - 1] = notMerged[changeIndex];

				kernelIndexLen++;
				kernelIndex = override(kernelIndex, kernelIndexLen);
				kernelIndex[kernelIndexLen - 1] = changeIndex;
			}
		}
	}

		
	//Вывод таблицы
	printf("\n\nИмпликантная таблица:\n\n");
	printf("%*s", impLen + 3, ""); 
	for (int i = 0; i < funcLen; i++)
	{
		for (int j = 0; j < impLen; j++)
			printf("%d", func[i][j]);
		printf("|   ");
	}
	for (int i = 0; i < notMergedLen; i++)
	{
		printf("\n");
		for (int j = 0; j < funcLen * (impLen + 4) + impLen; j++)
			printf("-");
		printf("\n");
		for (int j = 0; j < impLen; j++)
			printf("%d", notMerged[i][j]);

		for (int j = 0; j < funcLen; j++)
			if (sets[j][i])
				printf("%*s%*s", 4, "x", impLen, "|");
			else
				printf("%*s", impLen + 4, "|");
	}

	//Вывод ядра функции
	printf("\n\nЯдро функции:\n\n");
	for (int i = 0; i < kernelLen; i++)
	{
		for (int j = 0; j < impLen; j++)
			printf("%d", kernel[i][j]);
		printf("\n");
	}


	//Метод Петрика
	
	//Создать массив вида [[a, b], [c, d], [d, e], [b, e, f]], где каждый подмассив формируется минтермом
	char** petrik = NULL;
	int petrikLen = 0;
	int covered;											//Флаг покрытия
	int* notCoveredIndex = NULL;
	int notCoveredIndexLen = 0;

	printf("\n\nИмпликантная таблица после вычёркивания:\n\n");
	printf("%*s", impLen + 6, "");
	for (int minTerm = 0; minTerm < funcLen; minTerm++)
	{
		covered = 0;
		//Если покрывается импликантой из ядра, то пропускаем
		for (int kIndex = 0; kIndex < kernelIndexLen; kIndex++)
			if (sets[minTerm][kernelIndex[kIndex]])
				covered = 1;
		if (covered)
			continue;
		//массив не покрытых минтермов
		notCoveredIndexLen++;
		notCoveredIndex = override(notCoveredIndex, notCoveredIndexLen);
		notCoveredIndex[notCoveredIndexLen - 1] = minTerm;
		
		//Для каждого минтерма формируем дизъюнкцию, при чем известно, что ядро не будет покрывать этот минтерм
		petrikLen++;
		petrik = override2DChar(petrik, petrikLen);
		char* petrikTemp = (char*)calloc(notMergedLen, sizeof(char));								//Буффер, для хранения в нём "индексов" импликант
		for (int imp = 0; imp < notMergedLen; imp++)	
			if (sets[minTerm][imp])
				petrikTemp[imp] = 'a' + imp;
		petrik[petrikLen - 1] = petrikTemp;

		//Кусочек отрисовки таблицы после вычеркивания
		for (int j = 0; j < impLen; j++)
			printf("%d", func[minTerm][j]);
		printf("|   ");		
	}

	//Отрисовка таблицы после вычеркивания
	int isKernel = 0;												//Флаг включения импликанты в ядро
	for (int i = 0; i < notMergedLen; i++)
	{
		isKernel = 0;
		//Пропуск импликант
		for (int j = 0; j < kernelIndexLen; j++)
			if (i == kernelIndex[j])
				isKernel = 1;
		if (isKernel)
			continue;

		printf("\n");
		for (int j = 0; j < notCoveredIndexLen * (impLen + 4) + impLen + 3; j++)
			printf("-");
		printf("\n");
		printf("%-3c", 'a' + i);
		for (int j = 0; j < impLen; j++)
			printf("%d", notMerged[i][j]);

		for (int j = 0; j < notCoveredIndexLen; j++)
		{
			if (sets[notCoveredIndex[j]][i])
				printf("%*s%*s", 4, "x", impLen, "|");
			else
				printf("%*s", impLen + 4, "|");
		}
	}

	//Вывод первого этапа метода Петрика
	printf("\n\nМетод Петрика:\n\n");
	for (int i = 0; i < petrikLen; i++)
	{
		printf("(");
		for (int j = 0; j < notMergedLen; j++)
			if (petrik[i][j])
				printf(" %c ", petrik[i][j]);
		printf(")");
		if (i < petrikLen - 1)
			printf(" ∧ ");
	}

	//Этап раскрытия скобок
	char** petrikTransformed = NULL;				//В нём формируется результат раскрытия скобок - [abc, bcd, ...]
	int petrikTransformedLen = 0;
	char** newPetrikTransformed = NULL;
	int newPetrikTransformedLen = 0;

	//Скопировать из первой конъюнкции
	for (int i = 0; i < notMergedLen; i++)
	{
		if (!petrik[0][i])
			continue;
		petrikTransformedLen++;
		petrikTransformed = override2DChar(petrikTransformed, petrikTransformedLen);
		char* buffer = (char*)calloc(2, sizeof(char));
		buffer[0] = petrik[0][i];
		petrikTransformed[petrikTransformedLen - 1] = buffer;
	}

	for (int conj = 1; conj < petrikLen; conj++)
	{

		newPetrikTransformedLen = 0;
		newPetrikTransformed = NULL;

		//Перемножаем скобки
		for (int str = 0; str < petrikTransformedLen; str++)
		{
			for (int disj = 0; disj < notMergedLen; disj++)
			{
				if (petrik[conj][disj])
				{
					newPetrikTransformedLen++;
					newPetrikTransformed = override2DChar(newPetrikTransformed, newPetrikTransformedLen);
					newPetrikTransformed[newPetrikTransformedLen - 1] = combineStrChar(petrikTransformed[str], petrik[conj][disj]);
				}
			}
		}

		//Трансформация. Преобразование. Состоит из 2 шагов: замена двух одинаковых букв на одну. от частного к общему (bdca ∨ bd -> bd)
		//ЗАМЕНА
		for (int elem = 0; elem < newPetrikTransformedLen; elem++)
			newPetrikTransformed[elem] = removeDuplicates(newPetrikTransformed[elem]);
		//ИНДУКЦИЯ
		for (int i = 0; i < newPetrikTransformedLen; i++)
			for (int j = i + 1; j < newPetrikTransformedLen; j++)
				if (strlen(newPetrikTransformed[i]) <= strlen(newPetrikTransformed[j]) && contains(newPetrikTransformed[i], newPetrikTransformed[j]))
					removeString(newPetrikTransformed, &newPetrikTransformedLen, j);
				else if (strlen(newPetrikTransformed[i]) > strlen(newPetrikTransformed[j]) && contains(newPetrikTransformed[j], newPetrikTransformed[i]))
					removeString(newPetrikTransformed, &newPetrikTransformedLen, i);

		//Вывод
		printf("\n\n--------------------------------\n\n");
		printf("(");
		for (int i = 0; i < newPetrikTransformedLen; i++)
		{
			fputs(newPetrikTransformed[i], stdout);
			printf(" ");
		}
		printf(")");
		for (int i = conj + 1; i < petrikLen; i++)
		{
			printf(" ∧ ");
			printf("(");
			for (int j = 0; j < notMergedLen; j++)
				if (petrik[i][j])
					printf(" %c ", petrik[i][j]);
			printf(")");
		}


		//Скопировать из newPetrikTransformed в petrikTransformed
		petrikTransformed = newPetrikTransformed;
		petrikTransformedLen = newPetrikTransformedLen;
	}

	//Оценка сложности
	int* complexity = (int*)calloc(petrikTransformedLen, sizeof(int));
	int* wasInversion = (int*)calloc(impLen, sizeof(int));
	int complexityTemp = 0;
	int kernelComplexity = 0;
	int minComplexity = 0;
	//Сложность ядра
	for (int j = 0; j < kernelLen; j++)
		for (int k = 0; k < impLen; k++)
		{
			if (kernel[j][k] == 1)
				{kernelComplexity++;}
			else if (kernel[j][k] == 0)
			{
				if (wasInversion[k])
					{kernelComplexity++;}
				else
				{
					kernelComplexity += 2;
					wasInversion[k] = 1;
				}
			}
		}
	kernelComplexity += kernelLen;

	for (int i = 0; i < petrikTransformedLen; i++)
	{
		complexityTemp = kernelComplexity;
		int* wasInversionTemp = (int*)calloc(impLen, sizeof(int));
		memcpy(wasInversionTemp, wasInversion, impLen * sizeof(int)); 
		
		int sym;
		for (sym = 0; petrikTransformed[i][sym] != '\0'; sym++)
		{
			for (int impNum = 0; impNum < impLen; impNum++){
				if (notMerged[(int)(petrikTransformed[i][sym] - 'a')][impNum] == 1)
					{complexityTemp++;}
				else if (notMerged[(int)(petrikTransformed[i][sym] - 'a')][impNum] == 0)
				{
					if (wasInversionTemp[impNum])
					{
						complexityTemp++;
					}
					else
					{
						complexityTemp += 2;
						wasInversionTemp[impNum] = 1;
					}
				}
			}
		}
		complexityTemp += sym;
		if (minComplexity == 0 || minComplexity > complexityTemp)
			minComplexity = complexityTemp;
		complexity[i] = complexityTemp;

		free(wasInversionTemp);
	}
	
	//Вывод тупиковых форм
	printf("\n\n--------------------------------\n\n");
	printf("\n\nТупиковые формы:\n\n");
	for (int i = 0; i < petrikTransformedLen; i++)
	{
		printf("f = ");
		for (int j = 0; j < kernelLen; j++)
		{ 
			for (int k = 0; k < impLen; k++)
			{ 
				if (kernel[j][k] == 2)
					printf("x");
				else
					printf("%d", kernel[j][k]);
			}
			printf(" ∨ ");
		}
		for (int sym = 0; petrikTransformed[i][sym] != '\0'; sym++)
		{
			for (int impNum = 0; impNum < impLen; impNum++)
				if (notMerged[(int)(petrikTransformed[i][sym] - 'a')][impNum] == 2)
					printf("x");
				else
					printf("%d", notMerged[(int)(petrikTransformed[i][sym] - 'a')][impNum]);
			if (petrikTransformed[i][sym + 1] != '\0')
				printf(" ∨ ");
		}
		printf("\tсложность по Квайну: %d", complexity[i]);
		printf("\n\n");
	}
	printf("*инверсии считаются один раз\n\n");
	

	//Поиск минимальных тупиковых форм
	printf("\n\n--------------------------------\n\n");
	printf("\n\nМинимальные тупиковые формы:\n\n");
	int*** output = NULL;
	int outputLen = 0;
	//Добавление элементов с миниммальной сложностью
	for (int i = 0; i < petrikTransformedLen; i++)
	{
		if (complexity[i] == minComplexity)
		{
			int** buffer = (int**)calloc(kernelLen + strlen(petrikTransformed[i]) + 1, sizeof(int*));
			int bufferLen = 0;
			//Запись ядра
			for (int j = 0; j < kernelLen; j++)
				buffer[bufferLen++] = kernel[j];
			//Запись остальных импликант
			for (int sym = 0; petrikTransformed[i][sym] != '\0'; sym++)
			{
				buffer[bufferLen++] = notMerged[(int)(petrikTransformed[i][sym] - 'a')];
			}
			buffer[bufferLen] = NULL;
			//Запись в итоговый массив
			outputLen++;
			output = override3D(output, outputLen);
			output[outputLen - 1] = buffer;
		}
	}

	//Вывод минимальных тупиковых форм
	for (int i = 0; i < outputLen; i++)
	{
		printf("f = ");
		for (int j = 0; output[i][j] != NULL; j++)
		{
			for (int k = 0; k < impLen; k++)
			{
				if (output[i][j][k] == 2)
					printf("x");
				else
					printf("%d", output[i][j][k]);
			}
			if (output[i][j + 1] != NULL)
				printf(" ∨ ");
		}
		printf("\n\n");
	}


	free(arr1Stage);
	free(input);
	free(func);
	free(funcMin);
	free(kernel);
	free(kernelIndex);
	free(petrik);
	free(petrikTransformed);
	free(sets);
	free(output);
}

int main()
{
	return 0;
}