#ifndef LowPass_h
#define LowPass_h
#include <Arduino.h>
/*
simple resonant filter posted to musicdsp.org by Paul Kellett http://www.musicdsp.org/archive.php?classid=3#259
// set feedback amount given f and q between 0 and 1
fb = q + q/(1.0 - f);
// for each sample...
buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
buf1 = buf1 + f * (buf0 - buf1);
out = buf1;
Taken from mozzi
*/

 class LowPass {
public:
    LowPass() {
        
    }
    ~LowPass(void) {
        
    }
    void SetParameters(float f, float q)
    {
        if(1.0-f<0.001)
        {
          f = 0.999;
        }
        this->f = f;
        this->q = q;
        fb = q + q/(1.0 - f);
    }
    
    float Process(float in)
    {
        
        buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
        buf1 = buf1 + f * (buf0 - buf1);
        return buf1;    
    }
    float GetFreq()
    {
      return f;
    }
    float GetRes()
    {
      return q;
    }
protected:
    float q;
    float f;
    float fb;
    float buf0;
    float buf1;
    
    
};

#endif
