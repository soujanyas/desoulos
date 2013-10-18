#include <defs.h>
#include <sys/printf.h>
#include <sys/isr.h>
#define SHIFT 42
#define SHIFT_BREAK 86

void ack_isr();
char inb(uint16_t);

/*A common handler for all interrupts. Individual handlers will be called
based on the interrupt number passed*/
void isr_handler(uint64_t int_num){
	if(int_num == 32){
		isr32_handler();
	}else if (int_num == 33){
		isr33_handler();
	}else if (int_num == 0){
		isr0_handler();
	}
}

/*Interrupt handler for divide by zero operation*/
void isr0_handler(){
	printf("Divide by zero");
	ack_isr();
}

/*This is the handler for timer interrupt. The handler prints the time since
boot onto the console*/
void isr32_handler(){
	int ss[2];
	int mm[2];
	int hh[2];
	int dd[2];
	int time_elapsed;
	static uint64_t counter;
	counter++;
	if(counter%100 == 0){
		time_elapsed = counter/100;
		ss[0] = (time_elapsed%60)%10;
		ss[1] = (time_elapsed%60)/10;
		time_elapsed/=60;
		mm[0] = (time_elapsed%60)%10;
		mm[1] = (time_elapsed%60)/10;
		time_elapsed/=60;
		hh[0] = (time_elapsed%60)%10;
		hh[1] = (time_elapsed%60)/10;
		time_elapsed/=24;
		dd[0] = (time_elapsed%24)%10;
		dd[1] = (time_elapsed%24)/10;
		printTimer("%d%d:%d%d:%d%d:%d%d",dd[1],dd[0],hh[1],hh[0],mm[1],mm[0],ss[1],ss[0]);
	}
	ack_isr();
}

/*This is the handler for keyboard interrupts. The handler prints the key pressed
onto the console to the left of timer*/
void isr33_handler(){
	static int shift_pressed;
	int scancode=(int)inb(0x60);
	char kb_map[256]={
	0,27,'1','2','3','4','5','6','7','8','9','0',
	'-','=',' ',' ',
	'q','w','e','r','t','y','u','i','o','p','[',']',' ',' ',
	'a','s','d','f','g','h','j','k','l',';',
	'\'','`',0,'\\',
	'z','x','c','v','b','n','m',',','.','/',0,
	'*',0,' ',0,0,0,0,0,0,0,0,0,'-',0,0,0,'+',0,0,0,0,0,0,0,0,0,
	0,'~','!','@','#','$','%','^','&','*','(',')',
	'_','+',' ',' ',
	'Q','W','E','R','T','Y','U','I','O','P','{','}',' ',' ',
	'A','S','D','F','G','H','J','K','L',':',
	'\"','~',0,'|',
	'Z','X','C','V','B','N','M','<','>','?',0,
	'*',0,' ',0,0,0,0,0,0,0,0,0,'-',0,0,0,'+',0,0,0,0,0,0,0,0,0
	};
	if (scancode & 0x80){ //Key released
		if(scancode == -SHIFT_BREAK){
			shift_pressed=0;
		}
	}else{
		if(scancode == SHIFT){//Note that make code for SHIFT and CAPS_LOCK are both 42
			shift_pressed=1;
		}
		else{
			if(shift_pressed){
				scancode += 81;//Get scan codes for capital letters
			}
			printKb("%c",kb_map[(int)scancode]);
		}
	}
	
	ack_isr();
}

/*Method to acknoledge the master PIC that the interrupt is serviced.*/
void ack_isr(){
	char src=0x20;
	uint16_t dest=0x20;
	__asm__("outb %0,%1"
		:
		:"a"(src),"dN"(dest)
		);

}

/*Method to read a byte from a port passed as an argument*/
char inb(uint16_t src){
	char dest=0;
	__asm__("inb %1,%0"
		:"=a"(dest)
		:"dN"(src), "a"(dest)
		);
	return dest;	
}




