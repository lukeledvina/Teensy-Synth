#include <Bounce.h>
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// GUItool: begin automatically generated code
AudioSynthWaveform       waveformA;      //xy=186.23333740234375,264
AudioMixer4              mixerRight;         //xy=417.23333740234375,348.23333740234375
AudioMixer4              mixerLeft;         //xy=422.23333740234375,228.23333740234375
AudioOutputI2S           i2s1;           //xy=674.2333374023438,257.23333740234375
AudioConnection          patchCord1(waveformA, 0, mixerLeft, 0);
AudioConnection          patchCord2(waveformA, 0, mixerRight, 0);
AudioConnection          patchCord3(mixerRight, 0, i2s1, 1);
AudioConnection          patchCord4(mixerLeft, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=420.23333740234375,466.23333740234375
// GUItool: end automatically generated code



// When ready to do each additional menu state, uncomment one at a time and do each individually, test and then move on
enum menuState {
  MAIN,
  OSC_A,
  // OSC_B,
  // FILTER,
  // AMP_ENV,
  // FILTER_ENV,
  WAVEFORM_A,
  PITCH_A,
  PULSE_WIDTH_A,
  OSC_A_ON_OFF
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

int oscAPitchOffset = 0;
float oscAPulseWidth = 0.5f;
bool oscAOn = true;

Encoder myEncoder(2, 3);
const int encoderSwitchPin = 15;
Bounce encoderSwitch = Bounce(encoderSwitchPin,20);
long lastEncoderPosition = 0;

Bounce returnButton = Bounce(4, 20);
const int returnButtonPin = 4;

float volume = 0.5f;

void setup() {

  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(volume);

  mixerLeft.gain(0, 0.8);
  mixerRight.gain(0, 0.8);

  pinMode(encoderSwitchPin, INPUT_PULLUP);
  pinMode(returnButtonPin, INPUT_PULLUP);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // SSD1306 allocation failed
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
  updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);

  waveformA.begin(WAVEFORM_SQUARE);  // Or SINE, SAWTOOTH
  waveformA.frequency(440);
  waveformA.amplitude(1);

  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
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

    case PITCH_A:
      oscAPitchOffset++;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Pitch shft");
      display.print(oscAPitchOffset);
      display.print(" st");
      display.display();
      break;

    case PULSE_WIDTH_A:
      oscAPulseWidth = min(oscAPulseWidth + 0.02f, 1.00f);
      //waveformA.pulseWidth(oscAPulseWidth);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Pulse Wdth");
      display.print(oscAPulseWidth);
      display.print(" duty");
      display.display();
      break;

    case OSC_A_ON_OFF:

      break;

    // case OSC_B:

    //   break;

    // case FILTER:

    //   break;

    // case AMP_ENV:

    //   break;

    // case FILTER_ENV:

    //   break;
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

    case PITCH_A:
      oscAPitchOffset--;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Pitch shft");
      display.print(oscAPitchOffset);
      display.print(" st");
      display.display();
      break;

    case PULSE_WIDTH_A:
      oscAPulseWidth = max(oscAPulseWidth - 0.02f, 0.00f);
      //waveformA.pulseWidth(oscAPulseWidth);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Pulse Wdth");
      display.print(oscAPulseWidth);
      display.print(" duty");
      display.display();
      break;

    case OSC_A_ON_OFF:

      break;

    // case OSC_B:

    //   break;

    // case FILTER:

    //   break;

    // case AMP_ENV:

    //   break;

    // case FILTER_ENV:

    //   break;
  }
}

void select() {
  switch (currentMenu) {
    case MAIN:
      if (mainMenuIndex == 0) {
        // Osc A
        currentMenu = OSC_A;
        oscAMenuPageNumber = 0;
        oscAMenuIndex = 0;
        display.setCursor(0,0);
        updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
      }

      break;

    case OSC_A:
      if (oscAMenuIndex == 0) {
        currentMenu = WAVEFORM_A;
        waveformMenuIndex = 0;
        waveformMenuPageNumber = 0;
        updateMenu(waveformMenuPageNumber, waveformMenuIndex, waveformMenuLength, waveformMenuItems);
      } else if (oscAMenuIndex == 1) {
        currentMenu = PITCH_A;
        // show the number representing pitch for osc A, turning the encoder will update it in semitones
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Pitch Shft");
        display.print(oscAPitchOffset);
        display.print(" st");
        display.display();
      } else if (oscAMenuIndex == 2) {
        currentMenu = PULSE_WIDTH_A;
        //waveformA.pulseWidth(oscAPulseWidth);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Pulse Wdth");
        display.print(oscAPulseWidth);
        display.print(" duty");
        display.display();
      }
      break;

// works, just need it to actually alter the waveforms now
    case WAVEFORM_A:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Selected:");
      if (waveformMenuIndex == 0){
        //waveformA.begin(WAVEFORM_SINE);
        display.println("Sine");
      } else if (waveformMenuIndex == 1) {
        //waveformA.begin(WAVEFORM_SAWTOOTH);
        display.println("Sawtooth");
      } else if (waveformMenuIndex == 2) {
        //waveformA.begin(WAVEFORM_SQUARE);
        display.println("Square");
      } else if (waveformMenuIndex == 3) {
        //waveformA.begin(WAVEFORM_TRIANGLE);
        display.println("Triangle");
      } else if (waveformMenuIndex == 4) {
        //waveformA.begin(WAVEFORM_PULSE);
        //waveformA.pulseWidth(oscAPulseWidth);
        display.println("Pulse");
      }
      display.display();
      break;

    
    case PITCH_A:
      // do nothing
      break;

    case PULSE_WIDTH_A:
      // do nothing
      break;

    case OSC_A_ON_OFF:
      // do nothing
      break;

    // case OSC_B:

    //   break;

    // case FILTER:

    //   break;

    // case AMP_ENV:

    //   break;

    // case FILTER_ENV:

    //   break;
  }
}

void goBack() {
  switch (currentMenu) {
    case MAIN:
      break;

    case OSC_A:
      display.setCursor(0,0);
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

    case PITCH_A:
      currentMenu = OSC_A;
      oscAMenuIndex = 0;
      oscAMenuPageNumber = 0;
      updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
      break;

    case PULSE_WIDTH_A:
      currentMenu = OSC_A;
      oscAMenuIndex = 0;
      oscAMenuPageNumber = 0;
      updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
      break;

    case OSC_A_ON_OFF:
      currentMenu = OSC_A;
      oscAMenuIndex = 0;
      oscAMenuPageNumber = 0;
      updateMenu(oscAMenuPageNumber, oscAMenuIndex, oscAMenuLength, oscAMenuItems);
      break;

    // case OSC_B:

    //   break;

    // case FILTER:

    //   break;

    // case AMP_ENV:

    //   break;

    // case FILTER_ENV:

    //   break;
  }
}

void loop() {
  usbMIDI.read();

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

void onNoteOn(byte channel, byte note, byte velocity) {
  float freqA = 440.0 * pow(2.0, (note + oscAPitchOffset - 69) / 12.0);
  float adjustedVelocity = float(velocity)/127.0f;
  waveformA.frequency(freqA);
  waveformA.amplitude(adjustedVelocity);
}

void onNoteOff(byte channel, byte note, byte velocity) {
  waveformA.amplitude(0);
}

