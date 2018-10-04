# JSON
A SAX like JSON on-the-fly parser with no memory allocation while parsing.
It's inspired by JSMM for working principle but is completely rewritten for C++ and support partial parsing.
 
It was developped to fit a memory & flash constrained microcontroller, so many design choice are oriented toward saving binary size and memory.
 
It does not allocate any memory. It's deterministic.
It's designed for stream parsing (that is, there is no limit to parse end of "logical end")
It checks the JSON syntax, but accept few invalid constructions:
    - `{}{}` : it'll stop on the first object declaration but not return an error since it hasn't seen the other object.
    - `[123.E232-23++34.24...2424]`: Since number are not converted, this will be accepted as an array containing a number.
 
By default, it does not modify its input and work-in-place.
It does not convert/unescape strings unless you define UnescapeJSON (in that case, the input text is modified to store 0 at strings' ends)
Even in that case, it does not convert unicode escaped chars (\uABCD) to UTF-8 (usually, this is not required since such strings can be returned untouched).
It does not convert textual number to native number (you can use atoi/atof like functions yourself).
It tags all objects and arrays with a unique identifier (useful for partial parsing, see below).
Such identifier is limited to 4095 possible values (before wrap around).
 
Except for the limitations above, it should be very fast to parse and with minimal code size.
    (On my machine complete code compiles to less than 2.5kB)

It also supports (optional) partial parsing.
This allows calling the parse method with an incomplete buffer, start working with the partial JSON object, then, when more data is ready, continue parsing from where it lefts off.
This feature works mostly in zero-copy mode (it works on the input buffer, with no allocation).
Typically, that's good if you're receiving data from a socket and don't have enough buffers/memory to accumulate the complete JSON text before parsing.
 
Partial parsing is a feature that's almost as big as the parse method in binary so if you don't need it you either define SkipJSONPartialParsing, or build with `-ffunction-sections -fdata-sections -Wl,-gc-sections` flags so the linker will garbage collect the function.
This removes 800 bytes from the compiled binary on my machine.
The function is logically written so it resumes after the initial parse function returned Starving error.
 
A partial parsing rewrites the token stream so the object hierarchy to the last values (with key if available) found is kept valid but all previous values (opt. keys) are removed. For ease of use reasons, if the parsing stopped in the middle of a a key/value pair,
the token stream will contains the last key the (interrupted and resumed) value refers to.
When processing the token stream, you are ensured to always have a key before a value in an object.
Since object and arrays are identified, you can also figure out in which object the key/value refers to by remembering the container's ID. Try to accumulate as much data as possible before calling partialParsing, since this rewriting is computation expensive, so avoid to do that for each byte received.

The default implementation limits input JSON size to signed 16 bits (32768 bytes), and less than 4095 embedded objects/arrays.
If you intend to parse more than that, you'll need to define IndexType to a larger **signed** type.
The Token's size is, by default, 8 bytes long. Using signed 32 bits int for the IndexType will double it's size.
The parser memory size is 10 bytes by default. It requires less than a hundred bytes of stack space.
The code is very clean and small (less than 300 lines of code for the definition)

Because I got the question, you can not write a JSON stream with any parser and even more with this one.
Writing small JSON is trivial, but it's not the subject of this class.

There is no dependency on STL, and no exceptions either.
Only memmove is used in partialParsing for ensuring the previous key is present in the new stream to parse.
