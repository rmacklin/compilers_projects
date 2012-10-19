//Richard Macklin, CS160 F12, Professor Sherwood
//Project 0, 10/4/12

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stack>
#include <math.h>
#include <queue>
#include <vector>

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

//same as above but only for printing number tokens with their value
char* token_to_string(token_type c, int num) {
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch( c ) {
		case T_num: strncpy(buffer,"<num, %i>",MAX_SYMBOL_NAME_SIZE); break;
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
	//Done
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
		//Done
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
	//Done
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
	//Done

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
		case '0': //Professor Sherwood said we can accept leading 0s
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
	//Done

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
	//Done
	line = 1;
	num = -1;
	next_token_type = NULL;
	use_last_char = false;
}

int scanner_t::get_line()
{
	//WRITEME
	//Done
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
	void push(token_type t, int num);
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
	void printedge(stuple temp, int num); //prints edge from TOS->temp with num value
	stack<stuple> stuple_stack;
	char* stuple_to_string(const stuple& s); 
	char* stuple_to_string(const stuple& s, int num); 
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

//same as above, but modified for number terminals to print their value
void parsetree_t::push(token_type t, int num)
{
	counter ++;
	stuple temp;
	if ( t != T_num )
	{
		printf("why did you call this you FOOL! this is only for printing nums!\n");
		assert(0);
	}
	temp.t = t;
	temp.stype = TERMINAL;
	temp.uniq = counter;
	printedge( temp, num );
	//stuple_stack.push( temp, num );
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

// this modified private print function is called from the modified push. Basically it
// just prints out the command to draw an edge from the top of the stack (TOS)
// to the new symbol that was just pushed.  If it happens to be a terminal
// then it makes it a snazzy blue color so you can see your program on the leaves. It
// should only be used to print the num terminal with its value (hence the int parameter)
void parsetree_t::printedge(stuple temp, int num)
{
	if ( temp.stype == TERMINAL && temp.t == T_num ) {
		printf("\t\"%s%d\" [label=\"%s\",style=filled,fillcolor=powderblue]\n",
		  stuple_to_string(temp, num),
		  temp.uniq,
		  stuple_to_string(temp, num));
	}
	else
	{
		printf("why did you call this you FOOL! this is only for printing nums!\n");
		assert(0);
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

//modified private utility for helping with the printing of nums in the dot stuff
char* parsetree_t::stuple_to_string(const stuple& s, int num) 
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	if ( s.stype == TERMINAL && s.t == T_num )
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "<%s, %i>", token_to_string(s.t), num );
	else
	{
		printf("why did you call this you FOOL! this is only for printing nums!\n");
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

	//struct to store operators and operands that will
	//help with evaluating expressions
	struct calc_token {
		token_type t;
		int num_val;
		calc_token() { num_val = -2; }
	};

	//useful data structures for helping with expression evaluations
	stack<token_type> operator_stack;
	queue<calc_token> output_queue;
	vector<int> memory;

	void eat_token(token_type t);
	void eat_token(token_type t, int num);

	int read_mem(int location);
	void store_to_mem(int value, int location);
	void stack_queue_manage(token_type tok, int num = -3);
	bool is_operator(token_type t);
	int op_precedence(token_type op);
	int calculate(int o1, token_type op, int o2);

	void syntax_error(nonterm_type);
	void negative_index(int index);

	void List();
	//WRITEME: fill this out with the rest of the 
	//recursive decent stuff (more methods)
	//Done
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
	//initially memory will have size 1024 with all its
	//values initialized to 0. It grows as needed if an
	//expression/command tries to access/store to an index
	//outside this range.
	memory.resize(1024,0);
}

//this function not only eats the token (moving the scanner forward one
//token), it also makes sure that token is drawn in the parse tree 
//properly by calling push and pop.
void parser_t::eat_token(token_type t)
{
	parsetree.push(t);
	scanner.eat_token(t);
	parsetree.pop();
}

//same as above, but modified so it prints the value of a num token in the tree
void parser_t::eat_token(token_type t, int num)
{
	if ( t != T_num )
	{
		printf("why did you call this you FOOL! this is only for printing nums!\n");
		assert(0);
	}
	parsetree.push(t, num);
	scanner.eat_token(t);
	parsetree.pop();
}

//get the value from memory[location]
int parser_t::read_mem(int location)
{
	if ( location >= memory.size() )
		memory.resize(location+1, 0);
	return memory[location];
}

//store value to memory[location]
//the arguments are in this order because that is how they are in the
//language we are parsing, e.g. 3 * 4 - 2 -> m[3+4/3].
void parser_t::store_to_mem(int value, int location)
{
	if ( location >= memory.size() )
		memory.resize(location+1, 0);
	memory[location] = value;
}

void parser_t::stack_queue_manage(token_type tok, int num)
{
	calc_token ct;
	ct.t = tok;
	token_type tos;
	//implements a kind of shunting yard algorithm to handle
	//the evaluation of operations
	if ( is_operator(tok) )
	{
		//manage the operator stack
		while ( !operator_stack.empty() )
		{
			tos = operator_stack.top();
			if ( op_precedence(tok) <= op_precedence(tos) )
			{
				operator_stack.pop();
				ct.t = tos;
				ct.num_val = -4;
				output_queue.push(ct);
			}
			else break; // don't pop the rest of the operators on the stack
			// if tok has a higher precedence than tos (top of stack)
		}
		operator_stack.push(tok);
	}
	else
	{
		if ( tok == T_num )
		{
			//manage the output queue
			//if ( num < 0 ) negative_error(num);
			ct.num_val = num;
			output_queue.push(ct);
		}
		else
		{
			printf("why are you calling this and passing in an irrelevant token?!?!\n");
			exit(1);
		}
	}
}

bool parser_t::is_operator(token_type t)
{
	return ( t == T_plus || t == T_minus || t == T_times || t == T_div );
}

int parser_t::op_precedence(token_type op)
{
	switch ( op )
	{
		case T_times:
		case T_div:
			return 2;
		case T_plus:
		case T_minus:
			return 1;
		default:
			printf("why are you calling this and not passing an operator?!?!?!\n");
			exit(1);
	}
}

int parser_t::calculate(int o1, token_type op, int o2)
{
	switch ( op )
	{
		case T_times:
			return o1 * o2;
		case T_div:
			if ( o2 == 0 )
			{
				printf("division by zero error - line %i\n", scanner.get_line());
				exit(1);
			}
			return o1 / o2; //integer division, as required by the project description
		case T_plus:
			return o1 + o2;
		case T_minus:
			return o1 - o2;
		default:
			return -5;
	}
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

//call this error if a number result is negative, since all results are supposed
//to be in the range 0 to 2^31 - 1.
void parser_t::negative_index(int index)
{
	if ( index < 0 )
	{
		printf("memory index has negative value: %i\n", index);
		exit(1);
	}
	printf("why the hell did you call negative_index, %i is not negative!\n", index);
	exit(1);
}

//Once the recursive decent parser is set up, you simply call parse()
//to parse the entire input, all of which can be derived from the start
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
//Done

//Here is an example
void parser_t::List()
{
	int evaluated_result;
	//push this non-terminal onto the parse tree.
	//the parsetree class is just for drawing the finished
	//parse tree, and should in should have no effect the actual
	//parsing of the data
	parsetree.push(NT_List);
	evaluated_result = Cmd();
	fprintf(stderr, "%i\n", evaluated_result);
	eat_token(T_period);
	ListPrime();
	eat_token(T_eof);

	//now that we are done with List, we can pop it from the data
	//stucture that is tracking it for drawing the parse tree
	parsetree.pop();
}

//WRITEME: you will need to put the rest of the procedures here
void parser_t::ListPrime()
{
	parsetree.push(NT_ListPrime);
	int evaluated_result;
	switch ( scanner.next_token() )
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
	if( location != - 666 )
	{
		if ( location < 0 ) negative_index(location);
		// res -> m[location]
		store_to_mem(res, location);
	}
	parsetree.pop();
	return res;
}

//returns the location of memory we are going to store to, or -666 if we don't
//encounter a store operator next (i.e. we take the epsilon production instead)
int parser_t::Args()
{
	int location = -666;
	parsetree.push(NT_Args);
	switch ( scanner.next_token() )
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

	//at this point the recursive calls should have put the relevant tokens
	//in our operator_stack and output_queue that are needed to evaluate
	//this particular expression. so go ahead start evaluating it:
	while ( !operator_stack.empty() )
	{
		calc_token ct;
		ct.t = operator_stack.top();
		output_queue.push(ct);
		operator_stack.pop();
	}
	
	//everything is now in the output_queue in postfix notation, so we can
	//calculate it and the associativity should be correct!
	int o1, o2; //operands
	token_type op; //operator
	stack<int> operand_stack;
	calc_token front;
	while ( !output_queue.empty() )
	{
		front = output_queue.front();
		if ( front.t == T_num )
		{
			//if ( front.num_val < 0 ) negative_error(front.num_val);
			operand_stack.push(front.num_val);
		}
		if ( is_operator(front.t) )
		{
			if ( operand_stack.size() < 2 )
			{
				printf("encountered an operator without 2 operands. ABORT!!!\n");
				exit(1);
			}
			o2 = operand_stack.top();
			operand_stack.pop();
			o1 = operand_stack.top();
			operand_stack.pop();
//			if ( o1 < 0 ) negative_error(o1);
//			if ( o2 < 0 ) negative_error(o2);
			op = front.t;
			//evaluate o1 (op) o2
			int evaluation = calculate(o1, op, o2);
//			if ( evaluation < 0) negative_error(evaluation);
			operand_stack.push(evaluation);
		}
		output_queue.pop();
	}
	//the result should be the only element on the stack, so return it
	int size = operand_stack.size();
	if ( size != 1 )
	{
		printf("operand stack has %i items when it should contain exactly one (the final result). ABORT!!!\n", size);
		exit(1);
	}

	parsetree.pop();
	int result = operand_stack.top();
	operand_stack.pop();
printf("expression evaluated to: %i\n", result);
	return result;
}

void parser_t::ExprPrime()
{
	parsetree.push(NT_ExprPrime);
	switch ( scanner.next_token() )
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

void parser_t::TermPrime()
{
	parsetree.push(NT_TermPrime);
	switch ( scanner.next_token() )
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

void parser_t::Factor()
{
	int num;
	parsetree.push(NT_Factor);
	switch ( scanner.next_token() )
	{
		case T_openparen:
			eat_token(T_openparen);
			num = Expr();
			eat_token(T_closeparen);
			break;
		case T_m:
			eat_token(T_m);
			eat_token(T_openbracket);
			num = Expr();
			if ( num < 0 ) negative_index(num);
			eat_token(T_closebracket);
			num = read_mem(num);
			break;
		case T_num:
			num = scanner.get_num();
			eat_token(T_num);
			//eat_token(T_num, num); // this one will print the actual value in the tree
			break;
		default:
			syntax_error(NT_Factor);
			break;
	}

//	if ( num < 0 ) negative_error(num);
	stack_queue_manage(T_num, num);
	
	parsetree.pop();
}

/*** Main ***********************************************/

int main()
{
	parser_t parser;
	parser.parse();

	return 0;
}
