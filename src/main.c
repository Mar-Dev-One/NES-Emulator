#include <stdio.h>

#include "Components/2A03.h"
#include "Components/memory_bus.h"

int main(void)
{
    _2A03CPU maincpu;

    if (!init_cpu(&maincpu))
        printf("The cpu have not been initilized!");

    printf("The cpu have been initilized!\n");
    printf("%d\n", sizeof(memory_bus));

    printf("mem : %c\n", cpu_read(&maincpu, 0x0000));
    printf("mem : %c\n", cpu_read(&maincpu, 0x0001));

    cpu_write(&maincpu, 0x0000, 'Q');
    cpu_write(&maincpu, 0x0001, 'E');

    printf("mem : %c\n", cpu_read(&maincpu, 0x0000));
    printf("mem : %c\n", cpu_read(&maincpu, 0x0001));

    printf("%p\n", maincpu.bus->ram);

    

    destroy_cpu(&maincpu);
    return 0;
}