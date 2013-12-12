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

def make(argv=sys.argv, die=None, src=os.path.join(this_dir, 'main.c')):
    """
    Compiler l'example à partir du *argv* passé, et en gérant les erreurs par
    le *die* passé, un callable de la forme die(msg, code), où les 2
    arguments sont optionnels.
    Cette fonction est basée entièrement sur un programme dédié à la
    compilation, et par conséquent utilise sys.argv plutôt que des kwargs.
    """
    if die is None:
        def die(msg=None, code=1):
            if msg:
                sys.stderr.write(str(msg) + '\n')
            sys.exit(code)

    # options de compilation
    cc = ['gcc', src]
    cflags = ['std=c99', 'pthread', 'W', 'Wall']

    # mode débug
    debug = False
    try:
        argv.remove('-d')
    except ValueError:
        try:
            argv.remove('--debug')
        except ValueError:
            pass
        else:
            debug = True
    else:
        debug = True
    if debug:
        cflags.append('g')

    # nom de l'exécutable
    try:
        exe_name = argv[1]
    except IndexError:
        die("Nom de l'exécutable attendu !")
    else:
        cc += ['-o', exe_name]

    # SHORT_PRIORITY
    try:
        short_priority = int(argv[2])

        if max_short_priority < short_priority < min_short_priority:
            die('SHORT_PRIORITY devrait être entre %s et %s' \
                    % (min_short_priority, max_short_priority))
    except (IndexError, ValueError):
        pass
    else:
        cflags.append('DSHORT_PRIORITY=%s' % short_priority)

    # LONG_PRIORITY
    try:
        long_priority = int(argv[3])
        if max_long_priority < long_priority < min_long_priority:
            die('LONG_PRIORITY devrait être entre %s et %s' \
                    % (min_long_priority, max_long_priority))
    except (IndexError, ValueError):
        pass
    else:
        cflags.append('DLONG_PRIORITY=%s' % long_priority)

    # SHORT_POLICY
    try:
        short_policy = argv[4]
        if short_policy not in valid_short_policies:
            die('SHORT_POLICY should be in %s' % valid_short_policies)
    except IndexError:
        pass
    else:
        cflags.append('DSHORT_POLICY=%s' % short_policy)
    code = subprocess.call(cc + map(lambda s: '-' + s, cflags),
            stderr=subprocess.PIPE)
    if code != 0:
        die(code=code)

if __name__ == '__main__':
    def die(msg=None, code=1):
        raise RuntimeError(msg)

    def argv_samples():
        a, b = min_short_priority, max_short_priority
        for policy in valid_short_policies:
            for short_priority in xrange(a-1, b+1):
                c, d = min_long_priority, max_long_priority
                for long_priority in xrange(c-1, d+1):
                    yield [__name__, os.tmpnam(),
                            short_priority, long_priority, policy]

    def generate(enum_pair):
        i, argv = enum_pair
        exe_name = argv[1]
        short_priority, long_priority, policy = argv[2:]
        try:
            make(argv, die)
        except RuntimeError as e:
            print "Erreur d'exécution %s:" % i, str(e)
        else:
            output = subprocess.check_output(exe_name)
            fname = os.path.join(this_dir, 'datasets', '%s-%s-%s.csv' % \
                    (short_priority, long_priority, short_policy))
            with open(fname, 'w') as fp:
                fp.write(output)

    pool = multiprocessing.Pool()
    pool.map(generate, list(enumerate(argv_samples()))[:4])
