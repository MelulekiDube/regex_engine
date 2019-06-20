#ifndef NFA_H
#define NFA_H
#include <stdlib.h>
#define create(type, size) ((type*)malloc(sizeof(type)*size)) //create size objects of type type
#define push(stack, value) (*stack++ = value) //this will put the value in this current index and increment afterwards
#define pop(stack) (*--stack) //this give value in current index and then decrement 
#define peek(stack) (*(stack-1))
#define DEFAULT_STACK_SIZE 10


/*BEGIN	TYPEDEFS*/
typedef struct State State;
typedef struct Frag Frag;
typedef struct List List;
typedef union Ptrlist Ptrlist;
/*END TYPEDEFS*/

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == Match, no arrows out; matching state.
 * If c == Split, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */
enum{
	Match = 256,
	Split = 257
};

/**State struct declaration*/
struct State
{
	int c;
	State *out;
	State *out1;
	int lastlist;
	/*
		Each state will represent one NFA fragment depending on the value of cabs
		
		if c<256 then this is a normal transition
		if c==256 then this represents a split needed
		if c==257 then this represents a match in an accept state
	*/
};
State *state(int c, State *out, State *out1); //create a new state object
/*end of state declaration*/

/*Declaration of Frag*/
struct Frag
{
	State *start;
	Ptrlist *out;
};
Frag* frag(State *start, Ptrlist *out); //creates a new frag object
/*End of Frag Declaration*/

/*Declaration of list*/
struct List
{
	State **s;
	int n;
};
/*End of List Declaration*/

Ptrlist *list1(State **outp);// creates a new pointer list containing the single pointer outp
Ptrlist *append(Ptrlist *l1, Ptrlist *l2);//concatenates two pointer lists, returning the result
void patch(Ptrlist *l, State *s);//h connects the dangling arrows in the pointer list l to the state s: it sets *outp = s for each pointer outp in l

/*Method declaration*/
/*
*	This method will convert a given infix regular expresion an equavalent postfix regular expression
*/
void reg2post(char *re, char *pf);

/*
* 	Method that will insert the explicit concatenation characters
*/
void insert_concat_chars(char *re, char *p);
#endif
