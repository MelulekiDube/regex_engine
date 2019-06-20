#include <stdio.h>
#include "nfa.h"

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
				pop(opstack);
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

int main(){
	char *regex= "a(bb)+a" ;//expect  A B C * + D +
	char postfix[80];
	//insert_concat_chars(regex, reg);
	reg2post(regex, postfix);
	printf("Equivalent postfix is: %s\n", postfix);
	return 0;
}
