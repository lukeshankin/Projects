import time

def prime(x):
	for i in range (2, x):
		if x % i == 0:
			return False
	return True

def prime_list(top):
	total = 0
	returnee = []
	next_prime = 2
	while total < top:
		if prime(next_prime):
			total += next_prime
			returnee.append(next_prime)
		next_prime += 1
	return returnee


def sumlen(arr, start, end):
	returnee = 0
	for i in range(start, end):
		returnee += arr[i]
	return returnee


def main():
	t0 = time.clock()
	top = 1000000 #million
	#top = 100000
	primes = prime_list(top)
	#print(primes)
	#print(len(primes))
	greatest = 0
	total = 0
	for j in range(len(primes)):
		for i in range(0, j):
			#print("trying ", i, "through ", j)
			total = sumlen(primes, i, j)
			if prime(total) and total > greatest:
				greatest = total
	print(greatest)
	print("Calculated in ", time.clock() - t0, " seconds")


if __name__ == "__main__":
	main()
