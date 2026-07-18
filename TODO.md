- Allocator Utils needs a check bounds fucntion
- check all the parameters have const if they need them, and check it lines up in cpp
- add proper bounds check sin dual stack allocator
- add more testing for linked stack,
- start on pool allocator


pool logic:
create only memeory needed.
for each element just fill it with empty and the pointer to the next empty element.

when allocating take the first empty element, use it, tell the local empty marker  to look at the next empty 
