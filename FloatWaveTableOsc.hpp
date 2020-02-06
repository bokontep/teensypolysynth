//
//  WaveTableOsc.h
//
//  Created by Nigel Redmon on 2018-10-05
//  EarLevel Engineering: earlevel.com
//  Copyright 2018 Nigel Redmon
//
//  For a complete explanation of the wavetable oscillator and code,
//  read the series of articles by the author, starting here:
//  www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
//
//  This version has optimizations described here:
//  www.earlevel.com/main/2019/04/28/wavetableosc-optimized/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#ifndef FloatWaveTableOsc_h
#define FloatWaveTableOsc_h
#include <Arduino.h>



class  FloatWaveTableOsc {
public:
    FloatWaveTableOsc(void) {
        for (int idx = 0; idx < numWaveTableSlots; idx++) {
            
            mWaveTables[idx].waveTableLen = 0;
            mWaveTables[idx].waveTable = 0;
        }
    }
    ~FloatWaveTableOsc(void) {
        for (int idx = 0; idx < numWaveTableSlots; idx++) {
            float *temp = mWaveTables[idx].waveTable;
            if (temp != 0)
                delete [] temp;
        }
    }



    
    void SetFrequency(double freq, double sampleRate)
    {
      SetFrequency (freq/sampleRate);
    }
    //
    // SetFrequency: Set normalized frequency, typically 0-0.5 (must be positive and less than 1!)
    //
    void SetFrequency(float inc) {
        mPhaseInc = inc;

        // update the current wave table selector

    }

    //
    // SetPhaseOffset: Phase offset for PWM, 0-1
    //
    void SetPhaseOffset(float offset) {
        mPhaseOfs = offset;
    }

    //
    // UpdatePhase: Call once per sample
    //
    void UpdatePhase(void) {
        mPhasor += mPhaseInc;

        if (mPhasor >= 1.0)
            mPhasor -= 1.0;
    }

    //
    // Process: Update phase and get output
    //
    float Process(void) {
        UpdatePhase();
        if(mPhaseOfs!=0)
        {
          return GetOutputMinusOffset(); 
        }
        else
        {
          return GetOutput();
        }
    }

    //
    // GetOutput: Returns the current oscillator output
    //
    float GetOutput(void) {
        waveTable *waveTable = &mWaveTables[mCurWaveTable];

        // linear interpolation
        float temp = mPhasor *  (waveTable->waveTableLen);
        int32_t intPart = floor(temp);
        float fracPart = temp - intPart;
        float samp0 = (waveTable->waveTable[intPart]);
        float samp1 = (waveTable->waveTable[intPart + 1]);
        return samp0 + (samp1 - samp0) * fracPart;
    }

    //
    // getOutputMinusOffset
    //
    // for variable pulse width: initialize to sawtooth,
    // set phaseOfs to duty cycle, use this for osc output
    //
    // returns the current oscillator output
    //
    float GetOutputMinusOffset() {
        waveTable *waveTable = &mWaveTables[mCurWaveTable];
        float len = waveTable->waveTableLen;
        float *wave = waveTable->waveTable;



        float temp = mPhasor * len;
        int32_t intPart = floor(temp);
        float fracPart = temp - intPart;
        float samp0 = (wave[intPart]);
        float samp1 = (wave[intPart+ 1]);
        float samp = samp0 + (samp1 - samp0) * fracPart;

        // and linear again for the offset part
        float offsetPhasor = mPhasor + mPhaseOfs;
        if (offsetPhasor > 1.0)
            offsetPhasor -= 1.0;
        temp = offsetPhasor * len;
        intPart = temp;
        fracPart = temp - intPart;
        samp0 = wave[intPart];
        samp1 = wave[intPart+1];
        return samp - (samp0 + (samp1 - samp0) * fracPart);
    }

    //
    // AddWaveTable
    //
    // add wavetables
    // wavetables within an oscillator can be different lengths
    //
    // returns 0 upon success, or the number of wavetables if no more room is available
    //
    

    int AddSharedWaveTable(int32_t len, float *waveTableIn) {
        if (mNumWaveTables < numWaveTableSlots) {
            mWaveTables[mNumWaveTables].waveTable = waveTableIn;
            mWaveTables[mNumWaveTables].waveTableLen = len-1;
            
            ++mNumWaveTables;

            

            return 0;
        }
        return mNumWaveTables;
    }

    int AddWaveTable(int32_t len, float *waveTableIn) {
        if (mNumWaveTables < numWaveTableSlots) {
            float *waveTable = mWaveTables[mNumWaveTables].waveTable = new float[len + 1];
            mWaveTables[mNumWaveTables].waveTableLen = len;
            
            ++mNumWaveTables;

            // fill in wave
            for (long idx = 0; idx < len; idx++)
                waveTable[idx] = waveTableIn[idx];
            waveTable[len] = waveTable[0];  // duplicate for interpolation wraparound

            return 0;
        }
        return mNumWaveTables;
    }

    void ResetPhase()
    {
      mPhasor = 0.0;
    }
    void SetWaveTable(int waveTableIdx)
    {
      this->mCurWaveTable = waveTableIdx%mNumWaveTables;
    }
    int GetWaveTableCount()
    {
      return this->mNumWaveTables;
    }
protected:
    float mPhasor = 0.0;       // phase accumulator
    float mPhaseInc = 0.0;     // phase increment
    float mPhaseOfs = 0.0;     // phase offset for PWM

    // array of wavetables
    int32_t mCurWaveTable = 0;      // current table, based on current frequency
    int32_t mNumWaveTables = 0;     // number of wavetable slots in use
    struct waveTable {
      
        int32_t waveTableLen;
        float *waveTable;
    };
    static constexpr int numWaveTableSlots = 256;    // simplify allocation with reasonable maximum
    waveTable mWaveTables[numWaveTableSlots];
};

#endif
