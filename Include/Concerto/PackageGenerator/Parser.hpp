#pragma once

#include <ranges>
#include <Concerto/Core/Logger.hpp>
#include <toml11/parser.hpp>

#include "Defines.hpp"
#include "Concerto/PackageGenerator/Lexer.hpp"

class Parser
{
public:
	Parser(std::vector<Token> tokens)
		: tokens(std::move(tokens)), index(0)
	{
	}

	Package ParsePackage();

private:
	std::vector<Token> tokens;
	size_t index;
	std::string pendingAttr;

	bool IsAtEnd() const
	{
		return index >= tokens.size() || tokens[index].type == TokenType::EndOfFile;
	}

	const Token& Peek() const
	{
		return tokens[index];
	}

	const Token& Advance()
	{
		return tokens[index++];
	}

	bool Match(const std::string& expected)
	{
		if (!IsAtEnd() && tokens[index].lexeme == expected)
		{
			Advance();
			return true;
		}
		return false;
	}

	std::optional<Namespace> ParseNamespace();

	std::optional<Class> ParseClass();

	std::vector<Class::Method::Params> ParseMethodParameters();

	Enum ParseEnum();
};
