#include <stdio.h>

#define BT_ReceiverFifo        ((volatile unsigned char *)(0xFF210220))
#define BT_TransmitterFifo     ((volatile unsigned char *)(0xFF210220))
#define BT_InterruptEnableReg  ((volatile unsigned char *)(0xFF210222))
#define BT_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210224))
#define BT_FifoControlReg ((volatile unsigned char *)(0xFF210224))
#define BT_LineControlReg ((volatile unsigned char *)(0xFF210226))
#define BT_ModemControlReg ((volatile unsigned char *)(0xFF210228))
#define BT_LineStatusReg ((volatile unsigned char *)(0xFF21022A))
#define BT_ModemStatusReg ((volatile unsigned char *)(0xFF21022C))
#define BT_ScratchReg ((volatile unsigned char *)(0xFF21022E))
#define BT_DivisorLatchLSB ((volatile unsigned char *)(0xFF210220))
#define BT_DivisorLatchMSB ((volatile unsigned char *)(0xFF210222))

#define Sensor_ReceiverFifo        ((volatile unsigned char *)(0xFF210210))
#define Sensor_TransmitterFifo     ((volatile unsigned char *)(0xFF210210))
#define Sensor_InterruptEnableReg  ((volatile unsigned char *)(0xFF210212))
#define Sensor_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210214))
#define Sensor_FifoControlReg ((volatile unsigned char *)(0xFF210214))
#define Sensor_LineControlReg ((volatile unsigned char *)(0xFF210216))
#define Sensor_ModemControlReg ((volatile unsigned char *)(0xFF210218))
#define Sensor_LineStatusReg ((volatile unsigned char *)(0xFF21021A))
#define Sensor_ModemStatusReg ((volatile unsigned char *)(0xFF21021C))
#define Sensor_ScratchReg ((volatile unsigned char *)(0xFF21021E))
#define Sensor_DivisorLatchLSB ((volatile unsigned char *)(0xFF210210))
#define Sensor_DivisorLatchMSB ((volatile unsigned char *)(0xFF210212))

#define TRUE 1
#define FALSE 0

void Init_BT(void);
int putcharBT(int c);
int getcharBT(void);
int BTTestForReceivedData(void);
void BTFlush(void);
unsigned char* BTRead(void);
void BTsend(void);
void Init_Sensor(void);
int putcharSensor(int c);
int getcharSensor(void);
int SensorTestForReceivedData(void);
void SensorFlush(void);
unsigned char* SensorRead(void);
void Sensorsend(void);



int main(){

    Init_BT();
    Init_Sensor();
    while(1){

        //BTFlush();
        unsigned char *BT_data_temp;        
        unsigned char *BT_data;                 //Bluetooth json string
        unsigned char *temp;                    //body temperature data from sensor
        printf("Message:\n");
        BTsend();
        BT_data_temp = BTRead();
        if(BT_data_temp[0] == '{'){             //Only those valid Bluetooth string will be used.
            BT_data = BT_data_temp;
        }
        Sensorsend();
        temp = SensorRead();                    //Read temperature from the sensor.
         if(temp[0] >= '3' && BT_data[0] == '{'){
            WiFi_HTTP_request(BT_data,temp);
            printf("uploaded!\n");          
        }
    }
   
    return 0;
}


void Init_BT(void){

    unsigned char LCR = *BT_LineControlReg;
    LCR = LCR | 0x80;                               //0x80 = 8'b10000000
    *BT_LineControlReg = LCR;

    *BT_DivisorLatchMSB = 0;
    *BT_DivisorLatchLSB = 0x1B;                     //Baud rate = 115200Hz.

    *BT_LineControlReg = 0x03;

    *BT_FifoControlReg = *BT_FifoControlReg | 0x06;

    *BT_FifoControlReg = *BT_FifoControlReg ^ 0x06;

}

int putcharBT(int c){

    while((*BT_LineStatusReg & 0x20) != 0x20);

    *BT_TransmitterFifo = (unsigned char)c;

    return c;
}

int getcharBT( void )
{
 // wait for Data Ready bit (0) of line status register to be '1'
 while((*BT_LineStatusReg & 0x01) != 0x01);

 return (int) *BT_ReceiverFifo;
 // read new character from ReceiverFiFo register
 // return new character
}

int BTTestForReceivedData(void)
{
 // if RS232_LineStatusReg bit 0 is set to 1
 //return TRUE, otherwise return FALSE
    if((*BT_LineStatusReg & 0x01)== 0x01)
        return TRUE;
    else
        return FALSE;
}

void BTFlush(void)
{
 // while bit 0 of Line Status Register == ‘1’
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
    while((*BT_LineStatusReg & 0x01) == 0x01){
        int unwanted = *BT_ReceiverFifo;
    }
    return;
}

unsigned char* BTRead(void){

    unsigned char word;
    unsigned char words[1000];
    int i = 0;
    int j = 0;
    putcharBT('\r');
    putcharBT('\n');
    for(i = 0; i < 1000000; i++){
        if(BTTestForReceivedData() == TRUE){
            word = (unsigned char)getcharBT();
            if(j < 1000){
                words[j] = word;
                j++;
                if(word == '}')
                    break;
            }
         // printf("%c", word);
            i = 0;
        }
    }
    return words;
}

void BTsend(void){

    char words[] = "sending!";
    int i =0;
    for(i= 0; words[i] != '\0'; i++){
        putcharBT((int) words[i]);
    } 
    putcharBT('\r');
    putcharBT('\n');
    return;
}

void Init_Sensor(void){

    unsigned char LCR = *Sensor_LineControlReg;
    LCR = LCR | 0x80;
    *Sensor_LineControlReg = LCR;

    *Sensor_DivisorLatchMSB = 0;
    *Sensor_DivisorLatchLSB = 0x1B;

    *Sensor_LineControlReg = 0x03;

    *Sensor_FifoControlReg = *Sensor_FifoControlReg | 0x06;

    *Sensor_FifoControlReg = *Sensor_FifoControlReg ^ 0x06;

}

int putcharSensor(int c){

    while((*Sensor_LineStatusReg & 0x20) != 0x20);

    *Sensor_TransmitterFifo = (unsigned char)c;

    return c;
}

int getcharSensor( void )
{
 // wait for Data Ready bit (0) of line status register to be '1'
 while((*Sensor_LineStatusReg & 0x01) != 0x01);

 return (int) *Sensor_ReceiverFifo;
 // read new character from ReceiverFiFo register
 // return new character
}

int SensorTestForReceivedData(void)
{
 // if RS232_LineStatusReg bit 0 is set to 1
 //return TRUE, otherwise return FALSE
    if((*Sensor_LineStatusReg & 0x01)== 0x01)
        return TRUE;
    else
        return FALSE;
}

void SensorFlush(void)
{
 // while bit 0 of Line Status Register == ‘1’
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
    while((*Sensor_LineStatusReg & 0x01) == 0x01){
        int unwanted = *Sensor_ReceiverFifo;
    }
    return;
}

unsigned char* SensorRead(void){

    unsigned char word;
    unsigned char words[10];
    int i = 0;
    int j = 0;
    putcharSensor('\r');
    putcharSensor('\n');
    for(i = 0; i < 1000000; i++){
        if(SensorTestForReceivedData() == TRUE){
            word = (unsigned char)getcharSensor();
            if((word >= '0' && word <= '9' && j < 5) | (word == '.' && j == 2)){
                words[j] = word;
                j++;
            }

         //   printf("%c", word);
            i = 0;
        }
    }
    words[j] = '\0';
    printf("%s\n",words);
    return words;
}

void Sensorsend(void){

    char words[] = "sending!";
    int i =0;
    for(i= 0; words[i] != '\0'; i++){
        putcharSensor((int) words[i]);
    } 
    putcharSensor('\r');
    putcharSensor('\n');
    return;
}