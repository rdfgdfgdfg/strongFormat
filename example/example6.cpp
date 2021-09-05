#include "example.h"

using namespace TUT::format;

//you can inherit the TUT::format::Basic
class A : public Basic {
	//this class is equals to RepeatRange(StringOr(...), , )
protected:
	string wstr;
	size_t min;
	size_t max;

	inline void _clear() { Basic::_clear(); }//the function is used to clear that the box can match again
	//Although you don't need to do anything, add the _clear() of base class

	void check_singel() {
		if (wstr.find(ptr[size]) == string::npos) {//if wstr doesn't contain ptr[size]
			throw match_error();
		}
	}
public:
	A(bool greedy = false):min(0), max(0), Basic(greedy) {}//No args constructor
	A(string wstr, size_t min, size_t max, bool greedy = false) :wstr(wstr), min(min), max(max), Basic(greedy) {}
	//defined all constructor
	A(const A&obj):A(obj.wstr, obj.min, obj.max, obj.greedy) {}//copy constructor

	//these constructors are necessary

	virtual ~A() {}

	virtual void match(const wchar_t* sptr) {
		ptr = const_cast<wchar_t*>(sptr);//this is necessary
		//ptr is the start of matched string

		for (; size < min; size++) {//size is the size of matched string(wchar_t)
			check_singel();
		}//match
		if (greedy) {//if greedy is true
			for (; size < max; size++) {//match as much as possible
				try {
					check_singel();
				}
				catch (match_error) {
					return;
				}
			}
		}
	}

	virtual void rematch() {//get another result
		//remember! Don't change this->ptr in this function

		//you should try to change this->size in this function
		//otherwise, this function will cause bug or make programme slowwer
		if (greedy) {
			if (size > min) {
				size--;
			}
			else {
				throw match_error();
			}
		}
		else {
			if (size < max) {
				check_singel();
				size++;
			}
			else {
				throw match_error();
			}
		}
	}

	VIRTUAL_TEMPLATE_DEF(A, (wstr, min, max, greedy));
	//the first arg is your class name, the second is the longest init-list of your class
	//if you don't have this line
	//the class can't be instantiated
	//make sure it's in public area
};

POLYMORPHIC_PTR_MAKE(A_makePtr, A);//define the makePtr function of class A

int main() {//everything is done, let's test
	A a(L"0123456789", 0, 10, true);
	a.match(L"282384wmsjficaw");
	std::wcout << a.moveStrResult() << std::endl;
}