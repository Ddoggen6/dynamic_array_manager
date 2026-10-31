
#include <stdio.h>
#include <stdlib.h>

#include "array_manager.h"

/* Read an integer from the terminal. */

static int readNumber(const char *prompt, long long *value)
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

/* Display the selection error artwork. */

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

/* Display an array. */

static void displayArray(const ManagedArray *array)
{
    if (array == NULL ||
        array->data == NULL ||
        array->size == 0) {
        printf("Invalid data: error %d\n", ERR_NO_VALID_DATA);
        return;
    }

    printf("Array (%zu elements): [", array->size);

    for (size_t i = 0; i < array->size; i++) {
        printf("%d", array->data[i]);

        if (i < array->size - 1) {
            printf(", ");
        }
    }

    printf("]\n");
}

/* Ask the user to select an existing array. */

static ManagedArray *selectArray(
    ArrayManager *manager,
    int *status,
    size_t *selectedGroup,
    size_t *selectedSlot
)
{
    if (manager == NULL ||
        status == NULL ||
        selectedGroup == NULL ||
        selectedSlot == NULL) {

        if (status != NULL) {
            *status = SELECT_ERR_ARGUMENT;
        }

        return NULL;
    }

    long long groupNumber = 0;
    long long slotNumber = 0;
    int valid = 0;

    *status = SELECT_ERR_ARGUMENT;

    /* Request a valid group number. */

    for (int attempt = 1; attempt <= 3; attempt++) {
        if (!readNumber(
                "Enter group number (1-10): ",
                &groupNumber) ||
            groupNumber < 1 ||
            groupNumber > MAX_GROUPS) {

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

    /* Request a valid slot number. */

    for (int attempt = 1; attempt <= 3; attempt++) {
        if (!readNumber(
                "Enter slot number (1-100): ",
                &slotNumber) ||
            slotNumber < 1 ||
            slotNumber > MAX_ARRAY_PER_GROUP) {

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

    *selectedGroup = (size_t)(groupNumber - 1);
    *selectedSlot = (size_t)(slotNumber - 1);

    ManagedArray *array = getArray(
        manager,
        *selectedGroup,
        *selectedSlot
    );

    if (array == NULL) {
        printSelectionErrorArt();
        *status = SELECT_ERR_EMPTY;
        return NULL;
    }

    *status = SELECT_SUCCESS;
    return array;
}

/* Menu for actions on a selected array. */

static void arrayActionMenu(ManagedArray *array)
{
    if (array == NULL) {
        printf("Error: Invalid array selection.\n");
        return;
    }

    int option = -1;

    do {
        printf("\n=== Array Action Menu ===\n");
        printf("1. Display array\n");
        printf("2. Add element\n");
        printf("3. Remove element\n");
        printf("0. Return to main menu\n");

        long long input;

        if (!readNumber("Choose an option: ", &input)) {
            printf("Invalid input. Enter a number.\n");
            option = -1;
            continue;
        }

        option = (int)input;

        if (option == 1) {
            displayArray(array);

        } else if (option == 2) {
            long long position;
            long long value;

            if (!readNumber(
                    "Enter insertion position: ",
                    &position)) {
                printf("Invalid position input.\n");
                continue;
            }

            if (position < 1 ||
                (unsigned long long)position > array->size + 1) {
                printf("Invalid insertion position.\n");
                continue;
            }

            if (!readNumber(
                    "Enter the integer value to insert: ",
                    &value)) {
                printf("Invalid integer value.\n");
                continue;
            }

            int result = addElement(
                array,
                (size_t)(position - 1),
                (int)value
            );

            if (result == ADD_SUCCESS) {
                printf("Element inserted successfully.\n");
            } else {
                printf("Insertion failed. Error: %d\n", result);
            }

        } else if (option == 3) {
            long long position;

            if (array->size == 0) {
                printf("The array is empty.\n");
                continue;
            }

            if (!readNumber(
                    "Enter removal position: ",
                    &position)) {
                printf("Invalid position input.\n");
                continue;
            }

            if (position < 1 ||
                (unsigned long long)position > array->size) {
                printf("Invalid removal position.\n");
                continue;
            }

            int result = removeElement(
                array,
                (size_t)(position - 1)
            );

            if (result == REMOVE_SUCCESS) {
                printf("Element removed successfully.\n");
            } else {
                printf("Removal failed. Error: %d\n", result);
            }

        } else if (option == 0) {
            printf("Returning to main menu...\n");

        } else {
            printf("Invalid option. Please try again.\n");
        }

    } while (option != 0);
}

/* Main program. */

int main(void)
{
    ArrayManager manager;
    initializeManager(&manager);

    int option = -1;
    int shutdownRequested = 0;

    do {
        printf("\n=== Dynamic Array Manager ===\n");
        printf("1. Create array\n");
        printf("2. Resize array\n");
        printf("3. Select array\n");
        printf("4. Delete array\n");
        printf("0. Exit\n");

        long long input;

        if (!readNumber("Choose an option: ", &input)) {
            printf("Invalid input. Enter a number.\n");
            continue;
        }

        option = (int)input;

        if (option == 1) {
            long long requestedSize;
            int selectedGroup;
            int selectedSlot;

            if (!readNumber(
                    "Enter the array size (1-10000): ",
                    &requestedSize)) {
                printf("Invalid size input.\n");
                continue;
            }

            if (requestedSize < 1 || requestedSize > MAX_SIZE) {
                printf("Invalid array size.\n");
                continue;
            }

            int result = createArray(
                &manager,
                (size_t)requestedSize,
                &selectedGroup,
                &selectedSlot
            );

            if (result == CREATE_SUCCESS) {
                printf(
                    "Array created in Group %d, Slot %d.\n",
                    selectedGroup + 1,
                    selectedSlot + 1
                );
            } else {
                printf("Could not create the array. Error: %d\n", result);
            }

        } else if (option == 2) {
            long long groupNumber;
            long long slotNumber;
            long long newSize;

            if (!readNumber("Enter group number (1-10): ", &groupNumber) ||
                !readNumber("Enter slot number (1-100): ", &slotNumber)) {
                printf("Invalid input.\n");
                continue;
            }

            if (groupNumber < 1 || groupNumber > MAX_GROUPS ||
                slotNumber < 1 || slotNumber > MAX_ARRAY_PER_GROUP) {
                printf("Invalid group or slot number.\n");
                continue;
            }

            ManagedArray *array = getArray(
                &manager,
                (size_t)(groupNumber - 1),
                (size_t)(slotNumber - 1)
            );

            if (array == NULL) {
                printf("That slot does not contain an array.\n");
                continue;
            }

            printf("Current array size: %zu\n", array->size);

            if (!readNumber(
                    "Enter the new size (1-10000): ",
                    &newSize)) {
                printf("Invalid size input.\n");
                continue;
            }

            if (newSize < 1 || newSize > MAX_SIZE) {
                printf("Invalid array size.\n");
                continue;
            }

            int result = resizeArray(array, (size_t)newSize);

            if (result == 1) {
                printf("Array resized successfully to %zu elements.\n",
                       array->size);
            } else {
                printf("Could not resize the array. Error: %d\n", result);
            }

        } else if (option == 3) {
            int status;
            size_t groupIndex;
            size_t slotIndex;

            ManagedArray *array = selectArray(
                &manager,
                &status,
                &groupIndex,
                &slotIndex
            );

            if (status == SELECT_SUCCESS && array != NULL) {
                arrayActionMenu(array);

            } else if (status == SELECT_ERR_EMPTY) {
                printf("No array was selected.\n");

            } else if (status == SELECT_ERR_TOO_MANY_ATTEMPTS) {
                printf("Too many invalid attempts. Shutting down.\n");
                shutdownRequested = 1;

            } else {
                printf("Could not select an array. Error: %d\n", status);
            }

        } else if (option == 4) {
            long long groupNumber;
            long long slotNumber;

            if (!readNumber("Enter group number (1-10): ", &groupNumber) ||
                !readNumber("Enter slot number (1-100): ", &slotNumber)) {
                printf("Invalid input.\n");
                continue;
            }

            if (groupNumber < 1 || groupNumber > MAX_GROUPS ||
                slotNumber < 1 || slotNumber > MAX_ARRAY_PER_GROUP) {
                printf("Invalid group or slot number.\n");
                continue;
            }

            int result = deleteArray(
                &manager,
                (size_t)(groupNumber - 1),
                (size_t)(slotNumber - 1)
            );

            if (result == DELETE_SUCCESS) {
                printf("Array deleted successfully.\n");
            } else {
                printf("Could not delete the array. Error: %d\n", result);
            }

        } else if (option == 0) {
            printf("Exiting Dynamic Array Manager.\n");

        } else {
            printf("Invalid option. Please try again.\n");
        }

    } while (option != 0 && !shutdownRequested);

    /* Release all remaining allocations. */

    for (size_t g = 0; g < MAX_GROUPS; g++) {
        for (size_t s = 0; s < MAX_ARRAY_PER_GROUP; s++) {
            deleteArray(&manager, g, s);
        }
    }

    return 0;
}
