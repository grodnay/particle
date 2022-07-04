/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
#include <Arduino.h>
//#include <LibPrintf.h>
#include <stdio.h>
#include <string.h>
#include "SF.h"

int svon(String s);
int rst(String s);
int pclr(String s);
int ext_speed(String s);
int power(String s);
int pump(String s);
void setup(void);
void loop(void);
#line 7 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
#define POWER_PIN D6
#define PUMP_PIN D2
#define PRESSURE_PIN A0
#define FORWARD_PIN A3
#define STOP_PIN A2
#define REVERSE_PIN A1

MySF sf;
int32_t enc;
int16_t tmp;
int rpm, rpm_cmd;
double torque;
// int move(String s)
// {
//     int rpm_cmd = s.toInt();
//     if (rpm_cmd > 0)
//     {
//         sf.set_speed(rpm_cmd);
//         sf.fw();
//     }
//     else if (rpm_cmd < 0)
//     {
//         sf.set_speed(abs(rpm_cmd));
//         sf.rev();
//     }
//     else
//         sf.stop();
//     return rpm_cmd;
// }
// int bit_on(String s)
// {
//     return sf.in_bit_on(s.toInt());
// }
// int bit_off(String s)
// {
//     return sf.in_bit_off(s.toInt());
// }
// int servo(String s)
// {
//     int cmd = s.toInt();
//     if (cmd > 0)
//         return sf.servo_on();
//     return sf.servo_off();
// }
int svon(String s)
{
    return sf.set_svon(s.toInt());
}
int rst(String s)
{
    return sf.reset();
}
int pclr(String s)
{
    return sf.pclr();
}
int ext_speed(String s)
{
    int rpm_cmd = s.toInt();
    return sf.set_ext_speed(rpm_cmd, 0);
}
int power(String s)
{
    int cmd = s.toInt();
    if (cmd > 0)
        digitalWrite(POWER_PIN, HIGH);
    else
        digitalWrite(POWER_PIN, LOW);
    return 1;
}
int pump(String s)
{
    int cmd = s.toInt();
    if (cmd > 0)
        digitalWrite(PUMP_PIN, HIGH);
    else
        digitalWrite(PUMP_PIN, LOW);
    return 1;
}

void setup(void)
{
    Serial.begin(115200);
    sf.begin();
    delay(100);
    Particle.variable("Encoder", enc);
    Particle.variable("RPM", rpm);
    Particle.variable("Torque", torque);
    // Particle.function("bit_on", bit_on);
    // Particle.function("bit_off", bit_off);
    // Particle.function("int_cmd", move);
    Particle.function("ext_cmd", ext_speed);
    Particle.function("power", power);
    Particle.function("svon", svon);
    Particle.function("reset", rst);
    Particle.function("pclr", pclr);
    Particle.function("pump", pump);

    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, LOW);
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);
    pinMode(FORWARD_PIN, INPUT_PULLUP);
    pinMode(STOP_PIN, INPUT_PULLUP);
    pinMode(REVERSE_PIN, INPUT_PULLUP);

    delay(5000);
    Serial.printf("End setup\n");
}

long unsigned int i = 0;
void loop(void)
{
    //  sf.operation_mode(1);
    // sf.servo_on();
    Serial.printf("%ld. Encoder: (%d) %ld\n", i++, sf.get_encoder(enc), enc);
    Serial.printf("%ld. rpm: (%d) %d\n", i++, sf.get_rpm(tmp), tmp);
    rpm = tmp;
    Serial.printf("%ld. torque: (%d) %2.2f\n", i++, sf.get_torque(torque), torque);
    // Serial.printf("%ld. Remote op: (%d)\n", i++, sf.operation_mode());
    //   Serial.printf("%ld. Servo on: (%d)\n", i++, sf.servo_on());
    Serial.printf("\n");
    Serial.printf("RSFP: %ld%ld%ld%ld\n", digitalRead(REVERSE_PIN), digitalRead(STOP_PIN), digitalRead(FORWARD_PIN), analogRead(A0));

    delay(500);
}
