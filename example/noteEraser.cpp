#include "example.h"

using namespace TUT::format;

Ptr comment = Vector_makePtr(resVector({ 
	String_makePtr(L"/*"), 
	RepeatRange_makePtr(StringExcept_makePtr(), 0,-1),
	String_makePtr(L"*/")
}));

Ptr line = Vector_makePtr(resVector({
	String_makePtr(L"//"),
	Separate_makePtr(RepeatRange_makePtr(StringExcept_makePtr(L"\n"), 0,-1), String_makePtr(L"\\\n"), 0, -1, true),
	String_makePtr(L"\n")
}));

class Note : public Or {
	void _clear() {}
public:
	Note() :Or(resVector({
		comment, line
	})) {}
	Note(const Note& obj) :Or(obj) {}
	virtual ~Note() {}
	virtual void rematch() { throw match_error(); }

	VIRTUAL_TEMPLATE_DEF(Note, ());
};
POLYMORPHIC_PTR_MAKE(Note_makePtr, Note);

Ptr message = Separate_makePtr(RepeatRange_makePtr(StringExcept_makePtr(), 0, -1), Note_makePtr(), 0, -1, true);

#include <fstream>
#include <locale>
#include <codecvt>
#include <sstream>
using namespace std;
int main(int argv, char**argc) {
	if (argv != 3) { abort(); }
	ifstream iFile(argc[1]);
	ofstream oFile(argc[2]);
	stringstream ss;
	ss << iFile.rdbuf();
	wstring_convert< codecvt_utf8<wchar_t>> wcv;
	wstring wstr = wcv.from_bytes(ss.str());
	message.match(wstr.data());
	resVector result = message.get<Separate*>()->getResult();
	for (auto i = result.begin(); i != result.end(); i++++) {
		oFile << wcv.to_bytes((*i).moveStrResult());
	}
	iFile.close();
	oFile.close();
}