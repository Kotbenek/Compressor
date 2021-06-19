#include "Sort.h"

#include <algorithm>

void Sort::bubble_sort(Node** a, int size)
{
	for (int i = 0; i < size - 1; i++)
		for (int j = 0; j < size - i - 1; j++)
			if (a[j]->occurences < a[j + 1]->occurences)
				swap(a[j], a[j + 1]);
}
