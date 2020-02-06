#ifndef SynthVoice_h
#define SynthVoice_h

#include "FloatWaveTableOsc.hpp"
#include "ADSR.h"
#include "LowPass.h"
#include <Arduino.h>


class SynthVoice  {
public:
    SynthVoice()
    {
        this->sampleRate = 8000;
        this->modulation = 0;
        this->pwm = 0.5;
        this->fmod1 = 1.0;
        this->fmod2 = 1.0;
        this->fmod3 = 0.0;
        this->ffreq = 1.0;
        this->fq = 0.1;
        lowpass.SetParameters(ffreq, fq);
        
    }
    SynthVoice(double sampleRate) {
        this->sampleRate = sampleRate;
        this->modulation = 0;
        this->pwm = 0.5;
        this->fmod1 = 1.0;
        this->fmod2 = 1.0;
        this->fmod3 = 0.0;
        this->ffreq = 1.0;
        this->fq = 0.1;
        lowpass.SetParameters(ffreq, fq);
    }
    ~SynthVoice(void) {
        
    }
    void SetSampleRate(double newSampleRate)
    {
      this->sampleRate = newSampleRate;
    }
    
    void MidiNoteOn(uint8_t note, uint8_t vel)
    {
        
        double f = pow(2.0,(note*1.0-69.0)/12.0)*440.0;
        velocity = vel/128.0; 
        freq1 = f;
        freq2 = f;
        osc[0].SetFrequency(freq1,sampleRate);
        osc[1].SetFrequency(freq2,sampleRate);
        adsr[0].Gate(1);
        adsr[1].Gate(1);
    }
    void MidiNoteOff()
    {
        adsr[0].Gate(0);
        adsr[1].Gate(0);
    }
    void AddOsc1WaveTable(int len, float *waveTableIn)
    {
        osc[0].AddWaveTable(len,waveTableIn);
    }
    void AddOsc1SharedWaveTable(int len, float *waveTableIn)
    {
        osc[0].AddSharedWaveTable(len, waveTableIn);
    }
    void AddOsc2WaveTable(int len, float *waveTableIn)
    {
        osc[1].AddWaveTable(len,waveTableIn);
    }
    void AddOsc2SharedWaveTable(int len, float *waveTableIn)
    {
        osc[1].AddSharedWaveTable(len, waveTableIn);
    }
    
    void SetOsc1ADSR(float a, float d,float s,float r)
    {
        adsr[0].SetADSR(a,d,s,r);
    }
    void SetOsc2ADSR(float a, float d, float s, float r)
    {
        adsr[1].SetADSR(a,d,s,r);
    }
    void SetFmod1(uint8_t fmod)
    {
      this->fmod1 = fmod/64;
    }
    void SetFmod2(uint8_t fmod)
    {
      this->fmod2 = fmod/64;
    }
    void SetFmod3(uint8_t fmod)
    {
      this->fmod3 = fmod/64;
    }
    void MidiBend(int bend)
    {
      AudioNoInterrupts();
      float factor = ((bend - 8192.0)/8192.0);
      float mul = pow(2.0,(factor*12.0)/12.0);
      float bendfreq1 = freq1*mul;
      float bendfreq2 = freq2*mul;
      
      osc[0].SetFrequency(bendfreq1,sampleRate);
      osc[1].SetFrequency(bendfreq2,sampleRate);
      AudioInterrupts();
    }

    void ControlChange(byte channel, byte control, byte value)
    {
      AudioNoInterrupts();
      switch(control)
      {
          case 1: //Modulation
            MidiMod(value);
          break;
          case 16: //Osc1Wave
            MidiOsc1Wave(value);
          case 17: //Osc2Wave  
            MidiOsc2Wave(value);
          break;
          case 18: //osc1 attack
            SetOsc1ADSR(value*(1.0/127.0),adsr[0].GetDecay(),adsr[0].GetSustain(),adsr[0].GetRelease());
          break;
          case 19: //osc1 decay
            SetOsc1ADSR(adsr[0].GetAttack(),value*(1.0/127.0),adsr[0].GetSustain(),adsr[0].GetRelease());
          break;
          case 20: //osc1 systain
            SetOsc1ADSR(adsr[0].GetAttack(),adsr[0].GetDecay(),value*(1.0/127.0),adsr[0].GetRelease());
          break;
          case 21: //osc1 release
            SetOsc1ADSR(adsr[0].GetAttack(),adsr[0].GetDecay(),adsr[0].GetSustain(),value*(1.0/127.0));
          break;

          case 22: //osc2 attack
            SetOsc2ADSR(value*(1.0/127.0),adsr[1].GetDecay(),adsr[1].GetSustain(),adsr[1].GetRelease());
          break;
          case 23: //osc2 decay
            SetOsc2ADSR(adsr[1].GetAttack(),value*(1.0/127.0),adsr[1].GetSustain(),adsr[1].GetRelease());
          break;
          case 24: //osc2 systain
            SetOsc2ADSR(adsr[1].GetAttack(),adsr[1].GetDecay(),value*(1.0/127.0),adsr[1].GetRelease());
          break;
          case 25: //osc2 release
            SetOsc2ADSR(adsr[1].GetAttack(),adsr[1].GetDecay(),adsr[1].GetSustain(),value*(1.0/127.0));
          break;
          case 26: //filter freq
            lowpass.SetParameters(value*(1.0/127.0),lowpass.GetRes());
          break;
          case 27: //filter res
            lowpass.SetParameters(lowpass.GetFreq(),value*(1.0/127.0));
          break;
          case 28: //PWM
            MidiPwm(value);
          break;
  
      }
      AudioInterrupts();
    }
    void MidiMod(uint8_t newmod)
    {
      modulation = newmod/127.0;
      fmod1 = 1.0-0.1*modulation/127.0;
      fmod2 = 1.0-0.1*modulation/127.0;
      fmod3 = 0.5*modulation; 
    }
    void MidiPwm(uint8_t newmod)
    {
      pwm = newmod/127.0;
      if(newmod == 0)
      {
        osc[0].SetPhaseOffset(0);
        osc[1].SetPhaseOffset(0);
      }
      else
      {
        osc[0].SetPhaseOffset(pwm);
        osc[1].SetPhaseOffset(pwm);
      }
    }
    int GetOsc1WaveTableCount()
    {
      return osc[0].GetWaveTableCount();
    }
    int GetOsc2WaveTableCount()
    {
      return osc[1].GetWaveTableCount();
    }
    void SetOsc1PhaseOffset(uint8_t newphase)
    {
      osc[0].SetPhaseOffset(newphase/127.0);
    }
    void SetOsc2PhaseOffset(uint8_t newphase)
    {
      osc[1].SetPhaseOffset(newphase/127.0);
    }
    void MidiOsc1Wave(uint8_t newwave)
    {
      osc[0].SetWaveTable(newwave);
      wt1_idx = newwave;
    }
    void MidiOsc2Wave(uint8_t newwave)
    {
      osc[1].SetWaveTable(newwave);
      wt2_idx = newwave;
    }
    void SetFilterParameters(uint8_t filter_freq, uint8_t filter_q)
    {
      lowpass.SetParameters(filter_freq/127.0,filter_q/127.0);
    }
    float Process()
    {
      if(modulation<=0.01)
      {
        return (lowpass.Process(velocity*adsr[0].Process()*osc[0].Process()*fmod1+velocity*adsr[1].Process()*osc[1].Process()*fmod2));
      }
      else
      {
        return  lowpass.Process((velocity*adsr[0].Process()*osc[0].Process()*fmod1) + (velocity*adsr[1].Process()*osc[1].Process()*fmod2) + (velocity*(adsr[0].Process()*osc[0].Process()*osc[1].Process()*fmod3)));
      }
    }
    bool IsPlaying()
    {
        if(adsr[0].GetState()==ADSR::envState::env_idle && adsr[0].GetState()==ADSR::envState::env_idle)
        {
            return false;
        }
        return true;
    }
    
protected:
    FloatWaveTableOsc osc[2];
    ADSR adsr[2];
    double sampleRate;
    double freq1;
    double freq2;
    float velocity;
    float modulation;
    float pwm;
    float fmod1;
    float fmod2;
    float fmod3;
    float ffreq;
    float fq;
    LowPass lowpass;
    uint8_t wt1_idx;
    uint8_t wt2_idx;
};

#endif
