#!/usr/bin/env python
import minimalmodbus

x = minimalmodbus.Instrument('/dev/ttyUSB0', 1) 
x.serial.parity = 'E'

print x.read_bit(1, 1)


print x.read_bit(31, 1)
