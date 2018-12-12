#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rs232.h"
#include "dispenser.h"

int SERIAL_PORT = 6;       //COM7
int BAUDRATE    = 9600;
char LOG_FILE[50] = "log.txt";

void log_action(char *str1, char *str2)
{
    FILE *f = fopen(LOG_FILE, "a");
    time_t current_time;
    char* c_time_string;
    
    /* Obtain current time. */
    current_time = time(NULL);

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);
    fprintf(f, "\n%s-> %s, %s\n", c_time_string, str1, str2);

    fclose(f);
}

uint8_t open_port()
{
    char mode[]={'8','N','1',0};
    char str[30];

    if(RS232_OpenComport(SERIAL_PORT, BAUDRATE, mode))
    {
        sprintf(str, "Try to open serial port %d", SERIAL_PORT);
        log_action(str, "Can not open serial port");
        
        return 0;
    }
  
    return 1;
}

void print_success()
{
    printf("1\n");  
}

void print_failure()
{
    printf("0\n");  
}

void send_to_read_position()
{
    uint8_t data[10];
    char buffer[10];
    buffer[5] = 0;
    char str1[60], str2[50];

    int length, answer, i;
    
    length = pack(data, 2, CMD_DISPENSER_SEND_CARD_TO_READ_POSITION); // send to pos 1
    RS232_SendBuf(SERIAL_PORT, data, length);   
    sprintf(str1, "Received command SEND_CARD_TO_READ from application");
    sprintf(str2, "Sending: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    log_action(str1, str2);
        
    #ifdef _WIN32
    Sleep(1000);    /* sleep for 1000 milliSeconds */
#else
    usleep(1000000);  
#endif
    answer = RS232_PollComport(SERIAL_PORT, buffer, 10);
    
    if(answer > 0 && buffer[0] != RETURN_NAK)
    {
        sprintf(str2, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        log_action("Received response from MT166", str2);
    }
    else
    {
        log_action("No response from MT166", "--");
    }

    if(buffer[5] == RETURN_OPERATION_SUCCEED)
    {
        print_success();
    }
    else
    {
        print_failure();  
    }
}

void send_to_dispense_position()
{
    uint8_t data[10];
    char buffer[10];
    buffer[5] = 0;
    char str1[60], str2[50];

    int length, answer, i;
    
    length = pack(data, 2, CMD_DISPENSER_SEND_CARD_TO_DISPENSE_POSITION); // send to pos 2
    RS232_SendBuf(SERIAL_PORT, data, length);   
    sprintf(str1, "Received command DISPENSE_CARD from application");
    sprintf(str2, "Sending: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    log_action(str1, str2);
        
    #ifdef _WIN32
    Sleep(1000);    /* sleep for 1000 milliSeconds */
#else
    usleep(1000000);  
#endif
    answer = RS232_PollComport(SERIAL_PORT, buffer, 10);
    
    if(answer > 0 && buffer[0] != RETURN_NAK)
    {
        sprintf(str2, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        log_action("Received response from MT166", str2);
    }
    else
    {
        log_action("No response from MT166", "--");
    }

    if(buffer[5] == RETURN_OPERATION_SUCCEED)
    {
        print_success();
    }
    else
    {
        print_failure();  
    }
}

void check_card_at_dispense_position()
{
    uint8_t data[10];
    char buffer[10];
    buffer[5] = 0;
    char str1[70], str2[50];

    int length, answer, i;
    
    length = pack(data, 2, CMD_DISPENSER_CHECK_STATUS); 
    RS232_SendBuf(SERIAL_PORT, data, length);
    sprintf(str1, "Received command CHECK_CARD_AT_DISPENSE_POSITION from application");
    sprintf(str2, "Sending: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    log_action(str1, str2);
        
    #ifdef _WIN32
    Sleep(100);    /* sleep for 1000 milliSeconds */
#else
    usleep(100000);  
#endif
    answer = RS232_PollComport(SERIAL_PORT, buffer, 10);
    
    if(answer > 0 && buffer[0] != RETURN_NAK)
    {
        sprintf(str2, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        log_action("Received response from MT166", str2);
    }
    else
    {
        log_action("No response from MT166", "--");
    }

    if(buffer[5] & DISPENSER_STATUS_CARD_AT_DISPENSE)
    {
        print_success();
    }
    else
    {
        print_failure();  
    }
}

void check_card_at_read_position()
{
    uint8_t data[10];
    char buffer[10];
    buffer[5] = 0;
    char str1[70], str2[50];

    int length, answer, i;
    
    length = pack(data, 2, CMD_DISPENSER_CHECK_STATUS); 
    RS232_SendBuf(SERIAL_PORT, data, length);
    sprintf(str1, "Received command CHECK_CARD_AT_READ_POSITION from application");
    sprintf(str2, "Sending: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    log_action(str1, str2);
        
    #ifdef _WIN32
    Sleep(100);    /* sleep for 1000 milliSeconds */
#else
    usleep(100000);  
#endif
    answer = RS232_PollComport(SERIAL_PORT, buffer, 10);
    
    if(answer > 0 && buffer[0] != RETURN_NAK)
    {
        sprintf(str2, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        log_action("Received response from MT166", str2);
    }
    else
    {
        log_action("No response from MT166", "--");
    }

    if(buffer[5] & DISPENSER_STATUS_CARD_AT_READ)
    {
        print_success();
    }
    else
    {
        print_failure();  
    }
}

void check_if_box_is_pre_empty()
{
    uint8_t data[10];
    char buffer[10];
    buffer[5] = 0;
    char str1[70], str2[50];

    int length, answer, i;
    
    length = pack(data, 2, CMD_DISPENSER_CHECK_STATUS); 
    RS232_SendBuf(SERIAL_PORT, data, length);
    sprintf(str1, "Received command CHECK_IF_BOX_IS_ALMOST_EMPTY from application");
    sprintf(str2, "Sending: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    log_action(str1, str2);
        
    #ifdef _WIN32
    Sleep(100);    /* sleep for 1000 milliSeconds */
#else
    usleep(100000);  
#endif
    answer = RS232_PollComport(SERIAL_PORT, buffer, 10);
    
    if(answer > 0 && buffer[0] != RETURN_NAK)
    {
        sprintf(str2, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        log_action("Received response from MT166", str2);
    }
    else
    {
        log_action("No response from MT166", "--");
    }

    if(buffer[5] & DISPENSER_STATUS_CARD_SHORTAGE)
    {
        print_success();
    }
    else
    {
        print_failure();  
    }
}

void check_if_box_is_empty()
{
    uint8_t data[10];
    char buffer[10];
    buffer[5] = 0;
    char str1[70], str2[50];

    int length, answer, i;
    
    length = pack(data, 2, CMD_DISPENSER_CHECK_STATUS); 
    RS232_SendBuf(SERIAL_PORT, data, length);
    sprintf(str1, "Received command CHECK_IF_BOX_IS_EMPTY from application");
    sprintf(str2, "Sending: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    log_action(str1, str2);
        
    #ifdef _WIN32
    Sleep(100);    /* sleep for 1000 milliSeconds */
#else
    usleep(100000);  
#endif
    answer = RS232_PollComport(SERIAL_PORT, buffer, 10);
    
    if(answer > 0 && buffer[0] != RETURN_NAK)
    {
        sprintf(str2, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        log_action("Received response from MT166", str2);
    }
    else
    {
        log_action("No response from MT166", "--");
    }

    if(buffer[5] & DISPENSER_STATUS_BOX_EMPTY)
    {
        printf("1\n");    
    }
    else
    {
        printf("0\n");    
    }
}

void discard_card()
{
    uint8_t data[10];
    char buffer[10];
    buffer[5] = 0;
    char str1[60], str2[50];

    int length, answer, i;
    
    length = pack(data, 2, CMD_DISPENSER_ACCEPT_CARD_TO_RECYCLE); // send to recycle
    RS232_SendBuf(SERIAL_PORT, data, length);   
    sprintf(str1, "Received command DISCARD_CARD from application");
    sprintf(str2, "Sending: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    log_action(str1, str2);
        
    #ifdef _WIN32
    Sleep(2000);    /* sleep for 1000 milliSeconds */
#else
    usleep(2000000);  
#endif
    answer = RS232_PollComport(SERIAL_PORT, buffer, 10);
    
    if(answer > 0 && buffer[0] != RETURN_NAK)
    {
        sprintf(str2, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        log_action("Received response from MT166", str2);
    }
    else
    {
        log_action("No response from MT166", "--");
    }

    if(buffer[5] == RETURN_OPERATION_SUCCEED)
    {
        print_success();
    }
    else
    {
        print_failure();  
    }
}

void menu(int command)
{
    switch(command)
    {
        case 1: send_to_read_position();
        break;
        case 2: send_to_dispense_position();
        break;
        case 3: check_card_at_dispense_position();
        break;
        case 4: check_card_at_read_position();
        break;
        case 5: check_if_box_is_pre_empty();
        break;
        case 6: check_if_box_is_empty();
        break;
        case 7: discard_card();
        break;

        default: printf("Invalid parameter \n");           
    }    
}

void close_port()
{
    RS232_CloseComport(SERIAL_PORT);
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("%s: missing operand.\nUsage: %s <command> <serial port(optional)> <baudrate(optional)> <log file path(optional)> \n", argv[0], argv[0]);
        exit(0);
    }

    if(argc >= 3)
    {
        SERIAL_PORT = atoi(argv[2]); 
    }
    if(argc >= 4)
    {
        BAUDRATE    = atoi(argv[3]);
    }
    if(argc >= 5)
    {
        strcpy(LOG_FILE, argv[5]);
    }

    if(open_port())
    {
        menu(atoi(argv[1]));

        close_port();
    }
    return 0;
}

