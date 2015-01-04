// Methods for type_info for -*- C++ -*- Run Time Type Identification.


#include <cstddef>
#include "tinfo.h"
#include "new"			// for placement new

// We can't rely on having stdlib.h if we're freestanding.
extern "C" void abort ();

using std::type_info;

#if !__GXX_MERGED_TYPEINFO_NAMES

bool
type_info::before (const type_info &arg) const
{
  return __builtin_strcmp (name (), arg.name ()) < 0;
}

#endif

#include <cxxabi.h>

namespace __cxxabiv1 {

using namespace std;

// This has special meaning to the compiler, and will cause it
// to emit the type_info structures for the fundamental types which are
// mandated to exist in the runtime.
__fundamental_type_info::
~__fundamental_type_info ()
{}

__array_type_info::
~__array_type_info ()
{}

__function_type_info::
~__function_type_info ()
{}

__enum_type_info::
~__enum_type_info ()
{}

__pbase_type_info::
~__pbase_type_info ()
{}

__pointer_type_info::
~__pointer_type_info ()
{}

__pointer_to_member_type_info::
~__pointer_to_member_type_info ()
{}

bool __pointer_type_info::
__is_pointer_p () const
{
  	return true;
}

bool __function_type_info::
__is_function_p () const
{
  	return true;
}

bool __pbase_type_info::
__do_catch (const type_info *thr_type,
            void **thr_obj,
            unsigned outer) const
{
  	if (*this == *thr_type)
    	return true;      // same type
  	if (typeid (*this) != typeid (*thr_type))
    	return false;     // not both same kind of pointers
  
  	if (!(outer & 1))
    	// We're not the same and our outer pointers are not all const qualified
    	// Therefore there must at least be a qualification conversion involved
    	// But for that to be valid, our outer pointers must be const qualified.
    	return false;
  
  	const __pbase_type_info *thrown_type =
    	static_cast <const __pbase_type_info *> (thr_type);
  
  	if (thrown_type->__flags & ~__flags)
    	// We're less qualified.
    	return false;
  
  	if (!(__flags & __const_mask))
    	outer &= ~1;
  
  	return __pointer_catch (thrown_type, thr_obj, outer);
}

inline bool __pbase_type_info::
__pointer_catch (const __pbase_type_info *thrown_type,
                 void **thr_obj,
                 unsigned outer) const
{
  	return __pointee->__do_catch (thrown_type->__pointee, thr_obj, outer + 2);
}

bool __pointer_type_info::
__pointer_catch (const __pbase_type_info *thrown_type,
                 void **thr_obj,
                 unsigned outer) const
{
  	if (outer < 2 && *__pointee == typeid (void))
    {
      	// conversion to void
      	return !thrown_type->__pointee->__is_function_p ();
    }
  
  	return __pbase_type_info::__pointer_catch (thrown_type, thr_obj, outer);
}

bool __pointer_to_member_type_info::
__pointer_catch (const __pbase_type_info *thr_type,
                 void **thr_obj,
                 unsigned outer) const
{
  	// This static cast is always valid, as our caller will have determined that
  	// thr_type is really a __pointer_to_member_type_info.
  	const __pointer_to_member_type_info *thrown_type =
    	static_cast <const __pointer_to_member_type_info *> (thr_type);
  
  	if (*__context != *thrown_type->__context)
    	return false;     // not pointers to member of same class
  
  	return __pbase_type_info::__pointer_catch (thrown_type, thr_obj, outer);
}

} // namespace std
