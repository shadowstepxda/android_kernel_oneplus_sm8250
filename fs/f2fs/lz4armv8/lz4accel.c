#include "lz4accel.h"
#include <asm/cputype.h>

#ifdef CONFIG_CFI_CLANG
static inline int
__cfi_f2fs_lz4_decompress_asm(uint8_t **dst_ptr, uint8_t *dst_begin,
			 uint8_t *dst_end, const uint8_t **src_ptr,
			 const uint8_t *src_end, bool dip)
{
	return _f2fs_lz4_decompress_asm(dst_ptr, dst_begin, dst_end,
				   src_ptr, src_end, dip);
}

static inline int
__cfi_f2fs_lz4_decompress_asm_noprfm(uint8_t **dst_ptr, uint8_t *dst_begin,
				uint8_t *dst_end, const uint8_t **src_ptr,
				const uint8_t *src_end, bool dip)
{
	return _f2fs_lz4_decompress_asm_noprfm(dst_ptr, dst_begin, dst_end,
					  src_ptr, src_end, dip);
}

#define _f2fs_lz4_decompress_asm		__cfi_f2fs_lz4_decompress_asm
#define _f2fs_lz4_decompress_asm_noprfm	__cfi_f2fs_lz4_decompress_asm_noprfm
#endif

int f2fs_lz4_decompress_asm_select(uint8_t **dst_ptr, uint8_t *dst_begin,
			      uint8_t *dst_end, const uint8_t **src_ptr,
			      const uint8_t *src_end, bool dip) {
	const unsigned i = smp_processor_id();

	switch(read_cpuid_part_number()) {
	case ARM_CPU_PART_CORTEX_A53:
		f2fs_lz4_decompress_asm_fn[i] = _f2fs_lz4_decompress_asm_noprfm;
		return _f2fs_lz4_decompress_asm_noprfm(dst_ptr, dst_begin, dst_end,
						  src_ptr, src_end, dip);
	}
	f2fs_lz4_decompress_asm_fn[i] = _f2fs_lz4_decompress_asm;
	return _f2fs_lz4_decompress_asm(dst_ptr, dst_begin, dst_end,
				   src_ptr, src_end, dip);
}

int (*f2fs_lz4_decompress_asm_fn[NR_CPUS])(uint8_t **dst_ptr, uint8_t *dst_begin,
	uint8_t *dst_end, const uint8_t **src_ptr,
	const uint8_t *src_end, bool dip)
__read_mostly = {
	[0 ... NR_CPUS-1]  = f2fs_lz4_decompress_asm_select,
};

