#include "example.h"

using namespace TUT::format;

int main() {
	String str(L"hello");

	Ptr strP = String_makePtr<string>(L"hello");
	//use String_makePtr to make a Ptr of String safely
	//it's similar to std::shared_ptr
	//equals to makePtr<String, string>(L"hello");

	RepeatRange rrg0(strP, 3, 6, false);
	RepeatRange rrg1(strP, 3, 6, true);
	/*
	RepeatRange(
		Ptr repeated, //the ptr of repeated box.
		size_t min, //the min repeat-time
		size_t max, //the max repeat-time
		bool greedy = false//greedy option. if it's true, RepeatRange will try to match as much as possible
	)
	*/

	const wchar_t* wstr = L"hellohellohellohellohello";//5*hello
	rrg0.match(wstr);
	rrg1.match(wstr);

	resVector vec0 = rrg0.getResult();//use getResult method to get the result
	resVector vec1 = rrg1.getResult();//the type of the result is std::vector<Ptr>

	std::printf("The size of vec0 is %llu\n", vec0.size());
	std::printf("The size of vec1 is %llu\n", vec1.size());

	rrg0.rematch();//method rematch() is used to match again to get another result, but the start doesn't change
	rrg1.rematch();//if rematch failed, it will throw TUT::format::match_error too

	resVector vec0_ = rrg0.getResult();
	resVector vec1_ = rrg1.getResult();


	std::printf("The size of vec0 is %llu\n", vec0_.size());
	std::printf("The size of vec1 is %llu\n", vec1_.size());
	
	//tip:RepeatConstTimes is similar to RepeatRange
	//RepeatRange(repeatedPtr, n, n) = RepeatConstTimes(repeatedPtr, n)
}