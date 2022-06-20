#include <Arduino.h>

#include <stdio.h>
#include <string.h>

#include "sf.h"
#include "crc16.h"

uint16_t sf_packet::calc_crc16(void) const
{
    return crc_ccitt_false(0xffff, buff, (size_t)data_length + 2);
}

void sf_packet::memcpy_rev(uint8_t *dst, const uint8_t *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
        dst[i] = src[n - i - 1];
}

void sf_packet::encode(void)
{
    buff[0] = 0b00100000 | data_length;                           // A
    buff[1] = destination;                                        // B
    buff[2] = (dir << 7) | (toggle << 6) | error_code;            // C
    buff[3] = (command_group << 6) | command_code;                // D
    memcpy_rev(buff + 4, parameter_data, (size_t)param_length()); // E
    crc = calc_crc16();
    memcpy_rev(buff + data_length + 2, (uint8_t *)&crc, 2); // F
}

int sf_packet::decode(void)
{
    data_length = max(2, min(buff[0] & 0x1f, 31)); // A
    destination = buff[1];                         // B
    dir = buff[2] >> 7;                            // C
    toggle = (buff[2] >> 6) & 0x01;
    error_code = buff[2] & 0x0f;
    command_code = buff[3] & (0xff >> 2); // D
    command_group = buff[3] >> 6;
    memcpy_rev(parameter_data, buff + 4, (size_t)param_length()); // E
    memcpy_rev((uint8_t *)&crc, buff + data_length + 2, 2);       // F
    return check_crc();
}

void sf_packet::print(void)
{
    Serial.printf("len=%d, des=%d, tog=%d, dir=%d, err=%d, cod=0x%X, grp=%d, ", data_length, destination, toggle, dir, error_code, command_code, command_group);
    if (param_length() > 0)
    {
        Serial.printf("0x");
        for (int i = 0; i < param_length(); i++)
            Serial.printf("%X", parameter_data[i]);
    }
    else
        Serial.printf("None");
    Serial.printf(", crc=0x%02X", crc);
    if (check_crc())
        Serial.printf("(ok)");
    else
        Serial.printf("(BAD CRC!)");
    Serial.printf("\n");
}

void sf_packet::hexdump(void)
{
    for (int i = 0; i < packet_length(); i++)
        Serial.printf("%02X ", buff[i]);
    Serial.printf("\n");
}

int sf_protocol::write(void)
{
    digitalWrite(rts_pin, HIGH);
    while (UART.available())
    {
        UART.read();
    }
    int res = UART.write(out.buff, out.packet_length());
    UART.flush();
    digitalWrite(rts_pin, LOW);
    request_time = millis();
    return res;
}

int sf_protocol::read()
{
    UART.setTimeout(timeout);
    int res = UART.readBytes((char *)in.buff, expected_responce_data_length + 4);
    respone_time = millis();
    return res;
}

int sf_protocol::transaction(uint8_t _command_code, const uint8_t *_parameter_data, uint8_t _param_length, uint8_t *_responce_param_data, uint8_t _responce_param_length, unsigned long int _timeout)
{
    memcpy(out.parameter_data, _parameter_data, _param_length);
    out.command_code = _command_code;
    out.data_length = _param_length + 2;
    out.toggle = toggle = !toggle;
    timeout = _timeout;
    out.encode();
    actual_pause = millis() - respone_time;
    if (actual_pause < required_pause)
        delay(required_pause - actual_pause);
    write();
    expected_responce_data_length = _responce_param_length + 2;
    if (read() == expected_responce_data_length + 4)
    {
        required_pause = 5;
        int res = (in.decode() && (in.data_length == expected_responce_data_length) && (in.toggle == out.toggle) && (in.command_code == out.command_code) && (in.command_group == out.command_group) && (in.dir == 1));
        memcpy(_responce_param_data, in.parameter_data, _responce_param_length);
        if (!res)
            return -2;
        return in.error_code;
    }
    required_pause = 250;
    return -1;
}

void sf_protocol::begin(int _rts_pin)
{
    // MySerial=&_MySerial;
    rts_pin = _rts_pin;
    UART.begin(57600);
    pinMode(rts_pin, OUTPUT);
    digitalWrite(rts_pin, LOW);
}

void sf_protocol::print(void)
{
    Serial.printf("sent: ");
    out.print();
    Serial.printf("got: ");
    in.print();
    Serial.printf("exp_len=%d, ", expected_responce_data_length);
    Serial.printf("to=%ld, ", timeout);
    Serial.printf("t=%ld ", respone_time - request_time);
    Serial.printf("req_p=%ld, ", required_pause);
    Serial.printf("act_p=%ld, ", actual_pause);
    Serial.printf("\n");
}

int sf_protocol::NOP(void)
{
    return transaction(0x00, NULL, 0, NULL, 0, 20);
}
int sf_protocol::GET_PARAM_2(uint16_t param_group, uint16_t &param)
{
    return transaction(0x04, (uint8_t *)&param_group, sizeof(param_group), (uint8_t *)&param, sizeof(param), 20);
}
int sf_protocol::GET_PARAM_4(uint16_t param_group, uint32_t &param)
{
    return transaction(0x05, (uint8_t *)&param_group, sizeof(param_group), (uint8_t *)param, sizeof(param), 20);
}
int sf_protocol::SET_PARAM_2(uint16_t param_group, uint16_t param)
{
    uint16_t buff[] = {param, param_group}; // reversed order because encoding will revrese the byte order
    return transaction(0x07, (uint8_t *)&buff, sizeof(buff), NULL, 0, 20);
}
int sf_protocol::SET_PARAM_4(uint16_t param_group, uint32_t param)
{
#pragma pack(1)
    struct
    {
        uint32_t param;
        uint16_t param_group;
    } buff = {param, param_group}; // reversed order because encoding will revrese the byte order
#pragma pack(0)
    return transaction(0x09, (uint8_t *)&buff, sizeof(buff), NULL, 0, 20);
}
int sf_protocol::GET_STATE_VALUE_4(uint16_t param_group, uint32_t *param)
{
    return transaction(0x11, (uint8_t *)&param_group, sizeof(param_group), (uint8_t *)param, sizeof(param), 20);
}
int sf_protocol::UNLOCK_PARAM_ALL(uint16_t *unlock_code)
{
    return transaction(0x0A, NULL, 0, (uint8_t *)unlock_code, sizeof(unlock_code), 20);
}
int sf_protocol::SAVE_PARAMETER_ALL(uint16_t unlock_code)
{
    return transaction(0x0B, (uint8_t *)&unlock_code, sizeof(unlock_code), NULL, 0, 20);
}
int sf_protocol::GET_STATE_VALUE_2(uint16_t status_number, uint16_t &status_value)
{
    return transaction(0x10, (uint8_t *)&status_number, sizeof(status_number),
                       (uint8_t *)&status_value, sizeof(status_value), 20);
}
int sf_protocol::GET_STATE_VALUE_4(uint16_t status_number, uint32_t &status_value)
{
    return transaction(0x11, (uint8_t *)&status_number, sizeof(status_number),
                       (uint8_t *)&status_value, sizeof(status_value), 20);
}
int sf_protocol::SET_STATE_VALUE_WITHMASK_4(uint16_t status_number, uint32_t status_value, uint32_t mask,
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

