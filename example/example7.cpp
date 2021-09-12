#include "example.h"

#include <strongFormatParsing.hpp>

using namespace TUT::format::expres;
using namespace TUT::format;
int main() {
	_init();
	Ptr temp = object.matchPcopy();
	temp.matchFull(L"\"aa\\naaaa\"");

}