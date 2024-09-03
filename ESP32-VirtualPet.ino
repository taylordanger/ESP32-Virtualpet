/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>

/* Pin Definitions -----------------------------------------------------------*/
//#define PIN_BUTTON 2  // Define the button pin (uncomment and change this to your actual button pin if needed)

/* Entry point ----------------------------------------------------------------*/
void setup() {
    printf("EPD_1IN54_test v2 Demo\r\n");
    DEV_Module_Init();
    EPD_1IN54_V2_Init();
    EPD_1IN54_V2_Clear();
    DEV_Delay_ms(100);

    // Initialize the button pin (uncomment if needed)
    // pinMode(PIN_BUTTON, INPUT_PULLUP);  // Assuming a pull-up resistor

    // Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while(1);
    }
    Paint_NewImage(BlackImage, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    EPD_1IN54_V2_DisplayPartBaseImage(BlackImage);  // Set the base image for partial updates

    // Display initial bitmap
    Paint_DrawBitMap(epd_bitmap_Mouth_eyes_3);
    EPD_1IN54_V2_DisplayPart(BlackImage);
    DEV_Delay_ms(75);  // Display the image for a short time

    // Initialize partial update mode
    EPD_1IN54_V2_Init_Partial();
}

/* The main loop -------------------------------------------------------------*/
void loop() {
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while(1);
    }
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // Animation frames
    const int num_frames = 5;
    const UBYTE* frames[num_frames] = { epd_bitmap_Mouth_eyes_3, epd_bitmap_Mouth_eyes_4, epd_bitmap_Mouth_eyes_5, epd_bitmap_Mouth_eyes_6, epd_bitmap_Mouth_eyes_7 }; // Add all frames you want to display

    // Initial time setup
    PAINT_TIME sPaint_time = {12, 34, 56};

    // Start animation loop
    // while (digitalRead(PIN_BUTTON) == HIGH) {  // Loop until the button is pressed (uncomment for button functionality)
    while (true) {  // Infinite loop without button press requirement
        for(int i = 0; i < num_frames; i++) {
            // Update time
            sPaint_time.Sec++;
            if (sPaint_time.Sec == 60) {
                sPaint_time.Sec = 0;
                sPaint_time.Min++;
                if (sPaint_time.Min == 60) {
                    sPaint_time.Min = 9;
                    sPaint_time.Hour++;
                    if (sPaint_time.Hour == 12) sPaint_time.Hour = 12;
                }
            }

            // Clear the entire screen for partial update
            Paint_Clear(WHITE);
            
            // Display animation frame
            Paint_DrawBitMap(frames[i]);
            
            // Draw time after the animation frame
            Paint_DrawTime(0, 0, &sPaint_time, &Font20, BLACK, WHITE);

            // Update the display
            EPD_1IN54_V2_DisplayPart(BlackImage);

            DEV_Delay_ms(90);  // Adjust delay for each frame
        }
    }

    // When the button is pressed, exit the loop and stop the animation (uncomment for button functionality)
    // Paint_Clear(WHITE);  // Optionally clear the screen
    // Paint_DrawTime(175, 65, &sPaint_time, &Font20, BLACK, WHITE);  // Display the final time
    // EPD_1IN54_V2_DisplayPart(BlackImage);

    free(BlackImage);
}
