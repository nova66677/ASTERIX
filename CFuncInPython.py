from ctypes import *
so_file = "/home/n0va/Documents/Thales/ASTERIX/testCPython.so"
func = CDLL(so_file)

print(type(func))
print(func.square(100))

func.test()