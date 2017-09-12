// LexicalAnalyzer.cpp : Defines the entry point for the console application.
//
#include "token.h"
#include <fstream>
#include <regex>
#include <vector>
#include <cctype>
#include <iomanip>
#include <cstdlib>

#define W_TEXT_ALIGN 25

using namespace std;

string keywords[18] = { "young", "master", "padawan",
						"jedi", "sith",
						"republic", "yoda",
						"vader", "const", "stormtrooper", "anakin", "default",
						"sizeof", "wookie", "enter", "galaxy", "cpo", "rtwo" };

regex id("_[a-z|A-Z]+[0-9]*");
regex young("[0-9]+");
regex padawan("[0-9]+[.][0-9]+");
regex reserved("[a-z]+");

vector<tokens> token_list;
int line_count = 1;

bool is_logical_operator(string lexeme);
bool is_keyword(string lexeme);
void get_tokens(ifstream& tlc_code);

bool is_keyword(string lexeme)
{
	bool found = false;
	for(string s : keywords)
	{
		lexeme == s ? found = true : found = false;
		if (found)
			break;	
	}
	return found;
}

bool is_logical_operator(string lexeme)
{
	if (lexeme == "red")
	{
		token_list.push_back({ lexeme, "op_logico", "red", line_count });
		return true;
	}
	if (lexeme == "green")
	{
		token_list.push_back({ lexeme, "op_logico", "green", line_count });
		return true;
	}
	if (lexeme == "violet")
	{
		token_list.push_back({ lexeme, "op_logico", "violet", line_count });
		return true;
	}
	if (lexeme == "blue")
	{
		token_list.push_back({ lexeme, "op_logico", "blue", line_count });
		return true;
	}
	return false;
}

void get_tokens(ifstream& tlc_code)
{
	char reader;
	string lexeme;

	while (tlc_code >> noskipws >> reader)
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
			
			if (is_logical_operator(lexeme) ==  false)
			{
				if (is_keyword(lexeme))
				{
					token_list.push_back({ lexeme, lexeme, line_count });
				}
				else 
				{
					cout << line_count << ": \"" << lexeme << "\" nao reconhecido." << endl;
				}
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
			is_young ? token_list.push_back({ lexeme, "young", lexeme, line_count }) : token_list.push_back({ lexeme, "padawan", lexeme, line_count });
		}
		else
		{
			int char_counter = 0;
			int line_init;
			string comment_reader;
			
			switch (reader)
			{
			case '\n':
				line_count++;
				break;
			case '_':
				lexeme = reader;
				do
				{
					tlc_code >> noskipws >> reader;
					lexeme += reader;
				} while (regex_match(lexeme, id));

				tlc_code.unget();
				lexeme.pop_back();

				token_list.push_back({ lexeme, "id", lexeme, line_count });

				break;

			case '%':
				lexeme = reader;
				token_list.push_back({ lexeme, "op_arit", "mod", line_count });
				break;

			case '+':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				if (reader == '+')
				{
					lexeme += reader;
					token_list.push_back({ lexeme, "inc", line_count });
				}
				else
				{
					token_list.push_back({ lexeme, "op_arit", "soma", line_count });
					tlc_code.unget();
				}
				break;

			case '-':
				lexeme = reader;
				token_list.push_back({ lexeme, "op_arit", "sub", line_count });
				break;

			case '*':
				lexeme = reader;
				token_list.push_back({ lexeme, "op_arit", "mul", line_count });
				break;

			case '/':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				if (reader == '/')
				{
					lexeme += reader;
					getline(tlc_code, comment_reader);
					token_list.push_back({ lexeme, "comentario", line_count });
					line_count++;
				}
				else
				{
					token_list.push_back({ lexeme, "op_arit", "div", line_count });
					tlc_code.unget();
				}
				break;

			case '=':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				if (reader == '=')
				{
					lexeme += reader;
					token_list.push_back({ lexeme, "op_rel", "igual", line_count });
				}
				else if (reader == '/')
				{
					lexeme += reader;
					tlc_code >> noskipws >> reader;
					if (reader == '=')
					{
						lexeme += reader;
						token_list.push_back({ lexeme, "op_rel", "diferenca", line_count });
					} else
					{
						tlc_code.unget();
						tlc_code.unget();
						lexeme.pop_back();
						token_list.push_back({ lexeme, "atribuicao", line_count });
					}
				}
				else
				{
					tlc_code.unget();
					token_list.push_back({ lexeme, "atribuicao", line_count });
				}

				break;

			case '<':
				lexeme = reader;
				tlc_code >> noskipws >> reader;

				switch (reader)
				{
				case '=':
					lexeme += reader;
					token_list.push_back({ lexeme, "op_rel", "menor_igual", line_count });
					break;
				case '<':
					lexeme += reader;
					token_list.push_back({ lexeme, "operador de entrada", line_count });
					break;
				default:
					tlc_code.unget();
					token_list.push_back({ lexeme, "op_rel", "menor", line_count });
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
					token_list.push_back({ lexeme, "op_rel", "maior_igual", line_count });
					break;
				case '>':
					lexeme += reader;
					token_list.push_back({ lexeme, "operador de saida", line_count });
					break;
				default:
					tlc_code.unget();
					token_list.push_back({ lexeme, "op_rel", "maior", line_count });
					break;
				}
				break;

			case '{':
				lexeme = reader;
				token_list.push_back({ lexeme, "abre_chaves", line_count });
				break;
			case '}':
				lexeme = reader;
				token_list.push_back({ lexeme, "fecha_chaves", line_count });
				break;
			case '(':
				lexeme = reader;
				token_list.push_back({ lexeme, "abre_par", line_count });
				break;
			case ')':
				lexeme = reader;
				token_list.push_back({ lexeme, "fecha_par", line_count });
				break;
			case ';':
				lexeme = reader;
				token_list.push_back({ lexeme, "fim_de_comando", line_count });
				break;
			case '\'':
				lexeme = reader;
				tlc_code >> noskipws >> reader;
				if (reader != '\'')
				{
					string c;
					c = reader;
					lexeme += reader;
					tlc_code >> noskipws >> reader;
					if (reader == '\'')
					{
						lexeme += reader;
						token_list.push_back({ lexeme, "master", c, line_count });
					} else
					{
						cout << line_count << ": esta faltando um \'. Master corresponde a apenas um caractere." << endl;
						tlc_code.unget();
						tlc_code.unget();
					}
				} else
				{
					lexeme += reader;
					token_list.push_back({ lexeme, "master", "master vazio", line_count });
				}
				break;
			case '\"':
				line_init = line_count;
				lexeme = reader;
				do
				{
					tlc_code >> noskipws >> reader;
					lexeme += reader;
					if (reader == '\n') { line_count++; }
				} while(reader != '\"' && !tlc_code.eof());

				if (tlc_code.eof())
				{
					lexeme.pop_back();
					cout << line_init << ": string " << lexeme << endl << "incompleta, esta faltando um \" ." << endl;
				} else {
					token_list.push_back( {lexeme, "string", line_init } );
				}
				break;
			default:
				if (reader != ' ' && reader != '\t')
				{
					cout << line_count << ": caractere \'" << reader << "\' nao reconhecido." << endl;
				}
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
		cout << "./<programa> <codigo_tlc>.txt" << endl;
		return 1;
	}

	string in = argv[1];

	system("clear");
	string out = "saida_lexico/token_";
	out += in.substr(in.find("/") + 1);

	cout << "\n\t\t\t Analisador Lexico \"The Last Code\"" << endl;

	ifstream tlc_code(in);
	ofstream table_of_tokens(out);
	
	table_of_tokens << fixed;

	get_tokens(tlc_code);

	table_of_tokens << "#L" << "\t" << setw(W_TEXT_ALIGN) << left << "TOKEN" << setw(W_TEXT_ALIGN) << left << "CLASSE" << setw(W_TEXT_ALIGN) << left << "VALOR" << endl << endl;
	for (vector<tokens>::iterator it = token_list.begin(); it < token_list.end(); ++it)
	{
		table_of_tokens << it->t_line << "\t" << setw(W_TEXT_ALIGN) << left << it->token << setw(W_TEXT_ALIGN) << left << it->t_class << setw(W_TEXT_ALIGN) << left << it->t_value << endl;
	}

	table_of_tokens.close();
	tlc_code.close();

	cout << "\nPressione qualquer tecla para abrir o arquivo de tokens..." << endl;
	cin.ignore();

	out.insert(0, "xdg-open ");
	system(out.c_str());
	
	return 0;
}

