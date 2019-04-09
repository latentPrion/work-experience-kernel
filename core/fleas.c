#include <thread.h>
#include <libc/stdio.h>
#include <scheduler.h>

#define FLEAS1_ID       (11)
#define FLEAS2_ID       (12)
#define FLEAS3_ID       (13)
#define FLEAS4_ID       (14)

thread_t fleas1, fleas2, fleas3, fleas4;

void fleas1_main(void)
{
    /* Put your printf here! */
    printf("Fleas1\n");
}

void fleas2_main(void)
{
    /* Put your printf here! */
    printf("Fleas2\n");
}

void fleas3_main(void)
{
    /* Put your printf here! */
    printf("Fleas3\n");
}

void fleas4_main(void)
{
    /* Put your printf here! */
    printf("Fleas4\n");
}

/** EXPLANATION:
 ******************************************************************************/
int all_fleas_init(void)
{
    int ret1, ret2, ret3, ret4;

    ret1 = thread_init(&fleas1, FLEAS1_ID, &fleas1_main);
    ret2 = thread_init(&fleas2, FLEAS2_ID, &fleas2_main);
    ret3 = thread_init(&fleas3, FLEAS3_ID, &fleas3_main);
    ret4 = thread_init(&fleas4, FLEAS4_ID, &fleas4_main);

    return !(ret1 == 0 && ret2 == 0 && ret3 == 0 && ret4 == 0);
}
