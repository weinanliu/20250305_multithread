go:
	make a.out
	./a.out

a.out: halo.cpp
	g++ -Ofast  halo.cpp  -I../thread-pool/include -I../thread-pool-cpp/include

clean:
	rm a.out
