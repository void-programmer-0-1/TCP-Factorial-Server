package server

import (
	"log"
	"net"
)

type Server struct {
	address string
	listner net.Listener
}

func New(address string) *Server {
	return &Server{
		address: address,
	}
}

func (s *Server) Start() error {
	var err error
	s.listner, err = net.Listen("tcp", s.address)
	if err != nil {
		log.Fatal("Error Occured while getting TCP connection on port 8080 ", err)
	}

	log.Println("Created TCP connection and connection is listening on port 8080")

	for {

		conn, err := s.listner.Accept()
		if err != nil {
			log.Println("Error occured while accepting client connection ", err)
			continue
		}

		go handleClient(conn)

	}

}
