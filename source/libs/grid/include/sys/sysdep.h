
#define BP_SYM(name) _BP_SYM (name)
#if __BOUNDED_POINTERS__
# define _BP_SYM(name) __BP_##name
#else
# define _BP_SYM(name) name
#endif

#ifdef	__ASSEMBLER__

/* Used by some assembly code.  */
#ifdef NO_UNDERSCORES
#define C_SYMBOL_NAME(name)	name
#define C_LABEL(name)		name##:
#else
#define C_SYMBOL_NAME(name)	_##name
#define C_LABEL(name)		_##name##:
#endif

#endif