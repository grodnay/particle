/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
#include <Arduino.h>
//#include <LibPrintf.h>
#include <stdio.h>
#include <string.h>
#include "SF.h"

void setup(void);
void loop(void);
#line 7 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
sf_protocol sf;
int32_t res;

void setup(void)
{
    Serial.begin(115200);
    sf.begin(RTS_PIN);
    delay(100);
    Serial.printf("End setup\n");
    Particle.variable("Encoder",res);
}

long unsigned int i = 0;
void loop(void)
{
    Serial.printf("%ld. Encoder: (%d) %ld\n", i++, sf.GET_STATE_VALUE_4(195, (uint32_t *)&res), res);
    delay(500);
}
