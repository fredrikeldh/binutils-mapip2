#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

const bfd_arch_info_type bfd_mapip2_arch =
{
	32,		  		/* 32 bits in a word */
	32,		  		/* 32 bits in an address */
	8,		  		/* 8 bits in a byte */
	bfd_arch_mapip2, 		/* Architecture */
	0,		  		/* Machine number - 0 for now */
	"Mapip2",	  /* Architecture name */
	"Mapip2",	  /* Printable name */
	3,		  		/* Section align power */
	TRUE,		  	/* Is this the default architecture ? */
	bfd_default_compatible,	/* Architecture comparison function */
	bfd_default_scan,	   	  /* String to architecture conversion */
	NULL			  /* Next in list */
};
