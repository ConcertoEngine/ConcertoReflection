//
// Created by arthur on 09/12/2024.
//

#include "Concerto/PackageGenerator/Parser.hpp"

namespace
{
	std::string Trim(const std::string& s)
	{
		size_t start = 0;
		while (start < s.size() && std::isspace(s[start])) {
			++start;
		}
		size_t end = s.size();
		while (end > start && std::isspace(s[end - 1])) {
			--end;
		}
		return s.substr(start, end - start);
	}

	std::string InlineTomlToMultiple(const std::string& input)
	{
		std::vector<std::string> tokens;
		std::string token;

		bool in_quotes = false;  // Tracks whether we are inside quotes.
		int bracket_level = 0;   // Tracks nesting level for brackets (arrays).

		for (size_t i = 0; i < input.size(); ++i) {
			char c = input[i];

			// Toggle the in_quotes flag when a non-escaped double quote is encountered.
			if (c == '"') {
				in_quotes = !in_quotes;
				token.push_back(c);
				continue;
			}

			// Only check for brackets and delimiters when not in quotes.
			if (!in_quotes) {
				if (c == '[') {
					++bracket_level;
				}
				else if (c == ']') {
					--bracket_level;
				}

				// When a comma appears and we're not inside an array, it marks the end of a key-value pair.
				if (c == ',' && bracket_level == 0) {
					tokens.push_back(Trim(token));
					token.clear();
					continue; // Do not include the comma.
				}
			}

			token.push_back(c);
		}

		// Add the remaining token if any.
		if (!token.empty()) {
			tokens.push_back(Trim(token));
		}

		// Build the output string with each key-value pair on a new line.
		std::ostringstream oss;
		for (size_t i = 0; i < tokens.size(); ++i) {
			oss << tokens[i];
			if (i != tokens.size() - 1) {
				oss << "\n";
			}
		}

		return oss.str();
	}

	std::pair<std::string /*Attribute scope (Package, Enum, Class, Member, Method)*/, TomlAttributes> ConvertTomlAttributes(const std::string& attrStr)
	{
		auto firstParenthesis = attrStr.find('(');
		if (firstParenthesis == std::string::npos)
			return { attrStr, {} };
		std::string scope = attrStr.substr(0, firstParenthesis);
		std::string toml = attrStr.substr(firstParenthesis + 1, attrStr.size() - firstParenthesis - 2);
		toml = InlineTomlToMultiple(toml);

		auto result = toml::try_parse_str(toml);
		if (result.is_err())
		{
			for (auto& err : result.as_err())
				cct::Logger::Error("{}\n\t{}", err.title(), err.suffix());
			return {};
		}
		return { scope, TomlAttributes(result.as_ok()) };
	}

	TomlAttributes GetAttributesOr(TomlAttributes attributes, TomlAttributes defaultValue)
	{
		if (attributes.is_table() == false)
			return defaultValue;
		auto& attributesTable = attributes.as_table();
		auto it = attributesTable.find("Attributes");
		if (it == attributesTable.end())
			return defaultValue;
		return it->second;
	}
}


Package Parser::ParsePackage()
{
	Package pkg;

	while (!IsAtEnd())
	{
		if (Peek().type == TokenType::Attribute)
		{
			pendingAttr = Peek().lexeme;
			Advance();
			continue;
		}
		if (Peek().type == TokenType::Identifier)
		{
			const std::string& kw = Peek().lexeme;
			if (kw == "namespace")
			{
				std::optional<Namespace> ns = ParseNamespace();
				if (ns)
					pkg.namepsaces.push_back(ns.value());
				continue;
			}
			if (kw == "class" || kw == "struct")
			{
				std::optional<Class> cls = ParseClass();
				if (!cls)
					continue;
				if (cls->scope == "cct::Package")
				{
					if (pkg.name.empty())
					{
						pkg.name = cls->name;
						auto& attributesTable = cls->tomlAttributes.as_table();

						auto versionIt = attributesTable.find("version");
						if (versionIt != attributesTable.end())
							pkg.version = versionIt->second.as_string();
						else pkg.version = "Undefined";

						auto descriptionIt = attributesTable.find("description");
						if (descriptionIt != attributesTable.end())
							pkg.description = descriptionIt->second.as_string();
						else pkg.description = "Undefined";
					}
					else if (pkg.name != cls->name)
						cct::Logger::Error("Package has already been defined");
				}
				else
					pkg.classes.push_back(cls.value());
				continue;
			}
			if (kw == "enum")
			{
				Enum enm = ParseEnum();
				pkg.enums.push_back(enm);
				continue;
			}
		}
		Advance();
	}
	return pkg;
}

std::optional<Namespace> Parser::ParseNamespace()
{
	Namespace ns;
	// Consume the "namespace" keyword.
	Advance(); // skip 'namespace'
	if (!IsAtEnd() && tokens[index].type == TokenType::Identifier)
	{
		ns.name = tokens[index].lexeme;
		Advance();
	}
	// In this simple parser, we do not store attributes on namespaces.
	if (Match("{"))
	{
		// Parse declarations inside the namespace.
		while (!IsAtEnd() && Peek().lexeme != "}")
		{
			if (Peek().type == TokenType::Attribute)
			{
				pendingAttr = Peek().lexeme;
				Advance();
				continue;
			}
			if (Peek().type == TokenType::Identifier)
			{
				const std::string& kw = Peek().lexeme;
				if (kw == "namespace")
				{
					std::optional<Namespace> childNs = ParseNamespace();
					if (childNs)
						ns.namespaces.push_back(childNs.value());
					continue;
				}
				else if (kw == "class" || kw == "struct")
				{
					std::optional<Class> cls = ParseClass();
					if (cls && cls->scope == "cct::Class")
						ns.classes.push_back(cls.value());
					continue;
				}
				else if (kw == "enum")
				{
					Enum enm = ParseEnum();
					ns.enums.push_back(enm);
					continue;
				}
			}
			Advance();
		}
		Match("}");
	}

	// Nested namespaces can be represented with "::" in the name.
	if (ns.name.find("::") != std::string::npos)
	{
		using namespace std::string_view_literals;
		auto split = ns.name | std::ranges::views::split("::"sv);
		std::vector<std::string_view> res;
		for (const auto elem : split)
			res.emplace_back(elem.data(), elem.size());

		Namespace parentNs;
		parentNs.name = res.front();
		parentNs.namespaces.reserve(res.size() - 1);

		Namespace* last = &parentNs;

		for (size_t i = 1; i < res.size(); ++i)
		{
			Namespace childNs;
			childNs.name = res[i];
			last->namespaces.push_back(childNs);
			last = &last->namespaces.back();
		}

		last->classes = ns.classes;
		last->enums = ns.enums;
		last->namespaces = ns.namespaces;

		if (ns.classes.empty() && ns.enums.empty() && ns.namespaces.empty())
			return std::nullopt;
		return parentNs;
	}
	if (ns.classes.empty() && ns.enums.empty() && ns.namespaces.empty())
		return std::nullopt;
	return ns;
}

std::optional<Class> Parser::ParseClass()
{
	Class cls;
	// Consume the class/struct keyword.
	Advance();
	if (Peek().type == TokenType::Attribute)
	{
		auto [scope, attributes] = ConvertTomlAttributes(Peek().lexeme);
		if (scope != "cct::Class" && scope != "cct::Package")
		{
			cct::Logger::Warning("Invalid attribute scope for class: '{}'. Expected 'cct::Class' or 'cct::Package'.", scope);
			return std::nullopt;
		}
		cls.scope = scope;
		if (scope == "cct::Class")
			cls.tomlAttributes = GetAttributesOr(attributes, {});
		else if (scope == "cct::Package")
			cls.tomlAttributes = std::move(attributes);
		Advance();
	}
	else
		return std::nullopt;
	if (!IsAtEnd() && tokens[index].type == TokenType::Identifier)
	{
		cls.name = tokens[index].lexeme;
		Advance();
	}
	if (Match(":"))
	{
		Match("public");
		Match("private");
		Match("protected");

		if (!IsAtEnd() && tokens[index].type == TokenType::Identifier)
		{
			cls.base = tokens[index].lexeme;
			Advance();
		}
	}

	// Skip predeclared class
	Match(";");

	if (Match("{"))
	{
		while (!IsAtEnd() && Peek().lexeme != "}")
		{
			Match("public:");
			Match("private:");
			Match("protected:");
			Match("virtual");
			if (Peek().type == TokenType::Attribute)
			{
				pendingAttr = Peek().lexeme;
				Advance();
				continue;
			}

			if (Peek().type == TokenType::Identifier)
			{
				std::string typeOrReturn = Advance().lexeme;
				while (Peek().type == TokenType::Symbol)
					typeOrReturn += Advance().lexeme;
				if (!IsAtEnd() && Peek().type == TokenType::Identifier)
				{
					std::string name = Peek().lexeme;
					Advance();
					if (!IsAtEnd() && Peek().lexeme == "(")
					{
						bool isMethodScope = false;
						Class::Method method;
						method.returnValue = typeOrReturn;
						method.name = name;
						method.params = ParseMethodParameters();

						if (!pendingAttr.empty())
						{
							auto [scope, attributes] = ConvertTomlAttributes(pendingAttr);
							isMethodScope = scope == "cct::Method";
							if (isMethodScope)
								method.tomlAttributes = GetAttributesOr(attributes, {});
							pendingAttr.clear();
						}
						if (isMethodScope)
							cls.methods.push_back(method);
						if (Match("{"))
						{
							int braceCount = 1;
							while (!IsAtEnd() && braceCount > 0)
							{
								if (Peek().lexeme == "{") braceCount++;
								if (Peek().lexeme == "}") braceCount--;
								Advance();
							}
						}
						else
						{
							Match(";");
						}
					}
					else
					{
						bool isMemberScope = false;
						Class::Member member;
						member.type = typeOrReturn;
						member.name = name;
						if (!pendingAttr.empty())
						{
							auto [scope, attributes] = ConvertTomlAttributes(pendingAttr);
							isMemberScope = scope == "cct::Member";
							if (isMemberScope)
								member.tomlAttributes = GetAttributesOr(attributes, {});
							pendingAttr.clear();
						}
						if (isMemberScope)
							cls.members.push_back(member);
						while (!IsAtEnd() && Peek().lexeme != ";")
						{
							Advance();
						}
						Match(";");
					}
					continue;
				}
			}
			Match("default");
			Match("delete");
			Advance(); // skip any unrecognized tokens
		}
		Match("}"); // close class body
		Match(";"); // optional semicolon after class definition
	}
	return cls;
}

std::vector<Class::Method::Params> Parser::ParseMethodParameters()
{
	std::vector<Class::Method::Params> params;
	if (!IsAtEnd() && Peek().lexeme == "(")
	{
		Advance(); // consume '('

		if (!IsAtEnd() && Peek().lexeme == ")")
		{
			Advance(); // consume ')'
			return params;
		}

		while (!IsAtEnd() && Peek().lexeme != ")")
		{
			Class::Method::Params param;

			if (Peek().type == TokenType::Attribute)
			{
				Advance();
			}

			if (!IsAtEnd() && Peek().type == TokenType::Identifier)
			{
				Match("const");
				param.type = Peek().lexeme;
				Advance();
				//parse template
				if (!IsAtEnd() && Peek().lexeme == "<")
				{
					while (!IsAtEnd() && Peek().lexeme != ">")
					{
						param.type += Advance().lexeme;
					}
					param.type += Advance().lexeme;
				}

				while (!IsAtEnd() && Peek().type == TokenType::Symbol
					&& Peek().lexeme != "," && Peek().lexeme != ")")
				{
					Advance();
				}
			}

			if (!IsAtEnd() && Peek().type == TokenType::Identifier)
			{
				param.name = Peek().lexeme;
				Advance();
			}


			params.push_back(param);

			if (!IsAtEnd() && Peek().lexeme == ",")
			{
				Advance();
			}
			if (!IsAtEnd() && Peek().lexeme == ")")
			{
				Advance();
				Match("const");
				Match(";");
				return params;
			}
			Advance(); // skip any unrecognized tokens
		}

		Match(")");
	}

	return params;
}

Enum Parser::ParseEnum()
{
	Enum enm;
	Advance(); // consume "enum"

	if (!IsAtEnd() && (Peek().lexeme == "class" || Peek().lexeme == "struct"))
	{
		Advance();
	}

	bool isEnumScope = false;
	if (Peek().type == TokenType::Attribute)
	{
		auto [scope, attributes] = ConvertTomlAttributes(Peek().lexeme);
		isEnumScope = scope == "cct::Enum";
		if (isEnumScope)
			enm.tomlAttributes = GetAttributesOr(attributes, {});
		Advance();
	}

	if (!IsAtEnd() && Peek().type == TokenType::Identifier)
	{
		enm.name = Peek().lexeme;
		Advance();
	}
	// Optional base type.
	if (Match(":"))
	{
		if (!IsAtEnd() && Peek().type == TokenType::Identifier)
		{
			enm.base = Peek().lexeme;
			Advance();
		}
	}
	// Enum body.
	if (Match("{"))
	{
		while (!IsAtEnd() && Peek().lexeme != "}")
		{
			Enum::Element element;
			if (Peek().type == TokenType::Attribute)
			{
				Advance();
			}
			if (!IsAtEnd() && Peek().type == TokenType::Identifier)
			{
				element.name = Peek().lexeme;
				Advance();
			}
			if (Match("="))
			{
				if (!IsAtEnd() && Peek().type == TokenType::Symbol)
				{
					element.value = Peek().lexeme;
					Advance();
				}
			}
			enm.elements.push_back(element);
			Match(",");
		}
		Match("}");
		Match(";");
	}
	if (isEnumScope)
		return enm;
	return {};
}
