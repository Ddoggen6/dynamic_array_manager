#include "array_manager.h"


#include <stdlib.h>

/* Initialize every group and slot. */

void initializeManager(ArrayManager *manager)
{
    if (manager == NULL) {
        return;
    }

    for (size_t g = 0; g < MAX_GROUPS; g++) {
        manager->groups[g].count = 0;

        for (size_t s = 0; s < MAX_ARRAY_PER_GROUP; s++) {
            manager->groups[g].arrays[s].data = NULL;
            manager->groups[g].arrays[s].size = 0;
        }
    }
}

/* Create an array in the group with the most free slots. */

int createArray(
    ArrayManager *manager,
    size_t newSize,
    int *selectedGroup,
    int *selectedSlot
)
{
    if (manager == NULL ||
        selectedGroup == NULL ||
        selectedSlot == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    *selectedGroup = -1;
    *selectedSlot = -1;

    if (newSize == 0 || newSize > MAX_SIZE) {
        return ERR_INVALID_SIZE;
    }

    int bestGroup = -1;
    size_t mostFreeSlots = 0;

    for (size_t g = 0; g < MAX_GROUPS; g++) {
        size_t count = manager->groups[g].count;

        if (count > MAX_ARRAY_PER_GROUP) {
            return ERR_INVALID_ARGUMENT;
        }

        size_t freeSlots = MAX_ARRAY_PER_GROUP - count;

        if (freeSlots > mostFreeSlots) {
            mostFreeSlots = freeSlots;
            bestGroup = (int)g;
        }
    }

    if (bestGroup == -1 || mostFreeSlots == 0) {
        return ERR_NO_FREE_SLOTS;
    }

    Group *group = &manager->groups[bestGroup];
    int bestSlot = -1;

    for (size_t s = 0; s < MAX_ARRAY_PER_GROUP; s++) {
        if (group->arrays[s].data == NULL) {
            bestSlot = (int)s;
            break;
        }
    }

    if (bestSlot == -1) {
        return ERR_INVALID_ARGUMENT;
    }

    int *newData = calloc(newSize, sizeof *newData);

    if (newData == NULL) {
        return ERR_ALLOCATION_FAILED;
    }

    group->arrays[bestSlot].data = newData;
    group->arrays[bestSlot].size = newSize;
    group->count++;

    *selectedGroup = bestGroup;
    *selectedSlot = bestSlot;

    return CREATE_SUCCESS;
}

/* Retrieve an array using zero-based group and slot indices. */

ManagedArray *getArray(
    ArrayManager *manager,
    size_t groupIndex,
    size_t slotIndex
)
{
    if (manager == NULL ||
        groupIndex >= MAX_GROUPS ||
        slotIndex >= MAX_ARRAY_PER_GROUP) {
        return NULL;
    }

    ManagedArray *array =
        &manager->groups[groupIndex].arrays[slotIndex];

    if (array->data == NULL) {
        return NULL;
    }

    return array;
}

/* Release an array and update its group's count. */

int deleteArray(
    ArrayManager *manager,
    size_t groupIndex,
    size_t slotIndex
)
{
    if (manager == NULL ||
        groupIndex >= MAX_GROUPS ||
        slotIndex >= MAX_ARRAY_PER_GROUP) {
        return ERR_INVALID_ARGUMENT;
    }

    Group *group = &manager->groups[groupIndex];
    ManagedArray *array = &group->arrays[slotIndex];

    if (array->data == NULL) {
        return ERR_NO_VALID_DATA;
    }

    free(array->data);

    array->data = NULL;
    array->size = 0;

    if (group->count > 0) {
        group->count--;
    }

    return DELETE_SUCCESS;
}

/* Resize an existing array. A size of zero is not allowed here. */

int resizeArray(ManagedArray *array, size_t newSize)
{
    if (array == NULL || array->data == NULL) {
        return ERR_NO_VALID_DATA;
    }

    if (newSize == 0 || newSize > MAX_SIZE) {
        return ERR_INVALID_SIZE;
    }

    int *temp = realloc(
        array->data,
        newSize * sizeof *temp
    );

    if (temp == NULL) {
        return ERR_ALLOCATION_FAILED;
    }

    if (newSize > array->size) {
        for (size_t i = array->size; i < newSize; i++) {
            temp[i] = 0;
        }
    }

    array->data = temp;
    array->size = newSize;

    return 1;
}

/* Insert an element at a zero-based index. */

int addElement(ManagedArray *array, size_t index, int value)
{
    if (array == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (array->data == NULL) {
        return ERR_NO_VALID_DATA;
    }

    if (array->size >= MAX_SIZE) {
        return ADD_ERR_MAX_SIZE;
    }

    if (index > array->size) {
        return ADD_ERR_INVALID_INDEX;
    }

    int *temp = realloc(
        array->data,
        (array->size + 1) * sizeof *temp
    );

    if (temp == NULL) {
        return ERR_ALLOCATION_FAILED;
    }

    for (size_t i = array->size; i > index; i--) {
        temp[i] = temp[i - 1];
    }

    temp[index] = value;

    array->data = temp;
    array->size++;

    return ADD_SUCCESS;
}

/* Remove an element at a zero-based index. */

int removeElement(ManagedArray *array, size_t index)
{
    if (array == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (array->data == NULL || array->size == 0) {
        return ERR_NO_VALID_DATA;
    }

    if (index >= array->size) {
        return REMOVE_ERR_INVALID_INDEX;
    }

    for (size_t i = index; i < array->size - 1; i++) {
        array->data[i] = array->data[i + 1];
    }

    array->size--;

    return REMOVE_SUCCESS;
}
