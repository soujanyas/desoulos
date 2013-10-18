#include <defs.h>
#include <sys/gdt.h>
#include <sys/printf.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/timer.h>
#include<sys/paging.h>
#include <sys/pmap.h>

void start(uint32_t* modulep, void* physbase, void* physfree)
{
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
//	volatile int i =1;
	printf("module p is; %x, modulep[0]: %x",modulep,modulep[0]);

//	while(i);
	while(modulep[0] != 0x9001){ 
		modulep += modulep[1]+2;
		debug("Modulep : %p modulep[0]: %x\n",modulep,modulep[0]);
	}
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			printf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
			printf("length of segment: %p", smap->length);
			init_avail_memory(smap->base, smap->length, physfree);
		}
	}
	info("\nInitializing pages..\n");
	init_page_tables();	
	// kernel starts here
	while(1);
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	register char *temp1, *temp2;
//	volatile int i =1;
//	while(i);
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	set_debug_level(DEBUG);
	reload_gdt();
	setup_tss();
	remap_pic(0x20,0x28);
	load_idt();
	configure_timer();
	clrscr();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	for(
		temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
		*temp1;
		temp1 += 1, temp2 += 2
	) *temp2 = *temp1;
	while(1);



}
