// Written by theCatwoman

/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>
#include <time.h>

/* Global Variables ----------------------------------------------------------*/
PAINT_TIME sPaint_time = {12, 34, 56};      
PAINT_TIME confirmed_time = {12, 34, 56};   

UBYTE *BlackImage;  
int score = 0;      

// Define the button pin (GPIO 0 as discussed previously)
const int PIN_BUTTON = 0; 
const int PIN_BUTTON_COIL = 19;
/* Time Setting Functions ----------------------------------------------------*/

void incrementTime(char unit) {
    switch (unit) {
        case 'H':
            sPaint_time.Hour = (sPaint_time.Hour + 1) % 24;
            break;
        case 'M':
            sPaint_time.Min = (sPaint_time.Min + 1) % 60;
            break;
        case 'S':
            sPaint_time.Sec = (sPaint_time.Sec + 1) % 60;
            break;
    }
}


void decrementTime(char unit) {
    switch (unit) {
        case 'H':
            sPaint_time.Hour = (sPaint_time.Hour == 0) ? 23 : sPaint_time.Hour - 1;
            break;
        case 'M':
            sPaint_time.Min = (sPaint_time.Min == 0) ? 59 : sPaint_time.Min - 1;
            break;
        case 'S':
            sPaint_time.Sec = (sPaint_time.Sec == 0) ? 59 : sPaint_time.Sec - 1;
            break;
    }
}

// Confirm and save the current time settings
void confirmTime() {
    confirmed_time = sPaint_time; 
    Paint_Clear(WHITE); 
    Paint_DrawString_EN(0, 0, "Time Confirmed!", &Font20, BLACK, WHITE); 
    Paint_DrawTime(0, 40, &confirmed_time, &Font20, BLACK, WHITE); 
    EPD_1IN54_V2_DisplayPart(BlackImage);  
}

/* Setup and Initialization --------------------------------------------------*/
void setup() {
    printf("EPD_1IN54_test \r\n");
    DEV_Module_Init();
    EPD_1IN54_V2_Init();
    EPD_1IN54_V2_Clear();
    DEV_Delay_ms(50);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_BUTTON_COIL, INPUT_PULLUP);  

    UWORD Imagesize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while(1);
    }
    Paint_NewImage(BlackImage, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    EPD_1IN54_V2_DisplayPartBaseImage(BlackImage);  

    Paint_DrawBitMap(epd_bitmap_Bp_1);
    EPD_1IN54_V2_DisplayPart(BlackImage);
    DEV_Delay_ms(5);  // Display the image for a short time

    EPD_1IN54_V2_Init_Partial();
}

/* Main Loop -----------------------------------------------------------------*/
void loop() {
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    const int num_frames = 6;
    const UBYTE* frames[num_frames] = { 
        epd_bitmap_Bp_1, epd_bitmap_Bp_2, epd_bitmap_Bp_3, epd_bitmap_Bp_4,
        epd_bitmap_Bp_5, epd_bitmap_Bp_6, //epd_bitmap_Bp_6a, epd_bitmap_Bp_7,
        //epd_bitmap_Bp_7a, epd_bitmap_Bp_8a, //epd_bitmap_Bp_m0, epd_bitmap_Bp_m1,
        //epd_bitmap_Bp_m2, epd_bitmap_Bp_m2_1, epd_bitmap_Bp_m3, epd_bitmap_Bp_m4,
        //epd_bitmap_Bp_m4_1, epd_bitmap_Bp_m5_1, epd_bitmap_Bp_m5_2, epd_bitmap_Bp_m_0,
        //epd_bitmap_bitmap
    }; 

    while (true) {
        for(int i = 0; i < num_frames; i++) {
            if (digitalRead(PIN_BUTTON) == LOW) {
                score += 1;  // Increment the score by 1
                printf("Button pressed! Score: %d\n", score);
            }

            incrementTime('S');  
            if (sPaint_time.Sec == 60) {
                sPaint_time.Min = 1;
                incrementTime('M');  
                if (sPaint_time.Min == 60) {
                    sPaint_time.Hour = 1;
                    incrementTime('H');  
                }
            }

            Paint_Clear(WHITE);
            Paint_DrawBitMap(frames[i]);

            Paint_DrawRectangle(0, 0, 200, 24, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
            char scoreStr[20];
            sprintf(scoreStr, "Score: %d", score);
            Paint_DrawString_EN(0, 3, scoreStr, &Font20, BLACK, WHITE); 
            Paint_DrawRectangle(0, 180, 200, 200, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
            Paint_DrawTime(0, 180, &sPaint_time, &Font20, BLACK, WHITE);

            EPD_1IN54_V2_DisplayPart(BlackImage);
            DEV_Delay_ms(5);  // Adjust delay for each frame
        }
    }
}
}
