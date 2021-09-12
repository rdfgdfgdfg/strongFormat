#include "example.h"


int main() {
	Ptr spe = String_makePtr(L" ");
	Ptr rrg = RepeatRange_makePtr(spe, 0, -1);//match 0 to inf
	Ptr comma = StringOr_makePtr(L",£¬");
	Ptr vec = Vector_makePtr(resVector({ rrg, comma ,rrg }));
	Ptr secpt = StringExcept_makePtr(L"");
	Ptr rrg_ = RepeatRange_makePtr(secpt, 0, -1);
	Separate sep(rrg_, vec, 0, -1, true);//rrg_ vec rrg... rrg
	sep.match(L"kf,  a  , aac");
	for (auto i : sep.getResult()) {
		std::wcout << i.moveStrResult() << std::endl;
	}
}