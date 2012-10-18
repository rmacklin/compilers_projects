//Richard Macklin, CS160 F12, Professor Sherwood
//Project 0, 10/4/12

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stack>
#include <math.h>

using namespace std;


/*** Enums and Print Functions for Terminals and Non-Terminals  **********************/

#define MAX_SYMBOL_NAME_SIZE 25
#define MAX_DIGITS_IN_INT 10

//all of the terminals in the language
typedef enum {
	T_eof = 0,	// 0: end of file
	T_num,		// 1: numbers
	T_plus,		// 2: +
	T_minus,	// 3: - 
	T_times,	// 4: *
	T_period,	// 5: .
	T_store, 	// 6: -> 
	T_openparen,	// 7: (
	T_closeparen, 	// 8: )
	T_div,		// 9: /
	T_m,		// 10: m
	T_openbracket,	// 11: [
	T_closebracket	// 12: ]
} token_type;

//this function returns a string for the token.  It is used in the parsetree_t
//class to actually dump the parsetree to a dot file (which can then be turned
//into a picture).  Note that the return char* is a reference to a local copy
//and it needs to be duplicated if you are a going require multiple instances
//simultaniously
char* token_to_string(token_type c) {
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch( c ) {
		case T_eof: strncpy(buffer,"eof",MAX_SYMBOL_NAME_SIZE); break;
		case T_num: strncpy(buffer,"num",MAX_SYMBOL_NAME_SIZE); break;
		case T_plus: strncpy(buffer,"+",MAX_SYMBOL_NAME_SIZE); break;
		case T_minus: strncpy(buffer,"-",MAX_SYMBOL_NAME_SIZE); break;
		case T_times: strncpy(buffer,"*",MAX_SYMBOL_NAME_SIZE); break;
		case T_period: strncpy(buffer,".",MAX_SYMBOL_NAME_SIZE); break;
		case T_store: strncpy(buffer,"->",MAX_SYMBOL_NAME_SIZE); break;
		case T_openparen: strncpy(buffer,"(",MAX_SYMBOL_NAME_SIZE); break;
		case T_closeparen: strncpy(buffer,")",MAX_SYMBOL_NAME_SIZE); break;
                case T_div: strncpy(buffer,"/",MAX_SYMBOL_NAME_SIZE); break;
		case T_m: strncpy(buffer,"m",MAX_SYMBOL_NAME_SIZE); break;
		case T_openbracket: strncpy(buffer,"[",MAX_SYMBOL_NAME_SIZE); break;
		case T_closebracket: strncpy(buffer,"]",MAX_SYMBOL_NAME_SIZE); break;
		default: strncpy(buffer,"unknown_token",MAX_SYMBOL_NAME_SIZE); break;
	}
	return buffer;
}

//all of the non-terminals in the grammar (you need to add these in
//according to your grammar.. these are used for printing the thing out)
//please follow the convention we set up so that we can tell what the heck you
//are doing when we grade
typedef enum {
	epsilon = 100,
	NT_List,
	NT_Expr,
	//WRITEME: add symbolic names for you non-terminals here
	NT_ListPrime,
	NT_Cmd,
	NT_Args,
	NT_ExprPrime,
	NT_Term,
	NT_TermPrime,
	NT_Factor
} nonterm_type;

//this function returns a string for the non-terminals.  It is used in the parsetree_t
//class to actually dump the parsetree to a dot file (which can then be turned
//into a picture).  Note that the return char* is a reference to a local copy
//and it needs to be duplicated if you are a going require multiple instances
//simultaniously. 
char* nonterm_to_string(nonterm_type nt)
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch( nt ) {
		case epsilon: strncpy(buffer,"e",MAX_SYMBOL_NAME_SIZE); break;
		case NT_List: strncpy(buffer,"List",MAX_SYMBOL_NAME_SIZE); break;
		//WRITEME: add the other nonterminals you need here		
		case NT_Expr: strncpy(buffer,"Expr",MAX_SYMBOL_NAME_SIZE); break;
		case NT_ListPrime: strncpy(buffer,"List'",MAX_SYMBOL_NAME_SIZE); break;
		case NT_Cmd: strncpy(buffer,"Cmd",MAX_SYMBOL_NAME_SIZE); break;
		case NT_Args: strncpy(buffer,"Args",MAX_SYMBOL_NAME_SIZE); break;
		case NT_ExprPrime: strncpy(buffer,"Expr'",MAX_SYMBOL_NAME_SIZE); break;
		case NT_Term: strncpy(buffer,"Term",MAX_SYMBOL_NAME_SIZE); break;
		case NT_TermPrime: strncpy(buffer,"Term'",MAX_SYMBOL_NAME_SIZE); break;
		case NT_Factor: strncpy(buffer,"Factor",MAX_SYMBOL_NAME_SIZE); break;
		default: strncpy(buffer,"unknown_nonterm",MAX_SYMBOL_NAME_SIZE); break;
	}
	return buffer;
}

/*** Scanner Class ***********************************************/

class scanner_t {
  public:

	//eats the next token and prints an error if it is not of type c
	void eat_token(token_type c);

	//peeks at the lookahead token
	token_type next_token();

	//return line number for errors
	int get_line();

	//returns the last number read (to be used after eating a T_num
	//in order to get its value). It will be negative if a number
	//has not been read since our numbers must be positive
	int get_num();

	//constructor 
	scanner_t();

	//helper function that takes a string representation of a 10-digit
	//integer and returns true if the integer is greater than 2^31 -1
	//(10 is the maximum number of digits in a 32-bit integer; for all
	//other lengths, we *automatically* know whether or not the integer
	//is less than 2^31 - 1)
	bool out_of_range(char* num_string);
  private:

	//WRITEME: Figure out what you will need to write the scanner
	//and to implement the above interface.  It does not have to
	//be a state machine or anything fancy.  Just read in the
	//characters one at a time (using getchar would be a good way)
	//and group them into tokens.
	int line;
	int last_char_read;
	bool use_last_char;
	token_type* next_token_type;
	int num;

	//Reads in the next character using getchar and keeps track of
	//the number of newlines that have been read
	int read_char();

	//error message and exit for unrecognized character
	void scan_error(char x);
	//error message and exit for token mismatch
	void mismatch_error(token_type c);

	//This is a bogus member for implementing a useful stub, it should
	//be cut out once you get the scanner up and going.
	token_type bogo_token;

};

token_type scanner_t::next_token()
{
/*
	//WRITEME: replace this bogus junk with code that will take a peek
	//at the next token and return it to the parser.  It should _not_
	//actually consume a token - you should be able to call next_token()
	//multiple times without actually reading any more tokens in 
	if ( bogo_token!=T_plus && bogo_token!=T_eof ) return T_plus;
	else return bogo_token;
*/

	if ( next_token_type != NULL )    //we already looked at the next token,
		return *next_token_type;  //so just return the same thing

	next_token_type = new token_type();
	//If we are here, we haven't looked at the next token yet, so we
	//must read characters until we know what the token is.
	char c;
	c = (use_last_char ? last_char_read : read_char());
	while(isspace(c)) { c = read_char(); }
	use_last_char = false;
	// c should now be the first non-whitespace character of the
	// next token
	switch(c) {
		case EOF: *next_token_type = T_eof; break;
		case '+': *next_token_type = T_plus; break;
		case '*': *next_token_type = T_times; break;
		case '.': *next_token_type = T_period; break;
		case '(': *next_token_type = T_openparen; break;
		case ')': *next_token_type = T_closeparen; break;
		case '/': *next_token_type = T_div; break;
		case 'm': *next_token_type = T_m; break;
		case '[': *next_token_type = T_openbracket; break;
		case ']': *next_token_type = T_closebracket; break;
		case '-': // could be - or ->
		{
			char d = read_char();
			if(d == '>')
				*next_token_type = T_store;
			else
			{
				*next_token_type = T_minus;
				//we looked at an extra character that
				//wasn't part of this token, so we should
				//consider it as the first character of
				//the next token
				last_char_read = d;
				use_last_char = true;
			}
			break;
		}
		case '0': //on 10/9 after class prof said we could decide
			  //whether or not to accept integers with leading
			  //zeros as valid numbers. I could interpret them
			  //as octal, like C does, if I'm feeling really
			  //ambitious
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': //keep checking next char until it's not a digit
		{
			*next_token_type = T_num;
			char num_string[MAX_DIGITS_IN_INT+1];
			int i = 0;
			do {
				strncpy(num_string+i, &c, 1);
				c = read_char();
				i++;
			} while(c >= '0' && c <= '9' && i <= MAX_DIGITS_IN_INT);
			if ( (i > MAX_DIGITS_IN_INT) ||
			     (i == MAX_DIGITS_IN_INT && out_of_range(num_string)) )
			{
				num_string[i]='\0';
				printf("range error: number starting with '%s' is greater than (2^31 - 1) -line %d\n",num_string, get_line());
				exit(1);
			}
			//call atoi on num_string to get
			//the actual number associated with this token
			//(store it in a variable or something and have
			//a get_num() fct that returns it...then just
			//always call get_num() if the token was a T_num
			//by default could make it -1 and if get_num
			//returns a number < 0  we know we didn't actually
			//get the number)
			num = atoi(num_string);
			//by the nature of the while loop, we read an extra
			//character to make sure it wasn't another digit (which
			//would've still been part of this number token), so we
			//should use it as the first character of the next token
			last_char_read = c;
			use_last_char = true;
			break;
		}
		default: scan_error(c);
	}
	return *next_token_type;
}

void scanner_t::eat_token(token_type c)
{
	//if we are supposed to eat token c, and it does not match
	//what we are supposed to be reading from file, then it is a 
	//mismatch error ( call - mismatch_error(c) )
	if ( c != next_token() ) mismatch_error(c);

/*
	//WRITEME: cut this bogus stuff out and implement eat_token
	if ( rand()%10 < 8 ) bogo_token = T_plus;
	else bogo_token = T_eof;
*/

	//We just ate the token, so we have to call next_token() again
	//to know what's next. Thus, we set next_token_type to NULL (it
	//will be set to the next token type in the next call of
	//will be set to the next token type in the next call of
	//next_token() )
	delete next_token_type;
	next_token_type = NULL;
}

scanner_t::scanner_t()
{
	//WRITEME
	line = 1;
	num = -1;
	next_token_type = NULL;
	use_last_char = false;
}

int scanner_t::get_line()
{
	//WRITEME
	return line;
}

int scanner_t::get_num()
{
	temp = num;
	num = -1;
	return temp;
}

int scanner_t::read_char()
{
	char c;
	c = getchar();
	if ( c == '\n') line++;
	return c;
}

bool scanner_t::out_of_range(char* number_string)
{
	char num_string[MAX_DIGITS_IN_INT+1];
	strncpy(num_string, number_string, MAX_DIGITS_IN_INT+1);
	unsigned int num=0;
	int index;
	for(int i=0; i< MAX_DIGITS_IN_INT; i++)
	{
		index = MAX_DIGITS_IN_INT-i-1;
		num += pow(10,i)*atoi(&num_string[index]);
		num_string[index]='\0';
	}
	//printf("\tnum_string is: %s\n", number_string);
	//printf("\tnum is: %u\n", num);
	if(num > 2147483647) // this is 2^31 - 1
		return true; // num is out_of_range;
	//else, not out_of_range
	return false;
}

void scanner_t::scan_error (char x)
{
	printf("scan error: unrecognized character '%c' -line %d\n",x, get_line());
	exit(1);
}

void scanner_t::mismatch_error (token_type x)
{
	printf("syntax error: found %s ",token_to_string(next_token()) );
	printf("expecting %s - line %d\n", token_to_string(x), get_line());
	exit(1);
}


/*** ParseTree Class **********************************************/

//just dumps out the tree as a dot file.  The interface is described below
//on the actual methods.  This class is full and complete.  You should not
//have to touch a thing if everything goes according to plan.  While you don't
//have to modify it, you will have to call it from your recursive decent
//parser, so read about the interface below.
class parsetree_t {
  public:
	void push(token_type t);
	void push(nonterm_type nt);
	void pop();
	void drawepsilon();
	parsetree_t();
  private:
	enum stype_t{
		TERMINAL=1,
		NONTERMINAL=0,
		UNDEF=-1
	};

	struct stuple { 
		nonterm_type nt;
		token_type t;
		stype_t stype; 
		int uniq;
	};
	void printedge(stuple temp); //prints edge from TOS->temp
	stack<stuple> stuple_stack;
	char* stuple_to_string(const stuple& s); 
	int counter;
};


//the constructer just starts by initializing a counter (used to uniquely
//name all the parse tree nodes) and by printing out the necessary dot commands
parsetree_t::parsetree_t()
{
	counter = 0;
	printf("digraph G { page=\"8.5,11\"; size=\"7.5, 10\"\n");
}

//This push function takes a non terminal and keeps it on the parsetree
//stack.  The stack keeps trace of where we are in the parse tree as
//we walk it in a depth first way.  You should call push when you start
//expanding a symbol, and call pop when you are done.  The parsetree_t
//will keep track of everything, and draw the parse tree as you go.
//This particular function should be called if you are pushing a non-terminal
void parsetree_t::push(nonterm_type nt)
{
	counter ++;
	stuple temp;
	temp.nt = nt;
	temp.stype = NONTERMINAL;
	temp.uniq = counter;
	printedge( temp );
	stuple_stack.push( temp );
}

//same as above, but for terminals
void parsetree_t::push(token_type t)
{
	counter ++;
	stuple temp;
	temp.t = t;
	temp.stype = TERMINAL;
	temp.uniq = counter;
	printedge( temp );
	stuple_stack.push( temp );
}

//when you are done parsing a symbol, pop it.  That way the parsetree_t will
//know that you are now working on a higher part of the tree.
void parsetree_t::pop()
{
	if ( !stuple_stack.empty() ) {
		stuple_stack.pop();
	}

	if ( stuple_stack.empty() ) {
		printf( "};\n" );
	}
}

//draw an epsilon on the parse tree hanging off of the top of stack
void parsetree_t::drawepsilon()
{
	push(epsilon);
	pop();
}

// this private print function is called from push.  Basically it
// just prints out the command to draw an edge from the top of the stack (TOS)
// to the new symbol that was just pushed.  If it happens to be a terminal
// then it makes it a snazzy blue color so you can see your program on the leaves 
void parsetree_t::printedge(stuple temp)
{
	if ( temp.stype == TERMINAL ) {
		printf("\t\"%s%d\" [label=\"%s\",style=filled,fillcolor=powderblue]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	} else {
		printf("\t\"%s%d\" [label=\"%s\"]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	}

	//no edge to print if this is the first node
	if ( !stuple_stack.empty() ) {
		//print the edge
		printf( "\t\"%s%d\" ", stuple_to_string(stuple_stack.top()), stuple_stack.top().uniq ); 
		printf( "-> \"%s%d\"\n", stuple_to_string(temp), temp.uniq );
	}
}

//just a private utility for helping with the printing of the dot stuff
char* parsetree_t::stuple_to_string(const stuple& s) 
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	if ( s.stype == TERMINAL ) {
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", token_to_string(s.t) );
	} else if ( s.stype == NONTERMINAL ) {
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", nonterm_to_string(s.nt) );
	} else {
		assert(0);
	}

	return buffer;
}


/*** Parser Class ***********************************************/

//the parser_t class handles everything.  It has an instance of scanner_t
//so it can peek at tokens and eat them up.  It also has access to the
//parsetree_t class so it can print out the parse tree as it figures it out.
//To make the recursive decent parser work, you will have to add some
//methods to this class.  The helper functions are described below

class parser_t {
  private:
	scanner_t scanner;
	parsetree_t parsetree;
	void eat_token(token_type t);
	void syntax_error(nonterm_type);

	void List();
	//WRITEME: fill this out with the rest of the 
	//recursive decent stuff (more methods)
	void ListPrime();
	int Cmd();
	int Args();
	int Expr();
	int ExprPrime();
	int Term();
	int TermPrime();
	int Factor();

  public:	
	void parse();
};


//this function not only eats the token (moving the scanner forward one
//token), it also makes sure that token is drawn in the parse tree 
//properly by calling push and pop.
void parser_t::eat_token(token_type t)
{
	parsetree.push(t);
	scanner.eat_token(t);
	parsetree.pop();
}

//call this syntax error when you are trying to parse the
//non-terminal nt, but you fail to find a token that you need
//to make progress.  You should call this as soon as you can know
//there is a syntax_error. 
void parser_t::syntax_error(nonterm_type nt)
{
	printf("syntax error: found %s in parsing %s - line %d\n",
		token_to_string( scanner.next_token()),
		nonterm_to_string(nt),
		scanner.get_line() ); 
	exit(1);
}


//One the recursive decent parser is set up, you simply call parse()
//to parse the entire input, all of which can be dirived from the start
//symbol
void parser_t::parse()
{
	List();
}


//WRITEME: the List() function is not quite right.  Right now
//it is made to parse the grammar:  List -> '+' List | EOF
//which is not a very interesting language.  It has been included
//so you can see the basics of how to structure your recursive 
//decent code.

//Here is an example
void parser_t::List()
{
	//push this non-terminal onto the parse tree.
	//the parsetree class is just for drawing the finished
	//parse tree, and should in should have no effect the actual
	//parsing of the data
	parsetree.push(NT_List);
	Cmd();
	eat_token(T_period);
	ListPrime();

/*
	switch( scanner.next_token() ) 
	{
		case T_num:
			eat_token(T_num);
			List();
			break;
		case T_eof:
			parsetree.drawepsilon();
			break;
		default:
			syntax_error(NT_List);
			break;
	}
*/

	//now that we are done with List, we can pop it from the data
	//stucture that is tracking it for drawing the parse tree
	parsetree.pop();
}

//WRITEME: you will need to put the rest of the procedures here
void parser_t::ListPrime()
{
	parsetree.push(NT_ListPrime);
	switch ( scanner.next_token() )
	{
		case T_openparen:
		case T_m:
		case T_num:
			Cmd();
			eat_token(T_period);
			ListPrime();
			break;
		case T_eof:
			parsetree.drawepsilon();
			break;
		default:
			syntax_error(NT_ListPrime);
			break;
	}
	parsetree.pop();
}

void parser_t::Cmd()
{
	parsetree.push(NT_Cmd);
	Expr();
	Args();
	parsetree.pop();
}

void parser_t::Args()
{
	parsetree.push(NT_Args);
	switch ( scanner.next_token() )
	{
		case T_store:
			eat_token(T_store);
			eat_token(T_m);
			eat_token(T_openbracket);
			Expr();
			eat_token(T_closebracket);
			break;
		case T_period:
			parsetree.drawepsilon();
			break;
		default:
			syntax_error(NT_Args);
			break;
	}
	parsetree.pop();
}

void parser_t::Expr()
{
	parsetree.push(NT_Expr);
	switch ( scanner.next_token() )
	{
		case T_openparen:
		case T_m:
		case T_num:
			Term();
			ExprPrime();
			break;
		default:
			syntax_error(NT_Expr);
			break;
	}
	parsetree.pop();
}

void parser_t::ExprPrime()
{
	parsetree.push(NT_ExprPrime);
	switch ( scanner.next_token() )
	{
		case T_plus:
			eat_token(T_plus);
			Term();
			ExprPrime();
			break;
		case T_minus:
			eat_token(T_minus);
			Term();
			ExprPrime();
			break;
		case T_period:
		case T_store:
		case T_closeparen:
		case T_closebracket:
			parsetree.drawepsilon();
			break;
		default:
			syntax_error(NT_ExprPrime);
			break;
	}
	parsetree.pop();
}

void parser_t::Term()
{
	parsetree.push(NT_Term);
	Factor();
	TermPrime();
	parsetree.pop();
}

void parser_t::TermPrime()
{
	parsetree.push(NT_TermPrime);
	switch ( scanner.next_token() )
	{
		case T_times:
			eat_token(T_times);
			Factor();
			TermPrime();
			break;
		case T_div:
			eat_token(T_div);
			Factor();
			TermPrime();
			break;
		case T_plus:
		case T_minus:
		case T_period:
		case T_store:
		case T_closeparen:
		case T_closebracket:
			parsetree.drawepsilon();
			break;
		default:
			syntax_error(NT_TermPrime);
			break;
	}
	parsetree.pop();
}

void parser_t::Factor()
{
	int num;
	parsetree.push(NT_Factor);
	switch ( scanner.next_token() )
	{
		case T_openparen:
			eat_token(T_openparen);
			Expr();
			eat_token(T_closeparen);
			break;
		case T_m:
			eat_token(T_m);
			eat_token(T_openbracket);
			Expr();
			eat_token(T_closebracket);
			break;
		case T_num:
			num = scanner.get_num();
			if(num<0)
			{
				printf("num was negative, something went terribly wrong!");
				exit(1);
			}
			eat_token(T_num);
			break;
		default:
			syntax_error(NT_Factor);
			break;
	}
	parsetree.pop();
}

/*** Main ***********************************************/

int main()
{
	parser_t parser;
	parser.parse();

/*
	printf("***hello. starting up\n\n");
	scanner_t scanner;

	printf("testing out_of_range\n");
	char in_range[MAX_DIGITS_IN_INT+1]="1234567891";
	char out_of_range[MAX_DIGITS_IN_INT+1]="4123456789";
	char just_below[MAX_DIGITS_IN_INT+1]="2147483647";
	char just_above[MAX_DIGITS_IN_INT+1]="2147483648";
	printf("expect false\n");
	if (scanner.out_of_range(in_range))
		printf("result: true\n");
	else
		printf("result: false\n");

	printf("expect true\n");
	if (scanner.out_of_range(out_of_range))
		printf("result: true\n");
	else
		printf("result: false\n");

	printf("expect false\n");
	if (scanner.out_of_range(just_below))
		printf("result: true\n");
	else
		printf("result: false\n");

	printf("expect true\n");
	if (scanner.out_of_range(just_above))
		printf("result: true\n");
	else
		printf("result: false\n");

	token_type next_token = scanner.next_token();
	int i = 1;
	int line = scanner.get_line();
	while(next_token != T_eof)
	{
		printf("line %i: ", line);
		printf("token #%i is: %s", i, token_to_string(next_token));
		if(next_token == T_num) printf("<%i>", scanner.get_num());
		printf("\n");
		scanner.eat_token(next_token);
		next_token = scanner.next_token();
		i++;
		line = scanner.get_line();
	}	
	printf("token #%i is: %s", i, token_to_string(next_token));

	printf("\n\n***goodbye. shutting down\n\n");
*/

	return 0;
}
