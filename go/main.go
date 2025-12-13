package main

import (
	"factorialserver/server"
	"log"
)

func main() {

	server := server.New(":8080")

	if err := server.Start(); err != nil {
		log.Fatal(err)
	}

}
