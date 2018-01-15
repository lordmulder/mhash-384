import sys

if sys.version_info[0] < 3:
   raise SystemError("Python 3.x or newer is required to run this application !!!")

import timeit
import MHashPy384 as mhash384

print('MHashPy384 v{}.{}-{}'.format(*mhash384.MHash384.version()))
print('Running on {} v{}.{}.{}-{}\n'.format(sys.implementation.name, *sys.implementation.version))

if sys.implementation.name.lower() == "cpython":
    print("WARNING: You are running on CPython, which is VERY SLOW for actual computations!")
    print("You can use, for example, PyPy (https://pypy.org/) for *much* improved speed ;-)\n")

print('\nTook %.2f seconds.' % timeit.timeit(mhash384.MHash384.self_test, number=1))
