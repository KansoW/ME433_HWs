/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
#define DELAYTIME 20000 // 0.5 ms;

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    TRISAbits.TRISA4 = 0; // Pin 4 of Port A is LED1. Clear bit to zero for output.
    TRISBbits.TRISB4 = 1; // Pin 4 of Port B is USER button. Set to 1 for input.
    // TRISA = 0x00;
    LATAbits.LATA4 = 1; // Turn LED1 ON
    
    // initializations
    SPI1_init(); // Talk to LCD
    LCD_init();
    i2c_master_setup(); // Talk to IMU
    init_expander(); // Turn on accelerometer
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            if(!PORTBbits.RB4) {
                LATAbits.LATA4 = 0; // if button pressed, turn red LED off
            }

            else {
                _CP0_SET_COUNT(0);

                while(_CP0_GET_COUNT()< DURATION ){
                    ;// do nothing
                }
                LATAINV = 0b10000; // invert pin RA4
            }
        
            char msg[100];
            int arrlen = 14;
            unsigned char data[arrlen];
            LCD_clearScreen(BLACK);

            sprintf(msg, "WHOAMI output: %d", get_expander(WHO_AM_I)); //0b01101001
            draw_string(msg, 20, 10, RED, BLACK); //register returns 105

            while(1) {
                i2c_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, arrlen);

                // parse read values
                signed short temp = (data[1] << 8) | data[0]; //16-bit short
                signed short gyroX = (data[3] << 8) | data[2];
                signed short gyroY = (data[5] << 8) | data[4];
                signed short gyroZ = (data[7] << 8) | data[6];
                signed short accelX = (data[9] << 8) | data[8];
                signed short accelY = (data[11] << 8) | data[10];
                signed short accelZ = (data[13] << 8) | data[12];

                //scaling length and height
                float xscale = accelX*0.000061*100; 
                float yscale = accelY*0.000061*100;

                sprintf(msg, "x = %f", xscale);
                //draw_string(msg, 20, 100, RED, BLACK);
                sprintf(msg, "y = %f", yscale);
                //draw_string(msg, 20, 110, RED, BLACK);

                draw_xbar(64, 64, 50, 5, MAGENTA, BLACK, xscale); //xbar
                draw_ybar(64, 64, 5, 50, MAGENTA, BLACK, yscale); //ybar

                int i,j,xbox,ybox;
                for(i=0;i<5;i++){
                    xbox = 64+i;
                    for(j=0;j<5;j++){
                        ybox = 64+j;
                        LCD_drawPixel(xbox,ybox,WHITE);
                    }
                }

                //5Hz loop
                _CP0_SET_COUNT(0);
                while (_CP0_GET_COUNT() < 48000000/2/5){
                    ;
                }

            }// end infinite while

            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
