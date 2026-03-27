#include <stdlib.h>
#include "memoryFuncs.h"
#include "globals.h"
#include "structs.h"

ImplicantSets appendForm(ImplicantSets, ImplicantSet); 

//Поиск минимальных тупиковых форм
void minimalIrredundantForms(
	ImplicantSets irredundantForms,
	int* complexity, int minComplexity,
	ImplicantSets* output
)
{
	//По всем тупиковым формам
	for (int form = 0; form < irredundantForms.len; form++)
		//Тупиковые формы с минимальной сложностью по Квайну добавляем в результат
		if (complexity[form] == minComplexity)
			//Наполняется из irredundantForms -> notMerged
			*output = appendForm(*output, irredundantForms.set[form]);
}


ImplicantSets appendForm(ImplicantSets sets, ImplicantSet added)
{
	sets.len += 1;
	ImplicantSet* buffer = (ImplicantSet*)realloc(sets.set, sets.len * sizeof(ImplicantSet));
	if (!buffer)
		exit(1);
	buffer[sets.len - 1] = added;
	
	sets.set = buffer;

	return sets;
}