//
//  ADSR.cpp
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//
//  1.01  2016-01-02  njr   added calcCoef to SetTargetRatio functions that were in the ADSR widget but missing in this code
//  1.02  2017-01-04  njr   in calcCoef, checked for rate 0, to support non-IEEE compliant compilers
//


#include "ADSR.h"



ADSR::ADSR() {
  Reset();
  this->sampleRate = 44100;
  SetADSR(1.0, 1.0, 1.0, 1.0);
    
}

ADSR::ADSR(float sampleRate) {
  Reset();
	this->sampleRate = sampleRate;
	SetADSR(1.0, 1.0, 1.0, 1.0);
    
}

ADSR::~ADSR(void) {
}

void ADSR::SetADSR(byte attack, byte decay, byte sustain, byte release)
{
	SetSustainMidi(sustain);
	SetAttackMidi(attack);
	SetDecayMidi(decay);
	SetReleaseMidi(release);
}
void ADSR::SetAttack(float attack) {

  this->attack = attack;
	if (attack < 0.001)
	{
		attack = 0.001;
	}
  
    attackCoef = 1.0/attack;
    attackBase = 0.0;
    
}


void ADSR::SetDecay(float decay) {
	this->decay = decay;
	if (decay < 0.001)
	{
		decay = 0.001;
	}
	decayCoef = (1.0 - this->sustainLevel) / this->decay;
}

    


void ADSR::SetSustain(float level) {
    sustainLevel = level;
    decayBase = sustainLevel;
}

void ADSR::SetRelease(float releaseval)
{
	release = releaseval;
	if (release < 0.001)
	{
		release = 0.001;
	}
	releaseCoef = (this->sustainLevel / this->release);
	releaseBase = this->sustainLevel;
}

void ADSR::SetAttackMidi(byte attack)
{
  this->midiAttack=attack;
  this->attack = ((float)midiAttack)/127.0;
  double maxlength = 1.0*sampleRate;
  if(midiAttack==0)
  {
    attackCoef = 1.0;
  }
  else
  {
    attackCoef = 127.0 /(maxlength*(double)midiAttack);
  }
  attackBase = 0.0;
  
}
void ADSR::SetDecayMidi(byte decay)
{
  this->midiDecay=decay;
  this->decay = ((float)this->midiDecay)/127.0;
  double maxlength = 1.0*sampleRate;
  if(midiDecay==0)
  {
    decayCoef=1.0 - this->sustainLevel;  
  }
  else
  {
    decayCoef = ((1.0 - this->sustainLevel)*127.0) / (maxlength*(float)midiDecay);
  }
  
}

void ADSR::SetSustainMidi(byte sustain)
{
  this->midiSustain=sustain;
  this->sustainLevel = ((float)midiSustain)/127.0;
  decayBase = sustainLevel;
}

void ADSR::SetReleaseMidi(byte release)
{
  this->midiRelease=release;
  this->release = ((float)this->midiRelease)/127.0;
  double maxlength = sampleRate*5.0;
  
  if(midiRelease==0)
  {
    releaseCoef=this->sustainLevel;
  }
  else
  {
    releaseCoef = (this->sustainLevel*127.0) / (maxlength*((double)midiRelease));
    //releaseCoef = ((this->sustainLevel * 127.0)/maxlength)/(float)midiRelease; 
  }
  releaseBase = this->sustainLevel;
}
