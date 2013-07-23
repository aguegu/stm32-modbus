#!/usr/bin/env python
import minimalmodbus

x = minimalmodbus.Instrument('/dev/ttyUSB0', 1) 
x.serial.parity = 'E'

print x.read_bit(0, 2)

'''
print x.read_bit(0, 1)
print x.read_bit(1, 1)
print x.read_bit(31, 1)

print 

print x.read_bit(0, 2)
print x.read_bit(2, 2)
print x.read_bit(15, 2)

print

print x.read_bit(0, 1)
print x.write_bit(0, 1, 5)
print x.read_bit(0, 1)
print x.write_bit(0, 0, 5)

print

print x.read_bit(0, 1)
print x.write_bit(0, 1, 15)
print x.read_bit(0, 1)
print x.write_bit(0, 0, 15)

print 
print x.read_register(0, 0, 4, False)
print x.read_register(1, 0, 4, False)
print x.read_register(3, 0, 4, False)
print x.read_register(7, 0, 4, False)

print

print x.write_register(15, 123, 1, 6, False)
print x.read_register(15, 1, 3, False)
print x.write_register(0, 0, 0, 6, False)
print x.read_register(0, 0, 3, False)

print 

print x.write_register(1, 345, 0, 16, False)
print x.read_register(1, 0, 3, False)

x.write_long(2, 12312)
print x.read_long(2)

x.write_float(4, 3.14159, 2)
print x.read_float(4, 3, 2)

x.write_float(6, 3.1415926, 4)
print x.read_float(6, 3, 4)

x.write_string(10, "hello", 3)
print x.read_string(10, 3, 3)

'''
