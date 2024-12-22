#include <iostream>

#include "EeNotationParser.hpp"

int main(int argc, const char* argv[])
{
	if (argc < 3)
	{
		std::cout << "syntax: ee2json <in-ee-file> <out-json-file>";
		return 1;
	}
	EeNotationParser par;
	auto out = par.parse(soup::string::fromFile(argv[1]));
	soup::string::toFile(argv[2], out->encodePretty());
	return 0;
}
