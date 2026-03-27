#include <stdlib.h>
#include <string.h>


int** append(int** arr, int* len, int* added)
{
	*(len) += 1;
	int** buffer = (int**)realloc(arr, *(len) * sizeof(int*));
	if (!buffer)
		exit(1);
	buffer[*(len) - 1] = added;
	return buffer;
}

int** appendValueCopy(int** arr, int* len, int* added, int cpyLen)
{
	*(len) += 1;
	int** buffer = (int**)realloc(arr, *(len) * sizeof(int*));
	if (!buffer)
		exit(1);

	buffer[*(len)-1] = (int*)calloc(cpyLen, sizeof(int));
	if (!buffer[*(len)-1])
		exit(1);

	memcpy(buffer[*(len)-1], added, cpyLen * sizeof(int));
	return buffer;
}

int* appendNum(int* arr, int* len, int num)
{
	(*len) += 1;
	int* buffer = (int*)realloc(arr, *(len) * sizeof(int));
	if (!buffer)
		exit(1);
	buffer[*(len) - 1] = num;
	return buffer;
}

char** addMemStr(char** arr, int* len)
{
	(*len) += 1;
	char** buffer = (char**)realloc(arr, *(len) * sizeof(char*));
	if (!buffer)
		exit(1);
	buffer[(*len) - 1] = NULL;
	return buffer;
}

char** appendStrCopy(char** arr, int* len, char* str)
{
	(*len) += 1;
	char** buffer = (char**)realloc(arr, *(len) * sizeof(char*));
	if (!buffer)
		exit(1);
	memcpy(buffer[*(len)-1], str, strlen(str) + 1);
	return buffer;
}

char** appendStr(char** arr, int* len, char* str)
{
	(*len) += 1;
	char** buffer = (char**)realloc(arr, *(len) * sizeof(char*));
	if (!buffer)
		exit(1);
	buffer[*(len)-1] = str;
	return buffer;
}

char* appendChar(char* arr, char symbol)
{
	if (arr == NULL)
	{
		char* buffer = (char*)calloc(2, sizeof(char));
		if (!buffer)
			exit(1);
		buffer[0] = symbol;
		buffer[1] = '\0';
		return buffer;
	}

	int len = strlen(arr);
	char* buffer = (char*)realloc(arr, len + 2);
	if (!buffer)
		exit(1);

	buffer[len] = symbol;
	buffer[len + 1] = '\0';
	return buffer;
}