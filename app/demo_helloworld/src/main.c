#include "app_demo_helloworld.h"

int main(void)
{
    if (app_demo_helloworld_init() != 0) {
        LOG_E("demo_helloworld init failed");
        while (1) {
        }
    }

    app_demo_helloworld_run();

    return 0;
}
