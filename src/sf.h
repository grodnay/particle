// A partial implementation of the rs485 protocol for Digitax-SF AC Servo Drivers
// Guy Rodnay
#ifndef _SF_H_
#define _SF_H_

#define RTS_PIN 2
#define PULSE_PIN 3
#define DIR_PIN 4
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
    int NOP(void)
    {
        return transaction(0x00, NULL, 0, NULL, 0, 20);
    }
    int GET_PARAM_2(uint16_t param_group, uint16_t &param)
    {
        return transaction(0x04, (uint8_t *)&param_group, sizeof(param_group), (uint8_t *)&param, sizeof(param), 20);
    }
    int GET_PARAM_4(uint16_t param_group, uint32_t &param)
    {
        return transaction(0x05, (uint8_t *)&param_group, sizeof(param_group), (uint8_t *)param, sizeof(param), 20);
    }
    int SET_PARAM_2(uint16_t param_group, uint16_t param)
    {
        uint16_t buff[] = {param, param_group}; // reversed order because encoding will revrese the byte order
        return transaction(0x07, (uint8_t *)&buff, sizeof(buff), (uint8_t *)&param, sizeof(param), 20);
    }
    int SET_PARAM_4(uint16_t param_group, uint32_t param)
    {
#pragma pack(1)
        struct
        {
            uint32_t param;
            uint16_t param_group;
        } buff = {param, param_group}; // reversed order because encoding will revrese the byte order
#pragma pack(0)
        return transaction(0x09, (uint8_t *)&buff, sizeof(buff), (uint8_t *)&param, sizeof(param), 20);
    }
    int GET_STATE_VALUE_4(uint16_t param_group, uint32_t *param)
    {
        return transaction(0x11, (uint8_t *)&param_group, sizeof(param_group), (uint8_t *)param, sizeof(param), 20);
    }
    int UNLOCK_PARAM_ALL(uint16_t *unlock_code)
    {
        return transaction(0x0A, NULL, 0, (uint8_t *)unlock_code, sizeof(unlock_code), 20);
    }
    int SAVE_PARAMETER_ALL(uint16_t unlock_code)
    {
        return transaction(0x0B, (uint8_t *) &unlock_code, sizeof(unlock_code), NULL, 0, 20);
    }
    int GET_STATE_VALUE_2(uint16_t status_number, uint16_t &status_value)
    {
        return transaction(0x10, (uint8_t *)&status_number, sizeof(status_number),
                           (uint8_t *)&status_value, sizeof(status_value), 20);
    }
    int GET_STATE_VALUE_4(uint16_t status_number, uint32_t &status_value)
    {
        return transaction(0x11, (uint8_t *)&status_number, sizeof(status_number),
                           (uint8_t *)&status_value, sizeof(status_value), 20);
    }
    int SET_STATE_VALUE_WITHMASK_4(uint16_t status_number, uint32_t status_value, uint32_t mask,
                                   uint16_t &execution_result, uint32_t &status_value_returned)
    {
#pragma pack(1)
        struct
        {
            uint32_t mask, status_value;
            uint16_t status_number;
        } out_buff = {mask, status_value, status_number};
        struct
        {
            uint32_t status_value;
            uint16_t execution_result;
        } in_buff;
#pragma pack(0)
        int ret = transaction(0x66, (uint8_t *)&out_buff, sizeof(out_buff), (uint8_t *)&in_buff, sizeof(in_buff), 20);
        execution_result = in_buff.execution_result;
        status_value_returned = in_buff.status_value;
        return ret;
    }
    // int get_encoder_angle(int32_t & angle){return GET_STATE_VALUE_4(0xc3, angle);}
    void print(void);
};

class MySF : public sf_protocol
{
    const float rated_torqu = 2.0;

public:
    int get_torqu(float &t)
    {
        int16_t mils;
        int res = GET_STATE_VALUE_2(113, (uint16_t &)mils);
        t = (float)mils * rated_torqu / 1000.0;
        return res;
    }
    int get_rpm(int &rpm)
    {
        return GET_STATE_VALUE_2(113, (uint16_t &)rpm);
    }
    int in_bit_on(int i)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return  SET_STATE_VALUE_WITHMASK_4(288, 1<<i, 1<<i, execution_res, status_value);
    }
    int in_bit_off(int i)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return  SET_STATE_VALUE_WITHMASK_4(288, 0, 1<<i, execution_res, status_value);
    }
    int servo_on(void)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return  SET_STATE_VALUE_WITHMASK_4(288, 1, 1, execution_res, status_value);
    }
    int servo_off(void)
    {
        uint16_t execution_res;
        uint32_t status_value;
        return SET_STATE_VALUE_WITHMASK_4(288, 0, 1, execution_res, status_value);
    }
};
#endif // SF_H