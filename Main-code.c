#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define MAX_GROUPS 10
#define MAX_ARRAY_PER_GROUP 100
#define MAX_SIZE 10000

int createArray(int **array, size_t *size, size_t newSize);

void displayArray(const int *array, size_t size);

int addElement(int **array, size_t *size, int value);

int removeElement(int *array, size_t *size, size_t index);

void analyzeArray(const int *array, size_t size);

int resizeArray(int **array, size_t *size, size_t newSize){
      if (newSize > MAX_SIZE)
    {
        printf("Error: Maximum array size is %d.\n", MAX_SIZE);
        return 0;
    }

    if (newSize == 0)
    {
        free(*array);
        *array = NULL;
        *size = 0;

        printf("Array cleared successfully.\n");
        return 1;
    }

    int *temp = realloc(*array, newSize * sizeof(int));

    if (temp == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        return 0;
    }

    *array = temp;
    *size = newSize;

    printf("Array resized successfully to %zu elements.\n", *size);
    return 1;

};

void clearArray(int **array, size_t *size);


int main(){
  int *array = NULL;
  size_t size = 5;
  size_t newSize;
  int *temp;


  return 0;
}
