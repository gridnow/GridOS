#ifndef _GETOPT_H
#define _GETOPT_H 1


#ifdef	__cplusplus
extern "C" {
#endif
	
	/* For communication from `getopt' to the caller.
	 When `getopt' finds an option that takes an argument,
	 the argument value is returned here.
	 Also, when `ordering' is RETURN_IN_ORDER,
	 each non-option ARGV-element is returned here.  */
	
	extern char *optarg;
	
	/* Index in ARGV of the next element to be scanned.
	 This is used for communication to and from the caller
	 and for communication between successive calls to `getopt'.
	 
	 On entry to `getopt', zero means this is the first call; initialize.
	 
	 When `getopt' returns -1, this is the index of the first of the
	 non-option elements that the caller should itself scan.
	 
	 Otherwise, `optind' communicates from one call to the next
	 how much of ARGV has been scanned so far.  */
	
	extern int optind;
	
	/* Callers store zero here to inhibit the error message `getopt' prints
	 for unrecognized options.  */
	
	extern int opterr;
	
	/* Set to an option character which was unrecognized.  */
	
	extern int optopt;
	
#ifndef __need_getopt
	/* Describe the long-named options requested by the application.
	 The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
	 of `struct option' terminated by an element containing a name which is
	 zero.
	 
	 The field `has_arg' is:
	 no_argument		(or 0) if the option does not take an argument,
	 required_argument	(or 1) if the option requires an argument,
	 optional_argument 	(or 2) if the option takes an optional argument.
	 
	 If the field `flag' is not NULL, it points to a variable that is set
	 to the value given in the field `val' when the option is found, but
	 left unchanged if the option is not found.
	 
	 To have a long-named option do something other than set an `int' to
	 a compiled-in constant, such as set a value from `optarg', set the
	 option's `flag' field to zero and its `val' field to a nonzero
	 value (the equivalent single-letter option character, if there is
	 one).  For long options that have a zero `flag' field, `getopt'
	 returns the contents of the `val' field.  */
	
	struct option
	{
		const char *name;
		/* has_arg can't be an enum because some compilers complain about
		 type mismatches in all the code that assumes it is an int.  */
		int has_arg;
		int *flag;
		int val;
	};
	
	/* Names for the values of the `has_arg' field of `struct option'.  */
	
# define no_argument		0
# define required_argument	1
# define optional_argument	2
#endif	/* need getopt */
	
	
	/* Many other libraries have conflicting prototypes for getopt, with
	 differences in the consts, in stdlib.h.  To avoid compilation
	 errors, only prototype getopt for the GNU C library.  */
	extern int getopt (int ___argc, char *const *___argv, const char *__shortopts);

	extern int getopt_long (int ___argc, char *const *___argv,
							const char *__shortopts,
							const struct option *__longopts, int *__longind);
	
	extern int getopt_long_only (int ___argc, char *const *___argv,
								 const char *__shortopts,
								 const struct option *__longopts, int *__longind);
	
#ifdef	__cplusplus
}
#endif

#endif /* getopt.h */
