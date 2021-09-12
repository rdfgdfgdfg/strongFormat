#include "example.h"


//this is a wrong example
int main() {
	String str(L"hello");
	try {
		str.match(L"Ahhhhhhhhhhhhhhh!!!!!!!!!!!!!hello");
	}
	catch (match_error) {//if the box failed to match, it will throw a match_error
		std::wcout << L"remember : TUT::re-box won't search the C wstring\n";
	}
}