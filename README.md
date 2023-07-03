# About
Simple implementation of Observer pattern in C++

Done for self study.

# Design Goal
In basic implementation of the observer pattern (eg. https://refactoring.guru/design-patterns/observer/cpp/example), if a subscriber goes out of scope, the publisher would have a dangling pointer.

In this repo's implementation, we try to do better:
1. Use RAII to achieve auto unsubscribe if subscriber goes out of scope
2. Use smart pointers
3. Use template to make a general publisher class that could publish different types
4. Allow observer to be a std::function callback instead of forced to inherit an interface class

# Build

install meson by:
https://mesonbuild.com/Quick-guide.html

then
```
cd /path/to/ObserverPatternCpp
meson setup builddir
cd builddir
meson compile
```
# Run
```
cd builddir
./main
```
Output:
```
==== Case0: Normal pub sub
personA received news News1
personB received news News1
personA received salary 10
personB received salary 10

==== Case1: Subscriber dead before Publisher, subscription are canceled automatically
Unsubscribed 
Unsubscribed 
personB received news News2

==== Case2: Publisher closed before Subscriber, no segfault
personB received salary 9
Try unsubscribe but publisher already gone. No operation needed
Unsubscribed
```

# How project was set up
```
meson init --name ObserverPatternCpp --build
```

# Ref
Basic implementation:  
https://refactoring.guru/design-patterns/observer/cpp/example

Template implementation:  
https://gist.github.com/sacopon/f34d2c992dd683c9367d97786e05185d

Using RAII to achieve auto unsubscribe, use std::function callback:  
https://stackoverflow.com/questions/39516416/using-weak-ptr-to-implement-the-observer-pattern

