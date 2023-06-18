//Fernando Andrade
//Visual Studio 2019
//Lexical analyzer using a FSM


#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;


enum FSM_TRANSITIONS {
	REJECT = 0,
	INTEGER,
	REAL,
	OPERATOR,
	IDENTIFIER,
	UNKNOWN,
	SPACE,
	KEYWORD,
	COMMENT,
	SEPARATOR
};

int fsm_table[][20] = 
/*								INTEGER		REAL			OPERATOR			IDENTIFIER	UNKNOWN		SPACE		KEYWORD		COMMENT		SEPARATOR*/
					{ {0,		INTEGER,		REAL,		OPERATOR,			IDENTIFIER,	UNKNOWN,		SPACE,		KEYWORD,		COMMENT,		SEPARATOR},
/*STATE 1*/			{INTEGER,		INTEGER,		REAL,		REJECT,			REJECT,		REJECT,		REJECT,		REJECT,		COMMENT,		REJECT},
/*STATE 2*/			{REAL,		REAL,		UNKNOWN,		REJECT,			REJECT,		REJECT,		REJECT,		REJECT,		COMMENT,		REJECT},
/*STATE 3*/			{OPERATOR,	REJECT,		OPERATOR,		REJECT,			IDENTIFIER,	REJECT,		REJECT,		REJECT,		COMMENT,		REJECT},
/*STATE 4*/			{IDENTIFIER,	IDENTIFIER,	REJECT,		IDENTIFIER,		IDENTIFIER,	REJECT,		REJECT,		REJECT,		COMMENT,		REJECT},
/*STATE 5*/			{UNKNOWN,		UNKNOWN,		UNKNOWN,		UNKNOWN,			UNKNOWN,		UNKNOWN,		REJECT,		REJECT,		COMMENT,		REJECT},
/*STATE 6*/			{SPACE,		REJECT,		REJECT,		REJECT,			REJECT,		REJECT,		REJECT,		REJECT,		COMMENT,		REJECT},
/*STATE 7*/			{KEYWORD,		REJECT,		REJECT,		REJECT,			KEYWORD,		REJECT,		REJECT,		KEYWORD,		COMMENT,		REJECT},
/*STATE 8*/			{COMMENT,		COMMENT,		COMMENT,		COMMENT,			COMMENT,		COMMENT,		COMMENT,		COMMENT,		REJECT,		COMMENT},
/*STATE 9*/			{SEPARATOR,	REJECT,		REJECT,		REJECT,			REJECT,		REJECT,		REJECT,		REJECT,		COMMENT,		SEPARATOR}};

struct TokenName {
	string token;
	int lexeme;
	string lexeme_name;
};

vector<TokenName> Lexer(string code);
int get_fsm_column(char current);
string get_lexeme_name(int lexeme);
int find_if_keyword(string token);

int main() 
{
	ifstream source_code;
	string file_name = "";
	string code = "";
	vector<TokenName> tokens;

	//Input file to Analyze
	cout << "Enter file name: ";
	cin >> file_name;

	source_code.open(file_name.c_str());

	if (source_code.fail()) {
		cout << "ERROR: " << file_name << " could not be found";
		exit(1);
	}

	// Read each line of the file
	while (getline(source_code, code))
	{
		//Create tokens
		tokens = Lexer(code);

		//Output Lexeme Name and Tokens
		for (int i = 0; i < tokens.size(); ++i)
		{
			cout << left << setw(10) << tokens[i].lexeme_name << right << setw(16) << setfill(' ') << tokens[i].token << endl;
		}
	}

	source_code.close();

	system("pause");

	return 0;
}

vector<TokenName> Lexer(string code) 
{
	TokenName access;
	vector<TokenName> tokens;
	char curr_char = ' ';
	int column = REJECT;
	int curr_state = REJECT;
	int prev_state = REJECT;
	string curr_token = "";
	int isKeyword = 0;

	for (int i = 0; i < code.length();)
	{
		curr_char = code[i];

		column = get_fsm_column(curr_char);

		curr_state = fsm_table[curr_state][column];

		if (curr_state == REJECT) // If true then a token was found
		{
			if (prev_state != SPACE) //Ignore all spaces
			{
				access.token = curr_token;
				
				//Check if the IDENTIFIER is a KEYWORD
				if (prev_state == 4)
				{
					isKeyword = find_if_keyword(curr_token);

					if (isKeyword == KEYWORD)
					{
						prev_state = 7;
					}
				}

				access.lexeme = prev_state;
				access.lexeme_name = get_lexeme_name(access.lexeme);
				tokens.push_back(access);
			}

			curr_token = "";
		}

		else //Continue to next character till a token is found
		{
			curr_token += curr_char;
			++i;
		}

		prev_state = curr_state;
	}

	// Save the last token at the end of the for loop
	if (curr_state != SPACE && curr_token != "") //Ignore spaces check that token isn't empty
	{
		access.token = curr_token;

		//Check if the IDENTIFIER is a KEYWORD
		if (curr_state == 4)
		{
			isKeyword = find_if_keyword(curr_token);

			if (isKeyword == KEYWORD)
			{
				curr_state = 7;
			}
		}

		access.lexeme = curr_state;
		access.lexeme_name = get_lexeme_name(access.lexeme);
		tokens.push_back(access);
	}

	return tokens;
}

//Find the state of current char
int get_fsm_column(char curr_char)
{
	char comment = '!';
	char operators[8] = { '+', '-', '*', '/', '=', '>', '<', '$' };
	char separators[11] = { '\'', '(', ')', '{', '}', '.', ':', ';', '[', ']', ',' };



	//Check integers
	if (isdigit(curr_char))
	{
		return INTEGER;
	}
	//check real/floating point numbers
	else if (curr_char == '.')
	{
		return REAL;
	}
	//Check for characters
	else if (isalpha(curr_char))
	{
		return IDENTIFIER;
	}

	//check whitespaces
	else if (isspace(curr_char))
	{
		return SPACE;
	}

	//Check operators
	else if (ispunct(curr_char))
	{
		for (int i = 0; i < 8; i++)
		{
			if (curr_char == operators[i]) {
				return OPERATOR;
			}
		}

		//check separators
		for (int i = 0; i < 11; i++)
		{
			if (curr_char == separators[i])
			{
				return SEPARATOR;
			}
		}

		if (curr_char == comment)
		{
			return COMMENT;
		}
	}

	else
	{
		return UNKNOWN;	
	}
}

int find_if_keyword(string token) 
{
	// then, endif, endwhile, doend, forend, input, output, and, or, not are not used in C++ as keywords 
	string keywords[19] = { "int", "float", "bool", "true", "false", "else", "if", "while", "do",
							"for", "new", "struct", "class", "double", "char", "return" };

	for (int i = 0; i < 19; i++)
	{
		if (token == keywords[i])
		{
			return KEYWORD;
		}
	}

	return IDENTIFIER;
}

string get_lexeme_name(int lexeme)
{
	//Returning the appropriate fsm transition

	if (lexeme == INTEGER)
	{
		return "INTEGER";
	}

	else if (lexeme == REAL)
	{
		return "REAL";
	}

	else if (lexeme == OPERATOR)
	{
		return "OPERATOR";
	}

	else if (lexeme == IDENTIFIER)
	{
		return "IDENTIFIER";
	}

	else if (lexeme == SEPARATOR)
	{
		return "SEPARATOR";
	}

	else if (lexeme == COMMENT)
	{
		return "COMMENT";
	}

	else if (lexeme == KEYWORD)
	{
		return "KEYWORD";
	}
}




