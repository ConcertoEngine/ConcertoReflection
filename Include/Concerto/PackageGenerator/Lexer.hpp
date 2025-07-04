//
// Created by arthur on 14/04/2025.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

enum class TokenType
{
	Identifier,     // e.g. class names, keywords, etc.
	Attribute,      // the content inside [[ ... ]]
	Symbol,         // punctuation, operators, etc.
	EndOfFile       // marks end of input
};

struct Token
{
	TokenType type;
	std::string lexeme;
};

class Lexer {
public:
	Lexer(const std::string& input)
		: _input(input), _pos(0)
	{
	}

	std::vector<Token> tokenize()
	{
		std::vector<Token> tokens;
		Token token = nextToken();
		size_t i = 0;
		while (token.type != TokenType::EndOfFile)
		{
			tokens.push_back(token);
			++i;
			token = nextToken();
		}
		return tokens;
	}

private:
	std::string _input;
	size_t _pos;

	char Peek()
	{
		return _pos < _input.size() ? _input[_pos] : '\0';
	}

	char Get()
	{
		return _pos < _input.size() ? _input[_pos++] : '\0';
	}

	bool Eof() const
	{
		return _pos >= _input.size();
	}

	void SkipWhitespace()
	{
		while (!Eof() && std::isspace(Peek()))
			Get();
	}

	void SkipPreprocessor()
	{
		while (!Eof() && Peek() == '#')
			SkipLine();
	}

	void SkipUntilWhiteSpace()
	{
		while (!Eof() && (Peek() != ' ' && Peek() != '\n'))
			Get();
		SkipWhitespace();
	}

	void SkipCCTMacro()
	{
		if (!Eof() && _pos + 4 < _input.size() && _input[_pos] == 'C' && _input[_pos + 1] == 'C' && _input[_pos + 2] == 'T' && _input[_pos + 3] == '_')
			SkipUntilWhiteSpace();
	}

	void SkipComment()
	{
		if (!Eof() && _pos + 2 < _input.size() && _input[_pos] == '/' && _input[_pos + 1] == '*')
		{
			Get();
			Get();
			while (!Eof() && _pos + 2 < _input.size() && _input[_pos] != '*' || _input[_pos + 1] != '/')
				Get();
			Get();
			Get();
		}

		while (!Eof() && Peek() == '/')
			SkipLine();
	}

	void SkipLine()
	{
		while (!Eof() && Peek() != '\n')
			Get();
		while (!Eof() && Peek() == '\n')
			Get();
	}

	Token nextToken()
	{
		SkipWhitespace();
		SkipComment();
		SkipPreprocessor();
		SkipCCTMacro();

		if (Eof())
			return { .type = TokenType::EndOfFile, .lexeme = "" };

		char current = Peek();

		if (current == '[' && _pos + 1 < _input.size() && _input[_pos + 1] == '[')
			return parseAttribute();

		if (std::isalpha(current) || current == '_' || current == ':')
			return parseIdentifier();

		std::string symbol(1, Get());
		return { .type = TokenType::Symbol, .lexeme = symbol };
	}

	Token parseAttribute()
	{
		Get(); // consumes the first '['
		Get(); // consumes the second '['

		std::string content;
		while (!Eof() && !(Peek() == ']' && _pos + 1 < _input.size() && _input[_pos + 1] == ']'))
		{
			content.push_back(Get());
		}

		// Skip the closing "]]" if present.
		if (!Eof())
		{
			Get(); // consumes first ']'
			Get(); // consumes second ']'
		}

		return { .type = TokenType::Attribute, .lexeme = content };
	}

	Token parseIdentifier()
	{
		std::string identifier;
		while (!Eof() && (std::isalnum(Peek()) || Peek() == '_' || Peek() == ':' || Peek() == '.'))
		{
			identifier.push_back(Get());
		}
		return { .type = TokenType::Identifier, .lexeme = identifier };
	}
};
