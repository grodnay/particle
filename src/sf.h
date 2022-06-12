//A partial implementation of the rs485 protocol for Digitax-SF AC Servo Drivers 
//Guy Rodnay
#ifndef _SF_H_
#define _SF_H_

#define RTS_PIN 2   
#define PULSE_PIN 3
#define DIR_PIN 4
#define UART Serial1
class sf_packet
{
public:
    uint8_t buff[35]; //packet data to transsmit
    uint8_t data_length = 2, destination = 1,
            toggle = 1, dir = 0, error_code = 0,
            command_code = 0, command_group = 0;
    uint8_t parameter_data[29]; //intermediate storage for parameter data
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
    uint8_t toggle=0,expected_responce_data_length=0;
    int rts_pin;
    int write(void);
    int read();
    int transaction(const uint8_t *_parameter_data, uint8_t _param_length,
                    uint8_t *_responce_param_data, uint8_t _responce_param_length,
                    uint8_t _command_code, unsigned long int _timeout);
public:
    void begin(int _rts_pin = RTS_PIN);
    void set_destination(uint8_t _destination) { out.destination = _destination; }
    int NOP(void);
    int GET_PARAM_2(uint16_t param_group, uint16_t &param);
    void print(void);
};

#endif // SF_H