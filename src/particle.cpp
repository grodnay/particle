/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
#include <Arduino.h>
//#include <LibPrintf.h>
#include <stdio.h>
#include <string.h>
#include "SF.h"

int move(String s);
int bit_on(String s);
int bit_off(String s);
int ext_speed(String s);
void setup(void);
void loop(void);
#line 7 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
MySF sf;
int32_t enc;
int16_t tmp;
int rpm, rpm_cmd;
double torque;
int move(String s)
{
    int rpm_cmd = s.toInt();
    if (rpm_cmd > 0)
    {
        sf.set_speed(rpm_cmd);
        sf.fw();
    }
    else if (rpm_cmd < 0)
    {
        sf.set_speed(rpm_cmd);
        sf.rev();
    }
    else
        sf.stop();
    return rpm_cmd;
}
int bit_on(String s)
{
    return sf.in_bit_on(s.toInt());
}
int bit_off(String s)
{
    return sf.in_bit_off(s.toInt());
}
int ext_speed(String s)
{
    int rpm_cmd = s.toInt();
    return sf.set_ext_speed(rpm_cmd);
}

void setup(void)
{
    Serial.begin(115200);
    sf.begin();
    delay(100);
    Particle.variable("Encoder", enc);
    Particle.variable("RPM", rpm);
    Particle.variable("Torque", torque);
    Particle.function("bit_on", bit_on);
    Particle.function("bit_off", bit_off);
    Particle.function("cmd", move);

    Serial.printf("End setup\n");
}

long unsigned int i = 0;
void loop(void)
{

    Serial.printf("%ld. Encoder: (%d) %ld\n", i++, sf.get_encoder(enc), enc);
    Serial.printf("%ld. rpm: (%d) %d\n", i++, sf.get_rpm(tmp), tmp);
    rpm = tmp;
    Serial.printf("%ld. torque: (%d) %2.2f\n", i++, sf.get_torque(torque), torque);
    Serial.printf("%ld. Remote op: (%d)\n", i++, sf.operation_mode());
    // Serial.printf("%ld. Servo on: (%d)\n", i++, sf.servo_on());

    delay(500);
}
