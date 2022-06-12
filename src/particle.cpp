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

void setup(void)
{
    Serial.begin(115200);
    sf.begin(RTS_PIN);
    delay(100);
    Serial.printf("End setup\n");
}

long unsigned int i = 0;
void loop(void)
{
    Serial.printf("%ld. NOP: (%d)\n", i++,sf.NOP());
    sf.print();
    delay(5);
    uint16_t res;
    Serial.printf("%ld. GET_PARAM2: (%d) %d\n",i++, sf.GET_PARAM_2(238, res), res);
    sf.print();
    Serial.printf("\n");
    delay(1000);
}
