//Richard Macklin, CS160 F12, Professor Sherwood
//Project 0, 10/17/12

//The goals of this project are to learn how to write a simple scanner and
//parser from scratch, to dispell the magic around parsing and scanning, and
//to develop a simple functioning calculator that works with numbers that
//have some uncertainty in the values (adding "memory" to the calculator).

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stack>
#include <queue>
#include <vector>

using namespace std;


/*** Enums and Print Functions for Terminals and Non-Terminals ****************/

#define MAX_SYMBOL_NAME_SIZE 25
#define MAX_DIGITS_IN_INT 10 // a 32-bit int has at most 10 digits

//All of the terminals in the language
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

//This function returns a string for the token. It is used in the parsetree_t
//class to actually dump the parsetree to a dot file (which can then be turned
//into a picture). Note that the return char* is a reference to a local copy
//and it needs to be duplicated if you are a going require multiple instances
//simultaneously
char* token_to_string(token_type t)
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch(t)
	{
	case T_eof:
		strncpy(buffer,"eof",MAX_SYMBOL_NAME_SIZE); break;
	case T_num:
		strncpy(buffer,"num",MAX_SYMBOL_NAME_SIZE); break;
	case T_plus:
		strncpy(buffer,"+",MAX_SYMBOL_NAME_SIZE); break;
	case T_minus:
		strncpy(buffer,"-",MAX_SYMBOL_NAME_SIZE); break;
	case T_times:
		strncpy(buffer,"*",MAX_SYMBOL_NAME_SIZE); break;
	case T_period:
		strncpy(buffer,".",MAX_SYMBOL_NAME_SIZE); break;
	case T_store:
		strncpy(buffer,"->",MAX_SYMBOL_NAME_SIZE); break;
	case T_openparen:
		strncpy(buffer,"(",MAX_SYMBOL_NAME_SIZE); break;
	case T_closeparen:
		strncpy(buffer,")",MAX_SYMBOL_NAME_SIZE); break;
	case T_div:
		strncpy(buffer,"/",MAX_SYMBOL_NAME_SIZE); break;
	case T_m:
		strncpy(buffer,"m",MAX_SYMBOL_NAME_SIZE); break;
	case T_openbracket:
		strncpy(buffer,"[",MAX_SYMBOL_NAME_SIZE); break;
	case T_closebracket:
		strncpy(buffer,"]",MAX_SYMBOL_NAME_SIZE); break;
	default:
		strncpy(buffer,"unknown_token",MAX_SYMBOL_NAME_SIZE); break;
	}
	return buffer;
}

//All of the non-terminals in the grammar
typedef enum {
	epsilon = 100,
	NT_List,
	NT_Expr,
	NT_ListPrime,
	NT_Cmd,
	NT_Args,
	NT_ExprPrime,
	NT_Term,
	NT_TermPrime,
	NT_Factor
} nonterm_type;

//This function returns a string for the non-terminals. It is used in the
//parsetree_t class to actually dump the parsetree to a dot file (which can
//then be turned into a picture). Note that the return char* is a reference
//to a local copy and it needs to be duplicated if you are a going require
//multiple instances simultaniously. 
char* nonterm_to_string(nonterm_type nt)
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch(nt)
	{
	case epsilon:
		strncpy(buffer,"e",MAX_SYMBOL_NAME_SIZE); break;
	case NT_List:
		strncpy(buffer,"List",MAX_SYMBOL_NAME_SIZE); break;
	case NT_Expr:
		strncpy(buffer,"Expr",MAX_SYMBOL_NAME_SIZE); break;
	case NT_ListPrime:
		strncpy(buffer,"List'",MAX_SYMBOL_NAME_SIZE); break;
	case NT_Cmd:
		strncpy(buffer,"Cmd",MAX_SYMBOL_NAME_SIZE); break;
	case NT_Args:
		strncpy(buffer,"Args",MAX_SYMBOL_NAME_SIZE); break;
	case NT_ExprPrime:
		strncpy(buffer,"Expr'",MAX_SYMBOL_NAME_SIZE); break;
	case NT_Term:
		strncpy(buffer,"Term",MAX_SYMBOL_NAME_SIZE); break;
	case NT_TermPrime:
		strncpy(buffer,"Term'",MAX_SYMBOL_NAME_SIZE); break;
	case NT_Factor:
		strncpy(buffer,"Factor",MAX_SYMBOL_NAME_SIZE); break;
	default:
		strncpy(buffer,"unknown_nonterm",MAX_SYMBOL_NAME_SIZE); break;
	}
	return buffer;
}


/*** Scanner Class ************************************************************/

class scanner_t
{
  public:

	//Eats the next token and prints an error if it is not of type c
	void eat_token(token_type c);

	//Peeks at the lookahead token
	token_type next_token();

	//Return line number for errors
	int get_line();

	//Returns the last number read (to be used after eating a T_num in
	//order to get its value). It will be negative if a new number has not
	//been read since our numbers must be positive
	int get_num();

	//Constructor 
	scanner_t();

  private:

	int line;
	int last_char_read;
	bool use_last_char;
	token_type* next_token_type;
	int num;

	//Reads in the next character using getchar and keeps track of the
	//number of newlines that have been read
	int read_char();

	//Helper function that takes a string representation of a 10-digit
	//integer and returns true if the integer is greater than 2^31 -1
	//(10 is the maximum number of digits in a 32-bit integer; for all
	//other lengths, we *automatically* know whether or not the integer
	//is less than 2^31 - 1, so we wouldn't need to call this)
	bool out_of_range(char* num_string);

	//Error message and exit for unrecognized character
	void scan_error(char c);
	//Error message and exit for token mismatch
	void mismatch_error(token_type t);
	//Error message and ext for out of range number
	void range_error(char* num_string);
};

token_type scanner_t::next_token()
{
	//If we already looked at the next token, just return the same thing
	if( next_token_type != NULL ) return *next_token_type;

	//If we are here, we haven't looked at the next token yet, so we'll
	//read characters until we know what the token is

	next_token_type = new token_type();
	char c = (use_last_char ? last_char_read : read_char());
	while( isspace(c) ) { c = read_char(); }
	use_last_char = false;

	//c should now be the first non-whitespace character of the next token
	switch(c)
	{
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
	case '-': //The token could be - or -> so we need to keep reading
	    {
		char d = read_char();
		if(d == '>') *next_token_type = T_store;
		else
		{
			*next_token_type = T_minus;
			//we looked at an extra character that wasn't part of
			//this token, so we should consider it as the first
			//character of the next token
			last_char_read = d;
			use_last_char = true;
		}
		break;
	    }
	case '0': //We accept leading 0s in numbers
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    {
		*next_token_type = T_num;
		char num_string[MAX_DIGITS_IN_INT+1];
		int i = 0;

		//Keep checking next char until it's not a digit
		do
		{
			strncpy(num_string+i, &c, 1);
			c = read_char();
			i++;
		} while( c >= '0' && c <= '9' && i <= MAX_DIGITS_IN_INT );

		if( (i > MAX_DIGITS_IN_INT) ||
		    (i == MAX_DIGITS_IN_INT && out_of_range(num_string)) )
		{
			num_string[i]='\0';
			range_error(num_string);
		}

		num = atoi(num_string);
		//By the nature of the while loop, we read an extra character
		//to make sure it wasn't another digit (which would've still
		//been part of this number token), so we should use that
		//character as the first character of the next token
		last_char_read = c;
		use_last_char = true;
		break;
	    }
	default : scan_error(c);
	}

	return *next_token_type;
}

void scanner_t::eat_token(token_type c)
{
	//If we are supposed to eat token c, and it does not match what we
	//are supposed to be reading from file, then it is a mismatch error
	if( c != next_token() ) mismatch_error(c);

	//We're eating the token, so we'll have to call next_token() again to
	//know what's next. Thus, we set next_token_type to NULL here (it will
	//be set to the next token type in the subsequent next_token() call)
	delete next_token_type;
	next_token_type = NULL;
}

scanner_t::scanner_t()
{
	line = 1;
	num = -1;
	next_token_type = NULL;
	use_last_char = false;
}

int scanner_t::get_line()
{
	return line;
}

int scanner_t::get_num()
{
	int temp = num;
	num = -1;
	return temp;
}

int scanner_t::read_char()
{
	char c;
	c = getchar();
	if( c == '\n' ) line++;
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
	//Return whether or not this number is out of range (i.e. > 2^31 - 1)
	return ( num > 2147483647);
}

void scanner_t::scan_error(char c)
{
	printf("scan error: unrecognized character '%c' - line %d\n", c, get_line());
	exit(1);
}

void scanner_t::mismatch_error(token_type t)
{
	printf("syntax error: found %s ",token_to_string(next_token()) );
	printf("expecting %s - line %d\n", token_to_string(t), get_line());
	exit(1);
}

void scanner_t::range_error(char* num_string)
{
	printf("range error: number starting with '%s' ", num_string);
	printf("is greater than (2^31 - 1) - line %d\n", get_line());
	exit(1);
}


/*** ParseTree Class **********************************************************/

//Just dumps out the tree as a dot file. The interface is described below
//on the actual methods.
class parsetree_t
{
  public:
	void push(token_type t);
	void push(nonterm_type nt);
	void pop();
	void drawepsilon();
	parsetree_t();

  private:
	enum stype_t {
		TERMINAL = 1,
		NONTERMINAL = 0,
		UNDEF = -1
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


//The constructer just starts by initializing a counter (used to uniquely name
//all the parse tree nodes) and by printing out the necessary dot commands
parsetree_t::parsetree_t()
{
	counter = 0;
	printf("digraph G { page=\"8.5,11\"; size=\"7.5, 10\"\n");
}

//This push function takes a non-terminal and keeps it on the parsetree stack.
//The stack keeps track of where we are in the parse tree as we walk it in a
//depth first way. You should call push when you start expanding a symbol,
//and call pop when you are done expanding it. The parsetree_t class will
//keep track of everything, and draw the parse tree as you go.
//This particular function should be called if you are pushing a non-terminal
void parsetree_t::push(nonterm_type nt)
{
	counter++;
	stuple temp;
	temp.nt = nt;
	temp.stype = NONTERMINAL;
	temp.uniq = counter;
	printedge(temp);
	stuple_stack.push(temp);
}

//Same as above, but for terminals
void parsetree_t::push(token_type t)
{
	counter++;
	stuple temp;
	temp.t = t;
	temp.stype = TERMINAL;
	temp.uniq = counter;
	printedge(temp);
	stuple_stack.push(temp);
}

//When you are done parsing a symbol, pop it. That way the parsetree_t will
//know that you are now working on a higher part of the tree.
void parsetree_t::pop()
{
	if( !stuple_stack.empty() ) stuple_stack.pop();

	if( stuple_stack.empty() ) printf( "};\n" );
}

//Draw an epsilon on the parse tree hanging off of the top of stack
void parsetree_t::drawepsilon()
{
	push(epsilon);
	pop();
}

//This private print function is called from push.  Basically it just prints
//out the command to draw an edge from the top of the stack (TOS) to the new
//symbol that was just pushed. If it happens to be a terminal then it makes
//it a snazzy blue color so you can see your program on the leaves
void parsetree_t::printedge(stuple temp)
{
	if( temp.stype == TERMINAL )
	{
		printf("\t\"%s%d\" [label=\"%s\",style=filled,fillcolor=powderblue]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	}
	else
	{
		printf("\t\"%s%d\" [label=\"%s\"]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	}

	//No edge to print if this is the first node
	if( !stuple_stack.empty() )
	{
		//Print the edge
		printf( "\t\"%s%d\" ", stuple_to_string(stuple_stack.top()), stuple_stack.top().uniq );
		printf( "-> \"%s%d\"\n", stuple_to_string(temp), temp.uniq );
	}
}

//Just a private utility for helping with the printing of the dot stuff
char* parsetree_t::stuple_to_string(const stuple& s) 
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	if( s.stype == TERMINAL )
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", token_to_string(s.t) );
	else
	{
		if( s.stype == NONTERMINAL )
			snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", nonterm_to_string(s.nt) );
		else
			assert(0);
	}

	return buffer;
}


/*** Parser Class *************************************************************/

//The parser_t class handles everything. It has an instance of scanner_t so
//it can peek at tokens and eat them up. It also has access to the
//parsetree_t class so it can print out the parse tree as it figures it out.
class parser_t
{
  private:
	scanner_t scanner;
	parsetree_t parsetree;

	//Struct to store operators and operands that will help with
	//evaluating expressions
	struct calc_token {
		token_type t;
		int num_val;
		calc_token() { num_val = -2; }
	};

    //Useful data structures for helping with expression evaluations:
	//The operator stack stores all operators as they are read
	stack<token_type> operator_stack;
	//The top of the "bottom of stack ptr" stack holds an integer which
	//corresponds to what is considered the bottom of the operator stack
	//to the expression we are currently evaluating. It is needed for
	//when we have nested expressions (within parentheses or brackets)
	//to keep track of which operators on the operator stack belong to
	//the current nested expression being evaluated
	stack<int> bottom_of_stack_ptr;
	//An "output queue" is used to store the postfix notation version of
	//an expression, since this notation removes all ambiguity in terms
	//of operator precedence and associativity
	//We keep a stack of these "output queues" because there must be one
	//output queue for each nested expression
	stack< queue<calc_token> > stack_of_output_queues;

	//Our memory is stored as a vector since it can grow to arbitrary
	//length. All memory values are initially zero
	vector<int> memory;

	void eat_token(token_type t);

	//Memory access methods
	int read_mem(int location);
	void store_to_mem(int value, int location);

	//Helper functions to deal with expression evaluation
	void stack_queue_manage(token_type tok, int num = -3);
	bool is_operator(token_type t);
	int op_precedence(token_type op);
	int calculate(int o1, token_type op, int o2);

	//Error methods
	void syntax_error(nonterm_type);
	void negative_index(int index);
	void negative_num_error(int num);

	//Recursive decent parsing methods
	void List();
	void ListPrime();
	int Cmd();
	int Args();
	int Expr();
	void ExprPrime();
	void Term();
	void TermPrime();
	void Factor();

  public:
	parser_t();	
	void parse();
};

parser_t::parser_t()
{
	//Initially the bottom of the operator stack is position 0 since
	//we are evaluating the outermost (in terms of nesting) expression
	bottom_of_stack_ptr.push(0);
	//Initialize the base-level output queue (to be used in evaluating
	//the outermost expression)
	queue<calc_token> base_output_queue;
	stack_of_output_queues.push(base_output_queue);
}

//This function not only eats the token (moving the scanner forward one
//token), but it also makes sure that token is drawn in the parse tree
//properly by calling push and pop.
void parser_t::eat_token(token_type t)
{
	parsetree.push(t);
	scanner.eat_token(t);
	parsetree.pop();
}

//Get the value from memory[location]
int parser_t::read_mem(int location)
{
	if( location >= memory.size() ) memory.resize(location+1, 0);
	return memory[location];
}

//Store value to memory[location]
//The arguments are in this order (rather than in the order location, value)
//because that is the order in which they appear in the language we are
//parsing, e.g. 3*(4 - 2) -> m[3+4/3].
void parser_t::store_to_mem(int value, int location)
{
	if( location >= memory.size() ) memory.resize(location+1, 0);
	memory[location] = value;
}

//This function is responsible for managing the data structures that help
//evaluate expressions, namely the operator stack, and the top output queue
//on the stack of output queues.
//To do so, it implements a shunting yard algorithm* to evaluate expressions
//by converting them into postfix notation, which is unambiguous in terms of
//operator precedence and associativity.
//*See: http://en.wikipedia.org/wiki/Shunting_yard_algorithm
void parser_t::stack_queue_manage(token_type tok, int num)
{
	calc_token ct;
	ct.t = tok;
	token_type tos;
	if( is_operator(tok) )
	{
		//Manage the operator stack. Essentially this part compares
		//operator precedence to make sure the final postfix notation
		//form has the operators in the correct order. It makes use
		//of bottom_of_stack_ptr.top() to know when it has managed
		//all of the operators in the current subexpression (if it
		//is part of a nested expression)
		while( operator_stack.size() > bottom_of_stack_ptr.top() )
		{
			tos = operator_stack.top(); //tos = top of stack
			if( op_precedence(tok) <= op_precedence(tos) )
			{
				//Pop the operator from the operator stack
				operator_stack.pop();
				ct.t = tos;
				ct.num_val = -4;
				//Push the operator onto the current output queue
				stack_of_output_queues.top().push(ct);
			}
			else break; //Don't pop the rest of the operators on
				    //the stack if tok has a higher precedence
				    //than tos
		}
		operator_stack.push(tok);
	}
	else
	{
		if( tok == T_num )
		{
			//Manage putting numbers in the output queue (operators
			//were taken care of already)
			ct.num_val = num;
			stack_of_output_queues.top().push(ct);
		}
		else
		{
			fprintf(stderr, "why are you calling stack_queue_manage() and passing in an irrelevant token?!?!\n");
			exit(1);
		}
	}
}

//Returns whether or not t is an operator
bool parser_t::is_operator(token_type t)
{
	return ( t == T_plus || t == T_minus || t == T_times || t == T_div );
}

//Returns the precedence of the given operator
int parser_t::op_precedence(token_type op)
{
	switch(op)
	{
	case T_times:
	case T_div:
		return 2; //Highest precedence
	case T_plus:
	case T_minus:
		return 1; //Lowest precedence
	default: //Should never execute since only operators are passed to
		return 0; //this function
	}
}

//Returns the value of o1 (op) o2
int parser_t::calculate(int o1, token_type op, int o2)
{
	switch(op)
	{
	case T_times:
		return o1 * o2;
	case T_div:
		if( o2 == 0 )
		{
			printf("division by zero error - line %i\n", scanner.get_line());
			exit(1);
		}
		return o1 / o2; //Integer division
	case T_plus:
		return o1 + o2;
	case T_minus:
		return o1 - o2;
	default:
		printf("you passed an unknown operator...aborting!\n");
		exit(1);
	}
}

//Call this syntax error when you are trying to parse the non-terminal nt,
//but you fail to find a token that you need to make progress. You should
//call this as soon as you can know there is a syntax_error.
void parser_t::syntax_error(nonterm_type nt)
{
	printf("syntax error: found %s in parsing %s - line %d\n",
		token_to_string( scanner.next_token()),
		nonterm_to_string(nt),
		scanner.get_line() );
	exit(1);
}

//Call this error if the index to memory is negative
void parser_t::negative_index(int index)
{
	printf("error: memory index has negative value: %i", index);
	printf(" - line %i\n", scanner.get_line());
	exit(1);
}

//Call this error if the value returned by scanner.get_num() was negative
void parser_t::negative_num_error(int num)
{
	printf("Something went terribly wrong! The scanner returned a number");
	printf(" token with a negative value (%i)", num); 
	printf(" - line %i\n", scanner.get_line());
	exit(1);
}

//Once the recursive decent parser is set up, you simply call parse() to
//parse the entire input, all of which can be derived from the start symbol
void parser_t::parse()
{
	List();
}

//The first recursive decent method. This is the start symbol of our
//grammar, and it only has one production, so we don't even need to peek
//at the lookahead token (but otherwise we would need to).
void parser_t::List()
{
	//Push this non-terminal onto the parse tree. The parsetree class is
	//just for drawing the finished parse tree, and should in should have
	//no effect on the actual parsing of the data
	parsetree.push(NT_List);

	int evaluated_result = Cmd();
	fprintf(stderr, "%i\n", evaluated_result);
	eat_token(T_period);
	ListPrime();
	eat_token(T_eof);

	//Now that we are done with List, we can pop it from the data
	//stucture that is tracking it for drawing the parse tree
	parsetree.pop();
}

//The rest of the recursive decent methods are similar. We simply try to
//match the non-terminal that we are calling by following our grammar
//(which you can find in calc_def.y) and peeking at the lookahead token.
//We use first and follow sets to determine if each nonterminal has been
//matched. The first set of this non-terminal is composed of all tokens in
//this method that come before the tokens that lead to a drawepsilon() or
//syntax_error() call. If an element of the first set is matched, we simply
//follow the corresponding production in the grammar. The follow set of
//this non-terminal is composed of the tokens that lead to a drawepsilon()
//call. Any other tokens are necessarily a syntax error.
void parser_t::ListPrime()
{
	parsetree.push(NT_ListPrime);
	int evaluated_result;

	switch( scanner.next_token() )
	{
	case T_openparen:
	case T_m:
	case T_num:
		evaluated_result = Cmd();
		fprintf(stderr, "%i\n", evaluated_result);
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

int parser_t::Cmd()
{
	parsetree.push(NT_Cmd);

	int res = Expr();
	int location = Args();
	if( location != - 666 ) //Then we encountered a store operator
	{
		if( location < 0 ) negative_index(location);
		//res -> m[location]
		store_to_mem(res, location);
	}

	parsetree.pop();
	return res;
}

//Returns the location of memory we are going to store to, or -666 if we don't
//encounter a store operator next (i.e. we take the epsilon production instead)
int parser_t::Args()
{
	int location = -666;
	parsetree.push(NT_Args);

	switch( scanner.next_token() )
	{
	case T_store:
		eat_token(T_store);
		eat_token(T_m);
		eat_token(T_openbracket);
		location = Expr();
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
	return location;
}

int parser_t::Expr()
{
	parsetree.push(NT_Expr);

	switch( scanner.next_token() )
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

	//At this point the recursive calls should have put the relevant tokens
	//in our operator_stack and output_queue that are needed to evaluate
	//this particular expression. So we can go ahead and evaluate it.
	//(As a reminder, this could be a nested expression, so we consider the
	//bottom of the operator stack to be the value of
	//bottom_of_stack_ptr.top() and we use the output queue on the top of
	//our stack of output queues)
	while( operator_stack.size() > bottom_of_stack_ptr.top() )
	{
		calc_token ct;
		ct.t = operator_stack.top();
		stack_of_output_queues.top().push(ct);
		operator_stack.pop();
	}
	
	//Everything is now in the output_queue in postfix notation, so we can
	//calculate it and the associativity and precedence should be correct!
	//(Our grammar actually produces the correct precedence, but because we
	//had to transform it to be LL(1) in order to implement a recursive
	//decent parser, we inadvertently lost the left-associativity of our
	//operators since the grammar became right recursive in the process)

	//The following chunk of code is what actually takes the postfix
	//notation expression stored in stack_of_output_queues.top() (which
	//is of course an output queue) and calculates it. The way it does
	//so is pretty straightforward (since postfix notation is unambiguous):

	//It reads the output queue from front to back. If it reads a number,
	//it places the number on an "operand stack". If it reads an operator,
	//it takes the two numbers from the top of the operand stack, computes
	//the value o1 (op) o2, and pushes the result back onto the operand
	//stack. After everything has been read from the output queue, only
	//one item will remain on the operand stack, the overall evaluation
	//of this expression, which is what this function returns.
	  //(If that didn't make complete sense, this illustrative example may
	  //help: http://www.brpreiss.com/books/opus4/html/page145.html )
	int o1, o2; //operands
	token_type op; //operator
	stack<int> operand_stack;
	calc_token front;
	while( !stack_of_output_queues.top().empty() )
	{
		front = stack_of_output_queues.top().front();
		if( front.t == T_num )
		{
			operand_stack.push(front.num_val);
		}
		if( is_operator(front.t) )
		{
			if( operand_stack.size() < 2 )
			{
				//Our operators are just binary arithmetic
				//operators so they require 2 operands
				fprintf(stderr, "encountered an operator without 2 operands. ABORT!!!\n");
				exit(1);
			}
			o2 = operand_stack.top();
			operand_stack.pop();
			o1 = operand_stack.top();
			operand_stack.pop();
			op = front.t;
			//evaluate o1 (op) o2
			int evaluation = calculate(o1, op, o2);
			operand_stack.push(evaluation);
		}
		stack_of_output_queues.top().pop();
	}
	//The result should be the only element on the stack, so return it
	int size = operand_stack.size();
	if( size != 1 )
	{
		fprintf(stderr, "operand stack has %i items when it should contain exactly one (the final result). ABORT!!!\n", size);
		exit(1);
	}

	parsetree.pop();
	int result = operand_stack.top();
	operand_stack.pop();
	return result;
}

//This method is the one that actually eats + and - tokens, so we make sure
//to call stack_queue_manage after one of those tokens is eaten so that we
//will be able to correctly evaluate the expression
void parser_t::ExprPrime()
{
	parsetree.push(NT_ExprPrime);

	switch( scanner.next_token() )
	{
	case T_plus:
		eat_token(T_plus);
		stack_queue_manage(T_plus);
		Term();
		ExprPrime();
		break;
	case T_minus:
		eat_token(T_minus);
		stack_queue_manage(T_minus);
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

//This method is the one that actually eats * and / tokens, so we make sure
//to call stack_queue_manage after one of those tokens is eaten so that we
//will be able to correctly evaluate the expression
void parser_t::TermPrime()
{
	parsetree.push(NT_TermPrime);

	switch( scanner.next_token() )
	{
	case T_times:
		eat_token(T_times);
		stack_queue_manage(T_times);
		Factor();
		TermPrime();
		break;
	case T_div:
		eat_token(T_div);
		stack_queue_manage(T_div);
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

//This last recursive decent function is the one that properly deals with
//nested subexpressions. If we encounter an open parenthesis/bracket then
//we are starting a new nested subexpression. Thus, have to create a new
//output queue and push it onto our stack of output queues. In addition, we
//have to update what is considered to be the bottom of the operator stack
//for this subexpression (by pushing the current size of operator_stack onto
//bottom_of_stack_ptr). Upon encountering the subsequent closed parentheses/
//bracket, we undo this by popping the top element from bottom_of_stack_ptr
//and stack_of_output_queues.
void parser_t::Factor()
{
	int num, location;
	queue<calc_token> new_output_queue;
	parsetree.push(NT_Factor);

	switch( scanner.next_token() )
	{
	case T_openparen:
		eat_token(T_openparen);
		bottom_of_stack_ptr.push( operator_stack.size() );
		stack_of_output_queues.push(new_output_queue);
		num = Expr();
		eat_token(T_closeparen);
		bottom_of_stack_ptr.pop();
		stack_of_output_queues.pop();
		break;
	case T_m:
		eat_token(T_m);
		eat_token(T_openbracket);
		bottom_of_stack_ptr.push( operator_stack.size() );
		stack_of_output_queues.push(new_output_queue);
		location = Expr();
		if( location < 0 ) negative_index(location);
		eat_token(T_closebracket);
		bottom_of_stack_ptr.pop();
		stack_of_output_queues.pop();
		num = read_mem(location);
		break;
	case T_num:
		num = scanner.get_num();
		if( num < 0 ) negative_num_error(num);
		eat_token(T_num);
		break;
	default:
		syntax_error(NT_Factor);
		break;
	}

	//Pass num, which was either a number token, or the evaluated result
	//of a nested subexpression, to stack_queue_manage
	stack_queue_manage(T_num, num);	
	parsetree.pop();
}

/*** Main *********************************************************************/

int main()
{
	parser_t parser;
	parser.parse();

	return 0;
}
