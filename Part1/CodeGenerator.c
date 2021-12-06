#include "..\src\CodeGenerator.h"

typedef struct symbol_table
{
	struct variable *head; //the first node in the symbol table linked list
	struct variable *tail; //the last node in the symbol table linked list
} Symbol_table;

typedef struct variable
{
	char *name;			   //string
	char *type;			   //an extadecimal_number
	int size;			   //an integer
	int addr;			   //an extadecimal_number
	int nstLvl;			   // nested level of the item
	struct variable *next; //pointer to the next item in the symbal table
	struct variable *prev; //pointer to the privuse item in the symbal table
} Variable;

/*Functions Decleretion*/
Symbol_table *ST_init();												  //Function to initialize a new Symbal Table.
void ST_add(Symbol_table *ST, char *resName, char *resType, int resSize); //Function how add a new Variable node with the received varebels to the Symbul Table.
void ST_rmv(Symbol_table *ST, Variable *del);							  //Function to remove a given node from the Symbal Tabel.
void ST_rmv_block(Symbol_table *ST, int lvl);							  //Function to remove all the Variables from the same kind of nasted level as given.
Variable *ST_find(Symbol_table *ST, char *key);							  //Finds the value in the table of symbols with the same key value.
void ST_free(Symbol_table *ST);											  //Function how delete the given Symbal tabel and release the allocated memaory.

/*Global Varibels*/
static Symbol_table *ST;  //a pointer for the Symbol Table
static char *TYPE = NULL; //global string to hold the type of the variable

/*Counters*/
static int NESTED_LEVEL = 0; //a counter for the nested level of an item in the symbal table
static int LAST_ADDER = 5;	 //counter fore the last address to show in the symbol table.

/*Lable counters*/
int if_Count = 0;		 //counter for the number of 'if' statment
int if_else_Count = 0;	 //counter for the number of 'if-else' statment
int cond_expr_Count = 0; //counter for the number of ' cond-expr' statment
int for_Count = 0;		 //counter for the number of 'for' loop
int while_Count = 0;	 //counter for the number of 'while' loop
int do_while_Count = 0;	 //counter for the number of 'do-while' loop

/*Flages*/
static int PCODE = 1;	  //flag to show the pcode output - the flag is on whan we want to print the PCode
static int DECL_FLAG = 0; //flag for decleration case - the flag is on whan we want to declar on a new variabel
static int ADDR_FLAG = 0; //flag for showing address or value of a variabel in the symbol table - 0=Value 1=Address

/*
*	Input: treenode (AST)
*	Output: prints the Pcode on the console
*	This recursive function is the main method for Code Generation
*/
int code_recur(treenode *root)
{
	if_node *ifn;
	for_node *forn;
	leafnode *leaf;

	Variable *find; //a pointer to hold the Variable node to find in the symbol table.

	if (!ST) //Initialized a new Symbol Table
		ST = ST_init();

	if (!root)
		return SUCCESS;

	switch (root->hdr.which)
	{
	case LEAF_T:

		leaf = (leafnode *)root;
		switch (leaf->hdr.type)
		{
		case TN_LABEL:
			/* Maybe you will useg it later */
			break;

		case TN_IDENT:

			/* variable case */

			if ((strcmp(leaf->data.sval->str, "main"))) //Do not enter the name of the main function in the table of symbols
			{
				if (DECL_FLAG == 1) // a case for adding a new incremant to the Symbal table
				{
					ST_add(ST, leaf->data.sval->str, TYPE, 1); //add a new declaretion to the symbol table

					if (!PCODE) //print the the symbal table
						printf("Name: %s \t|\t Type: %s \t|\t Address: %d \t|\t Size: %d \t|\t Nested Level: %d\n", ST->tail->name, ST->tail->type, ST->tail->addr, ST->tail->size, ST->tail->nstLvl);
				}

				else //a case for assaiment
				{

					find = ST_find(ST, leaf->data.sval->str); //find the addres of the identifier in the Symbol Table.
					if (PCODE)								  //print the pcode
					{
						printf("ldc %d\n", find->addr); // puting the address.
						if (ADDR_FLAG == 0)				//in case we want to get the value.
							printf("ind\n");
					}
				}
			}
			break;

		case TN_COMMENT:
			/* Maybe you will use it later */
			break;

		case TN_ELLIPSIS:
			/* Maybe you will use it later */
			break;

		case TN_STRING:
			/* Maybe you will use it later */
			break;

		case TN_TYPE:
			/* Maybe you will use it later */
			break;

		case TN_INT:
			/* Constant case */
			if (PCODE)								 //print the pcode
				printf("ldc %d\n", leaf->data.ival); //load a intger value.
			break;

		case TN_REAL:
			/* Constant case */
			if (PCODE)								   //print the pcode
				printf("ldc %.2f\n", leaf->data.dval); //load a real value.
			break;
		}
		break;

	case IF_T:
		ifn = (if_node *)root;
		switch (ifn->hdr.type)
		{

		case TN_IF:
			if (ifn->else_n == NULL)
			{
				/* if case (without else)*/
				int if_localCounter = if_Count; //Create a local variable that holds the correct label type for this if expresion.
				if_Count++;						//Advance the label counter for the following calls.

				code_recur(ifn->cond);							//condition
				if (PCODE)										//print the pcode
					printf("fjp if_END_%d\n", if_localCounter); //jump to the end of the 'if' if the condition is false

				code_recur(ifn->then_n); //if true

				if (PCODE)									 //print the pcode
					printf("if_END_%d:\n", if_localCounter); //remain of the code.
			}
			else
			{
				/* if - else case*/
				int if_else_localCounter = if_else_Count; //Create a local variable that holds the correct label type for this if expresion.
				if_else_Count++;						  //Advance the label counter for the following calls.

				code_recur(ifn->cond);									   //condition
				if (PCODE)												   //print the pcode
					printf("fjp if-else_ELSE_%d\n", if_else_localCounter); //jump to the else case if tje condition is false

				code_recur(ifn->then_n); //if true
				if (PCODE)				 //print the pcode
				{
					printf("ujp if-else_END_%d\n", if_else_localCounter); //jump to the end of the if-else
					printf("if-else_ELSE_%d:\n", if_else_localCounter);	  //else case
				}
				code_recur(ifn->else_n); //if false

				if (PCODE)											   //print the pcode
					printf("if-else_END_%d:\n", if_else_localCounter); //remain of the code.
			}
			break;

		case TN_COND_EXPR:
			/* (cond)?(exp):(exp); */
			{
				int cond_expr_localCounter = cond_expr_Count; //Create a local variable that holds the correct label type for this if expresion.
				cond_expr_Count++;							  //Advance the label counter for the following calls.

				code_recur(ifn->cond);										  //condition
				if (PCODE)													  //print the pcode
					printf("fjp cond-expr_EXP_%d\n", cond_expr_localCounter); //if the answer os false jump to else case

				code_recur(ifn->then_n); //if true
				if (PCODE)				 //print the pcode
				{
					printf("ujp cond-expr_END_%d\n", cond_expr_localCounter); //jump to the end of the 'cond_expr' to rimain the code.
					printf("cond-expr_EXP_%d:\n", cond_expr_localCounter);	  // else case
				}

				code_recur(ifn->else_n);								   //if false
				if (PCODE)												   //print the pcode
					printf("cond-expr_END_%d:\n", cond_expr_localCounter); //remain of the code.
			}
			break;

		default:
			/* Maybe you will use it later */
			code_recur(ifn->cond);
			code_recur(ifn->then_n);
			code_recur(ifn->else_n);
		}
		break;

	case FOR_T:
		forn = (for_node *)root;
		switch (forn->hdr.type)
		{

		case TN_FUNC_DEF:
			/* Function definition */
			/* e.g. int main(...) { ... } */
			/* Look at the output AST structure! */
			code_recur(forn->init);
			code_recur(forn->test);
			code_recur(forn->incr);
			code_recur(forn->stemnt);
			break;

		case TN_FOR:
			/* For case*/
			/* e.g. for(i=0;i<5;i++) { ... } */
			{
				int for_localCounter = for_Count; //Create a local variable that holds the correct label type for this loop.
				for_Count++;

				code_recur(forn->init);							//int i=0
				if (PCODE)										//print the pcode
					printf("for_LOOP_%d:\n", for_localCounter); //begining of the for loop

				code_recur(forn->test); //condisen
				if (PCODE)				//print the pcode

					printf("fjp for_END_%d\n", for_localCounter); //if false go out of the loop.
				code_recur(forn->stemnt);						  //body

				code_recur(forn->incr); //i++
				if (PCODE)				//print the pcode
				{
					printf("ujp for_LOOP_%d\n", for_localCounter); //jump to the begining of the loop unconditonly
					printf("for_END_%d:\n", for_localCounter);
				}
			}
			break;

		default:
			/* Maybe you will use it later */
			code_recur(forn->init);
			code_recur(forn->test);
			code_recur(forn->stemnt);
			code_recur(forn->incr);
		}
		break;

	case NODE_T:
		switch (root->hdr.type)
		{
		case TN_PARBLOCK:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_PARBLOCK_EMPTY:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_TRANS_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_FUNC_DECL:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_FUNC_CALL:
			/* Function call */
			if (strcmp(((leafnode *)root->lnode)->data.sval->str, "printf") == 0)
			{
				/* printf case */
				code_recur(root->rnode->rnode);
				if (PCODE) //print the pcode
					printf("print\n");
			}
			else
			{
				/* other function calls - for HW3 */
				code_recur(root->lnode);
				code_recur(root->rnode);
			}
			break;

		case TN_BLOCK:
			/* Maybe you will use it later */
			NESTED_LEVEL++; //Increasing the value of the ested level

			/*Entrance to give the sub-trees coming out of the block node*/
			code_recur(root->lnode);
			code_recur(root->rnode);

			if (ST->head != NULL) //remove all new entries that have joined the symbol table
				ST_rmv_block(ST, NESTED_LEVEL);

			NESTED_LEVEL--; //Decreasing the value of the ested level
			break;

		case TN_ARRAY_DECL:
			/* array declaration - for HW2 */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_EXPR_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_NAME_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_ENUM_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_FIELD_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_PARAM_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_IDENT_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_TYPE_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_COMP_DECL:
			/* struct component declaration - for HW2 */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_DECL:
			/* structs declaration */
			code_recur(root->lnode);

			leafnode *type_leaf = (leafnode *)root->lnode->lnode; //holde to the node of the left grandson

			TYPE = toksym(type_leaf->hdr.tok, 0); //hold the type of the variable to use the statement the following variables

			DECL_FLAG = 1;
			code_recur(root->rnode); //Access to the declared variable
			DECL_FLAG = 0;

			TYPE = NULL; //Global variable boot
			break;

		case TN_DECL_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_DECLS:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_STEMNT_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_STEMNT:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_BIT_FIELD:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_PNTR:
			/* pointer - for HW2! */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_TYPE_NME:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_INIT_LIST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_INIT_BLK:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_OBJ_DEF:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_OBJ_REF:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_CAST:
			/* Maybe you will use it later */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_JUMP:
			if (root->hdr.tok == RETURN)
			{
				/* return jump - for HW2! */
				code_recur(root->lnode);
				code_recur(root->rnode);
			}
			else if (root->hdr.tok == BREAK)
			{
				/* break jump - for HW2! */
				code_recur(root->lnode);
				code_recur(root->rnode);
			}
			else if (root->hdr.tok == GOTO)
			{
				/* GOTO jump - for HW2! */
				code_recur(root->lnode);
				code_recur(root->rnode);
			}
			break;

		case TN_SWITCH:
			/* Switch case - for HW2! */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_INDEX:
			/* call for array - for HW2! */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_DEREF:
			/* pointer derefrence - for HW2! */
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		case TN_SELECT:
			/* struct case - for HW2! */
			if (root->hdr.tok == ARROW)
			{
				/* Struct select case "->" */
				/* e.g. struct_variable->x; */
				code_recur(root->lnode);
				code_recur(root->rnode);
			}
			else
			{
				/* Struct select case "." */
				/* e.g. struct_variable.x; */
				code_recur(root->lnode);
				code_recur(root->rnode);
			}
			break;

		case TN_ASSIGN:
			if (root->hdr.tok == EQ)
			{
				/* Regular assignment "=" */
				/* e.g. x = 5; */
				ADDR_FLAG = 1;
				code_recur(root->lnode); //addres
				ADDR_FLAG = 0;

				code_recur(root->rnode); //value
				if (PCODE)				 //print the pcode
					printf("sto\n");	 //store the new value
			}
			else if (root->hdr.tok == PLUS_EQ)
			{
				/* Plus equal assignment "+=" */
				/* e.g. x += 5; */
				ADDR_FLAG = 1;
				code_recur(root->lnode); //addres
				ADDR_FLAG = 0;

				code_recur(root->lnode); //address value

				code_recur(root->rnode); //added value
				if (PCODE)				 //print the pcode
				{
					printf("add\n"); //add operation
					printf("sto\n"); //store the new value
				}
			}
			else if (root->hdr.tok == MINUS_EQ)
			{
				/* Minus equal assignment "-=" */
				/* e.g. x -= 5; */
				ADDR_FLAG = 1;
				code_recur(root->lnode); //addres
				ADDR_FLAG = 0;

				code_recur(root->lnode); //addres value

				code_recur(root->rnode); //decris value
				if (PCODE)				 //print the pcode
				{
					printf("sub\n"); //subtraction operation
					printf("sto\n"); //store the new value
				}
			}
			else if (root->hdr.tok == STAR_EQ)
			{
				/* Multiply equal assignment "*=" */
				/* e.g. x *= 5; */
				ADDR_FLAG = 1;
				code_recur(root->lnode); //addres
				ADDR_FLAG = 0;
				code_recur(root->lnode); //addres value
				code_recur(root->rnode); //multiplay value
				if (PCODE)				 //print the pcode
				{
					printf("mul\n"); //multiplay operation
					printf("sto\n"); //store the new value
				}
			}
			else if (root->hdr.tok == DIV_EQ)
			{
				/* Divide equal assignment "/=" */
				/* e.g. x /= 5; */
				ADDR_FLAG = 1;
				code_recur(root->lnode); //addres
				ADDR_FLAG = 0;
				code_recur(root->lnode); //value
				code_recur(root->rnode); //sub addres value
				if (PCODE)				 //print the pcode
				{
					printf("div\n"); //Division operation
					printf("sto\n"); //store the new value
				}
			}
			break;

		case TN_EXPR:
			switch (root->hdr.tok)
			{
			case CASE:
				/* you should not get here */
				code_recur(root->lnode);
				code_recur(root->rnode);
				break;

			case INCR:
				/* Increment token "++" */
				/*++x*/
				if (root->lnode == NULL)
				{
					ADDR_FLAG = 1;
					code_recur(root->rnode); //load addres
					ADDR_FLAG = 0;

					code_recur(root->rnode); //load value
					if (PCODE)				 //print the pcode
					{
						printf("inc 1\n"); //Subtract 1 from the value at the top of the stack
						printf("sto\n");   //store the new value
					}
					code_recur(root->rnode); //get new value
				}

				/*x++*/
				else // (root->rnode==NULL)
				{
					code_recur(root->lnode); //load old value

					ADDR_FLAG = 1;
					code_recur(root->lnode); //load address
					ADDR_FLAG = 0;

					code_recur(root->lnode); //load value
					if (PCODE)				 //print the pcode
					{
						printf("inc 1\n"); //add 1 from the value at the top of the stack
						printf("sto\n");   //store the new value
					}
				}

				break;

			case DECR:
				/* Decrement token "--" */
				/*--x*/
				if (root->lnode == NULL)
				{
					ADDR_FLAG = 1;
					code_recur(root->rnode); //load addres
					ADDR_FLAG = 0;

					code_recur(root->rnode); //load value
					if (PCODE)				 //print the pcode
					{
						printf("inc -1\n"); //Subtract 1 from the value at the top of the stack
						printf("sto\n");	//store the new value
					}
					code_recur(root->rnode); //get new value
				}

				/*x--*/
				else // (root->rnode==NULL)
				{
					code_recur(root->lnode); //load old value

					ADDR_FLAG = 1;
					code_recur(root->lnode); //load address
					ADDR_FLAG = 0;

					code_recur(root->lnode); //load value
					if (PCODE)				 //print the pcode
					{
						printf("inc -1\n"); //Subtract 1 from the value at the top of the stack
						printf("sto\n");	//store the new value
					}
				}
				break;

			case PLUS:
				/* Plus token "+" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("add\n");	 //add operator
				break;

			case MINUS:
				/* Minus token "-" */
				if (root->lnode == NULL)
				{
					/*Onary negativ operetion*/
					code_recur(root->rnode); //call for an item
					if (PCODE)				 //print the pcode
						printf("neg\n");	 //negativ operator
				}

				else
				{
					/*Subtraction operator*/
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("sub\n");	 //subtract operator
				}
				break;

			case DIV:
				/* Divide token "/" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("div\n");	 //Division operation
				break;

			case STAR:
				/* multiply token "*" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("mul\n");	 //multiplay operator
				break;

			case AND:
				/* And token "&&" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("and\n");	 //and operator
				break;

			case OR:
				/* Or token "||" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("or\n");		 //or operator
				break;

			case NOT:
				/* Not token "!" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //-1 //call for second item
				if (PCODE)				 //print the pcode
					printf("not\n");	 //not operator
				break;

			case GRTR:
				/* Greater token ">" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("grt\n");	 //greater then operator
				break;

			case LESS:
				/* Less token "<" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("les\n");	 //less grater then operator
				break;

			case EQUAL:
				/* Equal token "==" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("equ\n");	 //equal operator
				break;

			case NOT_EQ:
				/* Not equal token "!=" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("neq\n");	 //not equal operator
				break;

			case LESS_EQ:
				/* Less or equal token "<=" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("leq\n");	 //less or equal operator
				break;

			case GRTR_EQ:
				/* Greater or equal token ">=" */
				code_recur(root->lnode); //call for first item
				code_recur(root->rnode); //call for second item
				if (PCODE)				 //print the pcode
					printf("geq\n");	 // Great equal operator
				break;

			default:
				code_recur(root->lnode);
				code_recur(root->rnode);
				break;
			}
			break;

		case TN_WHILE:
			/* While case */
			{
				int while_localCounter = while_Count; //Create a local variable that holds the correct label type for this loop.
				while_Count++;						  //Advance the label counter for the following calls.

				if (PCODE)											  //print the pcode
					printf("while_LOOP_%d:\n", while_localCounter);	  // the begining of the 'while' loop
				code_recur(root->lnode);							  //condition
				if (PCODE)											  //print the pcode
					printf("fjp while_END_%d\n", while_localCounter); //if the confition is false jump to remain the code(go out of the loop)

				code_recur(root->rnode); //body
				if (PCODE)				 //print the pcode
				{
					printf("ujp while_LOOP_%d\n", while_localCounter); //jump anconditionaly to te beginning of the loop
					printf("while_END_%d:\n", while_localCounter);	   // reamin the code
				}
			}
			break;

		case TN_DOWHILE:
			/* Do-While case */
			{
				int do_while_localCounter = do_while_Count; //Create a local variable that holds the correct label type for this loop.
				do_while_Count++;							//Advance the label counter for the following calls.

				if (PCODE)												  //print the pcode
					printf("do-while_LOOP_%d:\n", do_while_localCounter); // the begining of the 'do-while' loop
				code_recur(root->rnode);								  //body

				code_recur(root->lnode); //condition
				if (PCODE)				 //print the pcode
				{
					printf("fjp do-while_END_%d\n", do_while_localCounter);	 //if the confition is false jump to remain the code(go out of the loop)
					printf("ujp do-while_LOOP_%d\n", do_while_localCounter); //jump anconditionaly to te beginning of the loop
					printf("do-while_END_%d:\n", do_while_localCounter);	 // reamin the code
				}
			}
			break;

		case TN_LABEL:
			code_recur(root->lnode);
			code_recur(root->rnode);
			break;

		default:
			code_recur(root->lnode);
			code_recur(root->rnode);
		}
		break;

	case NONE_T:
		printf("Error: Unknown node type!\n");
		exit(FAILURE);
	}

	return SUCCESS;
}

/*
*	Input: treenode (AST)
*	Output: prints the Sumbol Table on the console
*	This function prints all the variables on your symbol table with their data
*/
void print_symbol_table(treenode *root)
{
	printf("Showing the Symbol Table:\n");
	printf("---------------------------------------\n");
	PCODE = 0;		  // a flag for not printing the PCode output
	code_recur(root); // move over the AST and insert valuse to the symbol table.
	PCODE = 1;		  // change the flag back to it show Pcode output.
	printf("---------------------------------------\n");
	ST_free(ST); // releas the symbol table content
}

/*Internal function*/

/*
*Input: None
*Output: None
*Function to initialize a new Symbal Table.
*/
Symbol_table *ST_init()
{

	Symbol_table *ST = (Symbol_table *)malloc(sizeof(Symbol_table *)); //allocate a new Symbal table in mamory.

	if (!ST) //if coldent allocate right show error.
	{
		printf("cannot allocate memory\n");
		exit(1);
	}

	/*resat the struct valuse to NULL*/
	ST->head = NULL;
	ST->tail = NULL;

	return ST;
}

/*
*Input: Symbal Table, Variable name (string), Variable Type (string), Variable Size (integer)
*Output: None
*Function how add a new Variable node with the received varebels to the Symbul Table.  
*/
void ST_add(Symbol_table *ST, char *resName, char *resType, int resSize)
{
	Variable *new = (Variable *)malloc(sizeof(Variable)); //allocate a new node in mamory.

	if (!new) //if coldent allocate right show error.
	{
		printf("cannot allocate memory\n");
		exit(1);
	}

	/*Stores the data of the new node.*/
	new->name = resName;
	new->type = resType;
	new->size = resSize;
	new->addr = LAST_ADDER++;	//pramote the addresss for the value in the symbal table.
	new->nstLvl = NESTED_LEVEL; //add the nested level of the carent block.
	new->next = NULL;			//predesesor node pointer

	/*add to empty list*/
	if (ST->head == NULL)
	{
		new->prev = NULL;
		ST->head = new; //update the head pointer
		ST->tail = new; //update the tail pointer

		return;
	}

	/*add to the end of the list*/
	ST->tail->next = new; // add the new node to the end of the list
	new->prev = ST->tail;
	ST->tail = new; //update the tail pointer
}

/*
*Input: Symbal Table and a pointer to a node we whant to delete (Variable*). 
*Output: None
*Function to remove a given node from the Symbal Tabel.
*/
void ST_rmv(Symbol_table *ST, Variable *del)
{
	Variable *curr = (ST->tail);

	/*update the tail pointer if the last node is the one to be remove.*/
	if ((ST->tail) == del)
		(ST->tail) = del->prev;
	/*delete the last item in the symbol table*/
	if (del->next != NULL)
		del->next->prev = del->prev;
	/*change the prev value if it isent the first item in the symbol table*/
	if (del->prev != NULL)
		del->prev->next = del->next;

	free(del); //free the mamory of the item thet have deleted.
	return;
}

/*
*Input: Symbol Table and a nasted level value (integer).
*Output: None.
*Function to remove all the Variables from the same kind of nasted level as given.
*/
void ST_rmv_block(Symbol_table *ST, int lvl)
{
	Variable *curr = (ST->tail); //Pointers to move on the symbol table list.

	while (curr->prev != NULL) //if we moved over the all list and colden't find the key.
	{
		if (curr->nstLvl == lvl) //every node in the nested level will remove from the Symbol table
		{
			ST_rmv(ST, curr);
		}
		curr = curr->prev; //Promotes the pointer to the table of symbols for the node before it.
	}
}

/*
*Input: Symbol Table and a key value (name string) 
*Output: Pointer to the requested link or error message if not.
* Finds the value in the table of symbols with the same key value. Returns a pointer to the node if it finds it.
*/
Variable *ST_find(Symbol_table *ST, char *key)
{
	Variable *curr = ST->tail; //Pointers to move on the symbol table list.

	/*a case whan the symbol table is empty*/
	if (ST->head == NULL)
	{
		printf("We coudent find your Variable\n");
		exit(1);
	}

	/*Find the node with the key value in the symbol table or tell you if it dosen't there*/
	while (strcmp(curr->name, key) != 0)
	{
		if (curr->prev == NULL) //if we moved over the all list and colden't find the key.
		{
			printf("Given name have not found.\n");
			exit(1);
		}
		curr = curr->prev; //Promotes the pointer to the table of symbols for the node before it.
	}
	return curr; //return the value with the key value.
}

/*
Input: Symbol Table
Output: None.
* Function how delete the given Symbal tabel and release the allocated memaory. 
*/
void ST_free(Symbol_table *ST)
{
	Variable *curr = (ST->tail), *del; //pointers to move on the symbol table list
	while (curr->prev != NULL)		   //A loop for going over the list and deleting each node in it from end to beginning.
	{
		del = curr;
		curr = curr->prev; //Move the list pointer backwords
		curr->next = NULL; //reles the last node frop the linked list
		del->prev = NULL;  //reles the last node frop the linked list
		free(del);		   //reles the last node allocated memory.
	}

	free(curr); //releas the first and only remain node allocated memory.

	/*reset the Symbal table pointers */
	(ST->tail) = NULL;
	(ST->head) = NULL;
	free(ST); //reles  allocated memory for the sumbal table.
}
