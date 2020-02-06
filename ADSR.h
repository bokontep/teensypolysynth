//
//  ADSR.h
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

#ifndef ADSR_h
#define ADSR_h
#include <Arduino.h>

class  ADSR {
public:
	ADSR(void);
	~ADSR(void);

	float Process(void);
    float GetOutput(void);
    int GetState(void);
	void SetADSR(float attack, float decay, float sustain, float release);
	void Gate(int on);
    void SetAttack(float attack);
    void SetDecay(float decay);
    void SetRelease(float release);
	void SetSustain(float level);
    float GetAttack(){return attack;}
    float GetDecay(){return decay;}
    float GetSustain(){return sustainLevel;}
    float GetRelease(){return release;}
    void Reset(void);

    enum envState {
        env_idle = 0,
        env_attack,
        env_decay,
        env_sustain,
        env_release
    };

protected:

    int state;
    float output;
	float attack;
	float decay;
	float release;
	int32_t counter;
	float attackCoef;
	float decayCoef;
	float releaseCoef;
	float sustainLevel;
    
    float attackBase;
    float decayBase;
    float releaseBase;
 
    
};

inline float ADSR::Process() {
	switch (state) {
        case env_idle:
			output = 0.0;
            break;
        case env_attack:
            output = output+attackBase +  attackCoef;
			
            if (output >= 1.0) {
                output = 1.0; //http://www.earlevel.com/main/2013/06/01/envelope-generators/
                state = env_decay;
				
            }
            break;
        case env_decay:
            output = output - decayCoef;
			
            if (output <= sustainLevel) {
                output = sustainLevel;
                state = env_sustain;
            }
            break;SetWaveTable
        case env_sustain:
			//output = sustainLevel;
            break;
        case env_release:
            output = output - releaseCoef;
			
            if (output <= 0.0) {
                output = 0.0;
                state = env_idle;
            }
			break;
	}
	return output;
}

inline void ADSR::Gate(int gate) {
	if (gate)
	{

		output = 0.0;
		state = env_attack;
	}
	else if (state != env_idle)
	{
		state = env_release;
		
	}
}

inline int ADSR::GetState() {
    return state;
}

inline void ADSR::Reset() {
    state = env_idle;
    output = 0.0;
}

inline float ADSR::GetOutput() {
	return output;
}

#endif
