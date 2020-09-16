//-----------------------------------------------------------------
//
//   ###   #  #   #  #  ####      ###  #      ##    ###  #  #
//   #  #  #   # #   #  #        #     #     #  #  #     # #
//   #  #  #    #    #  ###      #     #     #  #  #     ##
//   #  #  #   # #   #  #        #     #     #  #  #     # #
//   ###   #  #   #  #  ####      ###  ####   ##    ###  #  #
//
//   Dixie Clock - Version 1
//   M2theH - July 25, 2020
//
//-----------------------------------------------------------------

#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ST7735.h>      // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include <TimerOne.h>             // Timer functions


// __ Pins ________________________________________________________

#define SD_CS    10               // SD Card chip select; pin D10

#define TFT1_CS  9                // TFT No.1 chip select; pin D9
#define TFT2_CS  8                // TFT No.2 chip select; pin D8
#define TFT3_CS  7                // TFT No.3 chip select; pin D7
#define TFT4_CS  6                // TFT No.4 chip select; pin D6
#define TFT5_CS  5                // TFT No.5 chip select; pin D5

#define TFT1_RST  16              // TFT No.1 RESET; pin D16 (A2)
#define TFT2_RST  15              // TFT No.2 RESET; pin D15 (A1)
#define TFT3_RST  14              // TFT No.3 RESET; pin D14 (A0) 
#define TFT4_RST  4               // TFT No.4 RESET; pin D4
#define TFT5_RST  3               // TFT No.5 RESET; pin D3

#define TFT_DC   2                // TFT A0 display/command; pin D2


// __ Variables and Flags _________________________________________

int UpdateDisplayMin = 0;         // 1 = Write actual time - minutes
int UpdateDisplayHour = 0;        // 1 = Write actual time - hours
int DoAction_250ms = 0;           // 1 = Do action every 250ms
int DoAction_1s = 0;              // 1 = Do action every 1s
int ClockMode = 0;                // 1 = Start displaying time / 0 = Stop displaying time (after boot or during setup mode)
int SetUp = 0;                    // 1 = Call setup menu 
int ClearScreen = 0;              // 1 = Clar displays/tubes / black screen
int WritePicture = 1;             // 1 = Load pictures in displays/tubes 

int PictureType = 0;              // Select picture setup 

int Count250ms = 0;               // Counter 250ms
int Count1s = 0;                  // Counter 1s
int TimeSec = 0;                  // Clock counter seconds
int TimeMin = 0;                  // Clock counter minutes
int TimeHou = 0;                  // Clock counter hours

char DixieSec [10];               // Buffer for seconds
char DixieMin [10];               // Buffer for minutes
char DixieMin1 [10];              // Buffer for time output tube 1
char DixieMin2 [10];              // Buffer for time output tube 2
char DixieHour [10];              // Buffer for hours
char DixieHour1 [10];             // Buffer for time output tube 3
char DixieHour2 [10];             // Buffer for time output tube 4
char Endung [10] = ".bmp";        // File format of pictures on SD card
char Time[10];                    // Buffer for time output


// __ Display and SD Card _________________________________________

SdFat                SD;          // SD card filesystem
Adafruit_ImageReader reader(SD);  // Image-reader object, pass in SD filesystem

Adafruit_ST7735      tft1    = Adafruit_ST7735(TFT1_CS, TFT_DC, TFT1_RST); // display 1 Hours
Adafruit_ST7735      tft2    = Adafruit_ST7735(TFT2_CS, TFT_DC, TFT2_RST); // display 2 Hours
Adafruit_ST7735      tft3    = Adafruit_ST7735(TFT3_CS, TFT_DC, TFT3_RST); // display 3 Colon/Menu
Adafruit_ST7735      tft4    = Adafruit_ST7735(TFT4_CS, TFT_DC, TFT4_RST); // display 4 Minutes
Adafruit_ST7735      tft5    = Adafruit_ST7735(TFT5_CS, TFT_DC, TFT5_RST); // display 5 Minutes

                     
// ## HW Setup ####################################################

void setup(void) 
{
  // __ Initialize analog pins as digital input ___________________

  pinMode(A5,INPUT);                                // Analog Pin 5 >> Digital 19 - Setup Button
  digitalWrite(19,LOW);                             // Pulldown
  pinMode(A4,INPUT);                                // Analog Pin 4 >> Digital 18 - Hour/Min ++ Button
  digitalWrite(18,LOW);                             // Pulldown
  pinMode(A3,INPUT);                                // Analog Pin 3 >> Digital 17 - Hour/Min -- Button
  digitalWrite(17,LOW);                             // Pulldown
  
     
  // __ Initialize serial communication ___________________________

  Serial.begin(9600);                               // Initialize serial
  Serial.println("Init Serial... OK!");             // Info message - initialization Ok

  
  // __ Initialize SD-FAT Filesystem before using image reader functions 

  Serial.print("Init Filesystem... ");
  if(!SD.begin(SD_CS, SD_SCK_MHZ(10)))            
    { 
      Serial.println("SD Card ERROR");              // Info message - initialization error
      for(;;);                                    
    }
  Serial.println("OK!");                            // Info message - initialization OK
  

  // __ Initialize Interrupt Timer1 ______________________________

  Timer1.initialize(1000);                          // Interrupt every 1ms (1000)
  Timer1.attachInterrupt(Interrupt);                // Call interrupt function


  // __ Initialize Displays/Tubes ________________________________
  
  Serial.print("Init Tubes 1 to 5...");             // Info message - start initialization 
  tft1.initR(INITR_BLACKTAB);                       // Initialize Display 1
  tft1.fillScreen(ST7735_BLACK);                    // Fill display color black
  
  tft2.initR(INITR_BLACKTAB);                       // Initialize Display 2
  tft2.fillScreen(ST7735_BLACK);                    // Fill display color black
  
  tft3.initR(INITR_BLACKTAB);                       // Initialize display 3
  tft3.fillScreen(ST7735_BLACK);                    // Fill display color black
  
  tft4.initR(INITR_BLACKTAB);                       // Initialize Display 4
  tft4.fillScreen(ST7735_BLACK);                    // Fill display color black
  
  tft5.initR(INITR_BLACKTAB);                       // Initialize display 5
  tft5.fillScreen(ST7735_BLACK);                    // Fill display color black
  Serial.println("OK!");                            // Info message - initialization OK
}


// ## Interrupt function every 1ms ###############################

void Interrupt(void)                  
{
  Count250ms ++;
  Count1s ++;
  
  if (Count250ms >= 250)
    { 
      DoAction_250ms = 1;
      Count250ms = 0;
    }

  if (Count1s >= 1000)
    { 
      DoAction_1s = 1;
      Count1s = 0;

      TimeSec ++;
      if (TimeSec >= 60)
      {
        TimeSec = 0;
        TimeMin ++;
        UpdateDisplayMin = 1;
        if (TimeMin >= 60)
        {
          TimeMin = 0;
          TimeHou ++;
          UpdateDisplayHour = 1;
          if(TimeHou >= 24)
          {
            TimeHou = 0;
          }
        }
      }
    }
}


// ## Functions ##################################################

void PictureTubes (void)                            // Load pictures into the tubes after turning on the clock
{
  while(WritePicture)
  {
    if(PictureType == 0)                            
    {
      reader.drawBMP("/M0.bmp", tft1, 0, 60);       // Load dash picture into display/tube
      reader.drawBMP("/M0.bmp", tft2, 0, 60);       // Load dash picture into display/tube
      reader.drawBMP("/DOT.bmp", tft3, 0, 0);       // Load colon picture into display/tube
      reader.drawBMP("/M0.bmp", tft4, 0, 60);       // Load dash picture into display/tube
      reader.drawBMP("/M0.bmp", tft5, 0, 60);       // Load dash picture into display/tube
    }
    WritePicture = 0;
  }
}

void BuildTime (void)                                 // Write actual time in variable
{
    sprintf(Time,"%02u:%02u:%02u",TimeHou, TimeMin, TimeSec);       
}


// ---------------------------------------------------------------

void SerialOut (void)                                 // Output time on serial
{
  Serial.print("Time: ");         
  Serial.println(Time);           
}


// ---------------------------------------------------------------

void ReadButton (void)                                // Check Buttons if they are pressed
{
    if (digitalRead(19))                              // Pin A5 (D19) >> Button menu
    {
      SetUp ++;
      ClearScreen = 1;
      ClockMode = 0;
    }
    
    if (digitalRead(18))                              // Pin A4 (D18) >> Button ++
    {
      if (SetUp == 1)
      {
        TimeMin++;
        if(TimeMin >= 60)
        {
          TimeMin = 0;
        }
      }
      if (SetUp == 2)
      {
        TimeHou++;
        if(TimeHou >= 24)
        {
          TimeHou = 0;
        }
      }
    }
 
    if (digitalRead(17))                              // Pin A3 (D17) >> Button --
    {
      if (SetUp == 1)
      {
        TimeMin--;
        if (TimeMin <= -1)
        {
          TimeMin = 59;
        }
      }
      if (SetUp == 2)
      {
        TimeHou--;
        if (TimeHou <= -1)
        {
          TimeHou = 23;
        }
      }
    }
}


// ---------------------------------------------------------------

void ClearTubes (void)                      // Clear all tubes; Load menu pictures into display/tube 3
{
  while(ClearScreen)
  {
    tft1.fillScreen(ST7735_BLACK);          // Fill display/tube 1 - color black
    tft2.fillScreen(ST7735_BLACK);          // Fill display/tube 2 - color black
    tft3.fillScreen(ST7735_BLACK);          // Fill display/tube 3 - color black
    tft4.fillScreen(ST7735_BLACK);          // Fill display/tube 4 - color black
    tft5.fillScreen(ST7735_BLACK);          // Fill display/tube 5 - color black
    
    if(SetUp == 1)
    {
      Serial.println("Enter Setup Menu - Minutes.....");  // Info message - enter setup minutes
      reader.drawBMP("/M0.bmp", tft1, 0, 60);             // Load dash picture into display/tube
      reader.drawBMP("/M0.bmp", tft2, 0, 60);             // Load dash picture into display/tube
      reader.drawBMP("/MSM.bmp", tft3, 0, 0);             // Load menu picture into display/tube 
      reader.drawBMP("/M0.bmp", tft4, 0, 60);             // Load dash picture into display/tube
      reader.drawBMP("/M0.bmp", tft5, 0, 60);             // Load dash picture into display/tube
    }
    
    if(SetUp == 2)
    {
      Serial.println("Enter Setup Menu - Hours.....");    // Info message - enter setup hours
      reader.drawBMP("/M0.bmp", tft1, 0, 60);             // Load dash picture into display/tube
      reader.drawBMP("/M0.bmp", tft2, 0, 60);             // Load dash picture into display/tube
      reader.drawBMP("/MSH.bmp", tft3, 0, 0);             // Load menu picture into display/tube
      reader.drawBMP("/M0.bmp", tft4, 0, 60);             // Load dash picture into display/tube
      reader.drawBMP("/M0.bmp", tft5, 0, 60);             // Load dash picture into display/tube
    }
    
    ClearScreen = 0;        
  }
}


// ---------------------------------------------------------------

void TimeSetup (void)                               // Write actual time into display/tube 3 during setup
{
  if((SetUp == 1)&!(ClearScreen))                   // Setup minutes
  {
     tft3.setTextColor(ST7735_WHITE, ST7735_BLACK);
     tft3.setCursor(40, 100);
     tft3.print(Time);
  }

  if((SetUp == 2)&!(ClearScreen))                   // Setup hours
  {
     tft3.setTextColor(ST7735_WHITE, ST7735_BLACK);
     tft3.setCursor(40, 100);
     tft3.print(Time);
  }

  if(SetUp == 3)                                    // Leave setup menu
  {
      Serial.println("Exit Setup Menu; Start Clock Mode....."); // Info message - exit setup
      reader.drawBMP("DOT.bmp", tft3, 0, 0);        // Load colon/picture into tube 3
      UpdateDisplayMin = 1;                         // Write actual minutes flag
      UpdateDisplayHour = 1;                        // Write actual hours flag
      ClockMode = 1;                                // Start clock mode
      ClearScreen = 0;                              // Cancel clear screen (set by pressing menu button)
      SetUp = 0;                                    // Leave setup menu; Enter clock mode 
  }
}


// ---------------------------------------------------------------

void UpdateTubeMin (void)                               // Write actual time - minutes into displays/tubes
{
    sprintf(DixieMin,"%02u", TimeMin);
    sprintf(DixieMin1, "%c%s", DixieMin[0],Endung);     // build file name on SD card D0X.bmp -> X = number
    sprintf(DixieMin2, "%c%s", DixieMin[1],Endung);     // build file name on SD card D0X.bmp -> X = number
    
    reader.drawBMP(DixieMin1, tft4, 0, 0);              // Load number/picture into the tube 4
    reader.drawBMP(DixieMin2, tft5, 0, 0);              // Load number/picture into the tube 5
}   


// ---------------------------------------------------------------

void UpdateTubeHour (void)                              // Write actual time - hours into displays/tubes
{
    sprintf(DixieHour,"%02u", TimeHou);
    sprintf(DixieHour1, "%c%s", DixieHour[0],Endung);   // build file name on SD card D0X.bmp -> X = number
    sprintf(DixieHour2, "%c%s", DixieHour[1],Endung);   // build file name on SD card D0X.bmp -> X = number
    
    reader.drawBMP(DixieHour1, tft1, 0, 0);             // Load number/picture into the tube 1
    reader.drawBMP(DixieHour2, tft2, 0, 0);             // Load number/picture into the tube 2
}


// ## Main function ##############################################

void loop() 
{
  
  if (DoAction_250ms == 1)
  {
    ReadButton();         // Check buttons if they are pressed
    if (SetUp != 0 )
    {
      BuildTime();        // Write actual time in variable          
    }
    TimeSetup();          // Menu - setup time
    
    DoAction_250ms = 0;   
  }

//-----------------------------------------------

  if (DoAction_1s == 1)
  {

    if(SetUp == 0)
    {
      BuildTime();          // Write actual time in variable
      SerialOut();          // Output serial data    
    }
                                     
    DoAction_1s = 0;      
  }

//-----------------------------------------------

  if (UpdateDisplayMin)     // Write actual minutes into displays/tubes
  {
    if(ClockMode)
    {
      UpdateTubeMin();
    }
    UpdateDisplayMin = 0;
  }

//----------------------------------------------- 

  if (UpdateDisplayHour)  // Write actual hours into displays/tubes
  {   
    if(ClockMode)
    {
      UpdateTubeHour();
    }
    UpdateDisplayHour = 0;
  }
  
//----------------------------------------------- 

  if (ClearScreen)        // Clear displays/tubes
  {
    ClearTubes();
  }

//----------------------------------------------- 

  if (WritePicture)       // Load pictures into displays/tubes
  {
    PictureTubes();
  }
}
