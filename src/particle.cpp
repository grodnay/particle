/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
#include <Arduino.h>
//#include <LibPrintf.h>
#include <stdio.h>
#include <string.h>
#include "sf.h"
#include <Particle.h>
void StatusdThreadFunction(void);
int svon(String s);
int rst(String s);
int pclr(String s);
int ext_speed(String s);
int power(String s);
int pump(String s);
void setup(void);
void loop(void);
#line 7 "/Users/guyrodnay/Dropbox/Guy/work/2022/pv/code/particle/src/particle.ino"
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler;

#define POWER_PIN D8
#define PUMP_PIN D7
#define PRESSURE_PIN A0
#define FORWARD_PIN A3
#define STOP_PIN A2
#define REVERSE_PIN A1

MySF sf;
int32_t enc = 0;
int32_t rpm = 0;
double torque = 0.0;
int32_t pressure = 0;
int32_t alarmm = 0;
int32_t warn = 0;
int32_t io_status = 0;
int res = 0;
int dirty=1;

void StatusdThreadFunction(void)
{
    unsigned long int timeout=100, t0 = millis();
    while (true)
    {
        int16_t tmp = 0;
        uint16_t utmp;
        int res = TRUE;

        res = (res && (sf.get_encoder(enc) >= 0));

        res = (res && (sf.get_io(utmp) >= 0));
        io_status = utmp;

        res = (res && (sf.get_rpm(tmp) >= 0));
        rpm = int32_t(tmp);

        res = (res && (sf.get_io(utmp) >= 0));
        io_status = utmp;

        res = (res && (sf.get_torque(torque) >= 0));

        res = (res && (sf.get_io(utmp) >= 0));
        io_status = utmp;

        // res = sf.get_alarm(alarmm);

        // sf.get_io(utmp);
        // io_status = utmp;

        // sf.get_warning(utmp);
        // warn = utmp;

        // sf.get_io(utmp);
        // io_status = utmp;

        pressure = analogRead(PRESSURE_PIN);

        if (res)
        {
            t0 = millis();
            dirty = 0;
        }
        if (millis() - t0 > timeout)
            dirty = 1;
    }
    // You must not return from the thread function
}

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
    rpm_cmd = constrain(rpm_cmd, -500, 500);
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
    //   Serial.begin(115200);
    sf.begin();
    //   delay(100);
    Particle.variable("Encoder", enc);
    Particle.variable("RPM", rpm);
    Particle.variable("Torque", torque);
    Particle.variable("Pressure", pressure);
    Particle.variable("Alarm", alarmm);
    Particle.variable("res", res);
    Particle.variable("warn", warn);
    Particle.variable("io", io_status);

    // Particle.function("bit_on", bit_on);
    // Particle.function("bit_off", bit_off);
    // Particle.function("int_cmd", move);
    Particle.function("ext_cmd", ext_speed); // Move the motor: Reverse=-500, Stop=0, Forward=500. Careful; There in no protection from overspeed
    Particle.function("power", power);       // Power: On=1, Off=0
    Particle.function("svon", svon);         // Servo: On=1, Off=0
    Particle.function("reset", rst);         // Reset: Ignores the argument
    Particle.function("pclr", pclr);         // Counter clear: Ignores the argument
    Particle.function("pump", pump);         // Pump: On=1, Off=0

    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, LOW);
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);
    pinMode(FORWARD_PIN, INPUT_PULLUP);
    pinMode(STOP_PIN, INPUT_PULLUP);
    pinMode(REVERSE_PIN, INPUT_PULLUP);
    // delay(500);
    // Serial.printf("End setup\n");
    new Thread("StatusThread", StatusdThreadFunction);
}

long unsigned int i = 0;
void loop(void)
{
    //  sf.operation_mode(1);
    // sf.servo_on();

    // Serial.printf("%ld. Encoder: (%d) %ld\n", i++, sf.get_encoder(enc), enc);
    // Serial.printf("%ld. rpm: (%d) %d\n", i++, sf.get_rpm(tmp), tmp);
    // rpm = int32_t(tmp);
    // Serial.printf("%ld. torque: (%d) %2.2f\n", i++, sf.get_torque(torque), torque);
    // Serial.printf("%ld. alarm: (%d) %ld\n", i++, sf.get_alarm(alarmm), alarmm);

    // // Serial.printf("%ld. Remote op: (%d)\n", i++, sf.operation_mode());
    // //   Serial.printf("%ld. Servo on: (%d)\n", i++, sf.servo_on());
    // Serial.printf("\n");
    // Serial.printf("RSFP: %ld%ld%ld %ld\n", digitalRead(REVERSE_PIN), digitalRead(STOP_PIN), digitalRead(FORWARD_PIN), analogRead(PRESSURE_PIN));

    if ((io_status & (1 << 14)) == 0)
        ext_speed("500");
    else if ((io_status & (1 << 13)) == 0)
        ext_speed("-500");

    if (!digitalRead(STOP_PIN))
    {
        digitalWrite(POWER_PIN, HIGH);
        ext_speed("0");
    }
    else if (!digitalRead(REVERSE_PIN))
        ext_speed("-500");
    else if (!digitalRead(FORWARD_PIN))
        ext_speed("500");

    // delay(500);
}
