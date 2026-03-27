#include <stdlib.h>
#include "memoryFuncs.h"
#include "globals.h"
#include "structs.h"

//Формирование из дизъюнкции метода Петрика тупиковых форм
void irredundantFormsTransform(
	ImplicantSet kernel, int** notMerged, char** petrickDisjunction,
	ImplicantSets* irredundantForms
)
{
	irredundantForms->set = (ImplicantSet*)calloc(irredundantForms->len, sizeof(ImplicantSet));		//Хранит все тупиковые формы, наполняется из notMerged
	if (!(irredundantForms->set))
		exit(1);
	

	//По всем конъюнкциям в petrickDisjunction - "abcd", "dfgh" ...
	for (int conj = 0; conj < irredundantForms->len; conj++)
	{
		//Добавляем импликанты из ядра
		for (int imp = 0; imp < kernel.len; imp++)
			irredundantForms->set[conj].arr = append(irredundantForms->set[conj].arr, &(irredundantForms->set[conj].len), kernel.arr[imp]);

		//По всем импликантам - "a", "b", "c" ...
		for (int imp = 0; petrickDisjunction[conj][imp] != '\0'; imp++)
			irredundantForms->set[conj].arr = append(irredundantForms->set[conj].arr, &(irredundantForms->set[conj].len), notMerged[(int)(petrickDisjunction[conj][imp] - 'a')]);
	}
}