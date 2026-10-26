#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define MAX_GROUPS 10
#define MAX_ARRAY_PER_GROUP 100
#define MAX_SIZE 10000

#define CREATE_SUCCESS 100
#define ERR_NO_VALID_DATA -99
#define ERR_NO_FREE_SLOTS -1
#define ERR_ALLOCATION_FAILED -2
#define ERR_INVALID_SIZE -3
#define ERR_INVALID_ARGUMENT -4

#define SELECT_SUCCESS 0
#define SELECT_ERR_EMPTY -1
#define SELECT_ERR_ARGUMENT -2
#define SELECT_ERR_TOO_MANY_ATTEMPTS -5

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

void initializeManager(ArrayManager *manager);
int createArray(ArrayManager *manager, size_t newSize,
                int *selectedGroup, int *selectedSlot);
ManagedArray *selectArray(ArrayManager *manager, int *status);

void displayArray(const int *array, size_t size);
int addElement(int **array, size_t *size, int value);
int removeElement(int *array, size_t *size, size_t index);
void analyzeArray(const int *array, size_t size);
int resizeArray(int **array, size_t *size, size_t newSize);
void clearArray(int **array, size_t *size);
void arrayActionMenu(ManagedArray *array);

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

int createArray(ArrayManager *manager, size_t newSize,
                int *selectedGroup, int *selectedSlot)
{
    if (selectedGroup == NULL || selectedSlot == NULL || manager == NULL) {
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

    int *newData = malloc(newSize * sizeof *newData);
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

static void printSelectionErrorArt(void)
{
    printf("\n");
    printf("  +--------------------------------------+\n");
    printf("  |          ARRAY MANAGER ERROR         |\n");
    printf("  +--------------------------------------+\n");
    printf("                  .-''''-.\n");
    printf("                 /  x  x  \\\n");
    printf("                |    __    |\n");
    printf("                |  \\____/  |\n");
    printf("                 \\________/\n");
    printf("\n");
    printf("  The array has gone into hiding.\n");
    printf("  Check the group and slot, then try again.\n\n");
}

static int readSelectionNumber(const char *prompt, long long *value)
{
    printf("%s", prompt);
    if (scanf("%lld", value) == 1) {
        return 1;
    }

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
    return 0;
}

ManagedArray *selectArray(ArrayManager *manager, int *status)
{
    if (status == NULL) {
        return NULL;
    }

    *status = SELECT_ERR_ARGUMENT;
    if (manager == NULL) {
        return NULL;
    }

    long long groupNumber = 0;
    long long slotNumber = 0;
    int valid = 0;

    for (int attempt = 1; attempt <= 3; attempt++) {
        if (!readSelectionNumber("Enter group number (1-10): ",
                                 &groupNumber) ||
            groupNumber < 1 || groupNumber > MAX_GROUPS) {
            printf("That is not a valid option.\n");
            continue;
        }
        valid = 1;
        break;
    }

    if (!valid) {
        printSelectionErrorArt();
        *status = SELECT_ERR_TOO_MANY_ATTEMPTS;
        return NULL;
    }

    valid = 0;
    for (int attempt = 1; attempt <= 3; attempt++) {
        if (!readSelectionNumber("Enter slot number (1-100): ",
                                 &slotNumber) ||
            slotNumber < 1 || slotNumber > MAX_ARRAY_PER_GROUP) {
            printf("That is not a valid option.\n");
            continue;
        }
        valid = 1;
        break;
    }

    if (!valid) {
        printSelectionErrorArt();
        *status = SELECT_ERR_TOO_MANY_ATTEMPTS;
        return NULL;
    }

    ManagedArray *selected =
        &manager->groups[(size_t)(groupNumber - 1)]
             .arrays[(size_t)(slotNumber - 1)];

    if (selected->data == NULL) {
        printSelectionErrorArt();
        *status = SELECT_ERR_EMPTY;
        return NULL;
    }

    *status = SELECT_SUCCESS;
    return selected;
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

void displayArray(const int *array, size_t size)
{
    if (array == NULL || size == 0) {
        printf("Invalid data: error %d\n", ERR_NO_VALID_DATA);
        return;
    }

    printf("Array (%zu elements): [", size);

    for (size_t i = 0; i < size; i++) {
        printf("%d", array[i]);

        if (i < size - 1) {
            printf(", ");
        }
    }

    printf("]\n");
}

int main(void)
{
    ArrayManager manager;
    initializeManager(&manager);

    int option;
    int shutdownRequested = 0;

    do {
        printf("\n=== Dynamic Array Manager ===\n");
        printf("1. Create array\n");
        printf("2. Resize array\n");
        printf("3. Select array (test selection)\n");
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

            int result = createArray(&manager, newSize,
                                     &selectedGroup, &selectedSlot);

            if (result == CREATE_SUCCESS) {
                printf("Array created in Group %d, Slot %d.\n",
                       selectedGroup + 1, selectedSlot + 1);
            } else if (result == ERR_NO_FREE_SLOTS) {
                printf("No free slots available.\n");
            } else if (result == ERR_ALLOCATION_FAILED) {
                printf("Memory allocation failed.\n");
            } else if (result == ERR_INVALID_SIZE) {
                printf("Invalid array size.\n");
            } else {
                printf("Could not create the array. Error: %d\n", result);
            }
        } else if (option == 2) {
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

            ManagedArray *selected =
                &manager.groups[groupNumber - 1].arrays[slotNumber - 1];

            if (selected->data == NULL) {
                printf("That slot does not contain an array.\n");
                continue;
            }

            printf("Current array size: %zu\n", selected->size);
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

            if (!resizeArray(&selected->data, &selected->size, newSize)) {
                printf("Could not resize the array.\n");
            }
      } else if (option == 3) {
            int status;
            ManagedArray *selected = selectArray(&manager, &status);

            if (status == SELECT_ERR_TOO_MANY_ATTEMPTS) {
                printf("Too many invalid attempts. Shutting down.\n");
                shutdownRequested = 1;
            } else if (status == SELECT_SUCCESS && selected != NULL) {
                arrayActionMenu(selected);
            } else if (status == SELECT_ERR_EMPTY) {
                printf("No array was selected.\n");
            } else {
                printf("Could not select an array. Error: %d\n", status);
            }
        } else if (option == 0) {
            printf("Exiting Dynamic Array Manager.\n");
        } else {
            printf("Invalid option. Please try again.\n");
        }
    } while (option != 0 && !shutdownRequested);

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

void arrayActionMenu(ManagedArray *array)
{
    (void)array;
    printf("\nArray action menu opened.\n");
    printf("Returning to main menu...\n");
}
