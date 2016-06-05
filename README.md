### Realtime C++

I began this project to experiment on realtime data structures and
algorithms in C++. Many interesting things arose from this research.
My intentions is to end-up with a realtime version of the Standard
Template Library (STL), suitable for hard-realtime, low-latency and
high-availability.

#### Splitting node and array allocation in allocators

The first important conclusion of this project is that there should
be a split between node and array allocations in allocators. As a
result, I have written a proposal to the C++ standards committee that
can be found here

http://open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0310r0.pdf

or more easily here doc/proposal_allocator.pdf.

#### Alternative to traverse linked data structures.

A big concern today in the use of linked data structures is related
to their cache-unfriendliness. The proposal mentioned above plus some small
additions offer a strong alternative to traversal where we instead of
chasing pointers, we can traverse the buffers where the nodes have been
allocated. Benchmarks a quite impressive.

#### Matrix and vector operation suitable for 3D math/physics.

This is a very simple matrix class that offers most usable operators
with lazy evaluation and compile time check of compatible size for
matrix/vector operations. It makes code clean, intuitive and efficient,
avoiding temporaries.

#### Miscellaneous algorithms

Threaded bynary search trees, sorting algorithms etc.


