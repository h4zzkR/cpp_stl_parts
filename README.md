## Description

Template hash table with ability to set custom hasher and allocator, based on idea of open hashing, created with my implementation of std::list.

Full support of move-semantics is built-in. For creation, the Key and Value types are not required to have default constructors, a copying constructor, and a copying assignment operator, minimum requirements are move constructor and move-assignment operator.

There is support of all base methods of original std::unordered_map.

For detailed description (in Russian), please, follow [this](TS.md).
