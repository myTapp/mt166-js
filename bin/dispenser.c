#include "dispenser.h"

int pack(uint8_t *data, int length, uint8_t command, uint8_t parameter)
{
    int position = 0;
    
    data[position++] = STX;
    data[position++] = (length>>8 & 0xFF);
    data[position++] = (length & 0xFF);
    data[position++] = command;
    data[position++] = parameter;
    data[position++] = ETX;
    data[position++] = calc_bcc(data, position);

    return position;
}

int unpack(uint8_t *data, int frame_length)
{
    int position = 1;

//    if(data[0] == STX && data[frame_length-2] == ETX)         /* Start byte and end byte are OK */
//    {
//        if(data[frame_length-1] == calc_bcc(data, frame_length-1))  /* BCC byte are OK                */
/*        {
            length = data[position++]<<8;
            length += data[position++];
            command = data[position++];
            parameter = data[position++];
            response = data[position++];
            if(response == RETURN_OPERATION_SUCCEED)
            {

            }
            if(response == RETURN_OPERATION_FAILED)
            {

            }
        }
    } */
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

