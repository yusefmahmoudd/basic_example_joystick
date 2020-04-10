#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "ADC_HAL.h"
#include "graphics_HAL.h"


#define LEFT_THRESHOLD  1500



// This function initializes all the peripherals except graphics
void initialize();
void ModifyLEDColor(bool leftButtonWasPushed, bool rightButtonWasPushed);

int main(void)
{

    Graphics_Context g_sContext;

    initialize();
    InitGraphics(&g_sContext);
    draw_Base(&g_sContext);

    unsigned vx, vy;

    while (1)
    {

        getSampleJoyStick(&vx, &vy);
        bool joyStickPushedtoRight = false;
        bool joyStickPushedtoLeft = false;
        drawXY(&g_sContext, vx, vy);

        if (vx < LEFT_THRESHOLD)
        {
            joyStickPushedtoLeft = true;
        }

        MoveCircle(&g_sContext, joyStickPushedtoLeft,joyStickPushedtoRight);

     }
}




void initialize()
{
    // stop the watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Initialize the timers needed for debouncing
    Timer32_initModule(TIMER32_0_BASE, // There are two timers, we are using the one with the index 0
                       TIMER32_PRESCALER_1, // The prescaler value is 1; The clock is not divided before feeding the counter
                       TIMER32_32BIT, // The counter is used in 32-bit mode; the alternative is 16-bit mode
                       TIMER32_PERIODIC_MODE); //This options is irrelevant for a one-shot timer

    Timer32_initModule(TIMER32_1_BASE, // There are two timers, we are using the one with the index 1
                       TIMER32_PRESCALER_1, // The prescaler value is 1; The clock is not divided before feeding the counter
                       TIMER32_32BIT, // The counter is used in 32-bit mode; the alternative is 16-bit mode
                       TIMER32_PERIODIC_MODE); //This options is irrelevant for a one-shot timer


    initADC();
    initJoyStick();
    startADC();
}


