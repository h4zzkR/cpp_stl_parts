## Description

Simplified, but yet powerful std::function implementation. The implementation is close to the origin in STL to achieve acceptable performance, namely:

- NO virtual functions
- Memory friendly - size of wrapper is <= 32 bytes
- Heap allocation friendly - if callable-object size is <= 16 bytes, then I allocate it on stack
- Heap allocation friendly - there is no reallocation if new object can be stored in old one's place on assignments.

For detailed description (on Russian), please, follow [this](TS.md).

