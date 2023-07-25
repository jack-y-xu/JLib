# JLib
A collection of libraries and header files I wrote for additional functionality so I don't have to rewrite things over and over again.


Current index:

safer_random.h - a library for safe(r) & efficient parallel random generators. C++'s mt19937 is not thread safe, so we cannot use static w/out mutex. Initializing generator every time we want random isn't efficient, nor is using random_device every time. But initializing generator in each thread function feels like code bloat. The workaround here is to use static thread_local mt19937 that gets automatically and lazily initialized randomly by random_device upon thread creation. Also includes some quality of life functions for random.



TODOS:
* Add some benchmarks for safer_random (compare initializing generator each time vs using static thread_local)
* Add python-like print functions header




