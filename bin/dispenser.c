#include "dispenser.h"
#include <stdio.h>

int pack(uint8_t *data, int length, uint8_t command, uint8_t parameter)
{   
    data[0] = STX;
    data[1] = (length>>8 & 0xFF);
    data[2] = (length & 0xFF);
    data[3] = command;
    data[4] = parameter;
    data[5] = ETX;
    data[6] = calc_bcc(data, 6);

    return 7;
}

int unpack(uint8_t *data, int frame_length)
{
    int position = 1;
    return position;
}

uint8_t calc_bcc(uint8_t *buffer, int length)
{
    int i;
    uint8_t bcc = 0;
    
    for(i = 0; i < length; i++)
    {
        bcc = bcc ^ buffer[i];
    }    
    return bcc;    
}

