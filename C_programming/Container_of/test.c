#include <stdio.h>

#define container_of(ptr, type, member) ((type*)((char*)ptr - (char*)(&((type*)0)->member))) 
struct some_data
{
    char a;
    int b;
    char c;
    int d;
};

void get_container_of(char *ptr)
{
    int offset;
    
    offset = (long)(&((struct some_data *)0)->c);
    struct some_data *data = (struct some_data *)(ptr - offset);

    printf("data->a = %c\n", data->a);
    printf("data->b = %d\n", data->b);
    printf("data->c = %c\n", data->c);
    printf("data->d = %d\n", data->d);
}

int main()
{
    struct some_data data;
    data.a = 'a';
    data.b = 1; 
    data.c = 'c';   
    data.d = 2;
    struct some_data *data1 = container_of(&data.c, struct some_data, c);
    printf("data-1>a = %c\n", data1->a);
    printf("data1->b = %d\n", data1->b);
    printf("data1->c = %c\n", data1->c);
    printf("data1->d = %d\n", data1->d);
#if 0
    get_container_of(&(data.c)) ;
#endif

    return 0;
}