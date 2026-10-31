#ifndef ARRAY_MANAGER_H
#define ARRAY_MANAGER_H

#include <stddef.h>

#define MAX_GROUPS 10
#define MAX_ARRAY_PER_GROUP 100
#define MAX_SIZE 10000

/* Creation errors */
#define CREATE_SUCCESS 100
#define ERR_NO_VALID_DATA -99
#define ERR_NO_FREE_SLOTS -1
#define ERR_ALLOCATION_FAILED -2
#define ERR_INVALID_SIZE -3
#define ERR_INVALID_ARGUMENT -4

/* Selection results */
#define SELECT_SUCCESS 0
#define SELECT_ERR_EMPTY -6
#define SELECT_ERR_ARGUMENT -7
#define SELECT_ERR_TOO_MANY_ATTEMPTS -8

/* Insertion results */
#define ADD_SUCCESS 101
#define ADD_ERR_INVALID_INDEX -10
#define ADD_ERR_MAX_SIZE -11

/* Removal results */
#define REMOVE_SUCCESS 102
#define REMOVE_ERR_INVALID_INDEX -12

/* Deletion results */
#define DELETE_SUCCESS 103

typedef struct {
    int *data;
    size_t size;
} ManagedArray;

typedef struct {
    ManagedArray arrays[MAX_ARRAY_PER_GROUP];
    size_t count;
} Group;

typedef struct {
    Group groups[MAX_GROUPS];
} ArrayManager;

/* Manager operations */
void initializeManager(ArrayManager *manager);

int createArray(
    ArrayManager *manager,
    size_t newSize,
    int *selectedGroup,
    int *selectedSlot
);

ManagedArray *getArray(
    ArrayManager *manager,
    size_t groupIndex,
    size_t slotIndex
);

int deleteArray(
    ArrayManager *manager,
    size_t groupIndex,
    size_t slotIndex
);

/* Array operations */
int resizeArray(
    ManagedArray *array,
    size_t newSize
);

int addElement(
    ManagedArray *array,
    size_t index,
    int value
);

int removeElement(
    ManagedArray *array,
    size_t index
);

#endif
