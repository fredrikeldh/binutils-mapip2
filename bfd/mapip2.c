#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "libiberty.h"
#include "elf-bfd.h"
#include <assert.h>

#define MAPIP_BIN 0

#if MAPIP_BIN
static const bfd_target* mapip2_object_p(bfd *abfd);

static bfd_boolean mapip2_mkobject(bfd *abfd);
static bfd_boolean mapip2_write_object_contents(bfd *abfd);

// this target describes the mapip2 bytecode file format.

// return mapip2_vec if abfd is a valid mapip2 object file.
static const bfd_target* mapip2_object_p(bfd* abfd ATTRIBUTE_UNUSED) {
	// check magic word
	// check version
	// sanity-check contents: read header, check that values are sane, check section sizes against file size.
	abort();
}

static bfd_boolean mapip2_mkobject(bfd* abfd ATTRIBUTE_UNUSED) {
  return TRUE;
}

typedef struct _MA_HEAD_V1
{
	char magic[4];
	uint32_t version;
	uint32_t codeLen;
	uint32_t dataLen;
	uint32_t dataSize;
	uint32_t stackSize;
	uint32_t heapSize;
	uint32_t buildId;
	uint32_t entryPoint;
} MA_HEAD_V1;

#define ENSURE(a) if(!(a)) abort()

// fill the values codeLen and dataLen.
// abort if multiple sections of the same type are encountered.
static void fillHeader(bfd* abfd ATTRIBUTE_UNUSED, struct bfd_section* sect, void* user) {
	MA_HEAD_V1* hp = (MA_HEAD_V1*)user;
	if(sect->flags & SEC_CODE) {
		ENSURE(!(sect->flags & SEC_DATA));
		ENSURE(hp->codeLen == 0);
		hp->codeLen = sect->size;
	} else if(sect->flags & SEC_DATA) {
		ENSURE(hp->dataLen == 0);
		hp->dataLen = sect->size;
	} else {
		// there shouldn't be any other sections.
		abort();
	}
}

static void writeSection(bfd* abfd, struct bfd_section* sect, void* user) {
	bfd_boolean* success_p = (bfd_boolean*)user;
	ENSURE(sect->flags & SEC_IN_MEMORY);
	if(bfd_bwrite(sect->contents, sect->size, abfd) != sect->size)
		*success_p = FALSE;
}

typedef struct mapip2_data {
	uint32_t dataSize;
	uint32_t stackSize;
	uint32_t heapSize;
	uint32_t buildId;
	uint32_t entryPoint;
} mapip2_data;

static bfd_boolean mapip2_write_object_contents(bfd* abfd) {
	mapip2_data* mp2 = (mapip2_data*)abfd->tdata.any;
	// fill header.
	MA_HEAD_V1 head = {
		"MAP2",
		1,
		0,
		0,
		mp2->dataSize,
		mp2->stackSize,
		mp2->heapSize,
		mp2->buildId,
		mp2->entryPoint,
	};

	// look at the sections.
	bfd_map_over_sections(abfd, fillHeader, &head);

	ENSURE(head.codeLen != 0);
	ENSURE(head.dataLen != 0);

	// todo: count BSS
	ENSURE(head.dataSize >= head.dataLen + head.stackSize + head.heapSize);

	// write header.
	if(bfd_bwrite(&head, sizeof(head), abfd) != sizeof(head))
		return FALSE;

	// write sections. assume code section comes first.
	bfd_boolean success = TRUE;
	bfd_map_over_sections(abfd, writeSection, &success);

	return success;
}
#endif	//MAPIP_BIN

// size is not in bytes. see reloc.c:877.
#define MAPIP_HOWTO_TYPE(type, size, bitsize, pc_relative, complain, mask) \
	HOWTO(BFD_RELOC_##type, 0, size, bitsize, pc_relative, 0, complain, bfd_elf_generic_reloc, "R_MAPIP2_" #type, FALSE, 0, mask, FALSE),

#define MAPIP_HOWTOS(m) \
	m(NONE, 0, 0, FALSE, complain_overflow_dont, 0)\
	m(32_PCREL, 2, 32, TRUE, complain_overflow_signed, 0xffffffff)\
	m(32, 2, 32, FALSE, complain_overflow_signed, 0xffffffff)\


static reloc_howto_type elf_mapip_howto_table[] =
{
MAPIP_HOWTOS(MAPIP_HOWTO_TYPE)
};

static reloc_howto_type*
mapip2_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
	bfd_reloc_code_real_type code)
{
	for(size_t i=0; i<ARRAY_SIZE(elf_mapip_howto_table); i++) {
		reloc_howto_type* howto = &elf_mapip_howto_table[i];
		if(code == howto->type)
			return howto;
	}
	return NULL;
}

static reloc_howto_type *
mapip2_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
	const char *r_name)
{
	for(size_t i=0; i<ARRAY_SIZE(elf_mapip_howto_table); i++) {
		reloc_howto_type* howto = &elf_mapip_howto_table[i];
		if(strcasecmp (howto->name, r_name) == 0)
			return howto;
	}
	return NULL;
}

static void
mapip2_info_to_howto_rela (bfd * abfd ATTRIBUTE_UNUSED,
	arelent * cache_ptr,
	Elf_Internal_Rela * dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  assert (r_type < ARRAY_SIZE(elf_mapip_howto_table));
  cache_ptr->howto = & elf_mapip_howto_table[r_type];
}

#define TARGET_LITTLE_SYM mapip2_vec
#define TARGET_LITTLE_NAME "elf32-mapip2"

#define ELF_ARCH			bfd_arch_mapip2
#define ELF_MACHINE_CODE		0//EM_MAPIP2
#define ELF_MAXPAGESIZE			0x1000

#define elf_info_to_howto_rel		NULL
#define elf_info_to_howto		mapip2_info_to_howto_rela
//#define elf_backend_relocate_section	openrisc_elf_relocate_section
//#define elf_backend_gc_mark_hook	openrisc_elf_gc_mark_hook
//#define elf_backend_check_relocs	openrisc_elf_check_relocs

#define elf_backend_can_gc_sections	1
#define elf_backend_rela_normal		1

#define bfd_elf32_bfd_reloc_type_lookup mapip2_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup mapip2_reloc_name_lookup

//#define elf_backend_object_p                openrisc_elf_object_p
//#define elf_backend_final_write_processing  openrisc_elf_final_write_processing
#include "elf32-target.h"
