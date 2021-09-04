// example.cpp: 定义应用程序的入口点。
//

#include "example.h"

using namespace TUT::format;

int main()
{
	String str(L"hello");//create a box to save string
	const wchar_t* wstr = L"hello!!!!!!!!!!!!!!!!";//The matched C wstring
	str.match(wstr);//The box will try to match from start
	
	std::wcout << str.moveStrResult() << std::endl;//print "hello"
	
	wchar_t* result = str.getPtr();//get the start position of the box in original C wstring
	//because of this, it can't add a '\0' to the end
	//so, don't use wcout or wprintf to print the result
	//the right way is to use a ergodic
	
	wchar_t* end = result + str.getSize();
	for(; result < end; result++) {
		std::wcout << *result;
	}//print "hello"
	std::wcout << std::endl;
	return 0;
}
