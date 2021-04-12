#include <stdio.h>
// ALL parallel IO ports created by QSYS have a 32 bit wide interface, even though the
// port itself might only use the lower 8 or 10 or 4 bits of data we read/write,
// so we use a 32 bit address (Int pointer)
// to read/write to the port
#define SWITCHES (volatile unsigned int *)(0xFF200000)
#define PUSHBUTTONS (volatile unsigned int *)(0xFF200010)
#define LEDS (volatile unsigned int *)(0xFF200020)
#define HEX0_1 (volatile unsigned int *)(0xFF200030)
#define HEX2_3 (volatile unsigned int *)(0xFF200040)
#define HEX4_5 (volatile unsigned int *)(0xFF200050)
void main(void)
{
 int switches ;
printf("Hello from the CPEN 391 System\n");
 while(1) {
 switches = *SWITCHES ;
 *LEDS = switches;
 *HEX0_1 = switches;
 *HEX2_3 = switches;
 *HEX4_5 = switches;
 printf("Switches = %x\n", switches) ;
 printf("Buttons = %x\n", *PUSHBUTTONS) ;
 }
}