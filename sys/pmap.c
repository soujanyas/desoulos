#include<stdio.h>
#include<defs.h>
#define PAGE_ALIGN_FALSE 0
#define PAGE_ALIGN_TRUE 1
#define FOUR_KB_HEXA 0x1000
#define FIVE_PAGES_HEXA 0x5000


typedef struct page_info{
	int page_num;
	struct page_info* next;
}page_info_t;

typedef page_info_t * PAGE_INFO;
PAGE_INFO free_list_head;
PAGE_INFO tail = NULL;

void* placement_address;

void* kmalloc( int size, int page_align){
	void* curr = placement_address;
	if(page_align ==0 ){
		placement_address+=size;
	}
	return curr;
}

struct page_info* push_to_free_list(PAGE_INFO head,int page_num){
	PAGE_INFO page_info = (struct page_info*)kmalloc(sizeof(struct page_info),PAGE_ALIGN_FALSE);
	page_info->page_num = page_num;
	page_info->next = head;
	head = page_info;
	free_list_head = head;
	return head;
}

struct page_info* enqueue_free_list(PAGE_INFO tail, int page_num){
	PAGE_INFO page_info = (struct page_info*)kmalloc(sizeof(struct page_info),PAGE_ALIGN_FALSE);
	page_info->page_num = page_num;
	page_info->next = NULL;
	if(tail != NULL){
		tail->next = page_info;
	}else{
		free_list_head = page_info;
	}
	tail = page_info;
	return tail;
}

void print_list(){
	if(free_list_head == NULL) return;
	PAGE_INFO temp = free_list_head;
	while(temp != NULL){
		printf("Node content:%d next:%p\n",temp->page_num,temp->next);
		temp = temp->next;
	}
}

void* init_placement_address(void* pa){
	if(((uint64_t)pa & 0xfffff000) == (uint64_t)pa){
		placement_address = pa;
	} else{
		placement_address = (void*)(((uint64_t)pa)&0xfffff000)+0x1000;
	}
	printf("placement_address %p",placement_address);
	return placement_address;
}

void init_avail_memory(uint64_t smap_base,uint64_t smap_length,void* physfree){

	uint64_t temp;
	physfree = init_placement_address(physfree); // page aligned physfree
	uint64_t start_range = (uint64_t)physfree + FIVE_PAGES_HEXA;
	if( smap_base + smap_length > start_range){
		if(smap_base <= start_range){
			temp = start_range;
		}
		else if(smap_base > start_range){
			temp = smap_base;
		}
		for(;temp < smap_base + smap_length; temp+=FOUR_KB_HEXA){
			int page_num = temp/FOUR_KB_HEXA;
			tail = enqueue_free_list(tail, page_num);
		}
	}
}
