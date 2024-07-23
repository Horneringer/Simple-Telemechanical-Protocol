all: build_client

build_client:
	g++ -ggdb -O0  -o teleprot teleprot.cpp main.cpp
