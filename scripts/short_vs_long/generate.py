#!/usr/bin/env python
# encoding: utf-8

import os
import sys
import functools
import subprocess
import multiprocessing
import argparse

# configuration
os.chdir(os.path.dirname(os.path.realpath(__file__)))
src_file = 'main.c'
exe_dir = 'tmp'
min_short_priority = 1
max_short_priority = 99
min_long_priority = -20
max_long_priority = 19
valid_short_policies = ('SCHED_RR', 'SCHED_FIFO',)

def make(exe_name, debug=False, short_priority=None, long_priority=None,
        short_policy=None, src=src_file):
    """
    Compiler l'exemple avec les args passées.
    """
    # args de compilation
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
        raise RuntimeError('erreur de compilation')

def get_sample_args():
    for policy in valid_short_policies:
        for short_priority in xrange(min_short_priority, max_short_priority+1):
            for long_priority in xrange(min_long_priority, max_long_priority+1):
                yield (short_priority, long_priority, policy)

def run_generate(args, **extra_options):
    values = list(args)
    short_priority, long_priority, short_policy = values
    base_name = '%s-%s-%s' % (short_priority, long_priority, short_policy)
    exe_name = os.path.join(exe_dir, base_name)
    values.insert(0, exe_name)
    keys = ('exe_name', 'short_priority', 'long_priority', 'short_policy')
    kwargs = dict(zip(keys, values))
    kwargs.update(extra_options)
    print 'make %s' % kwargs
    make(**kwargs)
    print 'run %s' % exe_name
    try:
        output = subprocess.check_output(['sudo', exe_name])
    except subprocess.CalledProcessError as e:
        print 'run failed with exit code %s' % e.returncode
    else:
        fname = os.path.join('datasets', base_name + '.csv')
        print 'save to %s' % fname
        with open(fname, 'w') as fp:
            fp.write(output)

parser = argparse.ArgumentParser()
parser.add_argument('--debug', default=False, action='store_true')
parser.add_argument('--no-parallel', default=False, action='store_true')

if __name__ == '__main__':
    try:
        #if os.getuid() != 0:
            #raise RuntimeError('doit être lancé en tant que sudo')
        if not os.path.exists(exe_dir):
            os.mkdir(exe_dir)
        elif not os.path.isdir(exe_dir):
            raise RuntimeError('%s sera écrasé par le répertoire temporaire' \
                    % os.path.basename(exe_dir))
        parsed_args = parser.parse_args()
        generate = functools.partial(run_generate, debug=parsed_args.debug)
        sample_args = list(get_sample_args())
        if not parsed_args.no_parallel:
            pool = multiprocessing.Pool()
            pool.map(generate, sample_args)
        else:
            for args in sample_args:
                generate(args)
    except RuntimeError as e:
        sys.stderr.write("Erreur d'exécution: %s\n" % e)
        sys.exit(1)
