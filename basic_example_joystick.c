#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "graphics_HAL.h"


#define LEFT_THRESHOLD  1500



// This function initializes all the peripherals except graphics
void initialize();
void ModifyLEDColor(bool leftButtonWasPushed, bool rightButtonWasPushed);

void initADC();
void startADC();
void initJoyStick();
void getSampleJoyStick(unsigned *X, unsigned *Y);

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

    initADC();
    initJoyStick();
    startADC();
}


// Initializing the ADC which resides on SoC
void initADC() {
    ADC14_enableModule();

    // This sets the conversion clock to 3MHz
    ADC14_initModule(ADC_CLOCKSOURCE_SYSOSC,
                     ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1,
                      0
                     );

    // This configures the ADC to store output results
    // in ADC_MEM0 for joystick X.
    // Todo: if we want to add joystick Y, then, we have to use more memory locations
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM0, true);

    // This configures the ADC in manual conversion mode
    // Software will start each conversion.
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
}


void startADC() {
   // Starts the ADC with the first conversion
   // in repeat-mode, subsequent conversions run automatically
   ADC14_enableConversion();
   ADC14_toggleConversionTrigger();
}


// Interfacing the Joystick with ADC (making the proper connections in software)
void initJoyStick() {

    // This configures ADC_MEM0 to store the result from
    // input channel A15 (Joystick X), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A15,                 // joystick X
                                  ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A15 is multiplexed on GPIO port P6 pin PIN0
    // TODO: which one of GPIO_PRIMARY_MODULE_FUNCTION, or
    //                    GPIO_SECONDARY_MODULE_FUNCTION, or
    //                    GPIO_TERTIARY_MODULE_FUNCTION
    // should be used in place of 0 as the last argument?
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    // TODO: add joystick Y

}

void getSampleJoyStick(unsigned *X, unsigned *Y) {
    // ADC runs in continuous mode, we just read the conversion buffers
    *X = ADC14_getResult(ADC_MEM0);

    // TODO: Read the Y channel
}

