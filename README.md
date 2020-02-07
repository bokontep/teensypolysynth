# teensypolysynth
A polyphonic 2 oscillator synth implemented on a teensy 4.0!

I am into making synths for microcontrollers and single board computers. I have developed a simple synth engine working with fixed point math (Fixie library: https://github.com/raroni/fixie by Rasmus Rønn Nielsen!) and wavetable oscillators and envelopes based on the work from Nigel Redmon at earlevel http://www.earlevel.com/main/2013/06/01/envelope-generators/. I have a version running on esp32 using the builtin dac's and a version working on raspberrypi/linux using an i2s codec for audio output. I recently got my hands on a teensy 4.0 and decided to adapt my existing code to work on this little beast. Audio output is via an i2s breakout utilising the UDA1334A that I got for under 5 euros from China. I wired the codec according to the following table:
UDA1334A Connections:
  
VIN - 3.3V on Teensy
    
GND - GND on Teensy
    
DIN - OUT1A on Teensy (pin 7)
    
WSEL - LRCLK1 on Teensy (pin 20)
    
BCLK - BCLK1 on Teensy (pin 21)
    
MCLK1 on Teensy (pin 23) can be left disconnected
    
I found the wiring on this thread https://forum.pjrc.com/threads/57925-Teensy-4-with-UDA1334A-I2S-decoder-MCLK-not-needed
on the pjrc forums. You can see a photo of the teensy and codec below:
![image](https://raw.githubusercontent.com/bokontep/teensypolysynth/master/images/IMG_20200206_143354.jpg)

You can test the synth by sending midi through usb. I used VMPK (Virtual Midi Piano Keyboard) under linux to test. Just make sure before downloading the code to teensy that you have selected Serial + MIDI under Arduino Tools/USBType!
There are things that need refinement though!
For example there are currently no lfos and there is just one lowpass filter per voice.

To built the code you just need to have the Teensyduino extension (I used version 1.50)

Currently the following midi CCs are supported:

| # | MIDI CC | Parameter |
| --- | --- | --- |
| 1 | 1 | Modulation |
| 2 | 16 | Osc1 Wave |
| 3 | 17 | Osc2 Wave |
| 4 | 18 | Osc1 ADSR Attack |
| 5 | 19 | Osc1 ADSR Decay |
| 6 | 20 | Osc1 ADSR Sustain |
| 7 | 21 | Osc1 ADSR Release |
| 8 | 22 | Osc2 ADSR Attack |
| 9 | 23 | Osc2 ADSR Decay |
| 10 | 24 | Osc2 ADSR Sustain |
| 11 | 25 | Osc2 ADSR Release |
| 12 | 26 | Filter Frequency |
| 13 | 27 | Filter Resonance |
| 14 | 28 | PWM |

Enjoy!
