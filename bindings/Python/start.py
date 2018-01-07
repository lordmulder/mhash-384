import timeit
import MHashPy384

def self_test():
    MHashPy384.MHash384.self_test()

print("MHashPy384 v%d.%d-%d" % MHashPy384.MHash384.version())
print('Took %.2f seconds.' % timeit.timeit(self_test, number=1))
