#ifndef LED_RING

#define LED_RING

#include <Adafruit_NeoPixel.h>

#define MINBRIGHTNESS 3


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(4, 25, NEO_KHZ800 + NEO_GRB);



int currentColour = 4, oldColour = 0, newColour = 0;
uint32_t animationStartTime = millis();
int animationAngle = 0; // goes from 0 to 180
int transitionStartOffset = 0;
int transitionStepsLeft = 0;
bool transitionStarted = false;

int red[3] = {255,0,0};
int yellow[3] = {255,255,0};
int blue[3] = {0, 80, 255};
int black[3] = {0, 0, 0};
int intermediate[3] = {0, 0, 0};
int *animationColours[] = {blue, yellow, red, intermediate, black};

int clamp(int val, int low, int high){
  return min(max(val, low), high);
}

void initPixels(){
  pixels.begin();
  pixels.setBrightness(40);
}

void stepPixelAnimation(){
  pixels.clear();
   for(int i = 0; i<2; i++){
    float pixelBrightness = max((float)(245.f*((cos((float)(animationAngle/2 - i*90)*PI/180.f)+1)*.5f))/255.f, 0.09f);
    pixels.setPixelColor(i, clamp((int)(animationColours[currentColour][0]*pixelBrightness), MINBRIGHTNESS, 255), clamp((int)(animationColours[currentColour][1] * pixelBrightness), MINBRIGHTNESS, 255), clamp((int)(animationColours[currentColour][2] * pixelBrightness), MINBRIGHTNESS, 255));
    pixels.setPixelColor(i+2, clamp((int)(animationColours[currentColour][0]*pixelBrightness), MINBRIGHTNESS, 255), clamp((int)(animationColours[currentColour][1] * pixelBrightness), MINBRIGHTNESS, 255), clamp((int)(animationColours[currentColour][2] * pixelBrightness), MINBRIGHTNESS, 255));
  }
  pixels.show();
  if(++animationAngle>=720){
    animationAngle = 0;
  }
}

void changeRingColour(){
  if(transitionStepsLeft <= 0 && transitionStarted){
    transitionStarted = false;
    currentColour = newColour;
  }
  else if(transitionStarted){
    float colour1Amount = (cos(((float)(animationAngle-transitionStartOffset)*PI/180.f)/2.f)+1.f)*0.5f;
    float colour2Amount = (cos(((float)(animationAngle-transitionStartOffset)*PI/180.f)/2.f + PI)+1.f)*0.5f;
 
    for(int i = 0; i<3; i++){
      intermediate[i] = animationColours[oldColour][i] * colour1Amount + animationColours[newColour][i] * colour2Amount;
    }
    transitionStepsLeft--;
  }
  else{
    transitionStarted = true;
    transitionStepsLeft = 360;
    oldColour = currentColour;
    transitionStartOffset = animationAngle;
    currentColour = 3;
  }
}

void setNewColour(int col){
  if(col != newColour){
    newColour = col;
    changeRingColour();
  }
}

boolean getTransitionStarted(){
  return transitionStarted;
}

void updatePixels(){
  stepPixelAnimation();
  if(transitionStarted){
    changeRingColour();
  }
}

#endif
