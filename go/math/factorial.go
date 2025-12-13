package math

import "math/big"

func Factorial(n int, resultCh chan<- string) {
	result := big.NewInt(1)

	for i := 2; i <= n; i++ {
		result.Mul(result, big.NewInt(int64(i)))
	}

	resultCh <- result.String()
}
