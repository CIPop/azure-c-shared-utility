// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stddef.h>
#include <string.h>
#include "umockcall.h"

typedef struct UMOCKCALL_TAG
{
    char* function_name;
    void* umockcall_data;
    UMOCKCALL_DATA_FREE_FUNC umockcall_data_free;
    UMOCKCALL_DATA_STRINGIFY_FUNC umockcall_data_stringify;
    UMOCKCALL_DATA_ARE_EQUAL_FUNC umockcall_data_are_equal;
} UMOCKCALL;

UMOCKCALL_HANDLE umockcall_create(const char* function_name, void* umockcall_data, UMOCKCALL_DATA_FREE_FUNC umockcall_data_free, UMOCKCALL_DATA_STRINGIFY_FUNC umockcall_data_stringify, UMOCKCALL_DATA_ARE_EQUAL_FUNC umockcall_data_are_equal)
{
    UMOCKCALL* result;

    if ((function_name == NULL) ||
        (umockcall_data == NULL) ||
        (umockcall_data_free == NULL) ||
        (umockcall_data_stringify == NULL) ||
        (umockcall_data_are_equal == NULL))
    {
        /* Codes_SRS_UMOCKCALL_01_003: [ If any of the arguments are NULL, umockcall_create shall fail and return NULL. ] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_UMOCKCALL_01_001: [ umockcall_create shall create a new instance of a umock call and on success it shall return a non-NULL handle to it. ] */
        result = (UMOCKCALL*)malloc(sizeof(UMOCKCALL));
        /* Codes_SRS_UMOCKCALL_01_002: [ If allocating memory for the umock call instance fails, umockcall_create shall return NULL. ] */
        if (result != NULL)
        {
            size_t function_name_length = strlen(function_name);
            result->function_name = (char*)malloc(function_name_length + 1);
            if (result->function_name == NULL)
            {
                /* Codes_SRS_UMOCKCALL_01_002: [ If allocating memory for the umock call instance fails, umockcall_create shall return NULL. ] */
                free(result);
                result = NULL;
            }
            else
            {
                (void)memcpy(result->function_name, function_name, function_name_length + 1);
                result->umockcall_data = umockcall_data;
                result->umockcall_data_free = umockcall_data_free;
                result->umockcall_data_stringify = umockcall_data_stringify;
                result->umockcall_data_are_equal = umockcall_data_are_equal;
            }
        }
    }

    return result;
}

void umockcall_destroy(UMOCKCALL_HANDLE umockcall)
{
    /* Codes_SRS_UMOCKCALL_01_005: [ If the umockcall argument is NULL then umockcall_destroy shall do nothing. ]*/
    if (umockcall != NULL)
    {
        /* Codes_SRS_UMOCKCALL_01_004: [ umockcall_destroy shall free a previously allocated umock call instance. ] */
        umockcall->umockcall_data_free(umockcall->umockcall_data);
        free(umockcall->function_name);
        free(umockcall);
    }
}

/* Codes_SRS_UMOCKCALL_01_006: [ umockcall_are_equal shall compare the two mock calls and return whether they are equal or not. ] */
int umockcall_are_equal(UMOCKCALL_HANDLE left, UMOCKCALL_HANDLE right)
{
    int result;

    if (left == right)
    {
        /* Codes_SRS_UMOCKCALL_01_024: [ If both left and right pointers are equal, umockcall_are_equal shall return 1. ] */
        result = 1;
    }
    else if ((left == NULL) || (right == NULL))
    {
        /* Codes_SRS_UMOCKCALL_01_015: [ If only one of the left or right arguments are NULL, umockcall_are_equal shall return 0. ] */
        result = 0;
    }
    else
    {
        if (strcmp(left->function_name, right->function_name) != 0)
        {
            /* Codes_SRS_UMOCKCALL_01_025: [ If the function name does not match for the 2 calls, umockcall_are_equal shall return 0. ]*/
            result = 0;
        }
        else
        {
            /* Codes_SRS_UMOCKCALL_01_027: [ If the underlying umockcall_data_are_equal returns 1, then umockcall_are_equal shall return 1. ]*/
            result = left->umockcall_data_are_equal(left->umockcall_data, right->umockcall_data);
        }
    }

    return result;
}

char* umockcall_to_string(UMOCKCALL_HANDLE umockcall)
{
    char* result;

    if (umockcall == NULL)
    {
        result = NULL;
    }
    else
    {
        char* stringified_args = umockcall->umockcall_data_stringify(umockcall->umockcall_data);
        if (stringified_args == NULL)
        {
            result = NULL;
        }
        else
        {
            size_t function_name_length = strlen(umockcall->function_name);
            size_t stringified_args_length = strlen(stringified_args);
        
            /* 4 because () and [] */
            size_t call_length = function_name_length + stringified_args_length + 4;

            result = (char*)malloc(call_length + 1);
            if (result != NULL)
            {
                result[0] = '[';
                (void)memcpy(&result[1], umockcall->function_name, function_name_length);
                result[function_name_length + 1] = '(';
                (void)memcpy(&result[function_name_length + 2], stringified_args, stringified_args_length);
                result[function_name_length + stringified_args_length + 2] = ')';
                result[function_name_length + stringified_args_length + 3] = ']';
                result[function_name_length + stringified_args_length + 4] = '\0';
            }
        }
    }

    return result;
}

void* umockcall_get_call_data(UMOCKCALL_HANDLE umockcall)
{
    void* umockcall_data;

    if (umockcall == NULL)
    {
        umockcall_data = NULL;
    }
    else
    {
        umockcall_data = umockcall->umockcall_data;
    }

    return umockcall_data;
}