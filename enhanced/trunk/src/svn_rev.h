// This file is for use with build systems that want to set the revision number (like mine ;)
//   --yuriks

#ifdef SVN_REV
#	define HAVE_SVN_REV
#else
#	undef HAVE_SVN_REV
#	define SVN_REV 0xFFFF
#endif
