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
AudioSynthWaveform       waveformA;      //xy=65,252.5714111328125
AudioSynthWaveform       waveformB;      //xy=74.23333740234375,386.23333740234375
AudioEffectEnvelope      filterEnvelope;      //xy=111.76191711425781,87.4761848449707
AudioMixer4              oscMixer;         //xy=254.6190071105957,290.33332443237305
AudioAmplifier           filterEnvelopeAmp;           //xy=308.9047088623047,86.0476303100586
AudioFilterStateVariable filter1;        //xy=536.126148223877,118.51903915405273
AudioMixer4              filterTypeMixer;         //xy=570.4833374023438,258.98332595825195
AudioEffectEnvelope      ampEnvelope;      //xy=751.1976852416992,258.7333335876465
AudioAmplifier           amp1;           //xy=890.3047752380371,258.7333240509033
AudioMixer4              mixerLeft;         //xy=982.4832763671875,119.23332977294922
AudioMixer4              mixerRight;         //xy=998.4832763671875,396.23333740234375
AudioOutputI2S           i2s1;           //xy=1041.4832763671875,262.23333740234375
AudioConnection          patchCord1(waveformA, 0, oscMixer, 0);
AudioConnection          patchCord2(waveformB, 0, oscMixer, 1);
AudioConnection          patchCord3(filterEnvelope, filterEnvelopeAmp);
AudioConnection          patchCord4(oscMixer, 0, filterTypeMixer, 3);
AudioConnection          patchCord5(oscMixer, 0, filter1, 0);
AudioConnection          patchCord6(filter1, 0, filterTypeMixer, 0);
AudioConnection          patchCord7(filter1, 1, filterTypeMixer, 1);
AudioConnection          patchCord8(filter1, 2, filterTypeMixer, 2);
AudioConnection          patchCord9(filterTypeMixer, ampEnvelope);
AudioConnection          patchCord10(ampEnvelope, amp1);
AudioConnection          patchCord11(amp1, 0, mixerLeft, 0);
AudioConnection          patchCord12(amp1, 0, mixerRight, 0);
AudioConnection          patchCord13(mixerLeft, 0, i2s1, 0);
AudioConnection          patchCord14(mixerRight, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=516.5190315246582,462.804723739624
// GUItool: end automatically generated code


// When ready to do each additional menu state, uncomment one at a time and do each individually, test and then move on
enum menuState {
  MAIN,
  OSC_A,
  OSC_B,
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
  FILTER_ENV,
  FILTER_ATTACK,
  FILTER_DECAY,
  FILTER_SUSTAIN,
  FILTER_RELEASE,
  FILTER_AMOUNT,
  WAVEFORM_A,
  PITCH_A,
  PULSE_WIDTH_A,
  VOLUME_A,
  OSC_A_ON_OFF,
  WAVEFORM_B,
  PITCH_B,
  PULSE_WIDTH_B,
  VOLUME_B,
  OSC_B_ON_OFF
};

menuState currentMenu = MAIN;

const char* mainMenuItems[] = {"Osc A", "Osc B", "Filter", "Amp Env", "Filter Env", "Volume", "Mono/Poly"};
int mainMenuIndex = 0;
const int mainMenuLength = sizeof(mainMenuItems) / 4;
int mainMenuPageNumber = 0;
const int itemsPerPage = 3;

const char* oscAMenuItems[] = {"Waveform", "Pitch", "Pulse Wdth", "Volume", "Osc On/Off"};
int oscAMenuIndex = 0;
const int oscAMenuLength = sizeof(oscAMenuItems) / 4;
int oscAMenuPageNumber = 0;

const char* waveformAMenuItems[] = {"Sine", "Sawtooth", "Square", "Triangle", "Pulse"};
int waveformAMenuIndex = 0;
const int waveformAMenuLength = sizeof(waveformAMenuItems) / 4;
int waveformAMenuPageNumber = 0;

const char* oscBMenuItems[] = {"Waveform", "Pitch", "Pulse Wdth", "Volume", "Osc On/Off"};
int oscBMenuIndex = 0;
const int oscBMenuLength = sizeof(oscBMenuItems) / 4;
int oscBMenuPageNumber = 0;

const char* waveformBMenuItems[] = {"Sine", "Sawtooth", "Square", "Triangle", "Pulse"};
int waveformBMenuIndex = 0;
const int waveformBMenuLength = sizeof(waveformBMenuItems) / 4;
int waveformBMenuPageNumber = 0;

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

const char* filterEnvelopeMenuItems[] = {"Attack", "Decay", "Sustain", "Release", "Amount"};
int filterEnvelopeMenuIndex = 0;
const int filterEnvelopeMenuLength = sizeof(filterEnvelopeMenuItems) / 4;
int filterEnvelopeMenuPageNumber = 0;

int oscAPitchOffset = 0;
float oscAPulseWidth = 0.5f;
bool oscAOn = true;

int oscAWaveform = WAVEFORM_PULSE;

int oscBPitchOffset = 0;
float oscBPulseWidth = 0.5f;
bool oscBOn = false;

int oscBWaveform = WAVEFORM_PULSE;

Encoder myEncoder(2, 3);
const int encoderSwitchPin = 15;
Bounce encoderSwitch = Bounce(encoderSwitchPin,20);
long lastEncoderPosition = 0;

Bounce returnButton = Bounce(4, 20);
const int returnButtonPin = 4;



// This might need to be changed to allow for individual osc panning
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
float ampDecay = 5.0f; //ms
float ampSustain = 1.0f; //percentage
float ampRelease = 5.0f; //ms
float ampAmount = 1.0f; //percentage?


const float mixAlpha = 0.8f;
const int envelopeStepsMax = 100;
const float minEnvelopeAmount = 5.0f;
const float maxEnvelopeAmount = 15000.0f; 
float envelopeCurveAmount = 2.0f;

int ampAttackEnvelopeStep = 0;
int ampDecayEnvelopeStep = 0;
int ampReleaseEnvelopeStep = 0;


float filterAttack = 5.0f; //ms
float filterDecay = 150.0f; //ms
float filterSustain = 0.6f; //percentage
float filterRelease = 500.0f; //ms
float filterAmount = 0.0f; //percentage?
float maxFilterAmount = 1.0f; // will change


int filterAttackEnvelopeStep = 0;
int filterDecayEnvelopeStep = 0;
int filterReleaseEnvelopeStep = 0;

float oscAVolume = 0.5f;
float oscBVolume = 0.5f;

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

  waveformA.begin(oscAWaveform);
  waveformA.pulseWidth(oscAPulseWidth);

  waveformB.begin(oscBWaveform);
  waveformB.pulseWidth(oscBPulseWidth);

  oscMixer.gain(0, oscAVolume);
  oscMixer.gain(1, 0.0f);

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

  filterEnvelope.delay(0);
  filterEnvelope.hold(0);

  filterEnvelope.attack(filterAttack);
  filterEnvelope.decay(filterDecay);
  filterEnvelope.sustain(filterSustain);
  filterEnvelope.release(filterRelease); 

  filterEnvelopeAmp.gain(filterAmount);

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
      waveformAMenuIndex = (waveformAMenuIndex + 1) % waveformAMenuLength;
      updateMenu(waveformAMenuPageNumber, waveformAMenuIndex, waveformAMenuLength, waveformAMenuItems);
      break;

    case PITCH_A:
      oscAPitchOffset++;
      applyPitchShiftA();
      break;

    case PULSE_WIDTH_A:
      oscAPulseWidth = min(oscAPulseWidth + 0.02f, 1.00f);
      applyPulseWidthA();
      break;

    case VOLUME_A:
      oscAVolume = min(oscAVolume + 0.02f, 1.00f);
      applyVolumeA();
      break;

    case OSC_A_ON_OFF:
      // do nothing
      break;

    case OSC_B:
      oscBMenuIndex = (oscBMenuIndex + 1) % oscBMenuLength;
      updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);
      break;

    case WAVEFORM_B:
      waveformBMenuIndex = (waveformBMenuIndex + 1) % waveformBMenuLength;
      updateMenu(waveformBMenuPageNumber, waveformBMenuIndex, waveformBMenuLength, waveformBMenuItems);
      break;

    case PITCH_B:
      oscBPitchOffset++;
      applyPitchShiftB();
      break;

    case PULSE_WIDTH_B:
      oscBPulseWidth = min(oscBPulseWidth + 0.02f, 1.00f);
      applyPulseWidthB();
      break;

    case VOLUME_B:
      oscBVolume = min(oscBVolume + 0.02f, 1.00f);
      applyVolumeB();
      break;

    case OSC_B_ON_OFF:
      // do nothing
      break;
    

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
      ampReleaseEnvelopeStep = min(ampReleaseEnvelopeStep + 1, envelopeStepsMax);
      applyAmpRelease();
      break;

    case AMP_AMOUNT:
      ampAmount = min(ampAmount+ 0.02f, 1.00f);
      applyAmpAmount();
      break;

    case FILTER_ENV:
      filterEnvelopeMenuIndex = (filterEnvelopeMenuIndex + 1) % filterEnvelopeMenuLength;
      updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      break;

    case FILTER_ATTACK:
      filterAttackEnvelopeStep = min(filterAttackEnvelopeStep + 1, envelopeStepsMax);
      applyFilterAttack();
      break;

    case FILTER_DECAY:
      filterDecayEnvelopeStep = min(filterDecayEnvelopeStep + 1, envelopeStepsMax);
      applyFilterDecay();
      break;

    case FILTER_SUSTAIN:
      filterSustain = min(filterSustain + 0.02f, 1.00f);
      applyFilterSustain();
      break;

    case FILTER_RELEASE:
      filterReleaseEnvelopeStep = min(filterReleaseEnvelopeStep + 1, envelopeStepsMax);
      applyFilterRelease();
      break;

    case FILTER_AMOUNT:
      filterAmount = min(filterAmount + 0.05f, maxFilterAmount); // will be changed
      applyFilterAmount();
      break;
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
      waveformAMenuIndex = (waveformAMenuIndex - 1) % waveformAMenuLength;
      updateMenu(waveformAMenuPageNumber, waveformAMenuIndex, waveformAMenuLength, waveformAMenuItems);
      break;

    case PITCH_A:
      oscAPitchOffset--;
      applyPitchShiftA();
      break;

    case PULSE_WIDTH_A:
      oscAPulseWidth = max(oscAPulseWidth - 0.02f, 0.00f);
      applyPulseWidthA();
      break;

    case VOLUME_A:
      oscAVolume = max(oscAVolume - 0.02f, 0.00f);
      applyVolumeA();
      break;

    case OSC_A_ON_OFF:
      // do nothing
      break;

    case OSC_B:
      oscBMenuIndex = (oscBMenuIndex - 1) % oscBMenuLength;
      updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);
      break;

    case WAVEFORM_B:
      waveformBMenuIndex = (waveformBMenuIndex - 1) % waveformBMenuLength;
      updateMenu(waveformBMenuPageNumber, waveformBMenuIndex, waveformBMenuLength, waveformBMenuItems);
      break;

    case PITCH_B:
      oscBPitchOffset--;
      applyPitchShiftB();
      break;

    case PULSE_WIDTH_B:
      oscBPulseWidth = max(oscBPulseWidth - 0.02f, 0.00f);
      applyPulseWidthB();
      break;

    case VOLUME_B:
      oscBVolume = max(oscBVolume - 0.02f, 0.00f);
      applyVolumeB();
      break;

    case OSC_B_ON_OFF:
      // do nothing
      break;

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
      ampReleaseEnvelopeStep = max(ampReleaseEnvelopeStep - 1, 0.00f);
      applyAmpRelease();
      break;

    case AMP_AMOUNT:
      ampAmount = max(ampAmount - 0.02f, 0.00f);
      applyAmpAmount();
      break;

    case FILTER_ENV:
      filterEnvelopeMenuIndex = (filterEnvelopeMenuIndex - 1) % filterEnvelopeMenuLength;
      updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      break;

    case FILTER_ATTACK:
      filterAttackEnvelopeStep = max(filterAttackEnvelopeStep - 1, 0.00f);
      applyFilterAttack();
      break;

    case FILTER_DECAY:
      filterDecayEnvelopeStep = max(filterDecayEnvelopeStep - 1, 0.00f);
      applyFilterDecay();
      break;

    case FILTER_SUSTAIN:
      filterSustain = max(filterSustain - 0.02f, 0.00f);
      applyFilterSustain();
      break;

    case FILTER_RELEASE:
      filterReleaseEnvelopeStep = max(filterReleaseEnvelopeStep - 1, 0.00f);
      applyFilterRelease();
      break;

    case FILTER_AMOUNT:
      filterAmount = max(filterAmount - 0.05f, 0.00f);
      applyFilterAmount();
      break;
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
        currentMenu = OSC_B;
        oscBMenuPageNumber = 0;
        oscBMenuIndex = 0;
        display.setCursor(0,0);
        updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);        
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
      } else if (mainMenuIndex == 4) {
        currentMenu = FILTER_ENV;
        filterEnvelopeMenuPageNumber = 0;
        filterEnvelopeMenuIndex = 0;
        display.setCursor(0,0);
        updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      }
      break;

    case OSC_A:
      if (oscAMenuIndex == 0) {
        currentMenu = WAVEFORM_A;
        waveformAMenuIndex = 0;
        waveformAMenuPageNumber = 0;
        updateMenu(waveformAMenuPageNumber, waveformAMenuIndex, waveformAMenuLength, waveformAMenuItems);
      } else if (oscAMenuIndex == 1) {
        currentMenu = PITCH_A;
        applyPitchShiftA();
      } else if (oscAMenuIndex == 2) {
        currentMenu = PULSE_WIDTH_A;
        applyPulseWidthA();
        waveformA.pulseWidth(oscAPulseWidth);
      } else if (oscAMenuIndex == 3) {
        currentMenu = VOLUME_A;
        applyVolumeA();
      } else if (oscAMenuIndex == 4) {
        oscAOn = !oscAOn;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_WHITE);
        if(oscAOn) {
          display.println("Osc A On");
          oscMixer.gain(0, oscAVolume);
        } else {
          display.println("Osc A Off");
          oscMixer.gain(0, 0);
        }
        display.display();
      }
      break;

    case WAVEFORM_A:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Selected:");
      if (waveformAMenuIndex == 0){
        oscAWaveform = WAVEFORM_SINE;
        display.println("Sine");
      } else if (waveformAMenuIndex == 1) {
        oscAWaveform = WAVEFORM_SAWTOOTH;
        display.println("Sawtooth");
      } else if (waveformAMenuIndex == 2) {
        oscAWaveform = WAVEFORM_SQUARE;
        display.println("Square");
      } else if (waveformAMenuIndex == 3) {
        oscAWaveform = WAVEFORM_TRIANGLE;
        display.println("Triangle");
      } else if (waveformAMenuIndex == 4) {
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

    case VOLUME_A:
      // do nothing
      break;

    case OSC_A_ON_OFF:
      // do nothing
      break;

    case OSC_B:
      if (oscBMenuIndex == 0) {
        currentMenu = WAVEFORM_B;
        waveformBMenuIndex = 0;
        waveformBMenuPageNumber = 0;
        updateMenu(waveformBMenuPageNumber, waveformBMenuIndex, waveformBMenuLength, waveformBMenuItems);
      } else if (oscBMenuIndex == 1) {
        currentMenu = PITCH_B;
        applyPitchShiftB();
      } else if (oscBMenuIndex == 2) {
        currentMenu = PULSE_WIDTH_B;
        applyPulseWidthB();
        waveformB.pulseWidth(oscBPulseWidth);
      } else if (oscBMenuIndex == 3) {
        currentMenu = VOLUME_B;
        applyVolumeB();
      } else if (oscBMenuIndex == 4) {
        oscBOn = !oscBOn;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextColor(SSD1306_WHITE);
        if(oscBOn) {
          display.println("Osc B On");
          oscMixer.gain(1, oscBVolume);
        } else {
          display.println("Osc B Off");
          oscMixer.gain(1, 0);
        }
        display.display();
      }
      break;

    case WAVEFORM_B:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.println("Selected:");
      if (waveformBMenuIndex == 0){
        oscBWaveform = WAVEFORM_SINE;
        display.println("Sine");
      } else if (waveformBMenuIndex == 1) {
        oscBWaveform = WAVEFORM_SAWTOOTH;
        display.println("Sawtooth");
      } else if (waveformBMenuIndex == 2) {
        oscBWaveform = WAVEFORM_SQUARE;
        display.println("Square");
      } else if (waveformBMenuIndex == 3) {
        oscBWaveform = WAVEFORM_TRIANGLE;
        display.println("Triangle");
      } else if (waveformBMenuIndex == 4) {
        oscBWaveform = WAVEFORM_PULSE;
        display.println("Pulse");
      }
      waveformB.begin(oscBWaveform);
      display.display();
      break;

    case PITCH_B:
      // do nothing
      break;

    case PULSE_WIDTH_B:
      // do nothing
      break;

    case VOLUME_B:
      // do nothing
      break;

    case OSC_B_ON_OFF:
      // do nothing
      break;

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
      } else if (ampEnvelopeMenuIndex == 3) {
        currentMenu = AMP_RELEASE;
        applyAmpRelease();
      } else if (ampEnvelopeMenuIndex == 4) {
        currentMenu = AMP_AMOUNT;
        applyAmpAmount();
      }

      display.display();
      break;
    
    case AMP_ATTACK:
      // do nothing
      break;

    case AMP_DECAY:
      // do nothing
      break;

    case AMP_SUSTAIN:
      // do nothing
      break;

    case AMP_RELEASE:
      // do nothing
      break;

    case AMP_AMOUNT:
      // do nothing
      break;

    case FILTER_ENV:
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      if (filterEnvelopeMenuIndex == 0) {
        currentMenu = FILTER_ATTACK;
        applyFilterAttack();
      } else if (filterEnvelopeMenuIndex == 1) {
        currentMenu = FILTER_DECAY;
        applyFilterDecay();
      } else if (filterEnvelopeMenuIndex == 2) {
        currentMenu = FILTER_SUSTAIN;
        applyFilterSustain();
      } else if (filterEnvelopeMenuIndex == 3) {
        currentMenu = FILTER_RELEASE;
        applyFilterRelease();
      } else if (filterEnvelopeMenuIndex == 4) {
        currentMenu = FILTER_AMOUNT;
        applyFilterAmount();
      }
      display.display();
      break;

    case FILTER_ATTACK:
      // do nothing
      break;

    case FILTER_DECAY:
      // do nothing
      break;

    case FILTER_SUSTAIN:
      // do nothing
      break;

    case FILTER_RELEASE:
      // do nothing
      break;

    case FILTER_AMOUNT:
      // do nothing
      break;
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

    case VOLUME_A:
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

    case OSC_B:
      display.setCursor(0,0);
      currentMenu = MAIN;
      mainMenuIndex = 0;
      mainMenuPageNumber = 0;
      updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;

    case WAVEFORM_B:
      currentMenu = OSC_B;
      oscBMenuIndex = 0;
      oscBMenuPageNumber = 0;
      updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);
      break;

    case PITCH_B:
      currentMenu = OSC_B;
      oscBMenuIndex = 0;
      oscBMenuPageNumber = 0;
      updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);
      break;

    case PULSE_WIDTH_B:
      currentMenu = OSC_B;
      oscBMenuIndex = 0;
      oscBMenuPageNumber = 0;
      updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);
      break;

    case VOLUME_B:
      currentMenu = OSC_B;
      oscBMenuIndex = 0;
      oscBMenuPageNumber = 0;
      updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);
      break;

    case OSC_B_ON_OFF:
      currentMenu = OSC_B;
      oscBMenuIndex = 0;
      oscBMenuPageNumber = 0;
      updateMenu(oscBMenuPageNumber, oscBMenuIndex, oscBMenuLength, oscBMenuItems);
      break;
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

    case FILTER_ENV:
      currentMenu = MAIN;
      mainMenuIndex = 0;
      mainMenuPageNumber = 0;
      updateMenu(mainMenuPageNumber, mainMenuIndex, mainMenuLength, mainMenuItems);
      break;

    case FILTER_ATTACK:
      currentMenu = FILTER_ENV;
      filterEnvelopeMenuIndex = 0;
      filterEnvelopeMenuPageNumber = 0;
      updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      break;

    case FILTER_DECAY:
      currentMenu = FILTER_ENV;
      filterEnvelopeMenuIndex = 0;
      filterEnvelopeMenuPageNumber = 0;
      updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      break;

    case FILTER_SUSTAIN:
      currentMenu = FILTER_ENV;
      filterEnvelopeMenuIndex = 0;
      filterEnvelopeMenuPageNumber = 0;
      updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      break;

    case FILTER_RELEASE:
      currentMenu = FILTER_ENV;
      filterEnvelopeMenuIndex = 0;
      filterEnvelopeMenuPageNumber = 0;
      updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      break;

    case FILTER_AMOUNT:
      currentMenu = FILTER_ENV;
      filterEnvelopeMenuIndex = 0;
      filterEnvelopeMenuPageNumber = 0;
      updateMenu(filterEnvelopeMenuPageNumber, filterEnvelopeMenuIndex, filterEnvelopeMenuLength, filterEnvelopeMenuItems);
      break;      
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

void applyAmpRelease() {
  float norm = float(ampReleaseEnvelopeStep) / envelopeStepsMax;
  float warped = powf(norm, envelopeCurveAmount);
  float expTime = minEnvelopeAmount * powf(maxEnvelopeAmount / minEnvelopeAmount, warped);
  float linTime = norm * maxEnvelopeAmount;

  ampRelease = linTime * (1.0f - mixAlpha) + expTime * mixAlpha;

  ampEnvelope.release(ampRelease);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Amp Release:");
  display.print(int(ampRelease));
  display.println(" ms");
  display.display();  
}

void applyAmpAmount() {

  amp1.gain(ampAmount);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Amp Amnt:");
  display.println(ampAmount);
  display.display();  
}



void applyFilterAttack() {
  float norm = float(filterAttackEnvelopeStep) / envelopeStepsMax;
  float warped = powf(norm, envelopeCurveAmount);
  float expTime = minEnvelopeAmount * powf(maxEnvelopeAmount / minEnvelopeAmount, warped);
  float linTime = norm * maxEnvelopeAmount;

  filterAttack = linTime * (1.0f - mixAlpha) + expTime * mixAlpha;

  filterEnvelope.attack(filterAttack);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Filter Attack:");
  display.print(int(filterAttack));
  display.println(" ms");
  display.display();
}

void applyFilterDecay() {
  float norm = float(filterDecayEnvelopeStep) / envelopeStepsMax;
  float warped = powf(norm, envelopeCurveAmount);
  float expTime = minEnvelopeAmount * powf(maxEnvelopeAmount / minEnvelopeAmount, warped);
  float linTime = norm * maxEnvelopeAmount;

  filterDecay = linTime * (1.0f - mixAlpha) + expTime * mixAlpha;

  filterEnvelope.decay(filterDecay);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Filter Decay:");
  display.print(int(filterDecay));
  display.println(" ms");
  display.display();
}

void applyFilterSustain() {
  filterEnvelope.sustain(filterSustain);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Filter Sustain:");
  display.print(filterSustain * 100);
  display.println(" %");
  display.display();
}

void applyFilterRelease() {
  float norm = float(filterReleaseEnvelopeStep) / envelopeStepsMax;
  float warped = powf(norm, envelopeCurveAmount);
  float expTime = minEnvelopeAmount * powf(maxEnvelopeAmount / minEnvelopeAmount, warped);
  float linTime = norm * maxEnvelopeAmount;

  filterRelease = linTime * (1.0f - mixAlpha) + expTime * mixAlpha;

  filterEnvelope.release(filterRelease);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Filter Release:");
  display.print(int(filterRelease));
  display.println(" ms");
  display.display();  
}

void applyFilterAmount() {

  filterEnvelopeAmp.gain(filterAmount);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Filter Amnt:");
  display.println(filterAmount);
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

void applyVolumeA() {
  oscMixer.gain(0, oscAVolume);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Volume");
  display.println(oscAVolume);
  display.display();
}

void applyPitchShiftB() {
  // is handled in midi noteOn
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Pitch shft");
  display.print(oscBPitchOffset);
  display.print(" st");
  display.display();
}

void applyPulseWidthB() {
  waveformB.pulseWidth(oscBPulseWidth);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Pulse Wdth");
  display.print(oscBPulseWidth);
  display.print(" duty");
  display.display();
}

void applyVolumeB() {
  oscMixer.gain(1, oscBVolume);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println("Volume");
  display.println(oscBVolume);
  display.display();
}

void onNoteOn(byte channel, byte note, byte velocity) {
  float freqA = 440.0 * pow(2.0, (note + oscAPitchOffset - 69) / 12.0);
  float freqB = 440.0 * pow(2.0, (note + oscBPitchOffset - 69) / 12.0);

  float adjustedVelocity = float(velocity)/127.0f;
  waveformA.frequency(freqA);
  waveformA.amplitude(adjustedVelocity);
  waveformB.frequency(freqB);
  waveformB.amplitude(adjustedVelocity);
  
  ampEnvelope.noteOn();
  filterEnvelope.noteOn();
}

// This will need to be changed with envelope
void onNoteOff(byte channel, byte note, byte velocity) {
  ampEnvelope.noteOff();
  filterEnvelope.noteOff();
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