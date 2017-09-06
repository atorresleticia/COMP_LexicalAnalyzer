#pragma once
#include <string>
#include <iostream>

struct tokens
{
	std::string token;
	std::string t_class;
	std::string t_value = {""};
	int t_line = -1;

	// construtor
	tokens(const std::string& token, const std::string& t_class, const std::string& t_value)
		: token(token),
		  t_class(t_class),
		  t_value(t_value)
	{
	}

	tokens(const std::string& token, const std::string& cs)
		: token(token),
		  t_class(cs)
	{
	}
	
	tokens(const std::string& token, const std::string& t_class, const std::string& t_value, int t_line)
	: token(token),
	  t_class(t_class),
	  t_value(t_value),
	  t_line(t_line)
	{
	}

	tokens(const std::string& token, const std::string& cs, int t_line)
		: token(token),
		t_class(cs),
		t_line(t_line)
	{
	}
};

inline std::ostream& operator<< (std::ostream& stream, const tokens& t) {
	std::cout << t.token << "\t" << t.t_class << "\t" << t.t_value << std::endl;
	return stream;
}
