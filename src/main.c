#include <stdio.h>

#include "Components/2A03.h"

int main(void)
{
    _2A03CPU maincpu;

    if (!init_cpu(&maincpu))
        printf("The cpu have not been initilized!");

    printf("mem : %c\n", read_memory(&maincpu, 0));
    printf("mem : %c\n", read_memory(&maincpu, 0xFFFE));

    printf("%p\n", maincpu.memory);

    

    destroy_cpu(&maincpu);
    return 0;
}