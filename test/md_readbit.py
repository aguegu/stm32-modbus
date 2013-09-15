#!/usr/bin/env python
import sys
import minimalmodbus
import time

if len(sys.argv) == 2:
	slave = int(sys.argv[1], 0)
else:
	slave = 0x02

x = minimalmodbus.Instrument('/dev/ttyUSB0', slave) 
x.serial.parity = 'E'

#x.debug = True

print "bit_input: %d" % x.read_register(16, 0, 4, False)
print "coil: %d" % x.read_register(17, 0, 4, False)
print "short_input: %d" % x.read_register(18, 0, 4, False)
print "holding: %d" % x.read_register(19, 0, 4, False)
print

print "short inputs:"
print "[0].type: %d" % x.read_register(20, 0, 4, False)
print "[0].value: %d" % x.read_register(21, 0, 4, False)
print

print "[1].type: %d" % x.read_register(22, 0, 4, False)
print "[1].value: %d" % x.read_register(23, 0, 4, False)
print

#x.write_register(0, 1, 6, False)
print "lamp.type: %d" % x.read_register(0, 0, 3, False)
x.write_register(1, 1, 0, 6, False)
print "lamp.value: %d" % x.read_register(1, 0, 3, False)

print "led[0].type: %d" % x.read_register(2, 0, 3, False)
x.write_register(3, 1, 0, 6, False)
print "led[0].value: %d" % x.read_register(3, 0, 3, False)

print "led[1].type: %d" % x.read_register(4, 0, 3, False)
x.write_register(5, 1, 0, 6, False)
print "led[1].value: %d" % x.read_register(5, 0, 3, False)

print "led[2].type: %d" % x.read_register(6, 0, 3, False)
x.write_register(7, 1, 0, 6, False)
print "led[2].value: %d" % x.read_register(7, 0, 3, False)

print "led[3].type: %d" % x.read_register(8, 0, 3, False)
x.write_register(9, 1, 0, 6, False)
print "led[3].value: %d" % x.read_register(9, 0, 3, False)


time.sleep(0.2)

print

print "pattern.type: %d" % x.read_register(10, 0, 3, False)
print "pattern.val: %d" % x.read_register(11, 0, 3, False)

x.write_register(11, 1, 0, 6, False)
print "pattern.val: %d" % x.read_register(11, 0, 3, False)
time.sleep(3)

x.write_register(11, 2, 0, 6, False)
print "pattern.val: %d" % x.read_register(11, 0, 3, False)
time.sleep(3)

x.write_register(11, 3, 0, 6, False)
print "pattern.val: %d" % x.read_register(11, 0, 3, False)





'''
x.write_bit(0, 0, 15)
print "coil [0]: " + str(x.read_bit(0, 1))

print

adc = x.read_register(20 , 0, 4, False)
print "short_input[0]: %4d\t%.2f%%" % (adc, adc / 40.95)
adc = x.read_register(21, 0, 4, False)
print "short_input[1]: %4d\t%.2f%%" % (adc, adc / 40.95)

print

print "uuid: %s" % x.read_string(0, 16, 4)


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
