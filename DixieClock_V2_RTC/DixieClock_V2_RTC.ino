//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//   ________________________________________________________
//   R E A L   T I M E
//   ###   #  #   #  #  ####      ###  #      ##    ###  #  #
//   #  #  #   # #   #  #        #     #     #  #  #     # #
//   #  #  #    #    #  ###      #     #     #  #  #     ##
//   #  #  #   # #   #  #        #     #     #  #  #     # #
//   ###   #  #   #  #  ####      ###  ####   ##    ###  #  #
//   ________________________________________________________
//
//   Dixie Clock - Version 2.00 
//   M2theH - October 02, 2021
//   
//   Required hardware changes:
//   - Remove push button 2 (Pin A4) and push button 3 (Pin A5) and the associated resistors and capacitors.
//   - Connect the RTC-Shield-DS3231 to the Arduino via I2C (Pin A4->SDA and Pin A5->SCL).
//
//   Info: To use the RTC-Shield, an initialization of the time is necessary.  
//         Therefore the example "interactive" from the libary ds3231FS can be used. 
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


// __ Libraries ________________________________________________________________________________________________________________________________________________________

#include <Adafruit_GFX.h>                                         // Core graphics library
#include <Adafruit_ST7735.h>                                      // Hardware-specific library
#include <SdFat.h>                                                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>                                    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h>                                 // Image-reading functions
#include <TimerOne.h>                                             // Timer functions
#include <Wire.h>                                                 // I2C libary
#include <ds3231.h>                                               // DS3231 Real Time Clock libary "ds3231FS" 

// __ Pins ______________________________________________________________________________________________________________________________________________________________

#define SD_CS    10                                               // SD Card chip select; pin D10
#define TFT1_CS  9                                                // TFT No.1 chip select; pin D9
#define TFT2_CS  8                                                // TFT No.2 chip select; pin D8
#define TFT3_CS  7                                                // TFT No.3 chip select; pin D7
#define TFT4_CS  6                                                // TFT No.4 chip select; pin D6
#define TFT5_CS  5                                                // TFT No.5 chip select; pin D5
#define TFT1_RST  16                                              // TFT No.1 RESET; pin D16 (A2)
#define TFT2_RST  15                                              // TFT No.2 RESET; pin D15 (A1)
#define TFT3_RST  14                                              // TFT No.3 RESET; pin D14 (A0) 
#define TFT4_RST  4                                               // TFT No.4 RESET; pin D4
#define TFT5_RST  3                                               // TFT No.5 RESET; pin D3
#define TFT_DC   2                                                // TFT A0 display/command; pin D2

// __ Variables and Flags _______________________________________________________________________________________________________________________________________________

struct ts t;                                                      // ts is a struct findable in ds3231.h

int Count500ms = 0;                                               // Counter 500ms
int DoAction_500ms = 0;                                           // 1 = Do action every 500ms
int UpdateDisplayMin = 1;                                         // 1 = Update tubes - minutes
int UpdateDisplayHour = 1;                                        // 1 = Update tubes - hours

int SEC = 0;                                                      // Seconds
int MIN = 0;                                                      // Minutes
int HOUR = 0;                                                     // Hours

char DixieSec [10];                                               // Buffer for seconds
char DixieMin [10];                                               // Buffer for Minutes
char DixieMin1 [10];                                              // Buffer for time output tube 1
char DixieMin2 [10];                                              // Buffer for time output tube 2
char DixieHour [10];                                              // Buffer for Hours
char DixieHour1 [10];                                             // Buffer for time output tube 3
char DixieHour2 [10];                                             // Buffer for time output tube 4
char Endung [10] = ".bmp";                                        // File format of pictures on SD card

// __ Display and SD Card _______________________________________________________________________________________________________________________________________________

SdFat                SD;                                          // SD card filesystem
Adafruit_ImageReader reader(SD);                                  // Image-reader object, pass in SD filesystem

Adafruit_ST7735 tft1 = Adafruit_ST7735(TFT1_CS, TFT_DC, TFT1_RST);// display 1 Hours
Adafruit_ST7735 tft2 = Adafruit_ST7735(TFT2_CS, TFT_DC, TFT2_RST);// display 2 Hours
Adafruit_ST7735 tft3 = Adafruit_ST7735(TFT3_CS, TFT_DC, TFT3_RST);// display 3 Dot/Menu
Adafruit_ST7735 tft4 = Adafruit_ST7735(TFT4_CS, TFT_DC, TFT4_RST);// display 4 Minutes
Adafruit_ST7735 tft5 = Adafruit_ST7735(TFT5_CS, TFT_DC, TFT5_RST);// display 5 Minutes


// #####################################################################################################################################################################                     
// #### HW Setup #######################################################################################################################################################
// #####################################################################################################################################################################


void setup(void) 
{
  // __ Initialize I2C __________________________________________________________________________________________________________________________________________________

  Wire.begin();                                                   // Begin I2C bus 
  
  // __ Initialize Real Time Clock ______________________________________________________________________________________________________________________________________

  DS3231_init(DS3231_INTCN);                                      // Register the ds3231 (DS3231_INTCN is the default address of ds3231)
  
  // __ Initialize analog pins as digital input _________________________________________________________________________________________________________________________

  pinMode(A3,INPUT);                                              // Analog Pin 3 >> Digital 17 - Button
  digitalWrite(17,LOW);                                           // AActivate pulldown
  
  // __ Initialize serial communication _________________________________________________________________________________________________________________________________

  Serial.begin(9600);                                             // Initialize serial
  
  // __ Initialize SD-FAT Filesystem before using image reader functions ________________________________________________________________________________________________ 

  if(!SD.begin(SD_CS, SD_SCK_MHZ(10)))            
    { 
      Serial.println("SD Card ERROR");                            // Info message - initialization error
      for(;;);                                    
    }

  // __ Initialize Interrupt Timer1 _____________________________________________________________________________________________________________________________________

  Timer1.initialize(1000);                                        // Interrupt every 1ms (1000us)
  Timer1.attachInterrupt(Interrupt);                              // call interrupt function

  // __ Initialize Displays/Tubes _______________________________________________________________________________________________________________________________________
  
  tft1.initR(INITR_BLACKTAB);                                     // Initialize Display 1
  tft1.fillScreen(ST7735_BLACK);                                  // Fill display - color black
  
  tft2.initR(INITR_BLACKTAB);                                     // Initialize Display 2
  tft2.fillScreen(ST7735_BLACK);                                  // Fill display - color black
  
  tft3.initR(INITR_BLACKTAB);                                     // Initialize display 3
  tft3.fillScreen(ST7735_BLACK);                                  // Fill display - color black
  
  tft4.initR(INITR_BLACKTAB);                                     // Initialize Display 4
  tft4.fillScreen(ST7735_BLACK);                                  // Fill display - color black
  
  tft5.initR(INITR_BLACKTAB);                                     // Initialize display 5
  tft5.fillScreen(ST7735_BLACK);                                  // Fill display - color black

  reader.drawBMP("/DOT.bmp", tft3, 0, 0);                         // Load colon picture into display/tube

  Serial.println("Initialization OK");
}


// #####################################################################################################################################################################
// #### Interrupt function #############################################################################################################################################
// #####################################################################################################################################################################


void Interrupt(void)                                              // This function is called every 1ms              
{
  Count500ms++;

  if (Count500ms == 500)
    { 
      DoAction_500ms = 1;                                         // Flag to perform actions in main loop every 500ms
      Count500ms = 0;
    }
}


// #####################################################################################################################################################################
// #### Functions ######################################################################################################################################################
// #####################################################################################################################################################################


void ReadRTC(void)
{ 
  DS3231_get(&t);                                                 // Get the value and pass to the function the pointer to t
                                                                  
  SEC = t.sec;                                                    // Transfer value to variable - Seconds
  MIN = t.min;                                                    // Transfer value to variable - Minutes
  HOUR = t.hour;                                                  // Transfer value to variable - Hours

  if(SEC == 0)               { UpdateDisplayMin  = 1; }           // Check if a minute has passed
  if((SEC == 0)&&(MIN == 0)) { UpdateDisplayHour = 1; }           // Check if an hour has passed
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void BuildFilenameMinutes(void)
{ 
    sprintf(DixieMin,"%02u", MIN);
    sprintf(DixieMin1, "%c%s", DixieMin[0], Endung);              // Build file name on SD card D0X.bmp -> X = number
    sprintf(DixieMin2, "%c%s", DixieMin[1],Endung);               // Build file name on SD card D0X.bmp -> X = number
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void BuildFilenameHours(void)
{ 
    sprintf(DixieHour,"%02u", HOUR);
    sprintf(DixieHour1, "%c%s", DixieHour[0],Endung);             // Build file name on SD card D0X.bmp -> X = number
    sprintf(DixieHour2, "%c%s", DixieHour[1],Endung);             // Build file name on SD card D0X.bmp -> X = number
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void WriteTubeMinutes (void)                                      // Write actual time - minutes into displays/tubes
{
    reader.drawBMP(DixieMin1, tft4, 0, 0);                        // Load number/picture into the tube 4
    reader.drawBMP(DixieMin2, tft5, 0, 0);                        // Load number/picture into the tube 5
}   

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void WriteTubeHours (void)                                        // Write actual time - hours into displays/tubes
{
    reader.drawBMP(DixieHour1, tft1, 0, 0);                       // Load number/picture into the tube 1
    reader.drawBMP(DixieHour2, tft2, 0, 0);                       // Load number/picture into the tube 2
}


// #####################################################################################################################################################################
// #### Main function ##################################################################################################################################################
// #####################################################################################################################################################################


void loop() 
{

  if (DoAction_500ms == 1)                                        // Perform actions every 500ms
    {
      ReadRTC();                                                  // Get time from ds3231
      BuildFilenameHours();                                       // Build filenames of pictures on SD card - Hours
      BuildFilenameMinutes();                                     // Build filenames of pictures on SD card - Minutes
       
      DoAction_500ms = 0;   
    }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

  if (UpdateDisplayMin)                                           // This flag is set in fuction ReadRTC()
  {
    WriteTubeMinutes();                                           // Write actual minutes into displays/tubes

//    Serial.print("Time: ");                                       // Output time on serial
//    Serial.print(HOUR);                                           // Hours
//    Serial.print(":");                                            //
//    Serial.println(MIN);                                          // Minutes
    
    UpdateDisplayMin = 0;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

  if (UpdateDisplayHour)                                          // This flag is set in fuction ReadRTC()
  {   
    WriteTubeHours();                                             // Write actual hours into displays/tubes
   
    UpdateDisplayHour = 0;
  }

}
