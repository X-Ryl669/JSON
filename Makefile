CXXFLAGS:=-O0 -g -DIndexType="signed short" -DJSONPartialParsing

JSONDump: JSONDump.o JSON.o
	c++ -O0 -g -o $@ $^
JSONPartial: JSONPartialTest.o JSON.o
	c++ -O0 -g -o $@ $^

clean:
	-rm JSONDump
	-rm *.o
