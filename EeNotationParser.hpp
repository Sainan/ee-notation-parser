#pragma once

#include <stack>

#include <json.hpp>
#include <string.hpp>
#include <UniquePtr.hpp>

class EeNotationParser
{
protected:
	std::stack<soup::JsonNode*> stack{};
	std::string key;
	std::string buf;

public:
	[[nodiscard]] soup::UniquePtr<soup::JsonObject> parse(const std::string& data)
	{
		auto root = soup::make_unique<soup::JsonObject>();
		stack.push(root.get());
		bool pending_obj_or_arr = false;
		bool is_string = false;
		for (const auto& c : data)
		{
			if (is_string)
			{
				if (c == '"')
				{
					is_string = false;
					dischargeBuffer();
				}
				else if (c != '\t')
				{
					buf.push_back(c);
				}
				continue;
			}
			if (c == '=')
			{
				if (pending_obj_or_arr)
				{
					pending_obj_or_arr = false;
					pushAndAscend(soup::make_unique<soup::JsonObject>());
				}
				key = std::move(buf);
				buf.clear();
				soup::string::trim(key);
			}
			else if (c == '{')
			{
				buf.clear(); // discard indentation
				if (pending_obj_or_arr)
				{
					pushAndAscend(soup::make_unique<soup::JsonArray>());
				}
				pending_obj_or_arr = true;
			}
			else if (c == ',')
			{
				if (pending_obj_or_arr)
				{
					pending_obj_or_arr = false;
					pushAndAscend(soup::make_unique<soup::JsonArray>());
					soup::string::trim(buf); // discard indentation
					dischargeBuffer();
				}
				else
				{
					soup::string::trim(buf);
					if (!buf.empty())
					{
						dischargeBuffer();
					}
				}
			}
			else if (c == '}')
			{
				if (pending_obj_or_arr)
				{
					pending_obj_or_arr = false;
					pushAndAscend(soup::make_unique<soup::JsonArray>());
				}
				soup::string::trim(buf);
				if (!buf.empty())
				{
					dischargeBuffer();
				}
				stack.pop();
			}
			else if (c == '"')
			{
				if (pending_obj_or_arr)
				{
					pending_obj_or_arr = false;
					pushAndAscend(soup::make_unique<soup::JsonArray>());
				}
				if (buf.empty())
				{
					is_string = true;
				}
				else
				{
					buf.push_back(c);
				}
			}
			else if (c == '\n')
			{
				if (!pending_obj_or_arr)
				{
					soup::string::trim(buf);
					if (!buf.empty())
					{
						dischargeBuffer();
					}
				}
			}
			else if (c != '\r')
			{
				buf.push_back(c);
			}
		}
		SOUP_ASSERT(stack.top() == root.get());
		stack.pop();
		SOUP_ASSERT(stack.empty());
		SOUP_ASSERT(buf.empty());
		SOUP_ASSERT(key.empty());
		return root;
	}

protected:
	void dischargeBuffer()
	{
		char* endptr;
		auto ival = strtoll(buf.data(), &endptr, 10);
		if (endptr == buf.data() + buf.size())
		{
			if (buf.empty())
			{
				pushValue(soup::make_unique<soup::JsonString>(std::move(buf)));
			}
			else
			{
				pushValue(soup::make_unique<soup::JsonInt>(ival));
			}
		}
		else if (endptr != buf.data())
		{
			auto fval = strtod(buf.data(), &endptr);
			if (endptr == buf.data() + buf.size())
			{
				pushValue(soup::make_unique<soup::JsonFloat>(fval));
			}
			else
			{
				pushValue(soup::make_unique<soup::JsonString>(std::move(buf)));
			}
		}
		else
		{
			pushValue(soup::make_unique<soup::JsonString>(std::move(buf)));
		}
		buf.clear();
	}

	void pushAndAscend(soup::UniquePtr<soup::JsonNode> value)
	{
		SOUP_ASSERT(value->getType() == soup::JSON_ARRAY || value->getType() == soup::JSON_OBJECT);
		auto ptr = value.get();
		pushValue(std::move(value));
		stack.push(ptr);
	}

	void pushValue(soup::UniquePtr<soup::JsonNode> value)
	{
		if (stack.top()->getType() == soup::JSON_ARRAY)
		{
			static_cast<soup::JsonArray*>(stack.top())->children.emplace_back(std::move(value));
		}
		else
		{
			static_cast<soup::JsonObject*>(stack.top())->add(std::move(key), std::move(value));
			key.clear();
		}
	}

public:
	static std::string unparse(const soup::JsonObject& obj)
	{
		std::string str;
		unparse(obj, str);
		return str;
	}

protected:
	static void unparse(const soup::JsonObject& obj, std::string& out)
	{
		for (const auto& e : obj)
		{
			unparse(*e.first, out);
			out.push_back('=');
			unparse(*e.second, out);
			out.push_back('\n');
		}
	}

	static void unparse(const soup::JsonNode& n, std::string& out)
	{
		switch (n.getType())
		{
		case soup::JSON_OBJECT:
			out.append("{\n");
			unparse(n.reinterpretAsObj(), out);
			out.push_back('}');
			break;

		case soup::JSON_ARRAY: {
			out.push_back('{');
			for (const auto& e : n.reinterpretAsArr().children)
			{
				out.push_back('\n');
				unparse(*e, out);
				out.push_back(',');
			}
			if (out.back() == ',')
			{
				out.pop_back();
			}
			if (!n.reinterpretAsArr().empty())
			{
				out.push_back('\n');
			}
			out.push_back('}');
			break;
		}

		case soup::JSON_STRING:
			if (!n.reinterpretAsStr().value.empty())
			{
				out.append(n.reinterpretAsStr().value);
				break;
			}
			[[fallthrough]];
		default:
			out.append(n.encode());
			break;
		}
	}
};
