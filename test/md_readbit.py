#!/usr/bin/env python
import sys
import minimalmodbus

if len(sys.argv) == 2:
	slave = int(sys.argv[1], 0)
else:
	slave = 0x02

x = minimalmodbus.Instrument('/dev/ttyUSB0', slave) 
x.serial.parity = 'E'

#x.debug = True

print "bit_input [0]: %d" % x.read_bit(0, 2)
print "bit_input [1]: %d" % x.read_bit(1, 2)

x.write_bit(0, 1, 5)
print "coil [0]: " + str(x.read_bit(0, 1))

x.write_bit(0, 0, 15)
print "coil [0]: " + str(x.read_bit(0, 1))

adc = x.read_register(20 , 0, 4, False)
print "short_input[0]: %4d\t%.2f%%" % (adc, adc / 40.95)
adc = x.read_register(21, 0, 4, False)
print "short_input[1]: %4d\t%.2f%%" % (adc, adc / 40.95)

print "uuid: %s" % x.read_string(0, 16, 4)


'''
x.write_string(10, "hello", 3)
print x.read_string(10, 3, 3)

x.write_string(10, "world", 3)
print x.read_string(10, 3, 3)
'''

'''
02050000ff008C09
020500000000CDf9

00050000ff008DEB
000500000000CC1B

'''
