// A partial implementation of the rs485 protocol for Digitax-SF AC Servo Drivers
// Guy Rodnay
#ifndef _SF_H_
#define _SF_H_
#include <Arduino.h>

#define RTS_PIN A5
#define PULSE_PIN D8
#define DIR_PIN D7
#define SVON_PIN D3
#define RESET_PIN D4
#define PCLR_PIN D5

#define UART Serial1

class sf_packet
{
public:
    uint8_t buff[35]; // packet data to transsmit
    uint8_t data_length = 2, destination = 1,
            toggle = 1, dir = 0, error_code = 0,
            command_code = 0, command_group = 0;
    uint8_t parameter_data[29]; // intermediate storage for parameter data
    uint16_t crc;
    uint16_t calc_crc16(void) const;
    int check_crc(void) { return (calc_crc16() == crc); }
    void memcpy_rev(uint8_t *dst, const uint8_t *src, size_t n);
    uint8_t param_length(void) { return min(data_length - 2, 29); }
    uint8_t packet_length(void) { return min(data_length + 4, 35); }
    void encode(void);
    int decode(void);
    void print(void);
    void hexdump(void);
};

class sf_protocol
{
private:
    sf_packet in, out;
    long unsigned int request_time = 0,
                      respone_time = 0,
                      timeout = 200,
                      required_pause = 5,
                      actual_pause = 0;
    uint8_t toggle = 0, expected_responce_data_length = 0;
    int rts_pin;
    int write(void);
    int read();
    int transaction(uint8_t _command_code,
                    const uint8_t *_parameter_data, uint8_t _param_length,
                    uint8_t *_responce_param_data, uint8_t _responce_param_length,
                    unsigned long int _timeout);

public:
    void begin(int _rts_pin = RTS_PIN);
    void set_destination(uint8_t _destination) { out.destination = _destination; }
    int NOP(void);
    int GET_PARAM_2(uint16_t param_group, uint16_t &param);
    int GET_PARAM_4(uint16_t param_group, uint32_t &param);
    int SET_PARAM_2(uint16_t param_group, uint16_t param);
    int SET_PARAM_4(uint16_t param_group, uint32_t param);
    int GET_STATE_VALUE_4(uint16_t param_group, uint32_t *param);
    int UNLOCK_PARAM_ALL(uint16_t *unlock_code);
    int SAVE_PARAMETER_ALL(uint16_t unlock_code);
    int GET_STATE_VALUE_2(uint16_t status_number, uint16_t &status_value);
    int GET_STATE_VALUE_4(uint16_t status_number, uint32_t &status_value);
    int SET_STATE_VALUE_WITHMASK_4(uint16_t status_number, uint32_t status_value, uint32_t mask,
                                   uint16_t &execution_result, uint32_t &status_value_returned);
    void print(void);
};

class MySF : public sf_protocol
{
    const float rated_torqu = 2.39; // reduction = 15.0, wheel_diameter=0.2;
    const uint8_t pulse_pin = PULSE_PIN, dir_pin = DIR_PIN, svon_pin = SVON_PIN, reset_pin = RESET_PIN, pclr_pin = PCLR_PIN;
    const uint long count_per_revolution = 2 ^ 17;

public:
    void begin(void)
    {
        sf_protocol::begin();
        pinMode(dir_pin, OUTPUT);
        pinMode(pulse_pin, OUTPUT);
        noTone(pulse_pin);
        pinMode(reset_pin, OUTPUT);
        pinMode(svon_pin, OUTPUT);
        pinMode(pclr_pin, OUTPUT);
    }
    int get_torque(double &t)
    {
        int16_t mils;
        int res = GET_STATE_VALUE_2(113, (uint16_t &)mils);
        t = (float)mils * rated_torqu / 1000.0;
        return res;
    }
    int get_rpm(int16_t &rpm)
    {
        return GET_STATE_VALUE_2(98, (uint16_t &)rpm);
    }
    int get_encoder(int32_t &count)
    {
        return GET_STATE_VALUE_4(195, (uint32_t *)&count);
    }

    int in_bit_on(int i)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, 1 << i, 1 << i, execution_res, status_value);
    }
    int in_bit_off(int i)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, 0, 1 << i, execution_res, status_value);
    }
    enum vel_bits
    {
        VCRUN1 = 1 << 24,
        VCRUN2 = 1 << 25,
        VCSEL1 = 1 << 26,
        VCSEL2 = 1 << 27,
        VCSEL3 = 1 << 28,
        SVON = 1 << 0
    };
    int servo_on(void)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, SVON, SVON, execution_res, status_value);
    }
    int servo_off(void)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, 0, SVON, execution_res, status_value);
    }
    int fw(void)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, VCRUN2, VCRUN1 | VCRUN2, execution_res, status_value);
    }
    int rev(void)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, VCRUN1, VCRUN1 | VCRUN2, execution_res, status_value);
    }
    int stop(void)
    {
        ;
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, 0, VCRUN1 | VCRUN2, execution_res, status_value);
    }
    int set_speed(uint i)
    {
        uint32_t speed[] = {0, VCSEL1, VCSEL2, VCSEL1 | VCSEL2, VCSEL3, VCSEL3 | VCSEL1, VCSEL3 | VCSEL2, VCSEL3 | VCSEL1 | VCSEL2};
        i = max(1, min(abs(i), sizeof(speed) / sizeof(speed[0])));
        i--;
        Serial.printf("i=%d\n", i);
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, speed[i], VCSEL1 | VCSEL2 | VCSEL3, execution_res, status_value);
    }

    int set_ext_speed(int frequency, uint long duration = 5)
    {
        digitalWrite(dir_pin, (frequency > 0));
        if (frequency == 0)
            noTone(pulse_pin);
        else
        {
            noTone(pulse_pin);
            tone(pulse_pin, abs(frequency), duration);
        }
        return 1;
    }
    int control_mode(uint16_t mode = 0)
    {
        return SET_PARAM_2(2, mode);
    }
    int command_mode(uint16_t mode = 1)
    {
        return SET_PARAM_2(3, mode);
    }
    int input_pulse_form(uint16_t mode = 0);
    int operation_mode(uint16_t bit = 1)
    {
        return SET_PARAM_2(9, bit);
    }
    int set_svon(int i)
    {
        digitalWrite(svon_pin, i);
        return 1;
    }
    int reset()
    {
        digitalWrite(reset_pin, 1);
        delay(250);
        digitalWrite(reset_pin, 0);
        return 1;
    }
    int pclr()
    {
        digitalWrite(pclr_pin, 1);
        delay(250);
        digitalWrite(pclr_pin, 0);
        return 1;
    }
};
#endif // SF_H