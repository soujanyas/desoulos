#include<defs.h>
#include<sys/pmap.h>
#include<sys/memset.h>
#include<sys/printf.h>
#define FOUR_KB_HEXA 0x1000
#define BASE 0xffffffff80000000
#define MASK 0x1ff
#define PML4 1
#define PDIRPTR 2
#define PDIR 3
#define PAGETABLE 4

void init_pml4();
void init_pdir_ptr();
void init_pdir();
void init_page_table();

struct pml4e{
	uint64_t present:1;
	uint64_t r_w:1; 
	uint64_t u_s:1;
	uint64_t p_write_through:1;
	uint64_t p_cache_disable:1;
	uint64_t accessed:1;
	uint64_t ignore:1;
	uint64_t m_b_z:2;
	uint64_t avl:3;
	uint64_t pdp_base_address:40;
	uint64_t available:11;
	uint64_t nx:1;
};

struct pdpe{
	uint64_t present:1;
	uint64_t r_w:1; 
	uint64_t u_s:1;
	uint64_t p_write_through:1;
	uint64_t p_cache_disable:1;
	uint64_t accessed:1;
	uint64_t ignore:1;
	uint64_t zero:1;
	uint64_t m_b_z:1;
	uint64_t avl:3;
	uint64_t pdp_base_address:40;
	uint64_t available:11;
	uint64_t nx:1;
};

struct pml4_t{
	struct pml4e pml4_entries[512];
};


struct pdir_ptr_t{
	struct pdpe pdp_entries[512];
};

struct pde{
	uint64_t present:1;
	uint64_t r_w:1;
	uint64_t u_s:1;
	uint64_t p_write_through:1;
	uint64_t p_cache_disable:1;
	uint64_t accessed:1;
	uint64_t ignore1:1;
	uint64_t zero:1;
	uint64_t ignore2:1;
	uint64_t avl:3;
	uint64_t pt_base_address:40;
	uint64_t available:11;
	uint64_t nx:1;
};

struct pte{
	uint64_t present:1;
	uint64_t r_w:1;
	uint64_t u_s:1;
	uint64_t p_write_through:1;
	uint64_t p_cache_disable:1;
	uint64_t accessed:1;
	uint64_t d:1;
	uint64_t pat:1;
	uint64_t g:1;
	uint64_t avl:3;
	uint64_t pt_base_address:40;
	uint64_t available:11;
	uint64_t nx:1;
};

struct page_table_t{
	struct pte pt_entries[512];
};

struct page_dir_t{
	struct pde pd_entries[512];
};
uint64_t va_of_free_p(int page_num){
	uint64_t phys_add = page_num*FOUR_KB_HEXA;
	uint64_t virtual_add = phys_add + BASE;
	return virtual_add;
}

void init_page_tables(){
	init_pml4();
	init_pdir_ptr();
	init_pdir();
	init_page_table();
}

void init_pml4(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct pml4_t* pml4 = (struct pml4_t*)(page_address);
	debug("Initializing pml14 at free page:%p\n",(page_address));
	memset(pml4,0,512*sizeof(struct pml4_t));//initialze to zero	
}

void init_pdir_ptr(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct pdir_ptr_t* pdir_ptr = (struct pdir_ptr_t*)(page_address);
	debug("Initializing pdir_ptr at free page:%p\n",(page_address));
	memset(pdir_ptr,0,512*sizeof(struct pdir_ptr_t));//initialze to zero	
}

void init_pdir(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct page_dir_t* pdir = (struct page_dir_t*)(page_address);
	debug("Initializing pdir at free page:%p\n",(page_address));
	memset(pdir,0,512*sizeof(struct page_dir_t));//initialze to zero	

}


void init_page_table(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct page_table_t* pt = (struct page_table_t*)(page_address);
	debug("Initializing page_table at free page:%p\n",(page_address));
	memset(pt,0,512*sizeof(struct page_table_t));//initialze to zero	
}

uint64_t get_va_index(uint64_t virtual_addr, int flag){
	if(flag == PML4){
		printf("virtual addr %p", virtual_addr);
		virtual_addr = virtual_addr >> 39;
		printf("shifted virtual addr %p", virtual_addr);
		printf("MASK %p", MASK);
		uint64_t index = virtual_addr & MASK; 
		return index;
	}
	else if(flag == PDIRPTR){
		virtual_addr = virtual_addr >> 30;
		uint64_t index = virtual_addr & MASK; 
		return index;
	}
	else if(flag == PDIR){
		virtual_addr = virtual_addr >> 21;
		uint64_t index = virtual_addr & MASK; 
		return index;
	}
	else if(flag == PAGETABLE){
		virtual_addr = virtual_addr >> 12;
		uint64_t index = virtual_addr & MASK; 
		return index;
	}
	else{
		printf("Invalid flag passed to get_va_index()");
		return 0;
	}

}
