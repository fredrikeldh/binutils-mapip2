#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "libiberty.h"

static const bfd_target* mapip2_object_p(bfd *abfd);

static bfd_boolean mapip2_mkobject(bfd *abfd);
static bfd_boolean mapip2_write_object_contents(bfd *abfd);

// this target describes the mapip2 bytecode file format.

const bfd_target mapip2_vec =
{
	"mapip2",
	bfd_target_unknown_flavour,
	BFD_ENDIAN_LITTLE,
	BFD_ENDIAN_LITTLE,
	(EXEC_P |
		WP_TEXT ),
	/* section flags */
	(SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_CODE | SEC_DATA),
	'_',				/* Symbol leading char.  */
	' ',				/* AR_pad_char.  */
	16,				/* AR_max_namelen.  */
	0,				/* match priority.  */

	bfd_getb64, bfd_getb_signed_64, bfd_putb64,
	bfd_getb32, bfd_getb_signed_32, bfd_putb32,
	bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Data.  */
	bfd_getb64, bfd_getb_signed_64, bfd_putb64,
	bfd_getb32, bfd_getb_signed_32, bfd_putb32,
	bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Headers.  */

	/* bfd_check_format.  */
	{_bfd_dummy_target,	/* bfd_unknown */
		mapip2_object_p,	/* bfd_object */
		bfd_generic_archive_p,	/* bfd_archive */
		_bfd_dummy_target},	/* bfd_core */

	{bfd_false, mapip2_mkobject,		/* bfd_set_format.  */
		_bfd_generic_mkarchive, bfd_false},
	{bfd_false, mapip2_write_object_contents,/* bfd_write_contents.  */
		_bfd_write_archive_contents, bfd_false},

	BFD_JUMP_TABLE_GENERIC (_bfd_generic),
	BFD_JUMP_TABLE_COPY (_bfd_generic),
	BFD_JUMP_TABLE_CORE (_bfd_nocore),
	BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
	BFD_JUMP_TABLE_SYMBOLS (_bfd_nosymbols),
	BFD_JUMP_TABLE_RELOCS (_bfd_norelocs),
	BFD_JUMP_TABLE_WRITE (_bfd_generic),
	BFD_JUMP_TABLE_LINK (_bfd_nolink),
	BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

	NULL,

	NULL
};

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
