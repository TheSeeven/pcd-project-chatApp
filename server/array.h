// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// typedef struct GeneralArray GeneralArray;
// struct GeneralArray
// {
//     void *elements;
//     size_t inArray;
//     size_t size;
// };

// int getNextAlocationSize(GeneralArray *self)
// {
//     return self->size * 2;
// }

// void *initializeArray()
// {
//     void *arr = malloc(sizeof(void *) * 1);
//     memset(arr, '\0', 1);
//     return arr;
// }

// void addElements(GeneralArray *self, void *element)
// {
//     if (++self->inArray > self->size)
//     {
//         realloc(self->elements, getNextAlocationSize(self));
//     }
//     else
//     {
//         (self->elements)[self->inArray++];
//     }
// }

// int main()
// {

//     printf("hello");

//     return 0;
// }
