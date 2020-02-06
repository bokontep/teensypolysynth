
#include "AD.h"



AD::AD(void) {
    Reset();
    SetAD(1.0, 1.0);
    
}

AD::~AD(void) {
}

void AD::SetAD(float attack, float decay)
{
  
  SetAttack(attack);
  SetDecay(decay);
  
}
void AD::SetAttack(float attack) {
    this->attack = attack;
  if (attack < 1.0)
  {
    attack = 1.0;
  }
    attackCoef = 1.0/attack;
    attackBase = 0.0;
}


void AD::SetDecay(float decay) {
  this->decay = decay;
  if (decay < 1.0)
  {
    decay = 1.0;
  }
  decayCoef = 1.0/decay;
  
}

    
