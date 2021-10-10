# language
A language has been invented to make string developing easier. Its name is REL.
It's based on library [strongRegularExpression](https://github.com/rdfgdfgdfg/strongRegularExpression).

## box
Box is a container and it can store a string or a kind of string. 
A box can contain other boxes.  
## syntax
a REL code file is made up of many box definition sentences  
what a box definition sentence look like : `box-name : box expression;`   
Ways to refer a box  

|expression|type|note|example|
|---|---|---|---|
|`box-name`|direct refer|the name must be defined in early box definition sentences|`boxname`|
|`"string"` / \`string file\` | literal-string|in string definition, special characters like \n\t\\ must be escaped, in string file definition, the value is the path of string file, the \` in path must be escaped as \\`|`"hello world"` / \`/docs/hello.txt\`|
|`or"string"` / `or\`string file\``| or-string|the same coding rules to literal-string. It can match a character that it contains|`or"abcdef"` can match `a`,`b`,`c`,`d`,`e`,`f`
|`exc"string"` / `exc\`string file\``|except-string|the same coding rules to literal-string. It can match a character that it doesn't contain|`exc"abcdef"` can match any character except `a`,`b`,`c`,`d`,`e`,`f`
|`(min, max) * obj`|repeat-range-box|the `obj` will be called and match for at least `min` times, at most `max` times. `min` and `max` must be numbers, `-` stands for infinity|`(0, 2) * "hello"` can match empty string, `hello` or `hellohello`
|`<obj0, obj1, ... objN>`|or-box|a box in it will be called and match|`<"hello", (0, 2) * "world">` can match `hello`, empty string, `world` or `worldworld`
|`[obj0, obj1, ... objN]`|vector-box|all boxes in it will be called and match one by one|`["hello", (0, 2)*"world"]` can match `hello`,`helloworld`,`helloworldworld`
|`@ExtraClassName{...}`|extra-call-box|it stands for an extra box, the `...` will init the extra box|each `ExtraClass` has different use and initialization, see their docs please