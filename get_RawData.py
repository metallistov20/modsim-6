#!/usr/bin/python
#
# (C) Copyright 2016, TP-Link Inc, konstantin.mauch@tp-link.com
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

import usbtmc
import numpy
import matplotlib.pyplot as plot
import sys
 
scope =  usbtmc.Instrument(0x1ab1, 0x0588) # Rigol DS1102E

scope.write(":STOP")
 
timescale = float(scope.ask_raw(":TIM:SCAL?"))

timeoffset = float(scope.ask_raw(":TIM:OFFS?"))

voltscale1 = float(scope.ask_raw(':CHAN1:SCAL?'))
voltscale2 = float(scope.ask_raw(':CHAN2:SCAL?'))

voltoffset1 = float(scope.ask_raw(":CHAN1:OFFS?"))
voltoffset2 = float(scope.ask_raw(":CHAN2:OFFS?"))

scope.write(":WAV:POIN:MODE RAW")
rawdata1 = scope.ask_raw(":WAV:DATA? CHAN1")[10:]
rawdata2 = scope.ask_raw(":WAV:DATA? CHAN2")[10:]

data_size = len(rawdata1)
sample_rate = float(scope.ask_raw(':ACQ:SAMP?'))

print 'Data size:', data_size, "Sample rate:", sample_rate

scope.write(":KEY:FORCE")
scope.close()

data1 = numpy.frombuffer(rawdata1, 'B')
data2 = numpy.frombuffer(rawdata2, 'B')

data1 = data1 * -1 + 255
data2 = data2 * -1 + 255
 
data1 = (data1 - 130.0 - voltoffset1/voltscale1*25) / 25 * voltscale1
data2 = (data2 - 130.0 - voltoffset2/voltscale2*25) / 25 * voltscale2

time = numpy.linspace(timeoffset - 6 * timescale, timeoffset + 6 * timescale, num=len(data1))
 
if (time[-1] < 1e-3):
    time = time * 1e6
    tUnit = "uS"
elif (time[-1] < 1):
    time = time * 1e3
    tUnit = "mS"
else:
    tUnit = "S"
 
plot.plot(time, data1)
plot.plot(time, data2)
plot.title("Oscilloscope Channel 1")
plot.ylabel("Voltage (V)")
plot.xlabel("Time (" + tUnit + ")")
plot.xlim(time[0], time[-1])
##plot.show()

#for x in range(1,data_size):
#	print "(", x, " of ", data_size-1, " )  Probe1: (", data1[x], ")  Probe2 : (", data2[x], ")"

print "<num elements: ", data_size-1, ", time in ", tUnit, ">"
print "Time X(CH1) X(CH2)"
print "Second Volt Volt"

for x in range(1,data_size):
	print ("%2.6f,%2.5f,%2.5f" % (  time[x],data1[x],data2[x] ) ) ;

print "<end>"

