import operator
import re
import sys


def prod(iterable):
    return reduce(operator.mul, iterable, 1)


def five_product(file_name):
    try:
        f = open(file_name)
        number = ''.join(f.read().split())
        f.close()
    except IOError, e:
        sys.exit(e)

    if not re.match('^-?[0-9]+$', number):
        sys.exit('The given file contains non integer characters. '
                 + 'Please give a file only including integers.')
    max_product = 0
    i = 0
    while i < len(number)+1:
        if '0' in number[i:i+5]:
            i += 1
        if i+6 < len(number) + 1 and number[i+6] > number[i]:
            i += 1
        else:
            product = prod([int(n) for n in number[i:i+5]])
            if product > max_product:
                max_product = product
            i += 1

    return max_product

if __name__ == '__main__':
    file_name = ''
    if len(sys.argv) == 2:
        file_name = sys.argv[1]

    if file_name:
        max_product = five_product(file_name)
        sys.exit(str(max_product))
    else:
        sys.exit('This function requires a list of digits in a file as '
                 + 'input.')
