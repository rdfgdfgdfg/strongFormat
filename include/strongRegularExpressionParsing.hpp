#pragma once

#include <strongRegularExpression.hpp>
#include <map>

namespace TUT {
	namespace re {
		namespace expres {
			using map = std::map<string, Ptr>;
			map callMap;//@”≥…‰±Ì

			Ptr expressionName = StringExceptRepeat_makePtr(L"\n :£∫", 1, -1);
			Ptr empty = StringOrRepeat_makePtr(L" \n\t", 0, -1);
			Ptr StringObj = Or_makePtr();//"..." / `path`
			Ptr StringOrObj = Vector_makePtr();//or"..." / or`path`
			Ptr StringExceptObj = Vector_makePtr();//exc"..." / exc`path`
			Ptr RepeatRangeObj = Vector_makePtr();// (min, max) * obj
			Ptr OrObj = Vector_makePtr();//< ... >
			Ptr VectorObj = Vector_makePtr();//[...]
			//Ptr CallObj = Vector_makePtr();//@ClassName{...}

			Ptr object = Or_makePtr(resVector({
				StringObj,
				StringOrObj,
				StringExceptObj,
				RepeatRangeObj,
				OrObj,
				VectorObj
			}));

			Ptr all = Vector_makePtr(resVector({
				empty, expressionName, empty, StringOr_makePtr(L":£∫"), empty, object
				}));


			void _init() {
				Ptr escape = Or_makePtr(resVector({
					String_makePtr(L"\\n"),
					String_makePtr(L"\\t"),
					String_makePtr(L"\\\"")
				}));
				StringObj.get<Or*>()->setTemplated(resVector({
					Vector_makePtr(resVector({
						String_makePtr(L"\""), Separate_makePtr(StringExceptRepeat_makePtr(L"\\\"\n", 0, -1, true), escape, 0, -1), String_makePtr(L"\"")
						})),
					Vector_makePtr(resVector({
						String_makePtr(L"`"), Separate_makePtr(StringExceptRepeat_makePtr(L"`\n", 0, -1, true), String_makePtr(L"\\`"), 0, -1), String_makePtr(L"`")
						}))
				}));
				StringOrObj.get<Vector*>()->setTemplated(resVector({String_makePtr(L"or"), StringObj}));
				StringExceptObj.get<Vector*>()->setTemplated(resVector({ String_makePtr(L"exc"), StringObj }));
				Ptr numbers = StringOr_makePtr(L"0123456789");
				Ptr numbersTu = Vector_makePtr(resVector({ StringOr_makePtr(L"(£®"), empty, numbers, empty, StringOr_makePtr(L"£¨,"),empty, numbers,empty,String_makePtr(L")£©") }));
				RepeatRangeObj.get<Vector*>()->setTemplated(resVector({ numbersTu, empty, String_makePtr(L"*"), empty, object}));
				Ptr comma = Vector_makePtr(resVector({ empty, StringOr_makePtr(L",£¨"), empty }));
				OrObj.get<Vector*>()->setTemplated(resVector({ String_makePtr(L"<"),Separate_makePtr(object, comma, 1, -1),String_makePtr(L">")}));
				VectorObj.get<Vector*>()->setTemplated(resVector({ String_makePtr(L"["),Separate_makePtr(object, comma, 1, -1),String_makePtr(L"]") }));

			}



		}

	}
}