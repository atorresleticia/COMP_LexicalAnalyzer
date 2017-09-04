#pragma once
#include <string>
#include <iostream>

struct tokens
{
	std::string token;
	std::string t_class;
	std::string t_value = {""};

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
};

inline std::ostream& operator<< (std::ostream& stream, const tokens& t) {
	std::cout << t.token << "\t" << t.t_class << "\t" << t.t_value << std::endl;
	return stream;
}
