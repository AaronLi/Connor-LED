#define SEERAWPULSE

#define BLUE 0
#define YELLOW 1
#define RED 2
#define BLACK 4

#define YELLOWTHRESHOLD 95
#define REDTHRESHOLD 120
#define BLACKTHRESHOLD 40

#define PULSETHRESHOLD 1900 

#define PULSEAVERAGINGLOOKBACK 10
uint32_t lastPixelUpdate = millis(), lastPulseCheck = millis();
boolean aboveThreshold = false, aboveThresholdOld = false;
boolean pulseHighSide = false;
uint32_t lastPulseTime = millis();
uint32_t lastPulseIntervals[PULSEAVERAGINGLOOKBACK];
int pulseCount = 0, pulseIntervalPointer= 0;

void setup() {
  initPixels();
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

int calculateBPM(){

  if(pulseCount == 0){
    return 0;
  }
  
  uint32_t averagePulseLength = 0; // average milliseconds per pulse
  for(int i = 0; i<pulseCount; i++){
    averagePulseLength+=lastPulseIntervals[i];
  }
  averagePulseLength/=pulseCount;

 float secondsPerPulse = (float)averagePulseLength/1000.f;

 float pulsesPerSecond = 1.f/secondsPerPulse;

 return pulsesPerSecond*60; //beats per minute!
}

void loop() {
  if(millis() - lastPixelUpdate > 2){
    lastPixelUpdate = millis();
    delay(1);
    updatePixels();
  }
  /*if(Serial.available()){
    char cIn = Serial.read() | B00100000;

    Serial.println(cIn);
    
    switch(cIn){
      case 'r':
        setNewColour(RED);
        break;
      case 'y':
        setNewColour(YELLOW);
        break;
      case 'b':
        setNewColour(BLUE);
        break;
      default:
        break;
    }
  }*/
  if(millis() - lastPulseCheck > 50){
    int pulseReading = analogRead(A2);
    #ifdef SEERAWPULSE
    Serial.println(pulseReading);
    #endif
    aboveThreshold = pulseReading > PULSETHRESHOLD;
    /*#ifndef SEERAWPULSE
    if(aboveThreshold){
      Serial.print("90 ");
    }
    else{
      Serial.print("70 ");
    }
    #endif*/
    pulseHighSide = aboveThreshold && !aboveThresholdOld;    
    digitalWrite(13, pulseHighSide);
    if(pulseHighSide){
      lastPulseIntervals[pulseIntervalPointer++ % PULSEAVERAGINGLOOKBACK] = millis() - lastPulseTime;
      lastPulseTime = millis();
      pulseIntervalPointer %= PULSEAVERAGINGLOOKBACK;
      pulseCount = min(pulseCount+1, PULSEAVERAGINGLOOKBACK);
    }

    int bpm = calculateBPM();
    #ifndef SEERAWPULSE
    //Serial.println(bpm);
    #endif
    if(!getTransitionStarted()){
      if(bpm <=YELLOWTHRESHOLD){
        setNewColour(BLUE);
      }
      else if(bpm <= REDTHRESHOLD){
        setNewColour(YELLOW);
      }
      else{
        setNewColour(RED);
      }
    }
    
    aboveThresholdOld = aboveThreshold;
    lastPulseCheck = millis();
  }

  if(millis() - lastPulseTime > 10000){
    setNewColour(BLACK);
  }
  
}
