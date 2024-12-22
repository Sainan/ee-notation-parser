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
					key = std::move(buf);
				}
				else
				{
					key = std::move(buf);
				}
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
		SOUP_ASSERT(value->type == soup::JSON_ARRAY || value->type == soup::JSON_OBJECT);
		auto ptr = value.get();
		pushValue(std::move(value));
		stack.push(ptr);
	}

	void pushValue(soup::UniquePtr<soup::JsonNode> value)
	{
		if (stack.top()->type == soup::JSON_ARRAY)
		{
			static_cast<soup::JsonArray*>(stack.top())->children.emplace_back(std::move(value));
		}
		else
		{
			static_cast<soup::JsonObject*>(stack.top())->add(std::move(key), std::move(value));
			key.clear();
		}
	}
};
