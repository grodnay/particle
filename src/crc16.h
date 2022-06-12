// crc16.h

#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>
#include <stddef.h>
                                                                                                      


uint16_t crc_ccitt(uint16_t crc, uint8_t const *buffer, size_t len);
uint16_t crc_ccitt_false(uint16_t crc, uint8_t const *buffer, size_t len);



#endif // _CRC16_H_

// EOF