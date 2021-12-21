CXXFLAGS:=-O0 -g -DIndexType="signed short" -DJSONPartialParsing

JSONDump: JSONDump.o ../JSON.o
	c++ -O0 -g -o $@ $^
JSONPartial: JSONPartialTest.o ../JSON.o
	c++ -O0 -g -o $@ $^
JSONSaxTest: JSONSaxTest.o ../JSON.o
	c++ -O0 -g -o $@ $^

clean:
	-rm JSONDump
	-rm JSONPartial
	-rm JSONSaxTest
	-rm *.o
