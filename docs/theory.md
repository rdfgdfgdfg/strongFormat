# theory
The biggest difference between strongFormat and normal format library is recursiveness. 
A strongFormat-box can contain other boxes, it's useful for complex problems. 
In other ways, strongFormat and normal format library are similar  
### **the key to recursiveness is in `Queue`**
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