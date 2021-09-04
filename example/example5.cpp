#include <iostream>
#include <strongFormat.hpp>

using namespace TUT::format;

int main() {
	String spe(L" ");
	RepeatRange rrg(&spe, 0, -1);//match 0 to inf
	StringOr comma(L",£¬");
	Vector vec(resVector({ &rrg, &comma ,&rrg }));
	StringExcept secpt(L"");
	RepeatRange rrg_(&secpt, 0, -1);
	Separate sep(&rrg_, &vec, 0, -1, true);
	sep.match(L"kf,  a  , aac");

}