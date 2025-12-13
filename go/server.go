package main

import (
	"bufio"
	"log"
	"math/big"
	"net"
	"strconv"
	"strings"
)

func factorial(n int, resultCh chan *big.Int) {
	result := big.NewInt(1)
	for i := 2; i <= n; i++ {
		result.Mul(result, big.NewInt(int64(i)))
	}
	resultCh <- result
}

func handlerConnection(conn net.Conn) {
	defer conn.Close()

	log.Println("Client Connection: ", conn)

	reader := bufio.NewReader(conn)

	for {

		line, err := reader.ReadString('\n')
		if err != nil {
			log.Println("Client disconnected:", conn.RemoteAddr())
			return
		}

		line = strings.TrimSpace(line)
		n, err := strconv.Atoi(line)
		if err != nil || n < 0 {
			conn.Write([]byte("Invalid number\n"))
			continue
		}

		resultCh := make(chan *big.Int)
		go factorial(n, resultCh)

		result := <-resultCh
		conn.Write([]byte(result.String() + "\n"))
	}

}

func main() {

	listener, err := net.Listen("tcp", ":8080")
	if err != nil {
		log.Fatal("Error Occured while getting TCP connection on port 8080 ", err)
	}

	defer listener.Close()

	log.Println("Created TCP connection and connection is listening on port 8080")

	for {

		conn, err := listener.Accept()
		if err != nil {
			log.Println("Error occured while accepting client connection ", err)
			continue
		}

		go handlerConnection(conn)
	}

}
