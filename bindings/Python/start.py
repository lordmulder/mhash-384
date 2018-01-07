import sys
import timeit
import MHashPy384

def self_test():
    MHashPy384.MHash384.self_test()

print('MHashPy384 v{}.{}-{}'.format(*MHashPy384.MHash384.version()))
print('Running on {}, v{}.{}.{}-{}\n'.format(sys.implementation.name, *sys.version_info))
print('\nTook %.2f seconds.' % timeit.timeit(self_test, number=1))
