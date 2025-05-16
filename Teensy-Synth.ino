#include <Bounce.h>
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
  FILTER_ENV,
  WAVEFORM_A,
  PITCH_A,
  PULSE_WIDTH_A,
  OSC_A_ON
};

menuState currentMenu = MAIN;

const char* mainMenuItems[] = {"Osc A", "Osc B", "Amp Env", "Filter", "Filter Env"};
int mainMenuIndex = 0;
const int mainMenuLength = sizeof(mainMenuItems) / 4;
int mainMenuPageNumber = 0;
const int itemsPerPage = 3;

const char* oscAMenuItems[] = {"Waveform", "Pitch", "Pulse Wdth", "Osc On/Off"};
int oscAMenuIndex = 0;
const int oscAMenuLength = sizeof(oscAMenuItems) / 4;
int oscAMenuPageNumber = 0;

const char* waveformMenuItems[] = {"Sine", "Sawtooth", "Square", "Triangle", "Pulse"};
int waveformMenuIndex = 0;
const int waveformMenuLength = sizeof(waveformMenuItems) / 4;
int waveformMenuPageNumber = 0;

Encoder myEncoder(2, 3);
const int encoderSwitchPin = 20;
Bounce encoderSwitch = Bounce(20,10);
long lastEncoderPosition = 0;

Bounce returnButton = Bounce(4, 10);
const int returnButtonPin = 4;

void setup() {
  pinMode(encoderSwitchPin, INPUT_PULLUP);
  pinMode(returnButtonPin, INPUT_PULLUP);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // SSD1306 allocation failed
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
  updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
}

// needs to be aware of proper menu and update everything accordingly
void forward() {
  switch (currentMenu) {
    case MAIN:
      mainMenuIndex = (mainMenuIndex + 1) % mainMenuLength;
      updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;
    
    case OSC_A:
      oscAMenuIndex = (oscAMenuIndex + 1) % oscAMenuLength;
      updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
      break;

    case WAVEFORM_A:
      waveformMenuIndex = (waveformMenuIndex + 1) % waveformMenuLength;
      updateMenu(waveformMenuPageNumber, waveformMenuIndex, waveformMenuLength, waveformMenuItems);
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
      updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;
    
    case OSC_A:
      oscAMenuIndex = (oscAMenuIndex - 1) % oscAMenuLength;
      updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
      break;

    case WAVEFORM_A:
      waveformMenuIndex = (waveformMenuIndex - 1) % waveformMenuLength;
      updateMenu(waveformMenuPageNumber, waveformMenuIndex, waveformMenuLength, waveformMenuItems);
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
        updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
        currentMenu = OSC_A;

      }

      break;

    case OSC_A:
      if (oscAMenuIndex == 0) {
        display.setCursor(0,0);
        currentMenu = WAVEFORM_A;
        updateMenu(waveformMenuPageNumber, waveformMenuIndex, waveformMenuLength, waveformMenuItems);
      }
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

void goBack() {
  switch (currentMenu) {
    case MAIN:
      break;

    case OSC_A:
      // display.setCursor(0,0);
      currentMenu = MAIN;
      mainMenuIndex = 0;
      mainMenuPageNumber = 0;
      updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;
    
    case WAVEFORM_A:
      currentMenu = OSC_A;
      oscAMenuIndex = 0;
      oscAMenuPageNumber = 0;
      updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
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
  encoderSwitch.update();

  returnButton.update();

  if (positionChange >= 4) {
    lastEncoderPosition = newEncoderPosition;
    forward();
  } else if (positionChange <= -4) {
    lastEncoderPosition = newEncoderPosition;
    backward();
  }

  if (encoderSwitch.fallingEdge()) {
    select();
  }

  // Add back button functionality
  if(returnButton.fallingEdge()) {
    Serial.println("button pressed");
    goBack();
  }

}

void updateMenu(int pageNumber, int& menuIndex, int menuLength, const char* menuItems[]) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Reset text color so that when selecting options of multiples of 3, doesn't keep them highlighted

  if (menuIndex < 0) menuIndex = menuLength - 1;

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

