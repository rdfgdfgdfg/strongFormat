# theory
The biggest difference between strongRegularExpression and normal regular expression library is recursiveness. 
A strongRegularExpression-box can contain other boxes, it's useful for complex problems. 
In other ways, strongRegularExpression and normal regular expression library are similar  
### **the key to recursiveness is in `Queue`**
#### match
`Queue` has `result`. it's used to store boxes that match successfully. 
When I want to push back a box and match it, if it matches successfully, that's all right.
But if it fails, I should go back and rematch.  
<>	<>	<>	<>	<&>
(push back a box)  
<>	<>	<>	<>	<!>
(match fails)   
<>	<>	<>	<\*> <!>
(rematching)  
If rematch fails, I should go back again and rematch.  
<>	<>	<\*>	<!>	<!>  
<>	<\*>	<!>	<!>	<!>  
If any of the `result` can't rematch successfully, it'll throw a error. 
If one of them matches successfully, it'll go on matching  
If it fails again, then rematch again.   

#### rematch
When `Queue` tries to rematch, it will try to rematch `result`.
If this fails, it will try to push or pop a box.
When it tries to push a box for rematching, 
the result can't be rematch again, 
so it should reset the result first.
Otherwise, if it can't push a box successfully at once, 
it can't rematch the result and try more possibilities.
So, this may cause pushing fails and lose a possibility.