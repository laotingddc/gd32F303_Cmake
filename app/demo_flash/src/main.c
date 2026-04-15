#include "app_demo_flash.h"

int main(void)
{
    LOG_I("Demo: Flash");
    app_demo_flash_init();
    app_demo_flash_run();

    return 0;
}
