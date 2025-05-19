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
AudioSynthWaveform       waveformA;      //xy=69.23333740234375,220
AudioFilterStateVariable filter1;        //xy=333.9833679199219,135.2333221435547
AudioMixer4              filterTypeMixer;         //xy=500.4833679199219,258.98333740234375
AudioEffectEnvelope      ampEnvelope;      //xy=705.4833984375,258.73333740234375
AudioAmplifier           amp1;           //xy=871.7333374023438,258.73333740234375
AudioMixer4              mixerLeft;         //xy=982.4832763671875,119.23332977294922
AudioMixer4              mixerRight;         //xy=998.4832763671875,396.23333740234375
AudioOutputI2S           i2s1;           //xy=1041.4832763671875,262.23333740234375
AudioConnection          patchCord1(waveformA, 0, filter1, 0);
AudioConnection          patchCord2(waveformA, 0, filterTypeMixer, 3);
AudioConnection          patchCord3(filter1, 0, filterTypeMixer, 0);
AudioConnection          patchCord4(filter1, 1, filterTypeMixer, 1);
AudioConnection          patchCord5(filter1, 2, filterTypeMixer, 2);
AudioConnection          patchCord6(filterTypeMixer, ampEnvelope);
AudioConnection          patchCord7(ampEnvelope, amp1);
AudioConnection          patchCord8(amp1, 0, mixerLeft, 0);
AudioConnection          patchCord9(amp1, 0, mixerRight, 0);
AudioConnection          patchCord10(mixerLeft, 0, i2s1, 0);
AudioConnection          patchCord11(mixerRight, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=482.23333740234375,704.2333374023438
// GUItool: end automatically generated code




// When ready to do each additional menu state, uncomment one at a time and do each individually, test and then move on
enum menuState {
  MAIN,
  OSC_A,
  // OSC_B,
  FILTER,
  FILTER_TYPE,
  FILTER_CUTOFF,
  FILTER_RESONANCE,
  FILTER_ON_OFF,
  AMP_ENV,
  AMP_ATTACK,
  AMP_DECAY,
  AMP_SUSTAIN,
  AMP_RELEASE,
  AMP_AMOUNT,
  // FILTER_ENV,
  WAVEFORM_A,
  PITCH_A,
  PULSE_WIDTH_A,
  OSC_A_ON_OFF
};

menuState currentMenu = MAIN;

const char* mainMenuItems[] = {"Osc A", "Osc B", "Filter", "Amp Env", "Filter Env", "Volume", "Mono/Poly"};
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

const char* filterMenuItems[] = {"Type", "Cutoff", "Resonance", "Ftr On/Off"};
int filterMenuIndex = 0;
const int filterMenuLength = sizeof(filterMenuItems) / 4;
int filterMenuPageNumber = 0;

const char* filterTypeMenuItems[] = {"Low Pass", "Band Pass", "High Pass"};
int filterTypeMenuIndex = 0;
const int filterTypeMenuLength = sizeof(filterTypeMenuItems) / 4;
int filterTypeMenuPageNumber = 0;

const char* ampEnvelopeMenuItems[] = {"Attack", "Decay", "Sustain", "Release", "Amount"};
int ampEnvelopeMenuIndex = 0;
const int ampEnvelopeMenuLength = sizeof(ampEnvelopeMenuItems) / 4;
int ampEnvelopeMenuPageNumber = 0;

int oscAPitchOffset = 0;
float oscAPulseWidth = 0.5f;
bool oscAOn = true;

Encoder myEncoder(2, 3);
const int encoderSwitchPin = 15;
Bounce encoderSwitch = Bounce(encoderSwitchPin,20);
long lastEncoderPosition = 0;

Bounce returnButton = Bounce(4, 20);
const int returnButtonPin = 4;

int oscAWaveform = WAVEFORM_SINE;

float oscAGainLeft = 0.5f;
float oscAGainRight = 0.5f;
float volume = 0.5f;

int minCutoffFreq = 20;
int maxCutoffFreq = 15000;
int filterCutoffFreq = maxCutoffFreq;
float filterControl = 1.0f;
float filterCutoffCurve = 2.0f;

float filterResonance = 0.0f;
float minResonance = 0.7f;
float maxResonance = 5.0f;

bool filterOn = false;

// Defaults
float ampAttack = 5.0f; //ms
float ampDecay = 200.0f; //ms
float ampSustain = 1.0f; //percentage
float ampRelease = 650.0f; //ms
float ampAmount = 1.0f; //percentage?


const float mixAlpha = 0.8f;
int ampAttackEnvelopeStep = 0;
const int envelopeStepsMax = 100;

const float minEnvelopeAmount = 5.0f;
const float maxEnvelopeAmount = 15000.0f;
float envelopeCurveAmount = 2.0f;

int ampDecayEnvelopeStep = 0;

enum filterType {
  LOW_PASS,
  BAND_PASS,
  HIGH_PASS
};

filterType currentFilterType = LOW_PASS;



void setup() {
  AudioMemory(32);
  sgtl5000_1.enable();
  sgtl5000_1.volume(volume);

  waveformA.begin(WAVEFORM_PULSE);
  waveformA.pulseWidth(0.5f);

  mixerLeft.gain(0, oscAGainLeft);
  mixerRight.gain(0, oscAGainRight);

  filterTypeMixer.gain(0, 0);
  filterTypeMixer.gain(1, 0);
  filterTypeMixer.gain(2, 0);
  filterTypeMixer.gain(3, 1);

  // Do not plan on using these parameters for my envelopes
  ampEnvelope.delay(0);
  ampEnvelope.hold(0);

  ampEnvelope.attack(ampAttack);
  ampEnvelope.decay(ampDecay);
  ampEnvelope.sustain(ampSustain);
  ampEnvelope.release(ampRelease);

  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  
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

// Gets ran when encoder is rotated clockwise
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
      applyPitchShiftA();
      break;

    case PULSE_WIDTH_A:
      oscAPulseWidth = min(oscAPulseWidth + 0.02f, 1.00f);
      applyPulseWidthA();
      break;

    case OSC_A_ON_OFF:

      break;

    // case OSC_B:

    //   break;

    case FILTER:
      filterMenuIndex = (filterMenuIndex + 1) % filterMenuLength;
      updateMenu(filterMenuPageNumber, filterMenuIndex, filterMenuLength, filterMenuItems);
      break;

    case FILTER_TYPE:
      filterTypeMenuIndex = (filterTypeMenuIndex + 1) % filterTypeMenuLength;
      updateMenu(filterTypeMenuPageNumber, filterTypeMenuIndex, filterTypeMenuLength, filterTypeMenuItems);
      break;

    case FILTER_CUTOFF:
      filterControl = min(filterControl + 0.01f, 1.0f);
      applyFilterCutoff();
      break;

    case FILTER_RESONANCE:
      filterResonance = min(1.0f, filterResonance + 0.02f);
      applyFilterResonance();
      break;

    case FILTER_ON_OFF:
      // do nothing
      break;

    case AMP_ENV:
      ampEnvelopeMenuIndex = (ampEnvelopeMenuIndex + 1) % ampEnvelopeMenuLength;
      updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
      break;
    
    case AMP_ATTACK:
      ampAttackEnvelopeStep = min(ampAttackEnvelopeStep + 1, envelopeStepsMax);
      applyAmpAttack();
      break;

    case AMP_DECAY:
      ampDecayEnvelopeStep = min(ampDecayEnvelopeStep + 1, envelopeStepsMax);
      applyAmpDecay();
      break;

    case AMP_SUSTAIN:
      ampSustain = min(ampSustain + 0.02f, 1.00f);
      applyAmpSustain();
      break;

    case AMP_RELEASE:

      break;

    case AMP_AMOUNT:

      break;

    // case FILTER_ENV:

    //   break;
  }
}

// Gets ran when encoder is rotated counter clockwise
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
      applyPitchShiftA();
      break;

    case PULSE_WIDTH_A:
      oscAPulseWidth = max(oscAPulseWidth - 0.02f, 0.00f);
      applyPulseWidthA();
      break;

    case OSC_A_ON_OFF:
      // do nothing
      break;

    // case OSC_B:

    //   break;

    case FILTER:
      filterMenuIndex = (filterMenuIndex - 1) % filterMenuLength;
      updateMenu(filterMenuPageNumber, filterMenuIndex, filterMenuLength, filterMenuItems);
      break;

    case FILTER_TYPE:
      filterTypeMenuIndex = (filterTypeMenuIndex - 1) % filterTypeMenuLength;
      updateMenu(filterTypeMenuPageNumber, filterTypeMenuIndex, filterTypeMenuLength, filterTypeMenuItems);
      break;

    case FILTER_CUTOFF:
      filterControl = max(filterControl - 0.01f, 0.0f);
      applyFilterCutoff();
      break;

    case FILTER_RESONANCE:
      filterResonance = max(0.0f, filterResonance - 0.02f);
      applyFilterResonance();
      break;

    case FILTER_ON_OFF:
      // do nothing
      break;

    case AMP_ENV:
      ampEnvelopeMenuIndex = (ampEnvelopeMenuIndex - 1) % ampEnvelopeMenuLength;
      updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
      break;
    
    case AMP_ATTACK:
      ampAttackEnvelopeStep = max(ampAttackEnvelopeStep - 1, 0.00f);
      applyAmpAttack();
      break;

    case AMP_DECAY:
      ampDecayEnvelopeStep = max(ampDecayEnvelopeStep - 1, 0.00f);
      applyAmpDecay();
      break;

    case AMP_SUSTAIN:
      ampSustain = max(ampSustain - 0.02f, 0.00f);
      applyAmpSustain();
      break;

    case AMP_RELEASE:

      break;

    case AMP_AMOUNT:

      break;

    // case FILTER_ENV:

    //   break;
  }
}

// Gets ran when encoder is pressed down
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
      } else if (mainMenuIndex == 1) {

      } else if (mainMenuIndex == 2) {
        currentMenu = FILTER;
        filterMenuPageNumber = 0;
        filterMenuIndex = 0;
        display.setCursor(0,0);
        updateMenu(filterMenuPageNumber, filterMenuIndex, filterMenuLength, filterMenuItems);
      } else if (mainMenuIndex == 3) {
        currentMenu = AMP_ENV;
        ampEnvelopeMenuPageNumber = 0;
        ampEnvelopeMenuIndex = 0;
        display.setCursor(0,0);
        updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
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
        applyPitchShiftA();
      } else if (oscAMenuIndex == 2) {
        currentMenu = PULSE_WIDTH_A;
        applyPulseWidthA();
        waveformA.pulseWidth(oscAPulseWidth);
      } else if (oscAMenuIndex == 3) {
        oscAOn = !oscAOn;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_WHITE);
        if(oscAOn) {
          display.println("Osc A On");
          mixerLeft.gain(0, oscAGainLeft);
          mixerRight.gain(0, oscAGainRight);
        } else {
          display.println("Osc A Off");
          mixerLeft.gain(0, 0);
          mixerRight.gain(0, 0);
        }
        display.display();
      }
      break;

    case WAVEFORM_A:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Selected:");
      if (waveformMenuIndex == 0){
        oscAWaveform = WAVEFORM_SINE;
        display.println("Sine");
      } else if (waveformMenuIndex == 1) {
        oscAWaveform = WAVEFORM_SAWTOOTH;
        display.println("Sawtooth");
      } else if (waveformMenuIndex == 2) {
        oscAWaveform = WAVEFORM_SQUARE;
        display.println("Square");
      } else if (waveformMenuIndex == 3) {
        oscAWaveform = WAVEFORM_TRIANGLE;
        display.println("Triangle");
      } else if (waveformMenuIndex == 4) {
        oscAWaveform = WAVEFORM_PULSE;
        display.println("Pulse");
      }
      waveformA.begin(oscAWaveform);
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

    case FILTER:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      if (filterMenuIndex == 0) {
        //filter type
        currentMenu = FILTER_TYPE;
        filterTypeMenuPageNumber = 0;
        filterTypeMenuIndex = 0;
        display.setCursor(0,0);
        updateMenu(filterTypeMenuPageNumber, filterTypeMenuIndex, filterTypeMenuLength, filterTypeMenuItems);
      } else if (filterMenuIndex == 1) {
        //filter cutoff
        currentMenu = FILTER_CUTOFF;
        applyFilterCutoff();
      } else if (filterMenuIndex == 2) {
        //filter resonance
        currentMenu = FILTER_RESONANCE;
        applyFilterResonance();
      } else if (filterMenuIndex == 3) {
        filterOn = !filterOn;
        if (filterOn) {
          display.println("Filter On");
          filter1.frequency(filterCutoffFreq);
          switch (currentFilterType) {
            case LOW_PASS:
              filterTypeMixer.gain(0, 1);
              filterTypeMixer.gain(1, 0);
              filterTypeMixer.gain(2, 0);
              filterTypeMixer.gain(3, 0);
              break;

            case BAND_PASS:
              filterTypeMixer.gain(0, 0);
              filterTypeMixer.gain(1, 1);
              filterTypeMixer.gain(2, 0);
              filterTypeMixer.gain(3, 0);
              break;

            case HIGH_PASS:
              filterTypeMixer.gain(0, 0);
              filterTypeMixer.gain(1, 0);
              filterTypeMixer.gain(2, 1);  
              filterTypeMixer.gain(3, 0);                 
              break;       
          }

        } else {
          display.println("Filter Off");
          filterTypeMixer.gain(0, 0);
          filterTypeMixer.gain(1, 0);
          filterTypeMixer.gain(2, 0);
          filterTypeMixer.gain(3, 1);
        }
      }
      display.display();
      break;

    case FILTER_TYPE:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Selected: ");
      // filter1.frequency(filterCutoffFreq);
      if (filterTypeMenuIndex == 0) {
        currentFilterType = LOW_PASS;
        display.println("Low Pass");
        filterTypeMixer.gain(0, 1);
        filterTypeMixer.gain(1, 0);
        filterTypeMixer.gain(2, 0);
      } else if (filterTypeMenuIndex == 1) {
        currentFilterType = BAND_PASS;
        display.println("Band Pass");
        filterTypeMixer.gain(0, 0);
        filterTypeMixer.gain(1, 1);
        filterTypeMixer.gain(2, 0);
      } else if (filterTypeMenuIndex == 2) {
        currentFilterType = HIGH_PASS;
        display.println("High Pass");
        filterTypeMixer.gain(0, 0);
        filterTypeMixer.gain(1, 0);
        filterTypeMixer.gain(2, 1);
      }
      display.display();
      break;

    case FILTER_CUTOFF:
      // do nothing
      break;

    case FILTER_RESONANCE:
      // do nothing
      break;

    case FILTER_ON_OFF:
      // do nothing
      break;

    case AMP_ENV:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      if (ampEnvelopeMenuIndex == 0) {
        currentMenu = AMP_ATTACK;
        applyAmpAttack();
      } else if (ampEnvelopeMenuIndex == 1) {
        currentMenu = AMP_DECAY;
        applyAmpDecay();
      } else if (ampEnvelopeMenuIndex == 2) {
        currentMenu = AMP_SUSTAIN;
        applyAmpSustain();
      }

      display.display();
      break;
    
    case AMP_ATTACK:

      break;

    case AMP_DECAY:

      break;

    case AMP_SUSTAIN:

      break;

    case AMP_RELEASE:

      break;

    case AMP_AMOUNT:

      break;

    // case FILTER_ENV:

    //   break;
  }
}

// Gets ran when pushbutton is pressed down
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

    case FILTER:
      currentMenu = MAIN;
      mainMenuIndex = 0;
      mainMenuPageNumber = 0;
      updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;

    case FILTER_TYPE:
      currentMenu = FILTER;
      filterMenuIndex = 0;
      filterMenuPageNumber = 0;
      updateMenu(filterMenuPageNumber, filterMenuIndex, filterMenuLength, filterMenuItems);
      break;

    case FILTER_CUTOFF:
      currentMenu = FILTER;
      filterMenuIndex = 0;
      filterMenuPageNumber = 0;
      updateMenu(filterMenuPageNumber, filterMenuIndex, filterMenuLength, filterMenuItems);
      break;

    case FILTER_RESONANCE:
      currentMenu = FILTER;
      filterMenuIndex = 0;
      filterMenuPageNumber = 0;
      updateMenu(filterMenuPageNumber, filterMenuIndex, filterMenuLength, filterMenuItems);
      break;

    case FILTER_ON_OFF:
      currentMenu = FILTER;
      filterMenuIndex = 0;
      filterMenuPageNumber = 0;
      updateMenu(filterMenuPageNumber, filterMenuIndex, filterMenuLength, filterMenuItems);
      break;

    case AMP_ENV:
      currentMenu = MAIN;
      mainMenuIndex = 0;
      mainMenuPageNumber = 0;
      updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;
    
    case AMP_ATTACK:
      currentMenu = AMP_ENV;
      ampEnvelopeMenuIndex = 0;
      ampEnvelopeMenuPageNumber = 0;
      updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
      break;

    case AMP_DECAY:
      currentMenu = AMP_ENV;
      ampEnvelopeMenuIndex = 0;
      ampEnvelopeMenuPageNumber = 0;
      updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
      break;

    case AMP_SUSTAIN:
      currentMenu = AMP_ENV;
      ampEnvelopeMenuIndex = 0;
      ampEnvelopeMenuPageNumber = 0;
      updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
      break;

    case AMP_RELEASE:
      currentMenu = AMP_ENV;
      ampEnvelopeMenuIndex = 0;
      ampEnvelopeMenuPageNumber = 0;
      updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
      break;

    case AMP_AMOUNT:
      currentMenu = AMP_ENV;
      ampEnvelopeMenuIndex = 0;
      ampEnvelopeMenuPageNumber = 0;
      updateMenu(ampEnvelopeMenuPageNumber, ampEnvelopeMenuIndex, ampEnvelopeMenuLength, ampEnvelopeMenuItems);
      break;

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

void applyAmpAttack() {
  float norm = float(ampAttackEnvelopeStep) / envelopeStepsMax;
  float warped = powf(norm, envelopeCurveAmount);
  float expTime = minEnvelopeAmount * powf(maxEnvelopeAmount / minEnvelopeAmount, warped);
  float linTime = norm * maxEnvelopeAmount;

  ampAttack = linTime * (1.0f - mixAlpha) + expTime * mixAlpha;

  ampEnvelope.attack(ampAttack);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Amp Attack:");
  display.print(int(ampAttack));
  display.println(" ms");
  display.display();
}

void applyAmpDecay() {
  float norm = float(ampDecayEnvelopeStep) / envelopeStepsMax;
  float warped = powf(norm, envelopeCurveAmount);
  float expTime = minEnvelopeAmount * powf(maxEnvelopeAmount / minEnvelopeAmount, warped);
  float linTime = norm * maxEnvelopeAmount;

  ampDecay = linTime * (1.0f - mixAlpha) + expTime * mixAlpha;

  ampEnvelope.decay(ampDecay);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Amp Decay:");
  display.print(int(ampDecay));
  display.println(" ms");
  display.display();
}

void applyAmpSustain() {
  ampEnvelope.sustain(ampSustain);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Amp Sustain:");
  display.print(ampSustain * 100);
  display.println(" %");
  display.display();
}

void applyPitchShiftA() {
  // is handled in midi noteOn
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Pitch shft");
  display.print(oscAPitchOffset);
  display.print(" st");
  display.display();
}

void applyPulseWidthA() {
  waveformA.pulseWidth(oscAPulseWidth);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Pulse Wdth");
  display.print(oscAPulseWidth);
  display.print(" duty");
  display.display();
}

void onNoteOn(byte channel, byte note, byte velocity) {
  float freqA = 440.0 * pow(2.0, (note + oscAPitchOffset - 69) / 12.0);
  float adjustedVelocity = float(velocity)/127.0f;
  waveformA.frequency(freqA);
  waveformA.amplitude(adjustedVelocity);
  
  ampEnvelope.noteOn();
}

// This will need to be changed with envelope
void onNoteOff(byte channel, byte note, byte velocity) {
  ampEnvelope.noteOff();
}


void applyFilterCutoff() {
  float filterCurve = powf(filterControl, filterCutoffCurve);
  filterCutoffFreq = minCutoffFreq * powf(maxCutoffFreq / minCutoffFreq, filterCurve);

  filter1.frequency(filterCutoffFreq);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Cutoff: ");
  display.print((int)filterCutoffFreq);
  display.println(" Hz");
  display.display();
}

void applyFilterResonance() {
  float adjustedResonance = minResonance + (maxResonance - minResonance) * filterResonance;

  filter1.resonance(adjustedResonance);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Resonance:");
  display.print(filterResonance * 100);
  display.println(" %");
  display.display();
}