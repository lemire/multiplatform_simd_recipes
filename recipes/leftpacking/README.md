
"left packing". E.g., I give you a vector X, a vector Y.
You want ...

1. How many elements K are equal?
2. A new vector having the K elements in sequence... we don't care about leftover elements.


This is generally useful... It is in SVE's documentation. AVX-512 introduces instructions to do this
(and so does SVE).
