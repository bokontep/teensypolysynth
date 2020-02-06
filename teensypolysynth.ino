

#include <Audio.h>

#include "VAEngine.h"
const int WTCOUNT=128;
const int WTLEN=256;
float Waveforms[WTCOUNT*WTLEN];
VAEngine<16,256,256> vaEngine(&Waveforms[0]);
AudioOutputI2S i2s1;
AudioMixer4 mixer;
AudioConnection patchCord1(vaEngine, 0, mixer, 0);
AudioConnection patchCord2(mixer, 0, i2s1, 0);
AudioConnection patchCord3(mixer, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;
void initWaveForms()
{
  for(int i=0;i<WTLEN;i++)
  {
    Waveforms[i] = ((sin(2.0 * (PI / (float)WTLEN) * i)));
  }
  for (int i = 0; i < 128; i++)
  {
    Waveforms[WTLEN+i] = ( (-1.0 + i * (1.0 / ((double)WTLEN / 2.0))));
  }
  for (int i = 128; i < 256; i++)
  {
    Waveforms[WTLEN+i] = ( (1.0 - i * (1.0 / ((double)WTLEN / 2.0))));
  }
  for (int i = 0; i < 256; i++)
  {
    Waveforms[2*WTLEN+i] = (i < (WTLEN / 2) ? 1.0 : -1.0);
  }
  for (int i = 0; i < 256; i++)
  {
    Waveforms[3*WTLEN+i] = ( (-1.0 + (2.0 / WTLEN) * i));
  }
  for (int i = 0; i < WTLEN; i++)
  {
    Waveforms[4*WTLEN+i] = -1.0+random(1UL<<31)* (2.0/(1UL<<31));
  }
  
  for (int w=5;w<WTCOUNT;w++)
  {
    for(int i=0;i<256;i++)
    {
      float f1 = (WTCOUNT-w)/120;
      float f2 = ((WTCOUNT-w)%120)/110.0;
      float f3 = ((WTCOUNT-w)%62)/60.0;
      float f4 = ((WTCOUNT-w)%31)/31.0; 
      Waveforms[w*WTLEN+i] = ((f1*Waveforms[i]+f2*Waveforms[WTLEN+i]+f3*Waveforms[2*WTLEN+i]+f4*Waveforms[3*WTLEN+i])/4);   
    }
  }
}

void OnNoteOn(byte channel, byte note, byte velocity)
{
  vaEngine.handleNoteOn(channel,note,velocity);
}
void OnNoteOff(byte channel, byte note, byte velocity)
{
  vaEngine.handleNoteOff(channel,note,velocity);
}
void OnControlChange(byte channel, byte control, byte value)
{
  vaEngine.handleControlChange(channel, control, value);  
}
void OnPitchChange(byte channel,int pitch)
{
  vaEngine.handlePitchBend(channel,pitch+8192);
}
long lastrun=0;
void setup()
{
  initWaveForms();
  Serial.begin(115200);
  delay(4000);
  
  vaEngine.init(44100.0);
  Serial.println("init done!");
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);
  mixer.gain(0, 0.7);
  
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandlePitchChange(OnPitchChange);
  lastrun = millis();
  ADSR adsr(44100.0);
  for(int i=0;i<128;i++)
  {
    adsr.SetDecayMidi(i);
    Serial.print(i);
    Serial.print(":");
     
    Serial.print(adsr.GetDecayCoef());
    Serial.println();
  }
  Serial.println(1.43343434);
}

void loop()
{
  
  usbMIDI.read();
  long now=millis();
  if(now-lastrun>2000)
  {
    
    lastrun = now;
  }
  
}
