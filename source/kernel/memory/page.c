/**
	@brief Deallocate the pages in the section range
*/
void km_page_dealloc_range(struct kmp_process * where, unsigned long start, unsigned long size)
{
	unsigned long i;
	struct kmm * mem_dst;
	struct km_walk_ctx dst_ctx;
	unsigned long va_dst;

	/* Get the memory context */
	KM_WALK_INIT(&dst_ctx);
	dst_ctx.length				= size;
	dst_ctx.virtual_address		= start;
	dst_ctx.process				= where;
	mem_dst = km_ctx_get(dst_ctx.process);
	va_dst	= start;

	/* Loop the translation table and do delete */
	km_walk_loop(mem_dst, &dst_ctx, delete_page);

end:
	/* Flush the TLB */
	km_arch_flush_range(mem_dst, va_dst, dst_ctx.length);
	km_ctx_put(dst_ctx.process);
}