Left:
dual stack still needs overlap check
more reset tests
more tests in general
add try catch for testing when we know it should fail


PR CHECKING
- check all the parameters have const if they need them, and check it lines up in cpp
- add proper bounds check sin dual stack allocator
- add more testing for linked stack,
- add error throwing when in bad situations
- - thread safety