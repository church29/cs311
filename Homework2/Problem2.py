import os
import sys
import getopt


def create_class_dirs(term, klass):
    working_dir = os.environ['HOME'] + '/{0}/{1}'.format(term, klass)
    symlink_dir = '/usr/local/classes/eecs/{0}/{1}/src/README'.format(
        term, klass)
    try:
        if not os.path.isdir(symlink_dir):
            os.makedirs(symlink_dir)

        if not os.path.isdir(working_dir):
            os.makedirs(working_dir)

        if not os.path.islink(working_dir + '/README'):
            os.symlink(symlink_dir, working_dir + '/README')

        if not os.path.islink(working_dir + '/src_class'):
            os.symlink(('/').join(symlink_dir.split('/')[:-1]),
                       working_dir + '/src_class')
    except OSError, e:
        sys.exit(str(e))

    for d in 'assignments examples exams lecture_notes submissions'.split():
        if not os.path.isdir(working_dir + '/' + d):
            try:
                os.mkdir(working_dir + '/' + d)
            except OSError, e:
                sys.exit(str(e))


if __name__ == '__main__':
    options, remainder = getopt.getopt(
        sys.argv[1:], 't:c:h', ['term=', 'class=', 'help'])
    term, klass = '', ''
    for opt, arg in options:
        if opt in ('-h', '--help'):
            sys.exit('This function takes two arguments: \n'
                     + '-t or --term followed by the term name \n'
                     + '-c or --class followed by the class name')
        if opt in ('-t', '--term'):
            term = arg
        elif opt in ('-c', '--class'):
            klass = arg
    if term and klass:
        create_class_dirs(term, klass)
    else:
        sys.exit('This function requires two arguments. For help, call this'
                 + ' function with -h as an argument.')
