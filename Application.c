/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* HAL and Application includes */
#include <Application.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>

/**
 * The main entry point of your project. The main function should immediately
 * stop the Watchdog timer, call the Application constructor, and then
 * repeatedly call the main super-loop function. The Application constructor
 * should be responsible for initializing all hardware components as well as all
 * other finite state machines you choose to use in this project.
 *
 * THIS FUNCTION IS ALREADY COMPLETE. Unless you want to temporarily experiment
 * with some behavior of a code snippet you may have, we DO NOT RECOMMEND
 * modifying this function in any way.
 */
int main(void)
{
    // Stop Watchdog Timer - THIS SHOULD ALWAYS BE THE FIRST LINE OF YOUR MAIN
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();

    // Main super-loop! In a polling architecture, this function should call
    // your main FSM function over and over.
    while (true)
    {
        Application_loop(&app, &hal);
        HAL_refresh(&hal);
    }
}

/**
 * The main constructor for your application. This function should initialize
 * each of the FSMs which implement the application logic of your project.
 *
 * @return a completely initialized Application object
 */
Application Application_construct()
{
    Application app;

    // a 1-second timer (i.e. 1000 ms as specified in the SWTimer_contruct)
    app.Launchpad_LED2_blinkingTimer = SWTimer_construct(1000);
    SWTimer_start(&app.Launchpad_LED2_blinkingTimer);

    return app;
}

/**
 * The main super-loop function of the application. We place this inside of a
 * single infinite loop in main. In this way, we can model a polling system of
 * FSMs. Every cycle of this loop function, we poll each of the FSMs one time,
 * followed by refreshing all inputs to the system through a convenient
 * [HAL_refresh()] call.
 *
 * @param app:  A pointer to the main Application object.
 * @param hal:  A pointer to the main HAL object
 */
void Application_loop(Application* app, HAL* hal)
{
    // LED1 is toggled whenever Launchpad S1 is tapped (goes from released to pressed)
    // This is based on an FSM for push-button as well as a debouncing FSM
    if (Button_isTapped(&hal->launchpadS1)) {
        LED_toggle(&hal->launchpadLED1);
    }

    // Turn on the RGB Red LEDs (LED2 and BLED) ONLY WHEN L2 is pressed
    // This is a simple logic which has no memory
    if (Button_isPressed(&hal->launchpadS2))
    {
        LED_turnOn(&hal->launchpadLED2Red);
        LED_turnOn(&hal->boosterpackRed);
    }
    else
    {
        LED_turnOff(&hal->launchpadLED2Red);
        LED_turnOff(&hal->boosterpackRed);
    }

    if (SWTimer_expired(&app->Launchpad_LED2_blinkingTimer))
    {
        // Turn on the RGB Green LEDs (LED2 and BLED) ONLY WHEN B1 is pressed
        LED_toggle(&hal->launchpadLED2Green);
        SWTimer_start(&app->Launchpad_LED2_blinkingTimer);
    }



}

