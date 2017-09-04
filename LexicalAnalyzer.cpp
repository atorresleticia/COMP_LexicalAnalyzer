// LexicalAnalyzer.cpp : Defines the entry point for the console application.
//
#include "token.h"
#include <fstream>
#include <regex>
#include <vector>
#include <cctype>
#include <iomanip>

#define W_TEXT_ALIGN 25

using namespace std;

string keywords[22] = { "young", "master", "padawan",
						"jedi", "sith",
						"republic", "yoda",
						"red", "blue", "green", "violet",
						"vader", "const", "stormtrooper", "anakin", "default",
						"sizeof", "wookie", "enter", "galaxy", "cpo", "rtwo" };

regex id("_[a-z|A-Z]+[0-9]*");
regex young("[0-9]+");
regex padawan("[0-9]+[.][0-9]+");
regex reserved("[a-z]+");

vector<tokens> token;

bool is_keyword(string lexeme)
{
	bool found = false;
	for(string s : keywords)
	{
		lexeme == s ? found = true : found = false;
		if (found)
		{
			if (lexeme == "red")
			{
				token.push_back({ lexeme, "op_logico", "red" });
				found = false;
			} else if (lexeme == "green")
			{
				token.push_back({ lexeme, "op_logico", "green" });
				found = false;
			} else if (lexeme == "violet")
			{
				token.push_back({ lexeme, "op_logico", "violet" });
				found = false;
			} else if (lexeme == "blue")
			{
				token.push_back({ lexeme, "op_logico", "blue" });
				found = false;
			}
			break;
		}
	}
	return found;
}

void get_tokens(ifstream& tlc_code)
{
	char reader;
	string lexeme;
	string comment_reader;

	while (tlc_code >> noskipws >> reader) // noskipws � pra ler qualquer caractere, incluse espa�o, porque o "file >>" ignora espa�os
	{
		if (isalpha(reader))
		{
			lexeme = reader;
			do
			{
				tlc_code >> noskipws >> reader;
				lexeme += reader;
			} while (regex_match(lexeme, reserved));
			lexeme.pop_back();
			tlc_code.unget();
			if (is_keyword(lexeme))
			{
				token.push_back({ lexeme, "p_res" });
			}
		}
		else if (isdigit(reader))
		{
			bool is_young = false;
			lexeme = reader;
			while (regex_match(lexeme, young))
			{
				tlc_code >> noskipws >> reader;
				lexeme += reader;
				is_young = true;
			}
			tlc_code >> noskipws >> reader;
			if (lexeme.back() == '.' && isdigit(reader))
			{
				lexeme += reader;
				while(regex_match(lexeme, padawan))
				{
					tlc_code >> noskipws >> reader;
					lexeme += reader;
				}
				is_young = false;
			}
			else
			{
				tlc_code.unget();
			}
			lexeme.pop_back();
			tlc_code.unget();
			is_young ? token.push_back({ lexeme, "num, young", lexeme }) : token.push_back({ lexeme, "num, padawan", lexeme });
		}
		else
		{
			switch (reader)
			{
			case '_':
				lexeme = reader;
				do
				{
					tlc_code >> noskipws >> reader;
					lexeme += reader;
				} while (regex_match(lexeme, id));

				tlc_code.unget();
				lexeme.pop_back();

				token.push_back({ lexeme, "id" });

				break;

			case '%':
				lexeme = reader;
				token.push_back({ lexeme, "op_arit", "mod" });
				break;

			case '+':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				if (reader == '+')
				{
					lexeme += reader;
					token.push_back({ lexeme, "inc" });
				}
				else
				{
					token.push_back({ lexeme, "op_arit", "soma" });
					tlc_code.unget();
				}
				break;

			case '-':
				lexeme = reader;
				token.push_back({ lexeme, "op_arit", "sub" });
				break;

			case '*':
				lexeme = reader;
				token.push_back({ lexeme, "op_arit", "mul" });
				break;

			case '/':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				if (reader == '/')
				{
					lexeme += reader;
					getline(tlc_code, comment_reader);
					token.push_back({ lexeme, "coment" });
				}
				else
				{
					token.push_back({ lexeme, "op_arit", "div" });
					tlc_code.unget();
				}
				break;

			case '=':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				if (reader == '=')
				{
					lexeme += reader;
					token.push_back({ lexeme, "op_rel", "igual" });
				}
				else if (reader == '/')
				{
					lexeme += reader;
					tlc_code >> noskipws >> reader;
					if (reader == '=')
					{
						lexeme += reader;
						token.push_back({ lexeme, "op_rel", "diferenca" });
					} else
					{
						tlc_code.unget();
						tlc_code.unget();
						lexeme.pop_back();
						token.push_back({ lexeme, "atribuicao" });
					}
				}
				else
				{
					tlc_code.unget();
					token.push_back({ lexeme, "atribuicao" });
				}

				break;

			case '<':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				switch (reader)
				{
				case '=':
					lexeme += reader;
					token.push_back({ lexeme, "op_rel", "menor_igual" });
					break;
				case '<':
					lexeme += reader;
					token.push_back({ lexeme, "operador de entrada" });
					break;
				default:
					tlc_code.unget();
					token.push_back({ lexeme, "op_rel", "menor" });
					break;
				}
				break;

			case '>':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				switch (reader)
				{
				case '=':
					lexeme += reader;
					token.push_back({ lexeme, "op_rel", "maior_igual" });
					break;
				case '>':
					lexeme += reader;
					token.push_back({ lexeme, "operador de saida" });
					break;
				default:
					tlc_code.unget();
					token.push_back({ lexeme, "op_rel", "maior" });
					break;
				}
				break;

			case '{':
				lexeme = reader;
				token.push_back({ lexeme, "delimitador", "abre_chaves" });
				break;
			case '}':
				lexeme = reader;
				token.push_back({ lexeme, "delimitador", "fecha_chaves" });
				break;
			case '(':
				lexeme = reader;
				token.push_back({ lexeme, "delimitador", "abre_par" });
				break;
			case ')':
				lexeme = reader;
				token.push_back({ lexeme, "delimitador", "fecha_par" });
				break;
			case ';':
				lexeme = reader;
				token.push_back({ lexeme, "delimitador", "fim_de_comando" });
				break;
			case '\'':
				lexeme = reader;
				token.push_back({ lexeme, "delimitador", "aspas_simples" });
				tlc_code >> noskipws >> reader;
				if (isalpha(reader))
				{
					string c;
					c = reader;
					tlc_code >> noskipws >> reader;
					if (reader == '\'')
					{
						token.push_back({ c, "master", c });
						tlc_code.unget();
					} else
					{
						tlc_code.unget();
						tlc_code.unget();
					}
				} else
				{
					tlc_code.unget();
				}
				break;
			case '\"':
				do
				{
					tlc_code >> noskipws >> reader;
				} while (reader != '\"');
			default:
				break;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Numero de argumentos incorreto." << endl;
		cout << "<programa> <codigo_tlc>.txt" << endl;
		return 1;
	}

	string in = argv[1];

	#ifdef _WIN32
	string out = "lexico\\token_";
	out += in.substr(in.find("\\") + 1);
	#endif

	#ifdef __unix__
	string out = "lexico/token_";
	out += in.substr(in.find("/") + 1);
	#endif

	ifstream tlc_code(in);
	ofstream table_of_tokens(out);
	
	table_of_tokens << fixed;

	get_tokens(tlc_code);

	table_of_tokens << setw(W_TEXT_ALIGN) << left << "TOKEN" << setw(W_TEXT_ALIGN) << left << "CLASSE" << setw(W_TEXT_ALIGN) << left << "VALOR" << endl << endl;
	for (vector<tokens>::iterator it = token.begin(); it < token.end(); ++it)
	{
		table_of_tokens << setw(W_TEXT_ALIGN) << left << it->token << setw(W_TEXT_ALIGN) << left << it->t_class << setw(W_TEXT_ALIGN) << left << it->t_value << endl;
	}

	table_of_tokens.close();
	tlc_code.close();
    return 0;
}

