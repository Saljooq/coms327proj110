Assignment110: Assignment110.cpp
	g++ Assignment110.cpp -lcurses -o Assignment110

debug: Assignment110.cpp
	g++ Assignment110.cpp -o Assignment110 -lcurses -g

clean:
	rm -f Assignment110
