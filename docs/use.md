# use
everything is in namespace TUT::re  

|box name|inheritance path|use|template
|---|---|---|---
|`String`|`Basic`|match a portion of string that equals to the template|`string`
|`StringOr`|`Basic`|match a wchar in the template|`string`
|`StringExcept`|`Basic`|match a wchar not in the template|`string`
|`Or`|`Basic`|use any box in the template to match until success|`vector`<`Ptr`>
|`Vector`|`Basic` -> `Queue`|match all the boxes in the template one by one|`vector`<`Ptr`>
|`Separate`|`Basic` -> `Queue`|match the first one of template for many times, the separator is the second one of template|a pair of `Ptr`
|`RepeatConstTimes`|`Basic` -> `Queue` -> `Repeat`|match the template for many times|`Ptr`
|`RepeatRange`|`Basic` -> `Queue` -> `Repeat`|match the template for many times|`Ptr`

# `Basic`
|function|use
|---|---
|`Basic(bool greedy = false, bool banRematch = false)`|if greedy is true, the box will match as much as possible.If banRematch is true, the box won't try to rematch
|`virtual void match(const wchar_t\*)`|matching, set the start
|`virtual void rematch()`|match again to get another result, but don't change the start(seed to match first)
|`virtual void matchFull(const wchar_t\*)`|similar to `Basic::match`, but it will match until end
|`virtual Basic\* copy() const`|get a copy of itself
|`virtual Basic\* matchPcopy() const`|get a copy of itself only for matching, so some of the information will lost
|`virtual void clear()`|clear the result and prepare to `match`
|`string moveStrResult() const`|get the portion of string that matched sucessfully
-----
|variable that can be visited|get|set|meaning
|---|---|---|---
|`ptr`|`getPtr`||the start ptr
|`size`|`getSize`||how many it matches
|`greedy`|`getGreedy`|`setGreedy`|whether it matches as much as possible
-----

## `Ptr` ~~ `std::shared_ptr`<`Basic`>
It has every method that Basic has, but `get` is special  

|function|use
|---|---
|`Basic* get() const`|get the real ptr that it store
|`template<class T> T get() const`|get the real ptr and try to downcast
-----

# `String`, `StringOr`, `StringExcept`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`wstr`|`get`|`set`|the template string
-----

# `Or`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`templated`|`getTemplated`|`setTemplated`|the template
|`cpdPtr`|`getMatchedPtr`||the box that matches sucessfully in template(the box is a copy)
|`cpdIt`|`getMatchedOrgTemp->Ptr`||the box be copied by `cpdPtr`
-----

# `Queue`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`result`|`getResult`||the resVector that store boxes matched sucessfully
-----

# `Vector`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`templated`|`getTemplated`|`setTemplated`|the template
-----

# `Separate`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`templated`|`getTemplated`|`setTemplated`|the first of the template
|`split`|`getSplit`|`setSplit`|the second of the template (separator)
|`max`|`getMax`|`setMax`|the max repeating times of `templated`
|`min`|`getMin`|`setMin`|the min repeating times of `templated`
-----

# `Repeat`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`repeated`|`getRepeated`|`setRepeated`|the template
-----

# `RepeatConstTimes`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`times`|`getTimes`|`setTimes`|the repeating times of the template
-----

# `RepeatRange`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`max`|`getMax`|`setMax`|the max repeating times of `templated`
|`min`|`getMin`|`setMin`|the min repeating times of `templated`
-----

# `StringOrRepeat`, `StringExceptRepeat`
|variable that can be visited|get|set|meaning
|---|---|---|---
|`wstr`|`get`|`set`|the string
|`max`|`getMax`|`setMax`|the max size
|`min`|`getMin`|`setMin`|the min size