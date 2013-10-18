#include <defs.h>
#define REQ_CLOCK_FREQ 100

void outb(char s,uint16_t d);

/*This method configures the Timer Interrupt Controller*/
void configure_timer(){
	int divisor=1193180/REQ_CLOCK_FREQ;
	outb(0x36,0x43);
	outb(divisor&0xff,0x40);
	outb(divisor>>8,0x40);
}

