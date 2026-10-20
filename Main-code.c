#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define MAX_GROUPS 10
#define MAX_ARRAY_PER_GROUP 100
#define MAX_SIZE 10000

#define CREATE_SUCCESS 100
#define ERR_NO_FREE_SLOTS -1
#define ERR_ALLOCATION_FAILED -2
#define ERR_INVALID_SIZE -3
#define ERR_INVALID_ARGUMENT -4

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

/* Manager-level functions */
void initializeManager(ArrayManager *manager);
int createArray(ArrayManager *manager, size_t newSize,
                int *selectedGroup, int *selectedSlot);

/* Individual-array functions: declarations for later implementation */
void displayArray(const int *array, size_t size);
int addElement(int **array, size_t *size, int value);
int removeElement(int *array, size_t *size, size_t index);
void analyzeArray(const int *array, size_t size);
int resizeArray(int **array, size_t *size, size_t newSize);
void clearArray(int **array, size_t *size);

void initializeManager(ArrayManager *manager)
{
    if (manager == NULL) {
        return;
    }

    for (size_t groupIndex = 0; groupIndex < MAX_GROUPS; groupIndex++) {
        Group *group = &manager->groups[groupIndex];
        group->count = 0;

        for (size_t slotIndex = 0;
             slotIndex < MAX_ARRAY_PER_GROUP;
             slotIndex++) {
            group->arrays[slotIndex].data = NULL;
            group->arrays[slotIndex].size = 0;
        }
    }
}

int createArray(ArrayManager *manager, size_t newSize,
                int *selectedGroup, int *selectedSlot)
{
    if (selectedGroup == NULL || selectedSlot == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    *selectedGroup = -1;
    *selectedSlot = -1;

    if (manager == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (newSize == 0 || newSize > MAX_SIZE) {
        return ERR_INVALID_SIZE;
    }

    int bestGroup = -1;
    size_t mostFreeSlots = 0;

    /* Ties stay with the lowest group index. */
    for (size_t groupIndex = 0; groupIndex < MAX_GROUPS; groupIndex++) {
        size_t count = manager->groups[groupIndex].count;

        if (count > MAX_ARRAY_PER_GROUP) {
            return ERR_INVALID_ARGUMENT;
        }

        size_t freeSlots = MAX_ARRAY_PER_GROUP - count;
        if (freeSlots > mostFreeSlots) {
            mostFreeSlots = freeSlots;
            bestGroup = (int)groupIndex;
        }
    }

    if (bestGroup == -1 || mostFreeSlots == 0) {
        return ERR_NO_FREE_SLOTS;
    }

    Group *group = &manager->groups[bestGroup];
    int bestSlot = -1;

    for (size_t slotIndex = 0;
         slotIndex < MAX_ARRAY_PER_GROUP;
         slotIndex++) {
        if (group->arrays[slotIndex].data == NULL) {
            bestSlot = (int)slotIndex;
            break;
        }
    }

    /* Count and slot state should agree. */
    if (bestSlot == -1) {
        return ERR_INVALID_ARGUMENT;
    }

    int *newData = malloc(newSize * sizeof *newData);
    if (newData == NULL) {
        return ERR_ALLOCATION_FAILED;
    }

    /* Register only after allocation succeeds. */
    group->arrays[bestSlot].data = newData;
    group->arrays[bestSlot].size = newSize;
    group->count++;

    *selectedGroup = bestGroup;
    *selectedSlot = bestSlot;

    return CREATE_SUCCESS;
}

int resizeArray(int **array, size_t *size, size_t newSize)
{
    if (array == NULL || size == NULL) {
        printf("Error: Invalid array arguments.\n");
        return 0;
    }

    if (newSize > MAX_SIZE) {
        printf("Error: Maximum array size is %d.\n", MAX_SIZE);
        return 0;
    }

    if (newSize == 0) {
        free(*array);
        *array = NULL;
        *size = 0;
        printf("Array cleared successfully.\n");
        return 1;
    }

    int *temp = realloc(*array, newSize * sizeof *temp);
    if (temp == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 0;
    }

    *array = temp;
    *size = newSize;
    printf("Array resized successfully to %zu elements.\n", *size);
    return 1;
}

int main(void)
{
    ArrayManager manager;
    int groupIndex;
    int slotIndex;
    int result;

    initializeManager(&manager);

    result = createArray(&manager, 5, &groupIndex, &slotIndex);

    if (result == CREATE_SUCCESS) {
        printf("Array created in Group %d, Slot %d.\n",
               groupIndex + 1, slotIndex + 1);
    } else {
        printf("Array creation failed. Error code: %d\n", result);
    }

    /* Temporary test cleanup: free the one array created above. */
    if (result == CREATE_SUCCESS) {
        free(manager.groups[groupIndex].arrays[slotIndex].data);
        manager.groups[groupIndex].arrays[slotIndex].data = NULL;
        manager.groups[groupIndex].arrays[slotIndex].size = 0;
        manager.groups[groupIndex].count--;
    }

    return 0;
}
