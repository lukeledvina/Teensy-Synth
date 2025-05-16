#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* mainMenuItems[] = {"Osc A", "Osc B", "Filter", "Envelopes"};
int mainMenuIndex = 0;
const int mainMenuLength = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]); // Gets length by taking the memory size of each
int mainPageNumber = 0;
const int itemsPerPage = 3;



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

void loop() {
  long newEncoderPosition = myEncoder.read();
  long positionChange = newEncoderPosition - lastEncoderPosition;

  // code needs to be changed such that depending on if you are in a menu or in a parameter it properly updates that.
  if (positionChange >= 4) {
    lastEncoderPosition = newEncoderPosition;
    mainMenuIndex = (mainMenuIndex + 1) % mainMenuLength;
    updateMenu(mainPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
  } else if (positionChange <= -4) {
    lastEncoderPosition = newEncoderPosition;
    mainMenuIndex = (mainMenuIndex - 1 + mainMenuLength) % mainMenuLength;
    updateMenu(mainPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
  }

  if (digitalRead(encoderSwitch) == LOW) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE); // Reset text color so that when selecting options of multiples of 3, doesn't keep them highlighted
    display.setCursor(0,0);
    display.print("Selected: ");
    display.println(mainMenuItems[mainMenuIndex]);
    display.display();
    delay(500);
    while(digitalRead(encoderSwitch) == LOW);
  }
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

