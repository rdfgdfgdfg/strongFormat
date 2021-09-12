#include "example.h"


int main() {
	Ptr str = String_makePtr(L"--");
	Ptr str_ = String_makePtr(L"++");
	Ptr or_ = Or_makePtr(resVector({ str , str_ }));//match any of the template
	RepeatRange rrg(or_, 0, 10, true);
	const wchar_t* wstr = L"--++++--";
	rrg.match(wstr);
	resVector result = rrg.getResult();
	for (auto i : result) {
		Or* orPtr = i.get<Or*>();//get the real position
		Ptr rtPtr = orPtr->getMatchedPtr();//get the ptr that matches successfully
		Ptr temPtr = orPtr->getMatchedOrgTemp();//get the template-ptr that copies from
		//Ptr is a kind of shared-ptr
		//don't need to care about when to delete it


		if (temPtr == str) {
			std::wcout << L"-- matched\n";
		}
		else if (temPtr == str_) {
			std::wcout << L"++ matched\n";
		}
		else {
			std::wcout << L"match failed\n";
		}
	}
	std::wcout << L"-------------------------------------------\n";
	Ptr strOr = StringOr_makePtr(L"0123456789");//StringOr is similar to Or
	//StringOr(L"abc...") = Or(resVector({&String("a"), &String("b"), &String("c") ...}))
	//StringOr do some optimization, it's faster than Or

	RepeatRange rrg_(strOr, 0, 10, true);
	rrg_.match(L"43836418");
	resVector result_ = rrg_.getResult();
	for (auto i : result_) {
		StringOr* orPtr = i.get<StringOr*>();
		std::wprintf(L"%s matched\n", orPtr->moveStrResult().data());
	}
}