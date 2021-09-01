#include <iostream>
#include <strongFormat.hpp>

using namespace TUT::format;

int main() {
	String str(L"--");
	String str_(L"++");
	Or or_(resVector({ &str , &str_ }));//match any of the template
	RepeatRange rrg(&or_, 0, 10, true);
	const wchar_t* wstr = L"--++++--";
	rrg.match(wstr);
	resVector result = rrg.getResult();
	for (auto i : result) {
		Or* orPtr = dynamic_cast<Or*>(i);
		Basic* rtPtr = orPtr->getMatchedPtr();//get the ptr that matches successfully
		Basic* temPtr = orPtr->getMatchedOrgTemp();//get the template-ptr that copies from
		if (temPtr == &str) {
			std::wcout << L"-- matched\n";
		}
		else if (temPtr == &str_) {
			std::wcout << L"++ matched\n";
		}
		else {
			std::wcout << L"match failed\n";
		}
	}
	std::wcout << L"-------------------------------------------\n";
	StringOr strOr(L"0123456789");//StringOr is similar to Or
	//StringOr(L"abc...") = Or(resVector({&String("a"), &String("b"), &String("c") ...}))
	//StringOr do some optimization, it's faster than Or

	RepeatRange rrg_(&strOr, 0, 10, true);
	rrg_.match(L"43836418");
	resVector result_ = rrg_.getResult();
	for (auto i : result_) {
		StringOr* orPtr = dynamic_cast<StringOr*>(i);
		wchar_t* outPtr = orPtr->moveStrResult();
		std::wprintf(L"%s matched\n", outPtr);
		delete[] outPtr;
	}
}