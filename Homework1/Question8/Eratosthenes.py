import math


def Eratosthenes(n):
    if not isinstance(n, int):
        return "Please enter an integer."

    nums = list(xrange(n))
    primes, special, composites = [], [], []
    special.append(0)
    special.append(1)
    nums.remove(0)
    nums.remove(1)
    k = 1
    while k < math.sqrt(n):
        if nums and nums[0] > k:
            m = nums[0]
            for i in range(2, (n/m)+1):
                if i*m in nums:
                    composites.append(i*m)
                    nums.remove(i*m)
            primes.append(m)
            nums.remove(m)
            k = m

    primes += nums

    return len(primes), primes
