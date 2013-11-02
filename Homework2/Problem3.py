import os
import sys
import urllib


def wget(url, destination):
    if '/' in destination and not os.path.isdir(
            ('/').join(destination.split('/')[:-1])):
        try:
            os.makedirs(('/').join(destination.split('/')[:-1]))
        except OSError, e:
            sys.exit(str(e))
    if not urllib.urlopen(url).code/100 >= 4:
        try:
            urllib.urlretrieve(url, destination)
        except IOError, e:
            sys.exit(str(e))
    else:
        sys.exit("The given url is not valid.")


if __name__ == '__main__':
    url, dest = '', ''
    if len(sys.argv) == 3:
        url = sys.argv[1]
        destination = sys.argv[2]

    if url and destination:
        wget(url, destination)
    else:
        sys.exit('This function requires two arguments: \n'
                 + 'A valid url followed by a destination.')
