#include <stdio.h>
#include "nfa.h"

static int nstate = 0;
State matchstate = {MATCH, NULL, NULL, 0};
List l1, l2;
static int listid;

State *state(int c, State *out, State *out1){ //create a new state object
	State *s = create(State, 1);
	s->c = c;
	s->out = out;
	s->out1 = out1;
	s->lastlist = 0;
	
	++nstate;
	return s;
}

Frag frag(State *start, Ptrlist *out){ //creates a new frag object
	Frag f = {start, out};
	return f;//this returns a copy of f
}

/*
	Given an operator represented by a character this method will give us a value for that op so that we can compare operators precedence afterwards. Basically this method maps an operator to an int value of its precedence
*/
int get_prec(char operator){
	switch(operator){
		case '|': return 1;//least precedence
		case '.': return 2;
		case '?': return 3;
		case '+': return 4;
		case '*': return 5;//highest precedence
		default: return -1;
	}
}

/*
	Given a character c this method will check if this character is any of the regex operators
*/
int is_operator(char c){
	return c =='|'||c=='.';
}

/*Method declaration*/
/*
*	This method will convert a given infix regular expresion to an equavalent postfix regular expression
*	Assumes that p already has enough space to accomodate the postfix method
*/
void reg2post(char *re, char *pf){
	char operation_stack[DEFAULT_STACK_SIZE] = {0}; //stack to keep operations include
	char *opstack = operation_stack;
	char c, *p=pf;
	int natom = 0;
	int c_precedence;
	int paren[100], *pa = paren;
	while((c=*re++)){
		switch(c){
			case '|'://we want to get the apply this to the last and the next operand
				//first remove all high_precedence 
				while(--natom > 0)
					push(p, '.');
				 c_precedence = get_prec(c);
				while(opstack>operation_stack && c_precedence <= get_prec(peek(opstack))){
					char temp = pop(opstack);
					push(p, temp);//put the removed operations to the end of the new string
				}
				push(opstack, c);
				break;
			case '*': case '?': case '+':
				push(p, c);
				break;
			case '(':
				if(natom > 1){
					--natom;
					push(p, '.');
				}
				push(opstack, c);
				push(pa, natom);
				natom = 0;
				break;
			case ')':
				while(--natom > 0)
					push(p, '.');
				//pop all operators till we get (
				while(peek(opstack)!='('){
					push(p, pop(opstack));
				}
				--opstack;//this is to turn off warnings with using pop()
				natom = pop(pa);
				++natom;
				break;
			case ' ': break;
			default:
				if(natom > 1){
					--natom;
					push(p, '.');
				}
				++natom;
				push(p, c);
		}
	}
	while(--natom > 0)
		push(p, '.');
	while(opstack>=operation_stack){
		push(p, pop(opstack));
	}
	push(p, '\0');//put the null terminator to end of strin
}

/*
	Method to convert the postfix regex into an NFA
*/
State* post2nfa(char *postfix){
	char c;
	Frag stack[1000], *stackp, e1, e2, e;
	State *s;
	
	stackp = stack;
	while((c=*postfix++)){
		switch(c){
			default: //these are literats they push new NFA fragemetds into the stack
				s = state(c, NULL, NULL);
				push(stackp, frag(s, list1(&s->out)));
				break;
			case '.':
				e2 = pop(stackp);
				e1 = pop(stackp);
				patch(e1.out, e2.start);
				push(stackp, frag(e1.start, e2.out));
				break;
			case '|':
				e2 = pop(stackp);
				e1 = pop(stackp);
				s = state(SPLIT, e1.start, e2.start);
				push(stackp, frag(s, append(e1.out, e2.out)));
				break;
			case '?'://0 or 1
				e = pop(stackp);
				s = state(SPLIT, e.start, NULL);
				push(stackp, frag(s, append(e.out, list1(&s->out1))));
				break;
			case '+'://1 or more
				e = pop(stackp);
				s = state(SPLIT, e.start, NULL);
				patch(e.out, s);
				push(stackp, frag(e.start, list1(&s->out1)));
				break;
			case '*'://0 or more
				e = pop(stackp);
				s = state(SPLIT, e.start, NULL);
				patch(e.out, s);
				push(stackp, frag(s, list1(&s->out1)));
				break;
		}
	}
	e = pop(stackp);
	patch(e.out, &matchstate);
	return e.start;
}

/* Create singleton list containing just outp. */
Ptrlist *list1(State **outp){// creates a new pointer list containing the single pointer outp
	Ptrlist *l;
	
	l = (Ptrlist*)outp;
	l->next = NULL;///potential error
	return l;
}

void patch(Ptrlist *l, State *s){
	//h connects the dangling arrows in the pointer list l to the state s: it sets *outp = s for
	while(l){
		Ptrlist* next = l->next;
		l->s = s;
		l  = next;
	}
}

Ptrlist *append(Ptrlist *l1, Ptrlist *l2){
	//concatenates two pointer lists, returning the result
	Ptrlist* temp = l1;
	while(temp->next)
		temp = temp->next;
	temp->next = l2;
	return l1;
}

int match(State *start, char *s){
	List *clist, *nlist, *t;
	char c;
	/* l1 and l2 are preallocated globals */
	clist = startlist(start, &l1);
	nlist = &l2;
	while((c=*s++)){
		step(clist, c, nlist);
		t = clist; clist = nlist; nlist = t;	/* swap clist, nlist */
	}
	return ismatch(clist);
}


List* startlist(State *s, List *l){
	listid++;
	l->n=0;
	addstate(l,s);
	return l;
}

void step(List *clist, int c, List *nlist){
	State* s;
	for(int i=0; i<clist->n; ++i){
		s = clist->s[i];
		if(s->c == c)
			addstate(nlist, s->out);
	}
}

void addstate(List *l, State *s){
	if(s == NULL || s->lastlist == listid)
		return;
	if(s->c == SPLIT){
		/* follow unlabeled arrows */
		addstate(l, s->out);
		addstate(l, s->out1);
	}
	l->s[l->n++] = s;
}

int ismatch(List *l){
	
	for(int i=0; i<l->n; ++i)
		if(l->s[i] == &matchstate)
			return 1;
	return 0;
}

int main(int argc, char *argv[]){
	int i;
	char post[10000];
	State *start;
	if(argc< 3){
		fprintf(stderr, "usage: nfa regexp string...\n");
		return 1;
	}
	reg2post(argv[1], post);
	start = post2nfa(post);
	if(!start){
		fprintf(stderr, "error in post2nfa %s\n", post);
		return 1;
	}
	l1.s = malloc(nstate*sizeof l1.s[0]);
	l2.s = malloc(nstate*sizeof l2.s[0]);
	for(i=2; i<argc; i++){
		if(match(start, argv[i])){
			printf("%s matches\n", argv[i]);
		}else{
			printf("%s does not match\n", argv[i]);
		}
	}
	return 0;
}
