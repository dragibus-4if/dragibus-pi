#!/usr/bin/env python
# encoding: utf-8

import os
import sys
import subprocess
import multiprocessing

this_dir = os.path.dirname(os.path.realpath(__file__))

# configuration
min_short_priority = 1
max_short_priority = 99
min_long_priority = -20
max_long_priority = 19
valid_short_policies = ('SCHED_RR', 'SCHED_FIFO',)

def make(exe_name, debug=False, short_priority=None, long_priority=None,
        short_policy=None, src=os.path.join(this_dir, 'main.c')):
    """
    Compiler l'exemple avec les options passées.
    """
    # options de compilation
    cc = ['gcc', src]
    cflags = ['std=c99', 'pthread', 'W', 'Wall']

    # nom de l'exécutable
    cc += ['-o', exe_name]

    # mode débug
    if debug:
        cflags.append('g')

    # SHORT_PRIORITY
    if short_priority is not None:
        try:
            short_priority = int(short_priority)
            if max_short_priority < short_priority < min_short_priority:
                raise RuntimeError('SHORT_PRIORITY devrait être entre %s et %s' \
                        % (min_short_priority, max_short_priority))
        except ValueError:
            raise RuntimeError('SHORT_PRIORITY devrait être un entier')
        else:
            cflags.append('DSHORT_PRIORITY=%s' % short_priority)

    # LONG_PRIORITY
    if long_priority is not None:
        try:
            long_priority = int(long_priority)
            if max_long_priority < long_priority < min_long_priority:
                raise RuntimeError('LONG_PRIORITY devrait être entre %s et %s' \
                        % (min_long_priority, max_long_priority))
        except ValueError:
            raise RuntimeError('LONG_PRIORITY devrait être un entier')
        else:
            cflags.append('DLONG_PRIORITY=%s' % long_priority)

    # SHORT_POLICY
    if short_policy is not None:
        if short_policy not in valid_short_policies:
            raise RuntimeError('SHORT_POLICY should be in %s' \
                    % valid_short_policies)
        else:
            cflags.append('DSHORT_POLICY=%s' % short_policy)
    code = subprocess.call(cc + map(lambda s: '-' + s, cflags),
            stderr=subprocess.PIPE)
    if code != 0:
        raise RuntimeError('Erreur de compilation')

def main():
    """
    Programme principal, doit être lancé en tant que sudo. Lance une
    RuntimeError s'il y a une erreur.
    """
    if os.getuid() != 0:
        raise RuntimeError('Doit être lancé en tant que sudo')
    def sample_args():
        a, b = min_short_priority, max_short_priority
        for policy in valid_short_policies:
            for short_priority in xrange(a, b+1):
                c, d = min_long_priority, max_long_priority
                for long_priority in xrange(c, d+1):
                    yield (os.tmpnam(), short_priority, long_priority, policy)

    def generate(enum_pair):
        i, values = enum_pair
        exe_name, short_priority, long_priority, policy = values
        keys = ('exe_name', 'short_priority', 'long_priority', 'short_policy')
        kwargs = dict(zip(keys, values))
        make(**kwargs)
        output = subprocess.check_output(exe_name)
        fname = os.path.join(this_dir, 'datasets', '%s-%s-%s.csv' % \
                (short_priority, long_priority, short_policy))
        with open(fname, 'w') as fp:
            fp.write(output)

    pool = multiprocessing.Pool()
    pool.map(generate, list(enumerate(sample_args()))[:4])

if __name__ == '__main__':
    try:
        main()
    except RuntimeError as e:
        sys.stderr.write(str(e) + '\n')
        sys.exit(1)
