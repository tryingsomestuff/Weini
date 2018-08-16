#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

def mysplit(s, delim=None):
    return [x for x in s.split(delim) if x]

print "Reading data"
with open("tmp.tuning") as f:
    data = f.read()

print "Spliting data"
#data = data.split('\n')

data = [line for line in data.split('\n') if line.strip() != '']


print "Getting axis"
x = [mysplit(row,' ')[0] for row in data]
y = [mysplit(row,' ')[6] for row in data]

fig = plt.figure()

ax1 = fig.add_subplot(111)

ax1.set_title("")
ax1.set_xlabel('x')
ax1.set_ylabel('y')

ax1.plot(x,y, c='r', label='data')

leg = ax1.legend()

plt.show()
