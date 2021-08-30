#include "example.h"

using namespace TUT::format;

int main() {
	String str(L"hello");
	RepeatRange rrg0(&str, 3, 6, false);
	RepeatRange rrg1(&str, 3, 6, true);
	/*
	RepeatRange(
		Basic* repeated, //the ptr of repeated box. RepeatRange itself won't delete the ptr
		size_t min, //the min repeat-time
		size_t max, //the max repeat-time
		bool greedy = false//greedy option. if it's true, RepeatRange will try to match as much as possible
	)
	*/

	const wchar_t* wstr = L"hellohellohellohellohello";//5*hello
	rrg0.match(wstr);
	rrg1.match(wstr);

	resVector vec0 = rrg0.getResult();//use getResult method to get the result
	resVector vec1 = rrg1.getResult();//
}