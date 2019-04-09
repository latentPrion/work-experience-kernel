#include <thread.h>

#define FLEAS1_ID       (11)
#define FLEAS2_ID       (12)
#define FLEAS3_ID       (13)
#define FLEAS4_ID       (14)

thread_t fleas1, fleas2, fleas3, fleas4;

int fleas1_main(void)
{
}

int fleas2_main(void)
{
}

int fleas3_main(void)
{
}

int fleas4_main(void)
{
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

    return ret1 == 0 && ret2 == 0 && ret3 == 0 && ret4 == 0;
}
