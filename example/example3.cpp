#include "example.h"

using namespace TUT::format;

int main() {
	String strA(L"Aaaaa");
	String strB(L"Bbbbb");
	RepeatRange rrg(&strB, 2, 5);
	String strC(L"Ccccc");
	Vector vec(resVector({ &strA, &rrg, &strC }));//Vector will match the obj one by one
	//tip: equals to 
	//Vector vec;
	//vec.setTemplated(resVector({ &strA, &strB, &strC }));
	vec.match(L"AaaaaBbbbbBbbbbBbbbbBbbbbCcccc");//strA + (4 * strB) + strC
	resVector result = vec.getResult();//the way to get result is similar to RepeatRange or RepeatConstTimes
	wchar_t* wstr;
	for (auto i : result) {
		wstr = i->moveStrResult();
		std::wcout << wstr << std::endl;
		delete[] wstr;
	}

}