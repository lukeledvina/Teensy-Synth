#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum menuState {
  MAIN,
  OSC_A,
  OSC_B,
  FILTER,
  AMP_ENV,
  FILTER_ENV
};

menuState currentMenu = MAIN;

const char* mainMenuItems[] = {"Osc A", "Osc B", "Amp Env", "Filter", "Filter Env"};
int mainMenuIndex = 0;
const int mainMenuLength = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]); // Gets length by taking the memory size of each
int mainPageNumber = 0;
const int itemsPerPage = 3;

const char* oscAItems[] = {"Waveform", "Pitch", "Pulse Width", "Osc On/Off"};
int oscAMenuIndex = 0;
const int oscAMenuLength = sizeof(oscAItems) / sizeof(oscAItems[0]);
int oscAPageNumber = 0;



Encoder myEncoder(2, 3);
const int encoderSwitch = 20;
long lastEncoderPosition = 0;

void setup() {
  pinMode(encoderSwitch, INPUT_PULLUP);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // SSD1306 allocation failed
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
}

// needs to be aware of proper menu and update everything accordingly
void forward() {
  switch (currentMenu) {
    case MAIN:
      mainMenuIndex = (mainMenuIndex + 1) % mainMenuLength;
      updateMenu(mainPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      Serial.println(mainMenuIndex);
      break;
    
    case OSC_A:
      oscAMenuIndex = (oscAMenuIndex + 1) % oscAMenuLength;
      updateMenu(oscAPageNumber, oscAMenuIndex, oscAMenuLength, oscAItems);
      Serial.println(oscAMenuIndex);
      break;

    case OSC_B:

      break;

    case FILTER:

      break;

    case AMP_ENV:

      break;

    case FILTER_ENV:

      break;
  }
}

// same as forward
void backward() {
  switch (currentMenu) {
    case MAIN:
      mainMenuIndex = (mainMenuIndex - 1) % mainMenuLength;
      updateMenu(mainPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;
    
    case OSC_A:
      oscAMenuIndex = (oscAMenuIndex - 1) % oscAMenuLength;
      updateMenu(oscAPageNumber, oscAMenuIndex, oscAMenuLength, oscAItems);
      break;

    case OSC_B:

      break;

    case FILTER:

      break;

    case AMP_ENV:

      break;

    case FILTER_ENV:

      break;
  }
}

void select() {
  switch (currentMenu) {
    case MAIN:
      if (mainMenuIndex == 0) {
        // Osc A
        display.setCursor(0,0);
        display.setTextColor(SSD1306_WHITE); // Reset text color so that when selecting options of multiples of 3, doesn't keep them highlighted
        updateMenu(oscAPageNumber, oscAMenuIndex, oscAMenuLength, oscAItems);
        currentMenu = OSC_A;

      }

      break;

    case OSC_A:

      break;

    case OSC_B:

      break;

    case FILTER:

      break;

    case AMP_ENV:

      break;

    case FILTER_ENV:

      break;
  }


}

void loop() {
  long newEncoderPosition = myEncoder.read();
  long positionChange = newEncoderPosition - lastEncoderPosition;

  // code needs to be changed such that depending on if you are in a menu or in a parameter it properly updates that.
  if (positionChange >= 4) {
    lastEncoderPosition = newEncoderPosition;
    // implement "forward" function
    forward();
  } else if (positionChange <= -4) {
    lastEncoderPosition = newEncoderPosition;
    // Implement "backward" function
    backward();
  }

  if (digitalRead(encoderSwitch) == LOW) {
    
    
    // Implement "select" function
    select();
    while(digitalRead(encoderSwitch) == LOW);
  }

  // Add back button functionality

}

// refactor this to work with more menus, instead of hardcoding it to work with the main menu, get it to work with submenus based on input parameters
void updateMenu(int pageNumber, int menuIndex, int menuLength, const char* menuItems[]) {
  display.clearDisplay();
  

  pageNumber = menuIndex/3;
  int totalPages = (menuLength + itemsPerPage - 1) / itemsPerPage;
  bool isLastPage = (pageNumber == totalPages - 1);
  
  int startIndex = pageNumber * itemsPerPage;
  int endIndex = isLastPage ? menuLength : startIndex + itemsPerPage;

  for (int i = startIndex; i < endIndex; i++) {
    if (i == (menuIndex)) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE); 
    }
    display.setCursor(0, (i - startIndex) * 20);
    display.println(menuItems[i]);
  }

  display.display();
}

