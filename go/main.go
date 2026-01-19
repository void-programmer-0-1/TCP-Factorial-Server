package main

import (
	"factorialserver/server"
	"log"
)

func main() {

	server := server.New(":9000")

	if err := server.Start(); err != nil {
		log.Fatal(err)
	}

}
