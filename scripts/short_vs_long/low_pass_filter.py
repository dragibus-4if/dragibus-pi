#!/usr/bin/env python2.7

import sys

if len(sys.argv) < 2:
    sys.stderr.write('len(sys.argv) < 2\n')
    sys.exit(1)
size_filter = int(sys.argv[1])
ls = [0.0] * size_filter
for t, line in enumerate(sys.stdin.readline()):
    val = float(line.strip('\n'))
    ls = ls[1:] + [val]
    if t >= size_filter:
        print t, sum(ls) / size_filter
