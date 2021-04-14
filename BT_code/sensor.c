#include <stdio.h>

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

void Init_Sensor(void);
int putcharSensor(int c);
int getcharSensor(void);
int SensorTestForReceivedData(void);
void SensorFlush(void);
unsigned char* SensorRead(void);
void Sensorsend(void);



int main(){

    Init_Sensor();
    while(1){
        unsigned char *temp;
        //SensorFlush();
        printf("Message:\n");
        Sensorsend();
        temp = SensorRead();
        if(temp[0] >= '3'){
            printf("uploaded!\n");
        }
    }
   
    return 0;
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

    char words[] = "Good!";
    int i =0;
    for(i= 0; words[i] != '\0'; i++){
        putcharSensor((int) words[i]);
    } 
    putcharSensor('\r');
    putcharSensor('\n');
    return;
}