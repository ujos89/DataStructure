#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*token;
	int		freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos; // unused
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( void);

/* Deletes all data in list and recycles memory
*/
void destroyList( LIST *pList);

/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/
int addNode( LIST *pList, tTOKEN *dataInPtr);

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut);

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut);

/* returns number of nodes in list
*/
int countList( LIST *pList);

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList);

//int fullList( LIST *pList);

/* prints data from list (forward)
*/
void printList( LIST *pList);

/* prints data from list (backward)
*/
void printListR( LIST *pList);

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr);

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken( char *str);

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
void destroyToken( tTOKEN *pToken);

////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'F':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret = 0;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	
	// creates an empty list
	list = createList();
	
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}


	while(fscanf( fp, "%s", str) == 1)
	{
		
		pToken = createToken( str);
		
		// insert function call
		ret = addNode( list, pToken);
		
		if (ret == 2) // duplicated 
			destroyToken( pToken);
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				printList( list);
				break;
			
			case BACKWARD_PRINT:
				printListR( list);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a string to find: ");
				fscanf( stdin, "%s", str);
				
				if (searchList( list, str, &pToken))
				{
					fprintf( stdout, "(%s, %d)\n", pToken->token, pToken->freq);
				}
				else fprintf( stdout, "%s not found\n", str);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a string to delete: ");
				fscanf( stdin, "%s", str);
				
				if (removeNode( list, str, &pToken))
				{
					fprintf( stdout, "%s deleted\n", pToken->token);
					destroyToken( pToken);
				}
				else fprintf( stdout, "%s not found\n", str);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
			}
		
		if (action) fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}

LIST *createList(void){
	LIST *pList =  (LIST *)malloc(sizeof(LIST));
	
	pList -> count = 0;
	pList -> head = NULL;
	pList -> rear = NULL;
	pList -> pos = NULL;
	
	return pList;
}

tTOKEN *createToken( char *str){
	tTOKEN *ptoken = (tTOKEN *)malloc(sizeof(tTOKEN));
	
	ptoken -> token = strdup(str);
	ptoken -> freq = 1;
	
	return ptoken;
}

int addNode( LIST *pList, tTOKEN *dataInPtr){
	NODE *pPre;
	NODE *pLoc;
	
	int find = _search(pList, &pPre, &pLoc, dataInPtr->token);

	if(find == 0){
		_insert(pList, pPre, dataInPtr);
		pList -> count += 1;
		return 1;
	}
	else{
		pLoc -> dataPtr -> freq += 1;
		return 2;
	}
}

static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr){
	NODE *pNode = (NODE *)malloc(sizeof(NODE));
	pNode -> dataPtr = dataInPtr;
	
	if (pPre == NULL){
		pNode -> llink = NULL;
		pNode -> rlink = pList -> head;
		pList -> head = pNode;
		pList -> rear = pNode;
	}
	else{
		pNode -> rlink = pPre -> rlink;
		pNode -> llink = pPre;
	}
	
	if(pPre != NULL){
		if (pPre -> rlink == NULL) pList -> rear = pNode;
		else pPre -> rlink -> llink = pNode;
		pPre -> rlink = pNode;
	}
		
	return 1;
}

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu){
	*pPre = NULL;
	*pLoc = pList -> head;

	while( *pLoc != NULL && strcmp( (*pLoc) -> dataPtr -> token , pArgu ) < 0 ){
		*pPre = *pLoc;
		*pLoc = (*pLoc) -> rlink;
	}
	
	//Searching for end of file
	if( *pLoc == NULL ) return 0;
	//Successes for searching
	else if( strcmp( (*pLoc) -> dataPtr -> token , pArgu ) == 0 ) return 1;
	//find position to insert
	return 0;
}

void destroyToken( tTOKEN *pToken){
	free(pToken -> token);
	free(pToken);
}

void printList( LIST *pList){
	NODE* pLoc = pList -> head;
	while( pLoc != NULL){
		printf("%s\t%d\n", pLoc->dataPtr->token, pLoc->dataPtr->freq);
		pLoc = pLoc -> rlink;
	}
}

void printListR( LIST *pList){
	NODE* pLoc = pList -> rear;
	while( pLoc != NULL){
		printf("%s\t%d\n", pLoc->dataPtr->token, pLoc->dataPtr->freq);
		pLoc = pLoc -> llink;
	}
}

int countList( LIST *pList){
	return pList -> count;
}

int emptyList( LIST *pList){
	if (pList -> count == 0) return 1;
	else return 0;
}

int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut){
	NODE* pPre = NULL;
	NODE* pLoc = NULL;
	
	int find = _search(pList, &pPre, &pLoc, pArgu);
	if (find == 1){
		*pDataOut = pLoc -> dataPtr;
		return 1;
	}
	else return 0;
}

static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr){
	*dataOutPtr = createToken( pLoc-> dataPtr -> token );

	if(pLoc -> llink != NULL){
		pPre -> rlink = pLoc -> rlink;
	}
	else{
		pList -> head = pPre -> rlink;
	}
	
	if(pLoc -> rlink != NULL){
		pLoc -> rlink -> llink = pPre -> llink;
	}
	else{
		pList -> rear = pLoc -> llink;
	}
	pList -> count -= 1;
	destroyToken(pLoc -> dataPtr);
	free(pLoc);
}

int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut){
	NODE *pPre = NULL;
	NODE *pLoc = NULL;
	
	int find = _search(pList, &pPre, &pLoc, keyPtr);
	
	if ( pPre == NULL) return 0;
	
	if (find == 0) return 0;
	else{
		_delete(pList, pPre, pLoc, dataOut);
		return 1;
	}
}

void destroyList( LIST *pList){
	NODE *pLoc;
	
	while (pList -> head != NULL){
		pLoc = pList -> head;
		pList -> head = pLoc -> rlink;
		destroyToken(pLoc -> dataPtr);
		free(pLoc);
	}
	free(pList);
}