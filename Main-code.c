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
    initializeManager(&manager);

    int option;

    do {
        printf("\n=== Dynamic Array Manager ===\n");
        printf("1. Create array\n");
        printf("2. Resize array\n");
        printf("0. Exit\n");
        printf("Choose an option: ");

        if (scanf("%d", &option) != 1) {
            printf("Invalid input. Enter a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            option = -1;
            continue;
        }

        if (option == 1) {
            size_t newSize;
            int selectedGroup;
            int selectedSlot;

            printf("Enter the array size (1-%d): ", MAX_SIZE);

            if (scanf("%zu", &newSize) != 1) {
                printf("Invalid size input.\n");

                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {
                }

                continue;
            }

            int result = createArray(
                &manager,
                newSize,
                &selectedGroup,
                &selectedSlot
            );

            if (result == 100) {
                printf("Array created in Group %d, Slot %d.\n",
                       selectedGroup + 1, selectedSlot + 1);
            } else if (result == -1) {
                printf("No free slots available.\n");
            } else if (result == -2) {
                printf("Memory allocation failed.\n");
            } else if (result == -3) {
                printf("Invalid array size.\n");
            } else {
                printf("Could not create the array. Error: %d\n", result);
            }
        }
        else if (option == 2) {
            size_t groupNumber;
            size_t slotNumber;
            size_t newSize;

            printf("Enter group number (1-%d): ", MAX_GROUPS);

            if (scanf("%zu", &groupNumber) != 1) {
                printf("Invalid group input.\n");

                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {
                }

                continue;
            }

            printf("Enter slot number (1-%d): ", MAX_ARRAY_PER_GROUP);

            if (scanf("%zu", &slotNumber) != 1) {
                printf("Invalid slot input.\n");

                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {
                }

                continue;
            }

            if (groupNumber < 1 || groupNumber > MAX_GROUPS ||
                slotNumber < 1 || slotNumber > MAX_ARRAY_PER_GROUP) {
                printf("Invalid group or slot number.\n");
                continue;
            }

            ManagedArray *selectedArray =
                &manager.groups[groupNumber - 1]
                    .arrays[slotNumber - 1];

            if (selectedArray->data == NULL) {
                printf("That slot does not contain an array.\n");
                continue;
            }

            printf("Current array size: %zu\n", selectedArray->size);
            printf("Enter the new size (1-%d): ", MAX_SIZE);

            if (scanf("%zu", &newSize) != 1) {
                printf("Invalid size input.\n");

                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {
                }

                continue;
            }

            if (newSize < 1 || newSize > MAX_SIZE) {
                printf("Invalid size. Choose a value from 1 to %d.\n",
                       MAX_SIZE);
                continue;
            }

            if (resizeArray(
                    &selectedArray->data,
                    &selectedArray->size,
                    newSize)) {
                printf("Array resized successfully.\n");
            } else {
                printf("Could not resize the array.\n");
            }
        }
        else if (option == 0) {
            printf("Exiting Dynamic Array Manager.\n");
        }
        else {
            printf("Invalid option. Please try again.\n");
        }

    } while (option != 0);

    /* Release every allocated array before exiting. */
    for (size_t g = 0; g < MAX_GROUPS; g++) {
        for (size_t s = 0; s < MAX_ARRAY_PER_GROUP; s++) {
            free(manager.groups[g].arrays[s].data);
            manager.groups[g].arrays[s].data = NULL;
            manager.groups[g].arrays[s].size = 0;
        }

        manager.groups[g].count = 0;
    }

    return 0;
}
