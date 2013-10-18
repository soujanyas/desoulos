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

struct pml4_t* init_pml4();
struct pdir_ptr_t* init_pdir_ptr();
struct page_dir_t* init_pdir();
struct page_table_t* init_page_table();
void add_pde(uint64_t virtual_addr, uint64_t phys_addr,  struct page_dir_t* pdir);
void add_pte(uint64_t virtual_addr, uint64_t phys_addr,  struct page_table_t* pt);
void add_pdpe(uint64_t virtual_addr, uint64_t phys_addr,  struct pdir_ptr_t* pdir_ptr);
void add_pml4(uint64_t virtual_addr, uint64_t phys_addr,  struct pml4_t* pml4);
void load_cr3( struct pml4_t* pml4_address);


struct cr3_t{
	uint64_t reserved:3;
	uint64_t pwt:1;
	uint64_t pcd:1;
	uint64_t reserved2:7;
	uint64_t pml4_address:40;
	uint64_t reserved3:12;
};

struct pml4e_t{
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

struct pdpe_t{
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
	uint64_t pd_base_address:40;
	uint64_t available:11;
	uint64_t nx:1;
};

struct pml4_t{
	struct pml4e_t pml4_entries[512];
};


struct pdir_ptr_t{
	struct pdpe_t pdp_entries[512];
};

struct pde_t{
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

struct pte_t{
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
	struct pte_t pt_entries[512];
};

struct page_dir_t{
	struct pde_t pd_entries[512];
};
uint64_t va_of_free_p(int page_num){
	uint64_t phys_add = page_num*FOUR_KB_HEXA;
	uint64_t virtual_add = phys_add + BASE;
	return virtual_add;
}

struct pml4_t* init_pml4(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct pml4_t* pml4 = (struct pml4_t*)(page_address);
	debug("Initializing pml14 at free page:%p\n",(page_address));
	memset(pml4,0,512*sizeof(struct pml4_t));//initialze to zero	
	return pml4;
}

struct pdir_ptr_t* init_pdir_ptr(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct pdir_ptr_t* pdir_ptr = (struct pdir_ptr_t*)(page_address);
	debug("Initializing pdir_ptr at free page:%p\n",(page_address));
	memset(pdir_ptr,0,512*sizeof(struct pdir_ptr_t));//initialze to zero	
	return pdir_ptr;
}

uint64_t get_va_index(uint64_t virtual_addr, int flag){
	if(flag == PML4){
		virtual_addr = virtual_addr >> 39;
	}
	else if(flag == PDIRPTR){
		virtual_addr = virtual_addr >> 30;
	}
	else if(flag == PDIR){
		debug("\n ENTER virtual address %p",virtual_addr);
		virtual_addr = virtual_addr >> 21;
		debug("\nEXIT virtual address %p",virtual_addr);
	}
	else if(flag == PAGETABLE){
		virtual_addr = virtual_addr >> 12;
	}
	else{	
		printf("Invalid flag passed to get_va_index()");
		return -1;
	}
	uint64_t index = virtual_addr & MASK;
	return index;

}


struct page_dir_t* init_pdir(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct page_dir_t* pdir = (struct page_dir_t*)(page_address);
	debug("Initializing pdir at free page:%p\n",(page_address));
	memset(pdir,0,512*sizeof(struct page_dir_t));//initialze to zero	
	return pdir;
}


struct page_table_t* init_page_table(){
	uint64_t page_address = get_free_page() * FOUR_KB_HEXA;
	struct page_table_t* pt = (struct page_table_t*)(page_address);
	debug("Initializing page_table at free page:%p\n",(page_address));
	memset(pt,0,512*sizeof(struct page_table_t));//initialze to zero	
	return pt;//todo:virtual address
}

void map_kernel(void *physbase, void *physfree){
	uint64_t i;
	struct page_table_t* pt = init_page_table();//todo add some more pages if not sufficient
	struct page_dir_t* pdir = init_pdir();
	struct pdir_ptr_t *pdpe = init_pdir_ptr();
	struct pml4_t* pml4 = init_pml4();
	debug("Base is %p and phys free%p", BASE, physfree);
	for(i=BASE+(uint64_t)physbase;i < (uint64_t)physfree+BASE; i+=FOUR_KB_HEXA){
		add_pte( i, i-BASE, pt);
		//currently accommodates only 512 entries
	}
	add_pde(BASE+(uint64_t)physbase,(uint64_t)pt, pdir);
	add_pdpe(BASE+(uint64_t)physbase,(uint64_t)pdir, pdpe);
	add_pml4(BASE+(uint64_t)physbase,(uint64_t)pdpe, pml4);
	load_cr3(pml4);
}

/*virtual_addr indicates the offset where the entry has to be created
phys_addr gives value to be loaded onto pte entry*/
void add_pte(uint64_t virtual_addr, uint64_t phys_addr,  struct page_table_t* pt){
	uint64_t pte_offset = get_va_index(virtual_addr,PAGETABLE);
//	debug("\npte offset is %d",pte_offset);
	pt->pt_entries[pte_offset].present = 1;
	pt->pt_entries[pte_offset].r_w = 1;
	pt->pt_entries[pte_offset].u_s = 1;
	pt->pt_entries[pte_offset].p_write_through = 1;
	pt->pt_entries[pte_offset].p_cache_disable = 1;
	pt->pt_entries[pte_offset].accessed = 1;
	pt->pt_entries[pte_offset].d = 0;
	pt->pt_entries[pte_offset].pat = 1;//???
	pt->pt_entries[pte_offset].g = 1;
	pt->pt_entries[pte_offset].avl = 0;
	pt->pt_entries[pte_offset].pt_base_address = phys_addr>>12;
	pt->pt_entries[pte_offset].available = 0;
	pt->pt_entries[pte_offset].nx = 0;
//	debug("\nAdded pte entry:%x at location %p",pt->pt_entries[pte_offset],&(pt->pt_entries[pte_offset]));

}

void add_pde(uint64_t virtual_addr, uint64_t phys_addr,  struct page_dir_t* pdir){
	uint64_t pde_offset = get_va_index(virtual_addr,PDIR);
	debug("\npde offset is %d",pde_offset);
	pdir->pd_entries[pde_offset].present = 1;
	pdir->pd_entries[pde_offset].r_w = 1;
	pdir->pd_entries[pde_offset].u_s = 1;
	pdir->pd_entries[pde_offset].p_write_through = 1;
	pdir->pd_entries[pde_offset].p_cache_disable = 1;
	pdir->pd_entries[pde_offset].accessed = 1;
	pdir->pd_entries[pde_offset].ignore1 = 0;
	pdir->pd_entries[pde_offset].zero = 0;
	pdir->pd_entries[pde_offset].ignore2 = 0;
	pdir->pd_entries[pde_offset].avl = 0;
	pdir->pd_entries[pde_offset].pt_base_address = phys_addr>>12;
	pdir->pd_entries[pde_offset].available = 0;
	pdir->pd_entries[pde_offset].nx = 0;
	debug("\nAdded pde entry:%x at location %p",pdir->pd_entries[pde_offset],pdir);

}


void add_pdpe(uint64_t virtual_addr, uint64_t phys_addr,  struct pdir_ptr_t* pdir_ptr){
	uint64_t pdpe_offset = get_va_index(virtual_addr,PDIRPTR);
	debug("\nPdpe offset is %d",pdpe_offset);
	pdir_ptr->pdp_entries[pdpe_offset].present = 1;
	pdir_ptr->pdp_entries[pdpe_offset].r_w = 1;
	pdir_ptr->pdp_entries[pdpe_offset].u_s = 1;
	pdir_ptr->pdp_entries[pdpe_offset].p_write_through = 1;
	pdir_ptr->pdp_entries[pdpe_offset].p_cache_disable = 1;
	pdir_ptr->pdp_entries[pdpe_offset].accessed = 1;
	pdir_ptr->pdp_entries[pdpe_offset].ignore = 0;
	pdir_ptr->pdp_entries[pdpe_offset].zero = 0;
	pdir_ptr->pdp_entries[pdpe_offset].m_b_z = 0;
	pdir_ptr->pdp_entries[pdpe_offset].avl = 0;
	pdir_ptr->pdp_entries[pdpe_offset].pd_base_address = phys_addr>>12;
	pdir_ptr->pdp_entries[pdpe_offset].available = 0;
	pdir_ptr->pdp_entries[pdpe_offset].nx = 0;
	debug("\nAdded pdpe entry:%x at location %p",pdir_ptr->pdp_entries[pdpe_offset],pdir_ptr);

}


void add_pml4(uint64_t virtual_addr, uint64_t phys_addr,  struct pml4_t* pml4){
	uint64_t pml4_offset = get_va_index(virtual_addr,PML4);
	debug("\nPml4 offset is %d",pml4_offset);

	pml4->pml4_entries[pml4_offset].present = 1;
	pml4->pml4_entries[pml4_offset].r_w = 1;
	pml4->pml4_entries[pml4_offset].u_s = 1;
	pml4->pml4_entries[pml4_offset].p_write_through = 1;
	pml4->pml4_entries[pml4_offset].p_cache_disable = 1;
	pml4->pml4_entries[pml4_offset].accessed = 1;
	pml4->pml4_entries[pml4_offset].ignore = 0;
	pml4->pml4_entries[pml4_offset].m_b_z = 0;
	pml4->pml4_entries[pml4_offset].avl = 0;
	pml4->pml4_entries[pml4_offset].pdp_base_address = phys_addr>>12;
	pml4->pml4_entries[pml4_offset].available = 0;
	pml4->pml4_entries[pml4_offset].nx = 0;
	//debug("\nAdded pml4 entry:%x at location %p",pml4->pml4_entries[pml4_offset],pml4);
}

void load_cr3(struct pml4_t* pml4){
	struct cr3_t cr3;
	uint64_t cr3_content=0;
	cr3.reserved = 0;
	cr3.pwt=1;
	cr3.pcd = 0;
	cr3.reserved2 = 0;
	cr3.pml4_address = ((uint64_t)pml4) >> 12;
	cr3.reserved3 = 0;
	debug("\nContents getting loaded onto cr3 %p",cr3);

	__asm__("movq %1,%%cr3;"
		"movq %%cr3,%0;"
		:"=r"(cr3_content)
		:"b"(cr3)
		);

	printf("Contents of cr3 %p",0x123);	
}
