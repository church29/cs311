import getopt
import subprocess
import sys


def who(options):
    cmd = 'who '
    if options:
        cmd += options
    proc = subprocess.Popen(
        cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout_value = proc.communicate('through stdin to stdout')[0]
    sys.exit(stdout_value[:-1])


if __name__ == '__main__':
    options, remainder = getopt.getopt(
        sys.argv[1:], 'bdlprTtuw',
        ['boot', 'dead', 'login', 'process',
         'runlevel', 'mesg', 'time', 'users'])
    who_options = ''
    for opt, arg in options:
        who_options += '{0} '.format(opt)

    who(who_options)
