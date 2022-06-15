#include <Arduino.h>
//#include <LibPrintf.h>
#include <stdio.h>
#include <string.h>
#include "SF.h"

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
    //char s[100]; sprintf(s,"%ld",res);
    //Particle.publish("Encoder",String(res));
    delay(500);
}
