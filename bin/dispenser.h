#ifndef DISPENSER_H
#define DISPENSER_H

#include <stdint.h>

#define STX 0x02
#define ETX 0x03

#define CMD_DISPENSER_CHECK_SOFTWARE_VERSION            0x30, 0x30

#define CMD_DISPENSER_SEND_CARD_TO_READ_POSITION        0x31, 0x30
#define CMD_DISPENSER_SEND_CARD_TO_DISPENSE_POSITION    0x31, 0x31
#define CMD_DISPENSER_SEND_CARD_TO_READER_HOST_POSITION 0x31, 0x32

#define CMD_DISPENSER_CHECK_DEVICE_VERSION              0x31, 0x40
#define CMD_DISPENSER_CHECK_DEVICE_STATUS               0x31, 0x44

#define CMD_DISPENSER_CHECK_STATUS                      0x32, 0x30
#define CMD_DISPENSER_ACCEPT_CARD_TO_RECYCLE            0x33, 0x30
#define CMD_COLLECT_CARD                                CMD_DISPENSER_ACCEPT_CARD_TO_RECYCLE

#define DISPENSER_STATUS_BOX_EMPTY          0b10000000   
#define DISPENSER_STATUS_CARD_AT_DISPENSE   0b01000000   
#define DISPENSER_STATUS_CARD_AT_READ       0b00100000   
#define DISPENSER_STATUS_CARD_SHORTAGE      0b00010000   
#define DISPENSER_STATUS_DISPENSING_CARD    0b00001000   
#define DISPENSER_STATUS_ACCEPTING_CARD     0b00000100
#define DISPENSER_STATUS_DISPENSING_ERROR   0b00000010
#define DISPENSER_STATUS_CARD_TIMEOUT       0b00000001

#define RETURN_OPERATION_SUCCEED    0x59
#define RETURN_OPERATION_FAILED     0x4E
#define RETURN_NAK                  0x15


int pack(uint8_t *data, int length, uint8_t command, uint8_t parameter);
int unpack(uint8_t *data, int frame_length);
uint8_t calc_bcc(uint8_t *buffer, int length);


#endif