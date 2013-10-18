void init_page_tables();
uint64_t get_va_index(uint64_t , int);
void map_kernel(void *physbase, void *physfree);
