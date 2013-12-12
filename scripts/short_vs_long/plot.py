#!/usr/bin/python2.7

import sys
import pylab

# script args
if len(sys.argv) < 2:
    sys.stderr.write('len(sys.argv) < 2\n')
    sys.exit(1)
name = sys.argv[1]

# format data
data = map(lambda line: map(float, line.strip('\n').split(' ')), sys.stdin.readlines())
d = zip(*data)
if len(d) != 2:
    sys.stderr.write('%s: len(d) != 2 (d=%s)\n' % (name, d))
    sys.exit(1)

# plot data
pylab.plot(d[0], d[1], label=name)
pylab.legend()
pylab.title("Title of Plot")
pylab.xlabel("X Axis Label")
pylab.ylabel("Y Axis Label")
pylab.savefig('figures/%s' % name)
