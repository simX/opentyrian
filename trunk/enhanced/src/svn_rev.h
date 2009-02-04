// This file is for use with build systems that want to set the revision number (like mine ;)
//   --yuriks

#ifdef SVN_REV
#	define HAVE_SVN_REV
#	define STRINGIFY(str) (#str)
#	define SVN_REV_TMP STRINGIFY(SVN_REV)
#	define SVN_REV SVN_REV_TMP
#	undef SVN_REV_TMP
#	undef STRINGIFY
#else
#	undef HAVE_SVN_REV
#	define SVN_REV "0"
#endif
