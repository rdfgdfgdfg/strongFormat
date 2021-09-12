#include "example.h"

#include <strongRegularExpressionParsing.hpp>

using namespace TUT::re::expres;
using namespace TUT::re;
int main() {
	_init();
	Ptr temp = object.matchPcopy();
	temp.matchFull(L"[\"aaa\\nss\" ,  \"3dc\"]");

}