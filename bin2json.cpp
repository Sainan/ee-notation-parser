#include <iostream>

#include <FileReader.hpp>

#include "EeNotationParser.hpp"

int main(int argc, const char* argv[])
{
	if (argc < 3)
	{
		std::cout << "syntax: bin2json <in-bin-file> <out-json-file>";
		return 1;
	}
	soup::FileReader fr(argv[1]);
	fr.skip(20); // skip hash?
	std::string str;
	fr.str_lp<soup::u32le_t>(str);
	if (str.c_str()[0] == '/') // first string might be filename, assuming then that the metadata is right after it
	{
		fr.str_lp<soup::u32le_t>(str);
	}
	EeNotationParser par;
	auto out = par.parse(str);
	soup::string::toFile(argv[2], out->encodePretty());
	return 0;
}
