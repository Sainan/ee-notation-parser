#include <iostream>

#include "../EeNotationParser.hpp"

int main(int argc, const char* argv[])
{
	if (argc < 3)
	{
		std::cout << "syntax: json2ee <in-json-file> <out-ee-file>";
		return 1;
	}
	auto tree = soup::json::decode(soup::string::fromFile(argv[1]));
	if (!tree || !tree->isObj())
	{
		tree = soup::make_unique<soup::JsonObject>();
	}
	soup::string::toFile(argv[2], EeNotationParser::unparse(tree->asObj()));
	return 0;
}
