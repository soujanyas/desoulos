#include<stdio.h>
#include<defs.h>
#define PAGE_ALIGN_FALSE 0
#define PAGE_ALIGN_TRUE 1


typedef struct page_info{
	int page_num;
	struct page_info* next;
}page_info_t;

typedef page_info_t * PAGE_INFO;


void *placement_address;

void* kmalloc( int size, int page_align){
	int* curr = placement_address;
	if(page_align ==0 ){
		placement_address+=size;
	}
	return curr;
}

struct page_info* add_to_free_list(PAGE_INFO head,int page_num){
	PAGE_INFO page_info = (struct page_info*)kmalloc(sizeof(struct page_info),PAGE_ALIGN_FALSE);
	page_info->page_num = page_num;
	page_info->next = head;
	head = page_info;
	return head;
}

void print_list(PAGE_INFO head){
	if(head == NULL) return;
	PAGE_INFO temp = head;
	while(temp != NULL){	
		printf("Node content:%d next:%p\n",temp->page_num,temp->next);
		temp = temp->next;
	}
}

void init_placement_address(void* pa){
	placement_address = pa;
}
