/**
	FOR test for cpuls functions.
*/

#include <new>
#include <stdio.h>
#include <typeinfo>
using namespace std;
extern "C" int printf(const char *fmt, ...);
class name_a {
	private:
		volatile int a;
	public:
		name_a(){a = 0;}
		name_a(int b){a = b;}
		virtual int  value_a(){return a + 19;}
		//virtual void test(){}
		virtual ~name_a(){}
	};
	
class name_c: public name_a
{
	private:
		volatile int b;
	public:
		name_c(int c):name_a(c)
		{
			b = c ;
		}
		virtual int value_a(){return b;};
		virtual ~name_c(){}
		
};
int main(int agrc, char* argv[])
{
	
	try 
	{
#if 1
	name_a *a = new name_a(20);
	name_c *b = new name_c(12);
	const char *name = typeid(*b).name();
	//char *name = b.name();
	printf("a is %d, %d, a address is %d,name %s\n", b->value_a(), a->value_a(), b, name);
#endif	
	throw name_a(20);
	
	}
	catch (name_a &cc)
	{
		printf("catch bad_alloc %d\n", cc.value_a());
	}
	//printf("return bad_alloc\n");
	return 0;
}


