package server

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"strconv"
	"strings"
	"time"

	"factorialserver/math"
)

func handleClient(conn net.Conn) {
	defer cleanup(conn)

	log.Println("Client connected:", conn.RemoteAddr())

	reader := bufio.NewReader(conn)
	writer := bufio.NewWriter(conn)

	writeLine(writer, "Welcome! Enter a number, or type 'quit'")

	for {

		// Prevent idle connections
		conn.SetReadDeadline(time.Now().Add(5 * time.Minute))

		line, err := reader.ReadString('\n')
		if err != nil {
			log.Println("Read e.rror:", err)
			return
		}

		cmd := strings.TrimSpace(line)

		// Quit command
		if cmd == "quit" {
			writeLine(writer, "Bye")
			return
		}

		n, err := strconv.Atoi(cmd)
		if err != nil || n < 0 {
			writeLine(writer, "Invalid number")
			continue
		}

		// Compute factorial asynchronously
		resultCh := make(chan string, 1)
		go math.Factorial(n, resultCh)

		result := <-resultCh
		writeLine(writer, result)
	}
}

func cleanup(conn net.Conn) {
	log.Println("Cleaning up connection:", conn.RemoteAddr())
	conn.Close()
}

func writeLine(w *bufio.Writer, msg string) {
	fmt.Fprintln(w, msg)
	w.Flush()
}
