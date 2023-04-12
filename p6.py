def sum_sq_add(x):
	returnee = 0
	for i in range(0, x + 1):
		returnee += i*i
	return returnee

def sum_add_sq(x):
	return ((x + 1)*x/2)**2

def main():
	x = int(input("x = "))
	add_sq = sum_add_sq(x)
	sq_add = sum_sq_add(x)
	print(add_sq, '-', sq_add, '=', add_sq - sq_add)

if __name__ == "__main__":
	main()
