#include "..\src\CodeGenerator.h"
/*Symbol table - data structurs*/
typedef struct symbol_table
{
	struct variable *head; //the first node in the symbol table linked list
	struct variable *tail; //the last node in the symbol table linked list
} Symbol_table;

typedef struct variable
{
	char *name; //string
	float value;
	char *type;					//an extadecimal_number
	int ptrLvl;					//a level of the pointer if any
	int size;					//an integer
	int addr;					//an extadecimal_number
	int nstLvl;					// nested level of the item
	int arraySize;				//in a case of a array as the new variabel stor the size of the dimantions array that needed
	int *arrayHead;				//pointer to hold the size of the dimantions array
	struct variable *STRC_head; //apointer to the struct incremant if eny.
	struct variable *next;		//pointer to the next item in the symbal table
	struct variable *prev;		//pointer to the privuse item in the symbal table
} Variable;

/*Srtucts case - data structurs*/
typedef struct structs_table
{
	struct struct_types *head; //the first node in the symbol table linked list
	struct struct_types *tail; //the last node in the symbol table linked list
} Structs_table;

typedef struct struct_types
{
	char *name;
	int size;
	struct variable *head;	   //the first node in the struct linked list
	struct variable *tail;	   //the last node in the struct linked list
	struct struct_types *next; //the first node in the struct table linked list
	struct struct_types *prev; //the last node in the struct table linked list
} Struct_types;

typedef struct Loop_stack //holds the switches themselves-one for each created switch
{
	int depth;				 //hold the dept of the stack
	char *type;				 //loop_type Loop_Flag;
	int Case_count;			 //hold the number of the case in casr of switch
	struct Loop_stack *next; //pointher to the next value in the stack
} Loop_stack;

typedef enum flags
{
	NO_FLAG,
	DECL_FLAG,
	ASSIGN_FLAG,
	POINTER_FLAG,
	TYPE_FLAG,
	ADD_FLAG,
	STRC_DEF_FLAG,
	STRC_REF_FLAG,
	PRIMITIV_FLAG,
	ARRAY_FLAG,
	STRUCT_FLAG,
	FUNCTION_FLAG,
	STEMNT_FLAG,
	ASSIGN_FROM_FLAG,
	ASSIGN_TO_FLAG

} FLAG;

typedef enum Boolean
{
	False,
	True
} Boolean;

/*Functions Decleretion*/
void Print_STRC(Structs_table *STRC); //*	This function prints all the variables on your symbol table with their data.
/********************Struct Table functiuns********************/
Structs_table *STRCTab_init();													 //Function to initialize a new Struct Table.
void STRC_add(Structs_table *STRCTab, char *resName);							 //Function how add a new Struct_types node with the received varebels to the Struct Table.
void STRC_add_var(Structs_table *STRCTab, char *key, char *resName, int *array); //Function how add a new Variable node with the received varebels to the Struct in the Struct Table.
void STRCarrayDecl(Structs_table *STRCTab, char *key, int Dim, int i);			 //Function how save the array dimantions and the array dimantions size.
Struct_types *STRC_find(Structs_table *STRCTab, char *key);						 //Function how add finds the right struct in the Structs Table.
void STRC_size(Structs_table *STRCTab);											 //Function how moves over the struct table and update the size of every structs in it.
int STRC_size_recur(Structs_table *STRCTab, char *key);							 //Function how help to find and calculate the struct component size if it a struct type.
void STRC_rmv(Variable *del);													 //Function to remove a struct type, can be used to deley a struct node from the symbal tabel ot from a struct table.
void STRC_free(Structs_table *STRCTab);											 //Function how delete the given Struct tabel and release the allocated memaory.

/********************Symbol Table functiuns********************/
Symbol_table *ST_init();														  //Function to initialize a new Symbal Table.
void ST_add(Symbol_table *ST, Structs_table *STRCTab, char *resName, int *array); //*Function how add a new Variable node with the received varebels to the Symbul Table.
void STarrayDecl(Variable *find, int Dim, int i);
void ST_rmv(Symbol_table *ST, Variable *del);												//Function to remove a given node from the Symbal Tabel.
void ST_rmv_block(Symbol_table *ST, int lvl);												//Function to remove all the Variables from the same kind of nasted level as given.
Variable *ST_find(Symbol_table *ST, char *key);												//Finds the value in the table of symbols with the same key value. Returns a pointer to the node if it finds it.
Variable *find_STRC_comp(Variable *var, Structs_table *STRCTab, char *key, int *countSTRC); //Finds the value in the table of symbols with the same key value. Returns a pointer to the node if it finds it and update the pointer counter value. if the component is not in the symbol table add it to the symbal table.
void ST_free(Symbol_table *ST);																//Function how delete the given Symbal tabel and release the allocated memaory.

/********************Loop Stack functiuns********************/
void push_loop(Loop_stack **stack, char *type, int depth); //Function how push a new loop type to the Loop Stack.
Loop_stack *pop_loop(struct Loop_stack **stack);		   //Function how returns and delete the head loop from the Loop Stack.

/********************Optimazition functiuns********************/
Boolean is_constant(treenode *root, Boolean val); //Function how determent if the sub-tree that it gets is constent.
float calculate_value(treenode *root, float val); //Function how calculete the value of the sub-tree.

/*Global Varibels*/
static Symbol_table *ST;			//a pointer for the Symbol Table
static Structs_table *STRCTab;		//a pointer for the Struct Table
static char *TYPE = NULL;			//global string to hold the type of the variable
static char *STRC_NAME = NULL;		//global string to hold the Struct name
static char *ARRAY_NAME = NULL;		//global string to hold the array name
Variable *find = NULL;				//a pointer to hold the Variable node to find in the symbol table.
static Loop_stack *STK_LOOP = NULL; //a pointer for the Loop Stack
float CONST_SWITCH = 0;				//global variable how hold the value of a constant switch statment.

/*Counters*/
static int NESTED_LEVEL = 0;	//a counter for the nested level of an item in the symbal table
static int LAST_ADDER = 5;		//counter for the last address to show in the symbol table.
static int POINTER_COUNTER = 0; //counter for the pointer level to show in the symbol table.
static int MAX_ARRAY_COUNT = 0; //counter for the size of the dimantion array.

/*Lable counters*/
int if_Count = 0;		 //counter for the number of 'if' statment
int if_else_Count = 0;	 //counter for the number of 'if-else' statment
int cond_expr_Count = 0; //counter for the number of ' cond-expr' statment
int for_Count = 0;		 //counter for the number of 'for' loop
int while_Count = 0;	 //counter for the number of 'while' loop
int do_while_Count = 0;	 //counter for the number of 'do-while' loop
int switch_Count = 0;	 //counter for the number of 'switch' statment

/*Flages*/
static Boolean PCODE = True;			  //flag to show the pcode output - the flag is on whan we want to print the PCode
static FLAG STEP_FLAG = STEMNT_FLAG;	  //flag to know what step of the tree we handeling
static FLAG IDENT_FLAG = ASSIGN_FLAG;	  //flag to find in what part of the identifaier function we wont to opprite
static FLAG SRTC_FLAG = ADD_FLAG;		  //flag to handel a struct case.
static FLAG VAL_FLAG = PRIMITIV_FLAG;	  //flag for a struct appirens - the flag is on whan we want to a take care of a struct
static Boolean ADDR_FLAG = False;		  //flag for showing address or value of a variabel in the symbol table - 0=Value 1=Address
static Boolean CONST_SWITCH_FLAG = False; //flag to chack to handel a constant switch statmant

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

	if (!ST) //Initialized a new Symbol Table
		ST = ST_init();

	if (!STRCTab) //Initialized a new Symbol Table
		STRCTab = STRCTab_init();

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
		{

			/* variable case */
			if (STEP_FLAG == STEMNT_FLAG) //Do not enter the name of the main function in the table of symbols
			{

				if (IDENT_FLAG == DECL_FLAG) //case for declaretion
				{
					// case for a declaretion of a primitiv variabel
					if (VAL_FLAG == PRIMITIV_FLAG) //a declaretion of primitiv type
					{
						ST_add(ST, STRCTab, leaf->data.sval->str, NULL); //add a new declaretion to the symbol table
						if (!PCODE)										 //print the the symbal table
							printf("Name: %s\t|\tType: %s\t|\tPointer Level: %d\t|\tAddress: %d\t|\tSize: %d\t|\tNested Level: %d\n", ST->tail->name, ST->tail->type, ST->tail->ptrLvl, ST->tail->addr, ST->tail->size, ST->tail->nstLvl);
					}

					if (VAL_FLAG == ARRAY_FLAG) //a declaration of array type
					{
						int *array = (int *)malloc(sizeof(int) * MAX_ARRAY_COUNT); //allocate a new Symbal table in mamory.
						if (!array)
						{
							printf("Cannot allocate memory.\n");
							exit(1);
						}
						ARRAY_NAME = leaf->data.sval->str;		//save the array name
						ST_add(ST, STRCTab, ARRAY_NAME, array); //add a new declaretion to the symbol table
						find = ST_find(ST, ARRAY_NAME);
					}
				}

				else if (IDENT_FLAG == ASSIGN_FLAG) // case for assignmant
				{

					if (VAL_FLAG == PRIMITIV_FLAG || VAL_FLAG == ARRAY_FLAG || VAL_FLAG == ASSIGN_FROM_FLAG || VAL_FLAG == ASSIGN_TO_FLAG) // case for an assignmant to a primitiv variabel.
					{
						Variable *old_find = find;
						find = ST_find(ST, leaf->data.sval->str); //find the addres of the identifier in the Symbol Table.

						if (PCODE) //print the pcode
						{
							printf("ldc %d\n", find->addr); // puting the address.
							if (ADDR_FLAG == False)			//in case we want to get the value.
								printf("ind\n");
						}

						if (VAL_FLAG == ASSIGN_TO_FLAG)
						{
							if (find != NULL)
								old_find->value = find->value; //save the primitiv value in the right place in the symbol table
						}

						else if (VAL_FLAG == ARRAY_FLAG) //if it is an array save the array name
							ARRAY_NAME = leaf->data.sval->str;

						else //if its a primitiv value save it as a type name
							TYPE = leaf->data.sval->str;
					}

					if (VAL_FLAG == STRUCT_FLAG) //  case for an assignmant to a record variabel.
					{

						int countSTRC = 0; //holds the offset of the value

						find = find_STRC_comp(find, STRCTab, leaf->data.sval->str, &countSTRC); //find and hold the stract component in the symbol table amd te offset of it.
						if (PCODE)
						{
							printf("inc %d\n", countSTRC); // printing the relativ the address for the struct.
							if (ADDR_FLAG == False)		   //in case we want to get the value.
								printf("ind\n");
						}
						ARRAY_NAME = leaf->data.sval->str; //save the array name
					}
				}
				else if (IDENT_FLAG == TYPE_FLAG)
					TYPE = leaf->data.sval->str; // case for a declaration of a stract variabel
			}

			else if (STEP_FLAG == STRUCT_FLAG) // stract step
			{

				if (SRTC_FLAG == STRC_REF_FLAG) // case where we came from stract referenc
				{

					TYPE = leaf->data.sval->str;
				}

				else if (SRTC_FLAG == STRC_DEF_FLAG) // case where we came from stract defenition
				{
					STRC_NAME = leaf->data.sval->str; //save the struct name
					STRC_add(STRCTab, STRC_NAME);	  //hold the type of the variable to use the statement the following variables
				}

				else if (SRTC_FLAG == ADD_FLAG) //case of adding a new component to the struct
				{

					if (VAL_FLAG == ARRAY_FLAG) // case of adding a new array type as a component
					{
						int *array = (int *)malloc(sizeof(int) * MAX_ARRAY_COUNT); //allocate a new Symbal table in mamory.
						if (!array)
						{
							printf("Cannot allocate memory.\n");
							exit(1);
						}
						ARRAY_NAME = leaf->data.sval->str;					 //keep the array name
						STRC_add_var(STRCTab, STRC_NAME, ARRAY_NAME, array); //add a new declaretion to the symbol table
					}
					else //case of adding any other component.
					{
						STRC_add_var(STRCTab, STRC_NAME, leaf->data.sval->str, NULL); ////add a new declaretion to the symbol table
					}
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
			/*type leaf*/
			/* Maybe you will use it later */
			TYPE = toksym(root->hdr.tok, 0); //hold the type of the variable to use the statement the following variables
			break;

		case TN_INT:
			/* Constant case */

			if (VAL_FLAG == ASSIGN_FLAG)
			{
				if (find != NULL)
					find->value = (float)leaf->data.ival; //save the primitiv value in the right place in the symbol table
			}
			if (VAL_FLAG != ARRAY_FLAG)
			{
				if (PCODE)								 //print the pcode
					printf("ldc %d\n", leaf->data.ival); //load a intger value.
			}

			break;

		case TN_REAL:
			/* Constant case */
			if (VAL_FLAG == ASSIGN_FLAG)
			{
				if (find != NULL)
					find->value = (float)leaf->data.ival; //save the primitiv value in the right place in the symbol table
			}
			if (VAL_FLAG != ARRAY_FLAG)
			{
				if (PCODE)								   //print the pcode
					printf("ldc %.2f\n", leaf->data.dval); //load a real value.
			}

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
				if (is_constant(ifn->cond, True) == True && calculate_value(ifn->cond, 0) != 0) //if(True)
					code_recur(ifn->then_n);													//if true

				else if (!(is_constant(ifn->cond, True) == True && calculate_value(ifn->cond, 0) == 0))
				{
					int if_localCounter = if_Count; //Create a local variable that holds the correct label type for this if expresion.
					if_Count++;						//Advance the label counter for the following calls.

					code_recur(ifn->cond);							//condition
					if (PCODE)										//print the pcode
						printf("fjp if_END_%d\n", if_localCounter); //jump to the end of the 'if' if the condition is false

					code_recur(ifn->then_n); //if true

					if (PCODE)									 //print the pcode
						printf("if_END_%d:\n", if_localCounter); //remain of the code.
				}
			}
			else
			{
				/* if - else case*/
				if (is_constant(ifn->cond, True) == True && calculate_value(ifn->cond, 0) != 0) //if(True)
					code_recur(ifn->then_n);													//if true

				else if (is_constant(ifn->cond, True) == True && calculate_value(ifn->cond, 0) == 0) //if(False)
					code_recur(ifn->else_n);														 //if false

				else
				{
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
			}
			break;

		case TN_COND_EXPR:
			/* (cond)?(exp):(exp); */
			{
				if (is_constant(ifn->cond, True) == True && calculate_value(ifn->cond, 0) != 0) //if(True)
					code_recur(ifn->then_n);													//if true

				else if (is_constant(ifn->cond, True) == True && calculate_value(ifn->cond, 0) == 0) //if(False)
					code_recur(ifn->else_n);														 //if false

				else
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
			STEP_FLAG = STEMNT_FLAG;
			code_recur(forn->init);
			if (STRCTab->head != NULL) //in case of a struct declaretion update the size of the structs that have declerd
				STRC_size(STRCTab);
			if (!PCODE)
				Print_STRC(STRCTab);
			STEP_FLAG = FUNCTION_FLAG; //change the flag for a functuin state
			code_recur(forn->test);
			code_recur(forn->incr);
			STEP_FLAG = STEMNT_FLAG; //change the flag for a statment state
			code_recur(forn->stemnt);

			/*releas the tables*/
			ST_free(ST);		// releas the symbol table content
			STRC_free(STRCTab); // releas the struct table content
			break;

		case TN_FOR:
			/* For case*/
			/* e.g. for(i=0;i<5;i++) { ... } */
			{
				Loop_stack *currLoop = NULL;
				push_loop(&STK_LOOP, "for", for_Count);
				for_Count++;

				code_recur(forn->init);						   //int i=0
				if (PCODE)									   //print the pcode
					printf("for_LOOP_%d:\n", STK_LOOP->depth); //begining of the for loop

				code_recur(forn->test); //condisen
				if (PCODE)				//print the pcode

					printf("fjp for_END_%d\n", STK_LOOP->depth); //if false go out of the loop.
				code_recur(forn->stemnt);						 //body

				code_recur(forn->incr); //i++
				if (PCODE)				//print the pcode
				{
					printf("ujp for_LOOP_%d\n", STK_LOOP->depth); //jump to the begining of the loop unconditonly
					printf("for_END_%d:\n", STK_LOOP->depth);
				}
				currLoop = pop_loop(&STK_LOOP); //push the loop to the loop stack
				free(currLoop);
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

			//STEP_FLAG = STRUCT_FLAG;
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
			/*Array Declaration node*/
			{
				FLAG lastValFlag = VAL_FLAG; //backup fot the last variable flag state.
				int arrayDim = 0;
				int currArryCount = MAX_ARRAY_COUNT; //local array counter stege
				MAX_ARRAY_COUNT++;					 //incres the array counter
				VAL_FLAG = ARRAY_FLAG;				 //change the VAL_FLAG to ARRAY_FLAG

				code_recur(root->lnode);
				code_recur(root->rnode);
				arrayDim = ((leafnode *)root->rnode)->data.ival; //save the array dimantion how stay at his rigth son.

				if (STEP_FLAG == STRUCT_FLAG) //in case that we adding the array to a new declard struct
					STRCarrayDecl(STRCTab, ARRAY_NAME, arrayDim, MAX_ARRAY_COUNT - currArryCount - 1);

				else if (STEP_FLAG == STEMNT_FLAG) //in case that we adding the array to a new declard stage.
				{
					STarrayDecl(find, arrayDim, MAX_ARRAY_COUNT - currArryCount - 1);

					if (!PCODE) //print the the symbal table
					{
						if (currArryCount == 0) //print the symbal tabel whan the declaration of it is done
							printf("Name: %s\t|\tType: %s\t|\tPointer Level: %d\t|\tAddress: %d\t|\tSize: %d\t|\tNested Level: %d\n", ST->tail->name, ST->tail->type, ST->tail->ptrLvl, ST->tail->addr, ST->tail->size, ST->tail->nstLvl);
					}
				}

				if (currArryCount == 0) //reset the the  global values that have bein used.
				{
					ARRAY_NAME = NULL;
					MAX_ARRAY_COUNT = 0;
				}

				VAL_FLAG = lastValFlag; //go back to the lest stage of the flag
			}
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
			/*Type list node*/
			/* Maybe you will use it later */
			{
				FLAG currFlag = IDENT_FLAG; //backup fot the last identifaier flag state.
				IDENT_FLAG = TYPE_FLAG;		//change the IDENT_FLAG to TYPE_FLAG
				code_recur(root->lnode);
				code_recur(root->rnode);
				IDENT_FLAG = currFlag; //go back to the lest stage of the flag
				break;
			}

		case TN_COMP_DECL:
			/* struct component declaration - for HW2 */
			code_recur(root->lnode);
			code_recur(root->rnode); //Access to the declared variable

			/*Reset the Global variable*/
			TYPE = NULL;
			POINTER_COUNTER = 0;
			MAX_ARRAY_COUNT = 0;

			break;

		case TN_DECL:
			/* structs declaration */
			/*Declaration node*/
			{
				FLAG currFlag = IDENT_FLAG; //backup fot the last identifaier flag state.

				IDENT_FLAG = DECL_FLAG; //change the IDENT_FLAG to DECL_FLAG
				code_recur(root->lnode);
				code_recur(root->rnode); //Access to the declared variable
				IDENT_FLAG = currFlag;	 //go back to the lest stage of the flag

				/*Reset the Global variable*/
				//TYPE = NULL;
				POINTER_COUNTER = 0;
			}
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
			POINTER_COUNTER = 0;

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
			/* Pointer decleration */
			POINTER_COUNTER++; //prampte the POINTER_COUNTER
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
		{
			/* Maybe you will use it later */
			{
				FLAG currStepFlag = STEP_FLAG;
				FLAG currSTRCFlag = SRTC_FLAG; //backup fot the last struct flag state.
				FLAG currValFlag = VAL_FLAG;   //backup fot the last variable flag state.
				STEP_FLAG = STRUCT_FLAG;	   //change the flag for a struct decleretion state
				VAL_FLAG = STRUCT_FLAG;		   //change the VAL_FLAG to STRUCT_FLAG
				SRTC_FLAG = STRC_DEF_FLAG;	   //change the SRTC_FLAG to STRC_DEF_FLAG
				code_recur(root->lnode);
				SRTC_FLAG = currSTRCFlag; //go back to the lest stage of the flag
				code_recur(root->rnode);
				VAL_FLAG = currValFlag;	  //go back to the lest stage of the flag
				STEP_FLAG = currStepFlag; //go back to the lest stage of the flag
				STRC_NAME = NULL;		  //Reset the Global variable
			}
			break;
		}

		case TN_OBJ_REF:
			/* Maybe you will use it later */
			{
				FLAG currSTRCFlag = SRTC_FLAG; //backup fot the last struct flag state.
				FLAG currValFlag = VAL_FLAG;   //backup fot the last variable flag state.

				VAL_FLAG = STRUCT_FLAG;	   //change the VAL_FLAG to STRUCT_FLAG
				SRTC_FLAG = STRC_REF_FLAG; //change the SRTC_FLAG to STRC_DEF_FLAG
				code_recur(root->lnode);
				SRTC_FLAG = currSTRCFlag; //go back to the lest stage of the flag
				code_recur(root->rnode);
				VAL_FLAG = currValFlag; //go back to the lest stage of the flag
			}
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
				Loop_stack *STK_HEAD = STK_LOOP; //hold the had of the loop stack.

				code_recur(root->lnode);
				code_recur(root->rnode);

				if (PCODE)
				{
					if (CONST_SWITCH_FLAG == False)
					{
						if (strcmp(STK_HEAD->type, "for") == 0)			 //chack the type of the had of the stack
							printf("ujp for_END_%d\n", STK_HEAD->depth); //print the name in the head of the loop stack

						else if (strcmp(STK_HEAD->type, "while") == 0)	   //chack the type of the had of the stack
							printf("ujp while_END_%d\n", STK_HEAD->depth); //print the name in the head of the loop stack

						else if (strcmp(STK_HEAD->type, "do_while") == 0)	  //chack the type of the had of the stack
							printf("ujp do-while_END_%d\n", STK_HEAD->depth); //print the name in the head of the loop stack

						else if (strcmp(STK_HEAD->type, "switch") == 0)		//chack the type of the had of the stack
							printf("ujp switch_END_%d\n", STK_HEAD->depth); //print the name in the head of the loop stack
					}
				}
			}
			else if (root->hdr.tok == GOTO)
			{
				/* GOTO jump - for HW2! */
				code_recur(root->lnode);
				code_recur(root->rnode);
			}
			break;

		case TN_SWITCH:
		{
			/* Switch case - for HW2! */
			if (is_constant(root->lnode, True) == True) //case of aconstant switch
			{

				CONST_SWITCH_FLAG = True;
				CONST_SWITCH = calculate_value(root->lnode, 0); //save the value of the switch.
				code_recur(root->rnode);
				CONST_SWITCH_FLAG = False;
				CONST_SWITCH = 0;
			}

			else
			{
				Loop_stack *currLoop = NULL;
				push_loop(&STK_LOOP, "switch", switch_Count); //push the switch statmant to the loop stack

				switch_Count++;

				code_recur(root->lnode);

				code_recur(root->rnode);

				if (PCODE)
				{
					printf("switch%d_case%d:\n", STK_LOOP->depth, STK_LOOP->Case_count);
					printf("switch_END_%d:\n", STK_LOOP->depth);
				}

				currLoop = pop_loop(&STK_LOOP); //pop the switch from the loop stack
				free(currLoop);					//free the allocated mamory from the stack
			}
		}

		break;

		case TN_INDEX:
			/*Array index node*/
			/* call for array - for HW2! */
			{
				FLAG lastValFlag = VAL_FLAG;	   //backup fot the last variable flag state.
				Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.
				int currArryCount, lasrArryCount;  //local array counter stege
				Variable *localfind = NULL;		   //hold the last cald variable that hav found
				int dim = 1, size = 1;

				if (VAL_FLAG == PRIMITIV_FLAG)
				{
					lasrArryCount = MAX_ARRAY_COUNT; //local array counter stege
					MAX_ARRAY_COUNT = 0;
				}

				currArryCount = MAX_ARRAY_COUNT;
				MAX_ARRAY_COUNT++;	   //incres the array counter
				VAL_FLAG = ARRAY_FLAG; //change the SRTC_FLAG to STRC_DEF_FLAG
				ADDR_FLAG = True;	   //change the ADDR_FLAG to ON
				code_recur(root->lnode);
				localfind = find; //save last variable that have called from the symbol table
				VAL_FLAG = PRIMITIV_FLAG;
				ADDR_FLAG = 0; //go back to the lest stage of the flag
				code_recur(root->rnode);
				VAL_FLAG = lastValFlag;	   //go back to the lest stage of the flag
				ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag

				if (VAL_FLAG == PRIMITIV_FLAG)
				{
					MAX_ARRAY_COUNT = lasrArryCount; //local array counter stege
				}

				if (!(strcmp(localfind->type, "int") == 0 || strcmp(localfind->type, "double") == 0 || strcmp(localfind->type, "float") == 0 || localfind->ptrLvl > 0)) //case of a struct variable
				{

					Struct_types *sizefind = STRC_find(STRCTab, localfind->type); //find the struct in the struct table
					size = sizefind->size;										  //update the size variable
				}

				/*calculat the dimentions */
				for (int i = ((localfind->arraySize) - currArryCount); i < localfind->arraySize; i++)
				{

					dim *= *(localfind->arrayHead + i);
				}
				if (dim == 0)
					dim = 1;

				if (PCODE)
					printf("ixa %d\n", dim * size); //print the dimantion multiplay the type size

				if (currArryCount == 0) //print "dec 0" an reset global variabels when we finish geting the array indexes
				{
					if (PCODE) //print and
					{
						printf("dec 0 \n"); //offset of the array
						if (ADDR_FLAG == False)
							printf("ind \n"); //in case we whant to get the value of the array.
					}
					MAX_ARRAY_COUNT = 0;
				}
			}
			break;

		case TN_DEREF:
			/* pointer derefrence - assigment for HW2! */
			{
				FLAG lastIdentFlag = IDENT_FLAG; //backup the IDENT_FLAG
				code_recur(root->lnode);

				code_recur(root->rnode);
				if (IDENT_FLAG == ASSIGN_FLAG) //in case we are in an assagimant flag print "ind"
				{
					if (PCODE)			 //print the pcode
						printf("ind\n"); //print to get the value of a pointer
				}
			}

			break;

		case TN_SELECT:
			/*Struct selector node*/
			/* struct case - for HW2! */
			if (root->hdr.tok == ARROW)
			{
				/* Struct select case "->" */
				/* e.g. struct_variable->x; */
				FLAG currValFlag = VAL_FLAG;	   //backup fot the last variable flag state.
				Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.
				VAL_FLAG = PRIMITIV_FLAG;		   //change the VAL_FLAG to PRIMITIV_FLAG
				ADDR_FLAG = True;				   //turn the adderes flag on
				code_recur(root->lnode);
				VAL_FLAG = STRUCT_FLAG; //change the VAL_FLAG to STRUCT_FLAG
				code_recur(root->rnode);
				ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag
				VAL_FLAG = currValFlag;	   //go back to the lest stage of the flag
			}
			else
			{
				/* Struct select case "." */
				/* e.g. struct_variable.x; */
				FLAG currValFlag = VAL_FLAG;	   //backup fot the last variable flag state.
				Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.
				VAL_FLAG = PRIMITIV_FLAG;		   //change the VAL_FLAG to PRIMITIV_FLAG
				ADDR_FLAG = True;				   //turn the adderes flag on
				code_recur(root->lnode);
				ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag
				VAL_FLAG = STRUCT_FLAG;	   //change the VAL_FLAG to STRUCT_FLAG
				code_recur(root->rnode);
				VAL_FLAG = currValFlag; //go back to the lest stage of the flag
			}
			break;

		case TN_ASSIGN:
			if (root->hdr.tok == EQ)
			{
				{
					FLAG currIdentFlag = IDENT_FLAG;   //backup fot the last identifaier flag state.
					FLAG currValFlag = VAL_FLAG;	   //backup fot the last VAL_FLAG state.
					Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.

					IDENT_FLAG = ASSIGN_FLAG; //change the IDENT_FLAG to ASSIGN_FLAG
					ADDR_FLAG = True;		  //turn the adderes flag on
					code_recur(root->lnode);  //addres

					ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag
					VAL_FLAG = ASSIGN_TO_FLAG; //change the VAL_FLAG to ASSIGN_TO_FLAG
					code_recur(root->rnode);   //value
					if (PCODE)				   //print the pcode
						printf("sto\n");	   //store the new value

					VAL_FLAG = currValFlag;		//go back to the lest stage of the flag
					IDENT_FLAG = currIdentFlag; //go back to the lest stage of the flag
				}
			}
			else if (root->hdr.tok == PLUS_EQ)
			{
				/* Plus equal assignment "+=" */
				/* e.g. x += 5; */
				if (!(is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0)) //case !(X+=0)  don't print anything in casee of 0
				{
					FLAG currIdentFlag = IDENT_FLAG;   //backup fot the last identifaier flag state.
					FLAG currValFlag = VAL_FLAG;	   //backup fot the last VAL_FLAG state.
					Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.
					IDENT_FLAG = ASSIGN_FLAG;		   //change the IDENT_FLAG to ASSIGN_FLAG
					ADDR_FLAG = True;				   //turn the adderes flag on
					code_recur(root->lnode);		   //addres
					ADDR_FLAG = lastAdderFlag;		   //go back to the lest stage of the flag

					code_recur(root->lnode);   //address value
					VAL_FLAG = ASSIGN_TO_FLAG; //change the VAL_FLAG to ASSIGN_TO_FLAG
					code_recur(root->rnode);   //added value
					if (PCODE)				   //print the pcode
					{
						printf("add\n"); //add operation
						printf("sto\n"); //store the new value
					}

					VAL_FLAG = currValFlag;		//go back to the lest stage of the flag
					IDENT_FLAG = currIdentFlag; //go back to the lest stage of the flag
				}
			}
			else if (root->hdr.tok == MINUS_EQ)
			{
				/* Minus equal assignment "-=" */
				/* e.g. x -= 5; */
				if (!(is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0)) //case !(X-=0)  don't print anything in case of 0
				{
					FLAG currIdentFlag = IDENT_FLAG;   //backup fot the last identifaier flag state.
					FLAG currValFlag = VAL_FLAG;	   //backup fot the last VAL_FLAG state.
					Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.
					IDENT_FLAG = ASSIGN_FLAG;		   //change the IDENT_FLAG to ASSIGN_FLAG
					ADDR_FLAG = True;				   //turn the adderes flag on
					code_recur(root->lnode);		   //addres
					ADDR_FLAG = lastAdderFlag;		   //go back to the lest stage of the flag

					code_recur(root->lnode); //addres value

					VAL_FLAG = ASSIGN_TO_FLAG; //change the VAL_FLAG to ASSIGN_TO_FLAG
					code_recur(root->rnode);   //decris value
					if (PCODE)				   //print the pcode
					{
						printf("sub\n"); //subtraction operation
						printf("sto\n"); //store the new value
					}

					VAL_FLAG = currValFlag;		//go back to the lest stage of the flag
					IDENT_FLAG = currIdentFlag; //go back to the lest stage of the flag
				}
			}
			else if (root->hdr.tok == STAR_EQ)
			{
				/* Multiply equal assignment "*=" */
				/* e.g. x *= 5; */
				FLAG currIdentFlag = IDENT_FLAG;   //backup fot the last identifaier flag state.
				FLAG currValFlag = VAL_FLAG;	   //backup fot the last VAL_FLAG state.
				Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.

				if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0) //case X*=0  don't print anything in cas of 0
				{
					IDENT_FLAG = ASSIGN_FLAG;  //change the IDENT_FLAG to ASSIGN_FLAG
					ADDR_FLAG = True;		   //turn the adderes flag on
					code_recur(root->lnode);   //addres
					ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag

					if (PCODE) //print the pcode
					{
						printf("ldc 0\n"); //load 0
						printf("sto\n");   //store the new value
					}
					IDENT_FLAG = currIdentFlag; //go back to the lest stage of the flag
				}

				else if (!(is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 1)) //case !(X*=1)  don't print anything in cas of 1
				{
					IDENT_FLAG = ASSIGN_FLAG;  //change the IDENT_FLAG to ASSIGN_FLAG
					ADDR_FLAG = True;		   //turn the adderes flag on
					code_recur(root->lnode);   //addres
					ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag
					code_recur(root->lnode);   //addres value

					VAL_FLAG = ASSIGN_TO_FLAG;
					code_recur(root->rnode); //multiplay value
					if (PCODE)				 //print the pcode
					{
						printf("mul\n"); //multiplay operation
						printf("sto\n"); //store the new value
					}

					VAL_FLAG = currValFlag;		//go back to the lest stage of the flag
					IDENT_FLAG = currIdentFlag; //go back to the lest stage of the flag
				}
			}
			else if (root->hdr.tok == DIV_EQ)
			{
				/* Divide equal assignment "/=" */
				/* e.g. x /= 5; */
				if (!(is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 1)) //!(case X/=1), don't print anything in cas of 1
				{
					FLAG currIdentFlag = IDENT_FLAG;   //backup fot the last identifaier flag state.
					FLAG currValFlag = VAL_FLAG;	   //backup fot the last VAL_FLAG state.
					Boolean lastAdderFlag = ADDR_FLAG; //backup fot the last addres flag state.
					IDENT_FLAG = ASSIGN_FLAG;		   //change the IDENT_FLAG to ASSIGN_FLAG
					ADDR_FLAG = True;				   //turn the adderes flag on
					code_recur(root->lnode);		   //addres
					ADDR_FLAG = lastAdderFlag;		   //go back to the lest stage of the flag

					code_recur(root->lnode); //value

					VAL_FLAG = ASSIGN_TO_FLAG; //change the VAL_FLAG to ASSIGN_TO_FLAG
					code_recur(root->rnode);   //sub addres value
					if (PCODE)				   //print the pcode
					{
						printf("div\n"); //Division operation
						printf("sto\n"); //store the new value
					}

					VAL_FLAG = currValFlag;		//go back to the lest stage of the flag
					IDENT_FLAG = currIdentFlag; //go back to the lest stage of the flag
				}
			}
			break;

		case TN_EXPR:
			switch (root->hdr.tok)
			{
			case B_AND:
				/* address token "&" */
				/* e.g. &x; */
				{
					Boolean lastAdderFlag = ADDR_FLAG; //change the VAL_FLAG to ASSIGN_TO_FLAG
					code_recur(root->lnode);

					ADDR_FLAG = True; //turn the adderes flag on
					code_recur(root->rnode);
					ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag
				}
				break;

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
					Boolean lastAdderFlag = ADDR_FLAG; //Backup the Addres flag
					ADDR_FLAG = True;				   //turn the adderes flag on
					code_recur(root->rnode);		   //load addres
					ADDR_FLAG = lastAdderFlag;		   //go back to the lest stage of the flag

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
					Boolean lastAdderFlag = ADDR_FLAG; //Backup the Addres flag
					code_recur(root->lnode);		   //load old value

					ADDR_FLAG = True;		   //turn the adderes flag on
					code_recur(root->lnode);   //load address
					ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag

					code_recur(root->lnode); //load value
					if (PCODE)				 //print the pcode
					{
						printf("inc 1\n"); //add 1 to the value at the top of the stack
						printf("sto\n");   //store the new value
					}
				}

				break;

			case DECR:
				/* Decrement token "--" */
				/*--x*/
				if (root->lnode == NULL)
				{
					Boolean lastAdderFlag = ADDR_FLAG; //Backup the Addres flag
					ADDR_FLAG = True;				   //turn the adderes flag on
					code_recur(root->rnode);		   //load addres
					ADDR_FLAG = lastAdderFlag;		   //go back to the lest stage of the flag

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
					Boolean lastAdderFlag = ADDR_FLAG; //Backup the Addres flag
					code_recur(root->lnode);		   //load old value

					ADDR_FLAG = True;		   //turn the adderes flag on
					code_recur(root->lnode);   //load address
					ADDR_FLAG = lastAdderFlag; //go back to the lest stage of the flag

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
				{
					if (is_constant(root, True) == True) //constant value
					{
						if (PCODE)
							printf("ldc %f\n", calculate_value(root, 0)); //print the constant value
					}

					else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) == 0) //case 0+X
						code_recur(root->rnode);															 //call for second item

					else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0) //case X+0
						code_recur(root->lnode);															 //call for second item

					else
					{
						code_recur(root->lnode); //call for first item
						code_recur(root->rnode); //call for second item
						if (PCODE)				 //print the pcode
							printf("add\n");	 //add operator
					}
				}
				break;

			case MINUS:
				/* Minus token "-" */
				if (root->lnode == NULL)
				{
					/*Onary negativ operetion*/
					if (is_constant(root->rnode, True) == True) //-(CONST)
					{
						if (PCODE)
							printf("ldc %f\n", calculate_value(root->rnode, 0)); //print the constant value
					}

					else
					{
						code_recur(root->lnode);
						code_recur(root->rnode); //call for an item
						if (PCODE)				 //print the pcode
							printf("neg\n");	 //negativ operator
					}
				}

				else
				{
					/*Subtraction operator*/
					if (is_constant(root, True) == True) //constant value
					{
						if (PCODE)
							printf("ldc %f\n", calculate_value(root, 0)); //print the constant value
					}

					else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0) //case X-0
						code_recur(root->lnode);															 //call for second item

					else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) == 0) //case X-0
					{
						code_recur(root->rnode); //call for second item
						if (PCODE)				 //print the pcode
							printf("neg\n");	 //negativ operator
					}
					else
					{
						code_recur(root->lnode); //call for first item
						code_recur(root->rnode); //call for second item
						if (PCODE)				 //print the pcode
							printf("sub\n");	 //subtract operator
					}
				}
				break;

			case DIV:
				/* Divide token "/" */
				{
					if (is_constant(root, True) == True) //constant value
					{
						if (PCODE)
							printf("ldc %f\n", calculate_value(root, 0)); //print the constant value
					}

					else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 1) //case X/1
						code_recur(root->lnode);															 //call for second item

					else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) == 0) //case 0/X
					{
						if (PCODE)
							printf("ldc 0\n"); //call for second item
					}

					else
					{
						code_recur(root->lnode); //call for first item
						code_recur(root->rnode); //call for second item
						if (PCODE)				 //print the pcode
							printf("div\n");	 //Division operation
					}
				}
				break;

			case STAR:
				/* multiply token "*" */
				{

					if (is_constant(root, True) == True) //constant value
					{
						if (PCODE)
							printf("ldc %f\n", calculate_value(root, 0)); //print the constant value
					}

					else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) == 1) //case X*1
						code_recur(root->rnode);

					else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 1) //case 1*X
						code_recur(root->lnode);

					else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) == 0) //case X*0
					{
						if (PCODE)
							printf("ldc 0\n"); //print 0
					}

					else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0) //case 0*X
					{
						if (PCODE)
							printf("ldc 0\n"); //print 0
					}

					else
					{
						code_recur(root->lnode); //call for first item
						code_recur(root->rnode); //call for second item
						if (PCODE)				 //print the pcode
							printf("mul\n");	 //multiplay operator
					}
				}
				break;

			case AND:
				/* And token "&&" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) && calculate_value(root->rnode, 0)); //print the constant value
				}

				else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) != 0) //case X&&True
					code_recur(root->rnode);

				else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) != 0) //case True&&X
					code_recur(root->lnode);

				else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) == 0) //case X&&False
				{
					if (PCODE)
						printf("ldc 0\n"); //print 0
				}

				else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0) //case False&&X
				{
					if (PCODE)
						printf("ldc 0\n"); //print 0
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("and\n");	 //and operator
				}
				break;

			case OR:
				/* Or token "||" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) || calculate_value(root->rnode, 0)); //print the constant value
				}

				else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) == 0) //case X||False
					code_recur(root->rnode);

				else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0) //case False||X
					code_recur(root->lnode);

				else if (is_constant(root->lnode, True) == True && calculate_value(root->lnode, 0) != 0) //case X||True
				{
					if (PCODE)
						printf("ldc 1\n"); //print 1
				}

				else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) != 0) //case True||X
				{
					if (PCODE)
						printf("ldc 1\n"); //print 1
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("or\n");		 //or operator
				}
				break;

			case NOT:
				/* Not token "!" */
				if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) == 0) //!(FALSE)
				{
					if (PCODE)
						printf("ldc 1\n"); //print 1
				}

				else if (is_constant(root->rnode, True) == True && calculate_value(root->rnode, 0) != 0) //!(TRUE)
				{
					if (PCODE)
						printf("ldc 0\n"); //print 0
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //-1 //call for second item
					if (PCODE)				 //print the pcode
						printf("not\n");	 //not operator
				}
				break;

			case GRTR:
				/* Greater token ">" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) > calculate_value(root->rnode, 0));
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("grt\n");	 //greater then operator
				}
				break;

			case LESS:
				/* Less token "<" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) < calculate_value(root->rnode, 0));
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("les\n");	 //less grater then operator
				}
				break;

			case EQUAL:
				/* Equal token "==" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) == calculate_value(root->rnode, 0));
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("equ\n");	 //equal operator
				}
				break;

			case NOT_EQ:
				/* Not equal token "!=" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) != calculate_value(root->rnode, 0));
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("neq\n");	 //not equal operator
				}
				break;

			case LESS_EQ:
				/* Less or equal token "<=" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) <= calculate_value(root->rnode, 0));
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("leq\n");	 //less or equal operator
				}
				break;

			case GRTR_EQ:
				/* Greater or equal token ">=" */
				if (is_constant(root, True) == True) //constant value
				{
					if (PCODE)
						printf("ldc %d\n", calculate_value(root->lnode, 0) >= calculate_value(root->rnode, 0));
				}

				else
				{
					code_recur(root->lnode); //call for first item
					code_recur(root->rnode); //call for second item
					if (PCODE)				 //print the pcode
						printf("geq\n");	 // Great equal operator
				}
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
				Loop_stack *currLoop = NULL;
				push_loop(&STK_LOOP, "while", while_Count); //push a new while loop to the loop stack
				while_Count++;								//Advance the label counter for the following calls.

				if (PCODE)										   //print the pcode
					printf("while_LOOP_%d:\n", STK_LOOP->depth);   // the begining of the 'while' loop
				code_recur(root->lnode);						   //condition
				if (PCODE)										   //print the pcode
					printf("fjp while_END_%d\n", STK_LOOP->depth); //if the confition is false jump to remain the code(go out of the loop)

				code_recur(root->rnode); //body
				if (PCODE)				 //print the pcode
				{
					printf("ujp while_LOOP_%d\n", STK_LOOP->depth); //jump anconditionaly to te beginning of the loop
					printf("while_END_%d:\n", STK_LOOP->depth);		// reamin the code
				}
				currLoop = pop_loop(&STK_LOOP); //pop the do-while component from the loop stack
				free(currLoop);					//free the allocated memory for the do-while component
			}
			break;

		case TN_DOWHILE:
			/* Do-While case */
			{
				Loop_stack *currLoop = NULL;
				push_loop(&STK_LOOP, "do_while", do_while_Count); //push a new while loop to the loop stack
				do_while_Count++;								  //Advance the label counter for the following calls.

				if (PCODE)											//print the pcode
					printf("do-while_LOOP_%d:\n", STK_LOOP->depth); // the begining of the 'do-while' loop
				code_recur(root->rnode);							//body

				code_recur(root->lnode); //condition
				if (PCODE)				 //print the pcode
				{
					printf("fjp do-while_END_%d\n", STK_LOOP->depth);  //if the confition is false jump to remain the code(go out of the loop)
					printf("ujp do-while_LOOP_%d\n", STK_LOOP->depth); //jump anconditionaly to te beginning of the loop
					printf("do-while_END_%d:\n", STK_LOOP->depth);	   // reamin the code
				}
				currLoop = pop_loop(&STK_LOOP); //pop the do-while component from the loop stack
				free(currLoop);					//free the allocated memory for the do-while component
			}
			break;

		case TN_LABEL:
		{

			if (CONST_SWITCH_FLAG == True) //case of a constant switch
			{
				if (is_constant(root->lnode->rnode, True) == True && calculate_value(root->lnode->rnode, 0) == CONST_SWITCH) //if a constant value and the same case name leav only the "then" part.
					code_recur(root->rnode);
			}

			else
			{
				if (PCODE) //print the cases name
				{
					printf("switch%d_case%d:\n", STK_LOOP->depth, STK_LOOP->Case_count);
					printf("dpl\n");
				}
				STK_LOOP->Case_count++; //update the case counter

				code_recur(root->lnode);

				if (PCODE) //print the end of the case
				{
					printf("equ\n");
					printf("fjp switch%d_case%d\n", STK_LOOP->depth, STK_LOOP->Case_count);
				}

				code_recur(root->rnode);
			}
		}

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
=============================================================================================================================================================================================================================================
Exilary Functions 
*/

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
	ST_free(ST);		// releas the symbol table content
	STRC_free(STRCTab); // releas the struct table content
}

/*
*	Input: Struct table
*	Output: prints the Struct Table on the console
*	This function prints all the variables on your symbol table with their data
*/
void Print_STRC(Structs_table *STRC)
{
	Struct_types *currSTRC = STRC->head;
	Variable *currVal = currSTRC->head;

	if (STRC != NULL)
	{

		printf("============================Struct Table============================\n");
		while (currSTRC->next != NULL) //move over thw struct table
		{
			printf("struct name: %s \t|\t struct size: %d\n", currSTRC->name, currSTRC->size);
			printf("-----------------------------\n");

			while (currVal->next != NULL) //move over the vareabl component
			{
				printf("val name: %s \t|\t val size: %d \t|\t val array size: %d\n", currVal->name, currVal->size, currVal->arraySize);
				currVal = currVal->next;
			}
			printf("val name: %s \t|\t val size: %d \t|\t val array size: %d\n", currVal->name, currVal->size, currVal->arraySize);
			currSTRC = currSTRC->next;
			currVal = currSTRC->head;
		}
		//print the last stract in the struct table
		printf("struct name: %s \t|\t struct size: %d\n", currSTRC->name, currSTRC->size);
		printf("-----------------------------\n");
		while (currVal->next != NULL) //move over the vareabl component
		{
			printf("val name: %s \t|\t val size: %d \t|\t val array size: %d\n", currVal->name, currVal->size, currVal->arraySize);
			currVal = currVal->next;
		}
		printf("val name: %s \t|\t val size: %d \t|\t val array size: %d\n", currVal->name, currVal->size, currVal->arraySize);
		printf("-----------------------------\n");
	}
	printf("============================End Struct Table============================\n");
}

/*Internal function*/

/********************Struct Table functiuns********************/
/*
*Input: None
*Output: None
*Function to initialize a new Struct Table.
*/
Structs_table *STRCTab_init()
{
	Structs_table *STRCTab = (Structs_table *)malloc(sizeof(Structs_table)); //allocate a new Struct table in mamory.

	if (!STRCTab) //if coldent allocate right show error.
	{
		printf("Cannot allocate memory.\n");
		exit(1);
	}

	/*resat the Struct Table valuse to NULL*/
	STRCTab->head = NULL;
	STRCTab->tail = NULL;

	return STRCTab;
}

/*
*Input: Struct Table, Struct name (string) 
*Output: None
*Function how add a new Struct_types node with the received varebels to the Struct Table.  
*/
void STRC_add(Structs_table *STRCTab, char *resName)
{
	Struct_types *new = (Struct_types *)malloc(sizeof(Struct_types)); //allocate a new Struct node in mamory.

	if (!new) //if coldent allocate right show error.
	{
		printf("Cannot allocate memory.\n");
		exit(1);
	}

	/*resat the struct valuse*/
	new->name = resName;
	new->head = NULL; //init the pointer for the data strucer.
	new->next = NULL; //init the pointer of the last node in the linked list.
	new->size = -1;	  //init the struct value to -1 as a flag for a new struct how havent finish his declaration.

	/*add to empty list*/
	if (STRCTab->head == NULL)
	{
		new->prev = NULL;
		STRCTab->head = new; //update the head pointer
		STRCTab->tail = new; //update the tail pointer

		return;
	}

	/*add to the end of the list*/
	STRCTab->tail->next = new; // add the new node to the end of the list
	new->prev = STRCTab->tail;
	STRCTab->tail = new; //update the tail pointer
}

/*
*Input: Struct Table, Struct name (string), Variable name (string), pointer to array (pointer)
*Output: None
*Function how add a new Variable node with the received varebels to the Struct in the Struct Table.  
*/
void STRC_add_var(Structs_table *STRCTab, char *key, char *resName, int *array)
{
	Struct_types *STRC = STRC_find(STRCTab, key);		  //pointer to the right struct node in the struct table
	Variable *new = (Variable *)malloc(sizeof(Variable)); //allocate a new node in mamory.
	if (!new)											  //if coldent allocate right show error.
	{
		printf("Cannot allocate memory.\n");
		exit(1);
	}

	if (STRCTab == NULL) //if the struct table is empty show error.
	{
		printf("Struct table is empty.\n");
		exit(1);
	}

	/*Stores the data of the new node.*/
	new->name = resName;
	new->type = TYPE;
	new->ptrLvl = POINTER_COUNTER;
	new->arraySize = MAX_ARRAY_COUNT;
	new->arrayHead = array;

	if (TYPE != NULL && (strcmp(TYPE, "int") == 0 || strcmp(TYPE, "double") == 0 || strcmp(TYPE, "float") == 0 || POINTER_COUNTER > 0)) //if the new component is from a primitiv type give it a size of 1.
		new->size = 1;

	else //if the new component is'nt from a primitiv type so its a struct type and we put -1 as a flag to so it.
		new->size = -1;

	new->STRC_head = NULL; //reaset the STRC_head.
	new->next = NULL;	   //predesesor node pointer

	/*add to empty list*/
	if (STRC->head == NULL)
	{
		new->prev = NULL;
		STRC->head = new; //update the head pointer
		STRC->tail = new; //update the tail pointer

		return;
	}

	/*add to the end of the list*/
	STRC->tail->next = new; // add the new node to the end of the list
	new->prev = STRC->tail;
	STRC->tail = new; //update the tail pointer
}

/*
*Input: Struct Table, Struct name (string), dimantion (ineger), index for the array (ineger). 
*Output: None
*Function how save the array dimantions and the array dimantions size.  
*/
void STRCarrayDecl(Structs_table *STRCTab, char *key, int Dim, int i)
{

	Struct_types *findSRTC = STRC_find(STRCTab, STRC_NAME); //pointer to the right struct node in the struct table
	Variable *findVar = findSRTC->head;						//pointer to the head of the given struct components.

	while (findVar != NULL) //move over the struruct components to find the right component to store the array dimantions
	{
		if (strcmp(findVar->name, key) == 0) //chak if is it the right component
		{
			findVar->arrayHead[i] = Dim; //store the dimantions in the dimantians array.
			findVar->size *= Dim;		 //update the Variable size.

			return;
		}
		findVar = findVar->next;
	}
	printf("Coulden't complit the array decleration.");
}

/*
*Input: Struct Table, Struct name (string) 
*Output: Struct_types pointer
*Function how add finds the right struct in the Structs Table.  
*/
Struct_types *STRC_find(Structs_table *STRCTab, char *key)
{

	Struct_types *curr = STRCTab->head; //Pointers to move on the structs table list.

	/*a case whan the structs table is empty*/
	if (STRCTab->head == NULL)
	{
		printf("We coudent find your struct type.\n");
		exit(1);
	}

	/*Find the node with the key value in the structs table or tell you if it dosen't there*/
	while (strcmp(curr->name, key) != 0)
	{
		if (curr == NULL) //if we moved over the all list and colden't find the key.
		{
			printf("Given name have not found in the Struct Table.\n");
			exit(1);
		}
		curr = curr->next; //Promotes the pointer to the table of structs for the node before it.
	}
	return curr; //return the value with the key value.
}

/*
*Input: Struct Table
*Output: None
*Function how moves over the struct table and update the size of every structs in it.  
*/
void STRC_size(Structs_table *STRCTab)
{
	Struct_types *STRC = STRCTab->head; //pointer to the fitst struct in the Struct table
	Struct_types *find;					//pointer to hold search over other structs if needed
	Variable *curr;						//pointer to the current Variable node.

	if (STRCTab->head == NULL) //chack if the struct table is empty, if so show error.
	{
		printf("Struct Table is empty.\n");
		exit(1);
	}
	while (STRC != NULL) //move over the struct table , untill the end of it.
	{
		STRC->size = 0;		 //init the current size to 0.
		curr = STRC->head;	 //pointer to the struct components
		while (curr != NULL) //move over all the struct components, sum there sizes and store it as the struct size.
		{
			if (curr->size == -1) //if the component is a struct type.
			{

				find = STRC_find(STRCTab, curr->type); //find the struct in the struct table
				if (find->size != -1)				   // if the struct that have foud alrade have a size (not a-1 flag) sum it.
				{
					curr->size = find->size;
					STRC->size += curr->size;
				}

				else													//if the struct size that have found didn't calculeted yet.
					STRC->size += STRC_size_recur(STRCTab, curr->type); //calculate the struct sizes that have found.
			}

			else //if the component is isent a struct type sum it to struct size.
				STRC->size += curr->size;

			curr = curr->next; //move to the next component
		}
		STRC = STRC->next; //move to the next struct
	}
}

/*
*Input: Struct Table, Struct name (string)
*Output: size (integer)
*Function how help to find and calculate the struct component size if it a struct type.  
*/
int STRC_size_recur(Structs_table *STRCTab, char *key)
{

	Struct_types *STRC = STRC_find(STRCTab, key); //find the struct in the struct table
	Variable *curr = STRC->head;				  //pointer to the current Variable node.
	int size = 0;								  //holde the calculated struct size.
	while (curr != NULL)
	{
		if (curr->size == -1)							  //chaack if the camponent is a struct type
			size += STRC_size_recur(STRCTab, curr->type); // a ricorsicve call to find a atruct type camponent size

		else //if the camponent is primitiv so sum it.
			size += curr->size;
		curr = curr->next; //go to the next component
	}
	return size; //return the calculated size of the component.
}

/*
*Input: pointer to a node we whant to delete (Variable*). 
*Output: None
*Function to remove a struct type, can be used to deley a struct node from the symbal tabel ot from a struct table.
*/
void STRC_rmv(Variable *del)
{
	Variable *temp; //temporery pointer

	while (del->next != NULL) //move to the last incremant in the table
		del = del->next;

	while (del->prev != NULL) //move trowed the first incremant in the table
	{
		if (del->STRC_head != NULL) //delete an streuct incremant
		{
			STRC_rmv(del->STRC_head); //recursiv call for that incremant
			del->STRC_head = NULL;
		}

		if (del->arraySize > 0) //delete an array incremant
		{
			free(del->arrayHead);
			del->arrayHead = NULL;
		}

		/*delink the incremant and free it allocated memory */
		temp = del;
		del = del->prev;
		del->next = NULL;
		temp->prev = NULL;
		free(temp);
	}
}

/*
Input: Struct Table
Output: None.
* Function how delete the given Struct tabel and release the allocated memaory. 
*/
void STRC_free(Structs_table *STRCTab)
{
	Struct_types *curr = STRCTab->tail, *del; //poiners to the tail of the struct tabel a pointer to the node to be delited

	while (curr->prev != NULL)
	{
		STRC_rmv(curr->tail);
		del = curr;
		curr = curr->prev; //Move the list pointer backwords
		curr->next = NULL; //reles the last node frop the linked list
		del->prev = NULL;  //reles the last node frop the linked list

		free(del); //reles the last node allocated memory.
	}

	/*delete the struct table pointers  and free it allocated memory*/
	(STRCTab->tail) = NULL;
	(STRCTab->head) = NULL;
	free(STRCTab);
}

/********************Symbol Table functiuns********************/
/*
*Input: None
*Output: None
*Function to initialize a new Symbal Table.
*/
Symbol_table *ST_init()
{
	Symbol_table *ST = (Symbol_table *)malloc(sizeof(Symbol_table)); //allocate a new Symbal table in mamory.

	if (!ST) //if coldent allocate right show error.
	{
		printf("Cannot allocate memory.\n");
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
void ST_add(Symbol_table *ST, Structs_table *STRCTab, char *resName, int *array)
{
	Variable *new = (Variable *)malloc(sizeof(Variable)); //allocate a new node in mamory.
	Struct_types *STRC;

	if (!new) //if coldent allocate right show error.
	{
		printf("Cannot allocate memory.\n");
		exit(1);
	}

	/*Stores the data of the new node.*/
	new->name = resName;
	new->type = TYPE;
	new->addr = LAST_ADDER;
	new->ptrLvl = POINTER_COUNTER;
	new->nstLvl = NESTED_LEVEL;		  //add the nested level of the carent block.
	new->arraySize = MAX_ARRAY_COUNT; // stor the size of the dimantions array, 0 if non array type.
	new->arrayHead = array;			  //update the dimantions array if any , for non arry type hold NULL.
	new->STRC_head = NULL;

	/*update the size*/
	if (TYPE != NULL && (strcmp(TYPE, "int") == 0 || strcmp(TYPE, "double") == 0 || strcmp(TYPE, "float") == 0))
		new->size = 1;

	else //in case of srtuct type variable
	{
		STRC = STRC_find(STRCTab, TYPE); //find the struct in the struct table

		if (POINTER_COUNTER == 0)
			new->size = STRC->size; //
		else
			new->size = 1;
	}

	if (MAX_ARRAY_COUNT == 0)
		LAST_ADDER += new->size; //update the LAST_ADDER count
	new->next = NULL;			 //predesesor node pointer

	/*add to empty list*/
	if (ST->head == NULL)
	{
		new->prev = NULL; //update the pointer for the prev to hold NULL.
		ST->head = new;	  //update the head pointer
		ST->tail = new;	  //update the tail pointer

		return;
	}

	/*add to the end of the list*/
	ST->tail->next = new; // add the new node to the end of the list
	new->prev = ST->tail;
	ST->tail = new; //update the tail pointer
}

/*
*Input: Symbol Table, Variable name (string), dimantion (ineger), index for the array(ineger). 
*Output: None
*Function how save the array dimantions and the array dimantions size.  
*/
void STarrayDecl(Variable *find, int Dim, int i)
{
	find->arrayHead[i] = Dim; //store the dimantions in the dimantians array.
	find->size *= Dim;		  //update the Variable size.

	if (i == MAX_ARRAY_COUNT - 1)
		LAST_ADDER += find->size; //update the LAST_ADDER
}

/*
*Input: Symbal Table and a pointer to a node we whant to delete (Variable*). 
*Output: None
*Function to remove a given node from the Symbal Tabel.
*/
void ST_rmv(Symbol_table *ST, Variable *del)
{
	if (del->type != NULL && !(strcmp(del->type, "int") == 0 || strcmp(del->type, "double") == 0 || strcmp(del->type, "float") == 0)) //if the node is a struct
		STRC_rmv(del->STRC_head);																									  //remove the struct type node.

	if (del->arraySize > 0) //case for array
	{
		free(del->arrayHead); //free the space how allocated for the dimantion array
		del->arrayHead = NULL;
	}
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
		printf("We coudent find your Variable.\n");
		exit(1);
	}

	/*Find the node with the key value in the symbol table or tell you if it dosen't there*/
	while (strcmp(curr->name, key) != 0)
	{
		if (curr->prev == NULL) //if we moved over the all list and colden't find the key.
		{
			printf("Given name have not found in the Symbol Table.\n");
			exit(1);
		}
		curr = curr->prev; //Promotes the pointer to the table of symbols for the node before it.
	}
	return curr; //return the value with the key value.
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*Input: Variable and a key value (name string), pointer to int counter (pointer)
*Output: Pointer to the requested link or error message if not.
* Finds the value in the table of symbols with the same key value. Returns a pointer to the node if it finds it and update the pointer counter value.
*if the component is not in the symbol table add it to the symbal table.
*/
Variable *find_STRC_comp(Variable *var, Structs_table *STRCTab, char *key, int *countSTRC)
{

	Variable *curr = var->STRC_head; //Pointers to move on the symbol table list.
	Boolean flag = False;
	Struct_types *STRC = STRC_find(STRCTab, var->type);
	Variable *find = STRC->head;
	int count = 0;

	/********************Search in the symbol table********************/
	/*a case whan the symbol table is empty*/
	if (var != NULL)
	{
		/*Find the node with the key value in the symbol table or tell you if it dosen't there*/
		while (curr != NULL && strcmp(curr->name, key) != 0)
			curr = curr->next; //Promotes the pointer to the table of symbols for the node before it.

		if (curr == NULL) //if we moved over the all list and colden't find the key.
			flag = True;

		/********************Search in the Struct Table********************/
		/*a case whan the structs table is empty*/
		if (STRCTab->head == NULL)
		{
			printf("The Struct table is empty.\n");
			exit(1);
		}

		/*Find the node with the key value in the structs table or tell you if it dosen't there*/
		while (strcmp(find->name, key) != 0)
		{
			if (find == NULL) //if we moved over the all list and colden't find the key.
			{
				printf("Given name have not found in the Struct Table.\n");
				exit(1);
			}
			if (find->arraySize == 0)
				count++;
			else
				count += find->size;

			find = find->next; //Promotes the pointer to the table of structs for the node before it.
		}

		if (countSTRC != NULL)
			*countSTRC = count;
	}

	if (curr == NULL) //if the node is not in the symbol table add it to the symbol table.
	{
		Variable *new = (Variable *)malloc(sizeof(Variable)); //allocate a new node in mamory.

		/*Stores the data of the new node.*/
		new->name = find->name;
		new->type = find->type;
		new->size = find->size;
		new->ptrLvl = find->ptrLvl;
		new->nstLvl = NESTED_LEVEL; //add the nested level of the carent block.
		new->addr = LAST_ADDER;		//pramote the addresss for the value in the symbal table.
		new->arraySize = find->arraySize;
		new->arrayHead = find->arrayHead;
		new->STRC_head = NULL;
		new->prev = NULL;
		new->next = var->STRC_head; //predesesor node pointer

		if (var->STRC_head != NULL)
			var->STRC_head = var->STRC_head->next;

		return new;
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
	Variable *curr = ST->tail, *del; //pointers to move on the symbol table list

	while (curr->prev != NULL) //A loop for going over the list and deleting each node in it from end to beginning.
	{
		if (curr->type != NULL && !(strcmp(curr->type, "int") == 0 || strcmp(curr->type, "double") == 0 || strcmp(curr->type, "float") == 0)) //if the node is a struct
			STRC_rmv(curr->STRC_head);																										  //remove the struct type node.
		del = curr;
		curr = curr->prev; //Move the list pointer backwords
		curr->next = NULL; //reles the last node frop the linked list
		del->prev = NULL;  //reles the last node frop the linked list

		free(del); //reles the last node allocated memory.
	}

	free(curr); //releas the first and only remain node allocated memory.

	/*reset the Symbal table pointers */
	(ST->tail) = NULL;
	(ST->head) = NULL;
	free(ST); //reles  allocated memory for the sumbal table.
}

/********************Loop Stack functiuns********************/
/*
* Input: Loop stack (pointer to pointer) , type (String), depth (int)
* Output: None.
* Function how push a new loop type to the Loop Stack. 
*/
void push_loop(Loop_stack **stack, char *type, int depth)
{
	Loop_stack *new = (Loop_stack *)malloc(sizeof(Loop_stack)); // allocate new memory for the loop
	if (new == NULL)
	{
		printf("Cannot Allocate Memory");
		exit(1);
	}

	/*Stores the data of the new node.*/
	new->depth = depth;
	new->type = type;
	new->Case_count = 0;

	if ((*stack) == NULL) //first node in the stack
	{
		new->next = NULL;
		(*stack) = new;
		return;
	}
	else //any other node in the stack
	{
		new->next = (*stack);
		*stack = new;
		return;
	}
}

/*
*Input: Loop stack (pointer to pointer) 
*Output: Last added loop in the Loop Stack (Loop_stack pointer).
* Function how returns and delete the head loop from the Loop Stack. 
*/
Loop_stack *pop_loop(struct Loop_stack **stack)
{
	Loop_stack *pop = (*stack); //hold the head of the stack

	if ((*stack) != NULL) //if the stack is not empty
		*stack = (*stack)->next;

	pop->next = NULL; //cut of the head from the stack

	return pop; //turn the pop value
}

/********************Optimazition functiuns********************/

/*
* Input: Sub-Tree root (Tree) , Val (Boolean)
* Output: Boolean valeu to determent if the sub-tree is constant or not.
* Function how determent if the sub-tree that it gets is constent. 
*/
Boolean is_constant(treenode *root, Boolean val)
{
	/*Base conditions*/
	if (root->hdr.which == LEAF_T)
	{
		leafnode *leaf = (leafnode *)root; //cast the root type
		if (root == NULL)				   //if the leaf is a NULL node
			val = True;

		else if (!(leaf->hdr.type == TN_INT || leaf->hdr.type == TN_REAL)) //in case of non primitive value
			val = False;
	}
	else
	{
		/*Recurtion call*/
		Boolean x = is_constant(root->lnode, val);
		Boolean y = is_constant(root->rnode, val);

		/*aretmetics cases to make a variable a constant */
		if (root->hdr.tok == STAR && calculate_value(root->lnode, 0) == 0) //0*X multypal by 0
			val = True;

		else if (root->hdr.tok == STAR && calculate_value(root->rnode, 0) == 0) //X*0 multypal by 0
			val = True;

		else if (root->hdr.tok == DIV && calculate_value(root->lnode, 0) == 0) //0/X Divide 0
			val = True;

		else if (root->hdr.tok == AND && (calculate_value(root->lnode, 0) == 0 || calculate_value(root->rnode, 0) == 0)) // X&&(false) OR (false)&&X
			val = True;

		else
		{
			val = val && x && y; //for any other case a backtracking.
		}
	}
	return val;
}

/*
* Input: Sub-Tree root (Tree) , Val (Boolean)
* Output: Float valeu of the sub-tree components.
* Function how calculete the value of the sub-tree. 
*/
float calculate_value(treenode *root, float val)
{
	/*variable for casting*/
	if_node *ifn;
	leafnode *leaf;
	Variable *VAR;

	/*Base conditions*/
	if (root->hdr.which == LEAF_T)
	{
		/*return the value in the leaf*/
		leafnode *leaf = (leafnode *)root;

		if (leaf->hdr.type == TN_INT)
			val = (float)leaf->data.ival; //return the "Integer" type value

		else if (leaf->hdr.type == TN_REAL)
			val = leaf->data.dval; //return the "Real" type value

		else if (leaf->hdr.type == TN_IDENT)
		{
			VAR = ST_find(ST, leaf->data.str); //find the VAR in the Struct Table
			return VAR->value;				   //return the value holds in the VAR
		}
	}

	/*case of a "condition expretion" ((cond)?(exp):(exp);)*/
	else if (root->hdr.type == TN_COND_EXPR)
	{
		ifn = (if_node *)root;
		if (is_constant(ifn->cond, True) && calculate_value(ifn->cond, val) != 0) //case for "True"
		{
			if (is_constant(ifn->then_n, True)) //if constant return the calculeted value of "then"
				return calculate_value(ifn->then_n, val);
		}
		else if (is_constant(ifn->cond, True) && calculate_value(ifn->cond, val) == 0) //case for "False"
		{
			if (is_constant(ifn->else_n, True)) //if constant return the calculeted value of "else"
				return calculate_value(ifn->else_n, val);
		}
	}

	else
	{
		float x, y;

		/*Recurtion call*/
		if (root->lnode != NULL)
			x = calculate_value(root->lnode, val);
		if (root->rnode != NULL)
			y = calculate_value(root->rnode, val);

		/*for any other case a backtracking withe the right aritmetic operation.*/
		switch (root->hdr.tok)
		{
		case PLUS_EQ: /* Plus equal assignment "+=" */
			val = x + y;
			break;

		case MINUS_EQ: /* Minus equal assignment "-=" */
			val = x - y;
			break;

		case STAR_EQ:			  /* Multiplay equal assignment "*=" */
			if (x == 0 || y == 0) //multiplay by 0
				val = 0;

			val = x * y;
			break;

		case DIV_EQ: /* Divide equal assignment "/=" */
			if (y == 0)
			{
				printf("Cannot divide by 0.\n");
				exit(1);
			}
			else if (x == 0) //divid 0
				val = 0;

			val = x / y;
			break;

		case PLUS: /* Plus operator "+" */
			val = x + y;
			break;

		case MINUS: /* Minus operator "-" */
			val = x - y;
			break;

		case DIV:		/* Divide operator "/" */
			if (y == 0) //case for a divid by 0.
			{
				printf("Cannot divide by 0.\n");
				exit(1);
			}
			else if (x == 0) //divid 0
				val = 0;

			val = x / y;
			break;

		case STAR:				  /* Multipolition operator "*" */
			if (x == 0 || y == 0) //multiplay by 0
				val = 0;

			val = x * y;
			break;

		case AND:
			if (x == 0 || y == 0) //AND wite (flase)
				val = 0;

			val = x && y;
			break;

		case OR:
			if (x != 0 || y != 0) //OR with (True)
				val = 1;

			val = x || y;
			break;

		case GRTR: /* Greater token ">" */
			val = x > y;
			break;

		case LESS: /* Less token "<" */
			val = x < y;
			break;

		case EQUAL: /* Equal token "==" */
			val = x == y;
			break;

		case NOT_EQ: /* Not equal token "!=" */
			val = x != y;
			break;

		case LESS_EQ: /* Less or equal token "<=" */
			val = x <= y;
			break;

		case GRTR_EQ: /* Greater or equal token ">=" */
			val = x >= y;
			break;
		}
	}

	return val;
}
