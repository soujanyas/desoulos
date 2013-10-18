struct page_info;
void* kmalloc( int size, int page_align);
uint64_t get_free_page();
struct page_info* push_to_free_list(struct page_info * head,int page_num);
struct page_info* enqueue_free_list(struct page_info * tail, int page_num);
void print_list();
void init_avail_memory(uint64_t smap_base, uint64_t smap_length, void* physfree);
