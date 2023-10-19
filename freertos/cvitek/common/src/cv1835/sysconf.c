#include "linux/confname.h"

/* Return the system page size.  */
int __getpagesize(void)
{
	return 1;
//  assert (GLRO(dl_pagesize) != 0);
//  return GLRO(dl_pagesize);
}

long int sysconf(int name)
{
	switch (name) {
	case _SC_PAGESIZE:
		return __getpagesize();
		/* Also add obsolete or unnecessarily added constants here.  */
	case _SC_EQUIV_CLASS_MAX:
	default:
//		__set_errno (EINVAL);
		return -1;
	}
}
