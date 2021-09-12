# strongRegularExpression
RegularExpression++ for c++. An easy and useful objective-library, with big difference between normal regular expression library  
It's used for complex regular expression matching. The biggest difference between strongRegularExpression and normal regular expression library is recursiveness.  
strongRegularExpression has strong scalability, if you are not satisfied with the library, you can do some DIY.   

# example
`[aic8s0]+` -> a:`@StringOrRepeat{"aic8s0",0,-1}` -> StringOrRepeat_makePtr("aic8s0",0,-1)  
`[(abc)(efg)+]` -> a:`<"abc", (1,-1) * "efg">` -> Vector_makePtr(resVector({String_makePtr(L"abc"), RepeatRange_makePtr(String_makePtr(L"efg"),1,-1)}))  
`[0-9][a-z]+` -> a:`[numbers, (1,-1) * letters]` -> Vector_makePtr(resVector({numbers, RepeatRange_makePtr(letters, 0,-1)}))
`[(abc)(_*this)]` -> a:`["abc", a]


**Sadly, due to force majeure, the project has to be suspended**  
**I have invented the language and the parser (in include/strongRegularExpressionParsing.hpp), but I haven't invent the compiler**
**Luckily, the project is almost done. It can satisfy complex needs. The only shortcoming is the amount of code.**

if you have any questions. email me