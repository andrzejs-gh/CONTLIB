#include <stdio.h>
#include "cont.h"

void print_cont_err(int code)
{
	switch (code)
	{
		case INVALID_INDEX:
			printf("INVALID_INDEX \n"); break;
		case SIZE_OVERFLOW:
			printf("SIZE_OVERFLOW \n"); break;
		case NULL_ARRAY_POINTER:
			printf("NULL_ARRAY_POINTER \n"); break;
		case REALLOC_FAILURE:
			printf("REALLOC_FAILURE \n"); break;
		case MALLOC_FAILURE:
			printf("MALLOC_FAILURE \n"); break;
		case NULL_ITEM_POINTER:
			printf("NULL_ITEM_POINTER \n"); break;
		case NULL_ELEMENT_COUNT:
			printf("NULL_ELEMENT_COUNT \n"); break;
		case NULL_CAPACITY:
			printf("NULL_CAPACITY \n"); break;
		case MAX_CAPACITY_EXCEEDED:
			printf("MAX_CAPACITY_EXCEEDED \n"); break;
		case COUNT_LARGER_THAN_CURRENT_COUNT:
			printf("COUNT_LARGER_THAN_CURRENT_COUNT \n"); break;
		case INVALID_GROWTH_FACTOR_VALUE:
			printf("INVALID_GROWTH_FACTOR_VALUE \n"); break;
		case CONT_ALREADY_FREED:
			printf("CONT_ALREADY_FREED \n"); break;
		case EMPTY_CONT:
			printf("EMPTY_CONT \n"); break;
		case UNIT_MISMATCH:
			printf("UNIT_MISMATCH \n"); break;
		case SAME_CONT:
			printf("SAME_CONT \n"); break;
		case INVALID_RANGE:
			printf("INVALID_RANGE \n"); break;
		case CONT_IS_INVALID:
			printf("CONT_IS_INVALID \n"); break;
    }
}

void cont_info(cont* cont_)
{
	char* is_valid = cont_is_valid(cont_) ? "true" : "false";
	
    printf("\n=== *** ===\n");
    printf("unit: %zu \n", cont_->unit);
    printf("count: %zu \n", cont_->count);
    printf("capacity: %zu \n", cont_->capacity);
    printf("max capacity: %zu \n", cont_->max_capacity);
    printf("growth factor: %f \n", cont_->growth_factor);
    printf("heap address: %p \n", (void*)cont_->addr);
    printf("is valid: %s \n", is_valid);
    printf("=== *** ===\n\n");
}
