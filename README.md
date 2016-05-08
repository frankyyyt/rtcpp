### Realtime C++

I began this project to experiment on realtime data structures and
algorithms in C++. Many interesting things came up of this research,
below I will list the most important achievements.

## Splitting node and array allocation in allocators

The first important conclusion of this project is that there should
be a split between node and array allocations in allocators. As a
result, I have written a proposal to the C++ standards committee that
can be found here

http://open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0310r0.pdf

or more easily here doc/proposal_allocator.pdf.

I am implementing this proposal on libstdc++, for that I have made a
branch of the gcc compiler that you can find on my github page.

## Matrix and vector operation suitable for 3D math/physics.

This is a very simple matrix class that offers most usable operators
with lazy evaluation and compile time check of compatible size for
matrix/vector operations. It makes code clean, intuitive and efficient,
avoiding temporaries.

# Miscellaneous algorithms

Threaded bynary search trees, sorting algorithms etc.


