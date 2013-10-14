struct pde{
	uint64_t present:1;
	uint64_t r_w:1;
	uint64_t u_s:1;
	uint64_t p_write_through:1;
	uint64_t p_cache_disable:1;
	uint64_t accessed:1;
	uint64_t ignore:1;
	uint64_t zero:1;
	uint64_t ignore:1;
	uint64_t avl:3;
	uint64_t pt_base_address:40;
	uint64_t available:11;
	uint64_t nx:1;
}

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
}

struct page_table{
	struct pte[512];
}

struct page_dir{
	struct pde[512];
}

