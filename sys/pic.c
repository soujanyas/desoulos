#include <sys/pic.h>
#include <defs.h>

#define PIC1 0x20
#define PIC2 0x0A
#define ICW1 0x11
#define ICW4 0x01

void outb(char, uint16_t );

/*Remap PIC to handle interrupts*/
void remap_pic(char pic1, char pic2){

	/*Initialization instruction. Notifies PICs about three commands to follow*/
	outb(ICW1, PIC1);	
	outb(ICW1, PIC2);
	
	/*Master PIC vector offset*/
	outb(pic1, (PIC1+1));
	/*Slave PIC vector offset*/
	outb(pic2, (PIC2+1));

	/*Master-slave cascading information*/
	outb(4, (PIC1+1));
	outb(2, (PIC2+1));
	
	/*Control word with additional information about the env*/
	outb(ICW4, (PIC1+1));
	outb(ICW4, (PIC2+1));

	asm("sti"); //We need to enable interrupts once PIC is configured

}

/*Method used to output a byte onto a destination buffer*/
void outb(char src,uint16_t dest){
	__asm__("outb %0,%1;"
		:
		:"a"(src),"dN"(dest)
		);
}

