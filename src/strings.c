#include <stdlib.h>
#include <math.h>
#include <string.h>

//Добавляет символ, НЕ МЕНЯЯ переданную строку (использует calloc)
char* combineStrChar(char* str, char sym) 
{
	int len = strlen(str);
	//Новый размер: длина строки + 1 символ + '\0'
	char* new_str = (char*)calloc(len + 2, sizeof(char));
	if (!new_str)
		exit(1);
	//Копируем исходную строку
	strcpy(new_str, str);
	//Добавляем символ и завершающий нуль
	new_str[len] = sym;
	new_str[len + 1] = '\0';
	return new_str;
}

//smaller покрывает larger -> 1, иначе -> 0
int contains(char* smaller, char* larger) 
{
	for (int i = 0; smaller[i] != '\0'; i++)
		if (!strchr(larger, smaller[i]))
			return 0;
	return 1;
}

void removeString(char** arr, int* size, int index) 
{
	free(arr[index]);
	for (int i = index; i < (*size) - 1; i++)
		arr[i] = arr[i + 1];

	arr[(*size) - 1] = NULL;
	(*size) -= 1;
}


int createMask(int* arr, int impLen)
{
	int mask = 0;
	for (int i = 0; i < impLen; i++)
		if (arr[i] == 2)
			mask += arr[i] * pow(10, impLen - i - 1);
	return mask;
}

void removeDuplicates(char* str)
{
	int read = 0;   			//флаг чтения
	int write = 0; 				//флаг записи
	
	for (; str[read] != '\0'; read++)
	{
		char c = str[read];
		int duplicate = 0;
		//Встречался ли символ в [0..write)
		for (int i = 0; i < write; i++)
			if (str[i] == c)
			{
				duplicate = 1;
				break;
			}
		if (!duplicate)
		{
			str[write] = c;
			write++;
		}
	}

	//Завершение строки
	str[write] = '\0';
}