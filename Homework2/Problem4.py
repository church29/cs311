import sys


def Eratosthenes(n):
    if not isinstance(n, int) or n < 0:
        sys.exit("Please enter a positive integer.")
    current_range = 1000
    nums = list(xrange(2, current_range))
    primes = []
    k = 1
    while len(primes) < n:
        if nums and nums[0] > k:
            m = nums[0]
            for i in range(2, (current_range/m)+1):
                if i*m in nums:
                    nums.remove(i*m)
            nums.remove(m)
            primes.append(m)
            k = m
        else:
            nums = list(xrange(current_range, current_range * 2))
            current_range = current_range * 2
            for prime in primes:
                for i in range(2, (current_range/prime)+1):
                    if i * prime in nums:
                        nums.remove(i*prime)

    return primes.pop()

if __name__ == '__main__':
    n = 0
    if len(sys.argv) == 2:
        n = sys.argv[1]

    if n:
        try:
            n = int(n)
            print Eratosthenes(int(n))
        except ValueError:
            sys.exit('Please enter a positive integer.')
    else:
        sys.exit('This function requires one argument: \n'
                 + 'A positive integer.')
