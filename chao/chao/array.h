#ifndef CHAO_ARRAY_H
#define CHAO_ARRAY_H

typedef struct {
	int* items;
	size_t length;
	size_t capacity;
} IntArray;

// this one is mainly intended for holding pointers to const chars 
// won't work with allocated strings cause we only do simple equal ops
typedef struct {
	char** items;
	size_t length;
	size_t capacity;
} StringArray;

#define array_add(array, x)\
	do {\
		if (array.length >= array.capacity) {\
			if (array.capacity == 0) array.capacity = 8;\
			else array.capacity *= 2;\
			array.items = realloc(array.items, sizeof(*array.items)*array.capacity);\
		};\
		array.items[array.length++] = x;\
	} while(0)

#define array_clear(array)\
	do {\
		array.length = 0;\
	}while(0)

#define array_remove_at(array, index)\
	do {\
		if (index < array.length) {\
			memmove(&array.items[index], &array.items[index+1], sizeof(*array.items) *(array.length - index - 1));\
			array.length --;\
		}\
	}while(0)

#define array_remove(array, x)\
	do {\
		for (int i = 0; i < array.length; ++i) {\
		  if (array.items[i] == x) {\
		      array_remove_at(array, i);\
		      break;\
	      }\
        }\
	} while(0)

#define array_free(array)\
    do {\
        free(array.items);\
    } while(0)

#endif // CHAO_ARRAY_H

