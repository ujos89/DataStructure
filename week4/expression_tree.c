#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *createTree( void);

/* Deletes all data in tree and recycles memory
*/
void destroyTree( TREE *pTree);

static void _destroy( NODE *root);

/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE *_makeNode( char ch);

/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree( char *expr, TREE *pTree);

/* Print node in tree using inorder traversal
*/
void traverseTree( TREE *pTree);

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse( NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);

/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix( char *expr);

////////////////////////////////////////////////////////////////////////////////
void destroyTree( TREE *pTree)
{
	if (pTree)
	{
		_destroy( pTree->root);
	}
		
	free( pTree);
}

////////////////////////////////////////////////////////////////////////////////
void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree( TREE *pTree)
{
	_traverse(pTree->root);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];
	
	fprintf( stdout, "\nInput an expression (postfix): ");
	
	while (fscanf( stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();
		
		if (!tree)
		{
			printf( "Cannot create tree\n");
			return 100;
		}
		
		// postfix expression -> expression tree
		int ret = postfix2tree( expr, tree);
		if (!ret)
		{
			fprintf( stdout, "invalid expression!\n");
			destroyTree( tree);
			continue;
		}

		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree( tree);

		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree(tree);

		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);

		// destroy tree
		destroyTree( tree);

		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}

TREE *createTree( void){
	TREE* tree = (TREE *)malloc(sizeof(TREE));
	tree -> root = NULL;
	
	return tree;
}

int postfix2tree( char *expr, TREE *pTree){
	int i = 0, idx = -1, top_idx = -1;
	NODE* stack[MAX_STACK_SIZE];
	NODE* temp;
	
	while( expr[i] != '\0' ){
		if( isdigit(expr[i]) ){
			temp = _makeNode(expr[i]);
			idx++;
			stack[idx] = temp;
		}
		else{
			//impossible to pop 2 element
			if(idx < 1){
				if(idx == 0)
					_destroy(stack[idx]);
				pTree -> root = NULL;
				return 0;
			}
			else{
				temp = _makeNode(expr[i]);
				temp -> right = stack[idx];
				temp -> left = stack[idx-1];
				stack[idx] = NULL;
				stack[idx-1] = temp;
				idx--;
				pTree -> root = temp;
				top_idx = idx;
			}
		}
		i++;
	}
	
	//stack[0] is not root
	if( top_idx != 0 ){
		while(idx >= 0){
			_destroy(stack[idx]);
			stack[idx] = NULL;
			idx--;
		}
		pTree -> root = NULL;
		return 0;
	}

	return 1;
}

static NODE *_makeNode( char ch){
	NODE* temp = (NODE *)malloc(sizeof(NODE));
	temp -> left = NULL;
	temp -> right = NULL;
	temp -> data = ch;
	return temp;
}

static void _destroy( NODE *root){
	if (root == NULL)	return;
	
	_destroy( root -> left );
	_destroy( root -> right );
	free(root);
}

static void _traverse( NODE *root){
	if(root != NULL){
		if( isdigit(root->data) )
			printf("%c", root->data);
		
		else{
			printf("(");
			_traverse(root->left);
			printf("%c", root->data);
			_traverse(root->right);
			printf(")");
		}
	}
}

static void _infix_print( NODE *root, int level){
	if(root != NULL){
		_infix_print( root -> right, level+1 );

		for (int i = 0; i < level; i++)	printf("\t");
		printf("%c\n", root -> data);
		
		_infix_print( root -> left, level+1 );
	}
}

float evalPostfix( char *expr){
	int i = 0, idx = -1;
	char temp;
	float stack[MAX_STACK_SIZE];
	
	while(expr[i] != '\0'){
		temp = expr[i];
		
		if( isdigit(expr[i]) ){
			idx++;
			stack[idx] = atoi(&temp);
		}
		else{
			if( temp == '+'){
				stack[idx-1] = stack[idx-1] + stack[idx];
				stack[idx] = 0;
				idx--;
			}
			else if( temp == '-'){
				stack[idx-1] = stack[idx-1] - stack[idx];
				stack[idx] = 0;
				idx--;
			}
			else if( temp == '*'){
				stack[idx-1] = stack[idx-1] * stack[idx];
				stack[idx] = 0;
				idx--;
			}
			else if( temp == '/'){
				stack[idx-1] = stack[idx-1] / stack[idx];
				stack[idx] = 0;
				idx--;
			}
		}
		i++;
	}

	return stack[0];
}
