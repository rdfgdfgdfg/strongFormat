#include "example.h"


int main() {
	Ptr strA = String_makePtr(L"Aaaaa");
	Ptr strB = String_makePtr(L"Bbbbb");

	Ptr rrg = RepeatRange_makePtr(strB, 2, 5);//ClassName_`makePtr` is used to make a Ptr of ClassName
	//equals to makePtr<ClassName, initTypes...>(initArgs)

	Ptr strC = String_makePtr(L"Ccccc");
	Vector vec(resVector({ strA, rrg, strC }));//Vector will match the obj one by one
	//tip: equals to 
	//Vector vec;
	//vec.setTemplated(resVector({ strA, strB, strC }));
	vec.match(L"AaaaaBbbbbBbbbbBbbbbBbbbbCcccc");//strA + (4 * strB) + strC
	resVector result = vec.getResult();//the way to get result is similar to RepeatRange or RepeatConstTimes
	for (auto i : result) {//i is Ptr
		std::wcout << i.moveStrResult() << std::endl;
	}//the way to use Ptr is in the next example

}