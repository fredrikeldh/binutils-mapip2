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


static reloc_howto_type mapip2_elf_howto_table[] =
{
MAPIP_HOWTOS(MAPIP_HOWTO_TYPE)
};

static reloc_howto_type*
mapip2_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
	bfd_reloc_code_real_type code)
{
	size_t i;
	for(i=0; i<ARRAY_SIZE(mapip2_elf_howto_table); i++) {
		reloc_howto_type* howto = &mapip2_elf_howto_table[i];
		if(code == howto->type)
			return howto;
	}
	return NULL;
}

static reloc_howto_type *
mapip2_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
	const char *r_name)
{
	size_t i;
	for(i=0; i<ARRAY_SIZE(mapip2_elf_howto_table); i++) {
		reloc_howto_type* howto = &mapip2_elf_howto_table[i];
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
  assert (r_type < ARRAY_SIZE(mapip2_elf_howto_table));
  cache_ptr->howto = & mapip2_elf_howto_table[r_type];
}

#define HAVE_RELOCATE_SECTION 1
#if HAVE_RELOCATE_SECTION

/* Relocate an Mapip2 ELF section.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjusting the section contents as
   necessary, and (if using Rela relocs and generating a relocatable
   output file) adjusting the reloc addend as necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocatable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static bfd_boolean
mapip2_elf_relocate_section (bfd *output_bfd,
	struct bfd_link_info *info,
	bfd *input_bfd,
	asection *input_section,
	bfd_byte *contents,
	Elf_Internal_Rela *relocs,
	Elf_Internal_Sym *local_syms,
	asection **local_sections)
{
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes;
	Elf_Internal_Rela *rel;
	Elf_Internal_Rela *relend;

	symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes (input_bfd);
	relend = relocs + input_section->reloc_count;

	for (rel = relocs; rel < relend; rel++)
	{
		reloc_howto_type *howto;
		unsigned long r_symndx;
		Elf_Internal_Sym *sym;
		asection *sec;
		struct elf_link_hash_entry *h;
		bfd_vma relocation;
		bfd_reloc_status_type r;
		const char *name = NULL;
		int r_type;

		r_type = ELF32_R_TYPE (rel->r_info);
		r_symndx = ELF32_R_SYM (rel->r_info);

#if 0
		if (r_type == R_OPENRISC_GNU_VTINHERIT
			|| r_type == R_OPENRISC_GNU_VTENTRY)
			continue;
#endif

		if ((unsigned int) r_type >
			(sizeof mapip2_elf_howto_table / sizeof (reloc_howto_type)))
			abort ();

		howto = mapip2_elf_howto_table + ELF32_R_TYPE (rel->r_info);
		h = NULL;
		sym = NULL;
		sec = NULL;

		if (r_symndx < symtab_hdr->sh_info)
		{
			sym = local_syms + r_symndx;
			sec = local_sections[r_symndx];
			relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

			name = bfd_elf_string_from_elf_section
				(input_bfd, symtab_hdr->sh_link, sym->st_name);
			name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
		}
		else
		{
			bfd_boolean unresolved_reloc, warned;

			RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				r_symndx, symtab_hdr, sym_hashes,
				h, sec, relocation,
				unresolved_reloc, warned);
			name = h->root.root.string;
		}

		if (sec != NULL && elf_discarded_section (sec))
			RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
				rel, relend, howto, contents);

		if (info->relocatable)
			continue;

		r = _bfd_final_link_relocate (howto, input_bfd, input_section,
			contents, rel->r_offset, relocation, rel->r_addend);

		if (r != bfd_reloc_ok)
		{
			const char *msg = NULL;

			switch (r)
				{
				case bfd_reloc_overflow:
					r = info->callbacks->reloc_overflow
						(info, (h ? &h->root : NULL), name, howto->name,
						(bfd_vma) 0, input_bfd, input_section, rel->r_offset);
					break;

				case bfd_reloc_undefined:
					r = info->callbacks->undefined_symbol
						(info, name, input_bfd, input_section, rel->r_offset, TRUE);
					break;

				case bfd_reloc_outofrange:
					msg = _("internal error: out of range error");
					break;

				case bfd_reloc_notsupported:
					msg = _("internal error: unsupported relocation error");
					break;

				case bfd_reloc_dangerous:
					msg = _("internal error: dangerous relocation");
					break;

				default:
					msg = _("internal error: unknown error");
					break;
				}

			if (msg)
				r = info->callbacks->warning
					(info, msg, name, input_bfd, input_section, rel->r_offset);

			if (!r)
				return FALSE;
		}
	}

	return TRUE;
}
#endif

static bfd_boolean
mapip2_elf_object_p (bfd *abfd)
{
	abfd->flags |= D_PAGED;
	return TRUE;
}



#define TARGET_LITTLE_SYM mapip2_vec
#define TARGET_LITTLE_NAME "elf32-mapip2"

#define ELF_ARCH			bfd_arch_mapip2
#define ELF_MACHINE_CODE		EM_MAPIP2
#define ELF_MAXPAGESIZE			0x100

#define elf_info_to_howto_rel		NULL
#define elf_info_to_howto		mapip2_info_to_howto_rela
#if HAVE_RELOCATE_SECTION
#define elf_backend_relocate_section	mapip2_elf_relocate_section
#endif
//#define elf_backend_gc_mark_hook	openrisc_elf_gc_mark_hook
//#define elf_backend_check_relocs	openrisc_elf_check_relocs

#if 0	//causes crash unless elf_backend_relocate_section is defined
#define bfd_elf32_bfd_link_hash_table_create _bfd_elf_link_hash_table_create
#define bfd_elf32_bfd_link_add_symbols	bfd_elf_link_add_symbols
#define bfd_elf32_bfd_final_link	bfd_elf_final_link
#endif

#define elf_backend_can_gc_sections	1
#define elf_backend_rela_normal		1

#define bfd_elf32_bfd_reloc_type_lookup mapip2_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup mapip2_reloc_name_lookup

#define elf_backend_object_p mapip2_elf_object_p
//#define elf_backend_final_write_processing  openrisc_elf_final_write_processing
#include "elf32-target.h"
