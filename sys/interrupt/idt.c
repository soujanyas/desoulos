#include <defs.h>
#include <sys/idt.h>
#include <sys/isr.h>
#include <sys/printf.h>
#include <sys/string.h>
#define IDT_SIZE 255
#define FLAG_INT 0x8E

void initialize_idt();
void add_idt_entry(int index,uint64_t isr_address, char flag); 
void _div_by_zero();

/*Structure to contain idt entries */
typedef struct idt_entry{
	uint16_t offset_low;
	uint16_t selector;
	char zero_1; 
	char flag; //P DPL 0D110 
	uint16_t offset_mid;	
	uint32_t offset_high;
	uint32_t zero_2;
}__attribute__((packed)) idt_entry_t;

idt_entry_t idt[IDT_SIZE];

/*Structure that will be loaded to IDTR */
struct idtr_t{
	uint16_t limit;
	idt_entry_t* idt_base;
}__attribute__((packed));

void _x86_64_asm_lidt(struct idtr_t* idtr);

/*Content that will be loaded onto idt register IDTR*/
static struct idtr_t idtr = {
	sizeof(idt),
	(idt_entry_t*)idt
};

/*Initialize Interrupt Descriptor Table and load it to the IDT register (IDTR)*/
void load_idt(){
	initialize_idt();
	_x86_64_asm_lidt(&idtr);
}

/*Sequences creation of IDT entry for all the interrupts currently
handled in the system*/
void initialize_idt(){
	memset(&idt,0,sizeof(idt_entry_t)*IDT_SIZE);
	add_idt_entry(0,(uint64_t)isr0,FLAG_INT);
	add_idt_entry(0x20,(uint64_t)isr32,FLAG_INT);
	add_idt_entry(0x21,(uint64_t)isr33,FLAG_INT);
}

/*Creates an IDT entry based on the parameters passed and adds it 
to the Interrupt Desciptor Table*/
void add_idt_entry(int index,uint64_t isr_address, char flag){ 
	idt[index].offset_low = isr_address & 0xffff;
	idt[index].selector = 0x0008;
	idt[index].offset_mid = (isr_address >> 16) & 0xffff;
	idt[index].offset_high = (isr_address >> 32) & 0xffffffff;
	idt[index].zero_1 = 0;
	idt[index].zero_2 = 0;
	idt[index].flag = flag;
}

