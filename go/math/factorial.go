package math

import "math/big"

func Factorial(n int, resultCh chan<- string) {
	/*
		Calculates the factorial for the given n
		and shares the result through a string channel
	*/
	result := big.NewInt(1)

	for i := 2; i <= n; i++ {
		result.Mul(result, big.NewInt(int64(i)))
	}

	resultCh <- result.String()
}
