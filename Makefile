all: server client
client:
	g++ -g client.cpp server.cpp message.cpp cs_base.cpp service.cpp testClient.cpp -std=c++20 -o client -lSDL2 /usr/lib/libSDL2_net.so -lpthread
server:
	g++ -g server.cpp client.cpp message.cpp cs_base.cpp service.cpp testServer.cpp -std=c++20 -o server -lSDL2 /usr/lib/libSDL2_net.so -lpthread

clean:
	rm -f client server
