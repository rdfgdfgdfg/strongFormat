# introduction

strongFormat is a header-only library for c++. It's used for complex format matching.
It's easy to develop.

## concept
You can use **box** to match a string. **box** is a non-abstract subclass of TUT::format::Basic. 
In logic, it can store a portion of string. A box can contain other boxes.  
To control what box matches, user should set the template of the box.

## support class
string = std::wstring  
vector = std::vector  
Ptr ~~ std::shared_ptr<Basic>  
resVector = vector<Ptr>  