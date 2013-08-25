#!/usr/bin/env python
import minimalmodbus

x = minimalmodbus.Instrument('/dev/ttyUSB0', 2) 
x.serial.parity = 'E'

x.debug = True

print x.read_bit(0, 2)
print x.read_bit(1, 2)

x.write_bit(0, 1, 5)
print x.read_bit(0, 1)

x.write_bit(0, 0, 15)
print x.read_bit(0, 1)

adc = x.read_register(0, 0, 4, False)
print "%4d\t%.2f%%" % (adc, adc / 40.95)
adc = x.read_register(1, 0, 4, False)
print "%4d\t%.2f%%" % (adc, adc / 40.95)

x.write_string(10, "hello", 3)
print x.read_string(10, 3, 3)

x.write_string(10, "world", 3)
print x.read_string(10, 3, 3)

'''
02050000ff008C09
020500000000CDf9

00050000ff008DEB
000500000000CC1B

'''
