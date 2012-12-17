/*
 ============================================================================
 Name        : Panama Cannal
 Author      : Tom Ma
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
/*COSCI 21a Assignment P1*/
/*Tom Ma*/


/*************************************************************************************/
/*Includes and constants                                                             */
/*************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define debug 1
#define hashsize 103100
/*to turn it on, change 0 to 1*/
#define visualizer 0




/*************************************************************************************/
/*Position Definition And Operations                                                     */
/*************************************************************************************/
/*Position data type. */
typedef struct pnode {
	struct pnode *back; /*position from which this one came*/
	int cost;                   /*number of moves to get to this position*/
	int space;					/*indicating the empty space*/
	char piece;              /*piece that moved to this position*/
	int dir;                      /*direction moved to enter this position*/
	char board[12];       /*this position's board*/
	} PositionBody;
typedef PositionBody *TypePosition;

/*Allocate memory for a new position*/
TypePosition NEWPOSITION() {
TypePosition p = (TypePosition) malloc(sizeof(PositionBody));
if (p==NULL) {printf("Malloc for a new position failed.");exit(1);}
return p;
}

/*printing a position, how it changed*/
void PRINT(TypePosition p) {
	if (p!=NULL) {
		int i;
		printf("\nstep %d: ",p->cost);
		printf("move %c ",p->piece);
		if (p->dir==1){
			printf("west\n");
		}else if(p->dir==-1){
			printf("east\n");
		}else if(p->dir==6){
			printf("north\n");
		}else if(p->dir==-6){
			printf("south\n");
		}
		for(i=0;i<6;i++){
			printf("%c", p->board[i]);
		}
		printf("\n");
		for(i=6;i<12;i++){
			printf("%c", p->board[i]);
		}
	}
	return ;
}

/*make one board equal to another*/
void ChangeBoard(char board[12], char current[12]) {
	int i;
	for(i = 0;i<12;i++){
		board[i]=current[i];
	}
}
/*return 0 when two position have same boards, 1 when different*/
int Equal(TypePosition a, TypePosition b){
	int i;
	int j=0;
	for(i = 0;i<12;i++){
		if (a->board[i]!=b->board[i]){
			j=1;
			return j;
		}
	}
	return j;
}

/*the structure of a Queue*/
typedef struct Queue{
	int maxsize;		/*the maximum size the queue can get*/
	int size;			/*the actual size of the queue*/
	int front;			/*the beginning of the queue in the array*/
	int rear;			/*the end of the queue*/
	int maximum;		/*record maximum items ever included in the queue*/
	TypePosition *elements; /*the array that stores the positions*/
}Queue;

/*creating a new empty queue*/
Queue * createQueue(){
	Queue *Q;
	Q = (Queue *)malloc(sizeof(Queue));
	Q->elements = (TypePosition *)malloc(sizeof(TypePosition)*50000);
	Q->size = 0;
	Q->maxsize = 50000;
	Q->front = 0;
	Q->rear = 0;
	Q->maximum = 0;
	return Q;
}

/*deleting the front from the queue, return position deleted*/
TypePosition Dequeue(Queue *Q){
	int i;
	if(Q->size==0){
		printf("Queue is Empty\n");
		exit(0);
	}
	/* Removing an element is equivalent to incrementing index of front by one */
        else{
        	i=Q->front;
        	Q->size--;
        	Q->front++;
        	/* As we fill elements in circular fashion */
        	if(Q->front==Q->maxsize){
        		Q->front=0;
        	}
        }
	return Q->elements[i];
}

/*adding a position in the the rear of the queue*/
void Enqueue(Queue *Q,TypePosition element){
	if(Q->size == Q->maxsize){
		printf("Queue is Full\n");
		exit(0);
	}
	else{
		Q->elements[Q->rear] = element;
		Q->size++;
		Q->rear = Q->rear + 1;
		Q->maximum++;
		if(Q->rear == Q->maxsize){
			Q->rear = 0;
		}
	}
	return;
}

/*Define simple notation for NEXT and DATA like used in class.*/
#define BOARD(v) (v->board)

/*define the structure of a closed hash*/
typedef struct Hash{
	TypePosition *Buckets;/*the buckets that each has one position in*/
	int count; /*the amount of positions entered in the hash*/
}Hash;

/*creating a hash*/
Hash * createHash(){
	Hash *H;
	H = (Hash *)malloc(sizeof(Hash));
	H->Buckets = (TypePosition *)malloc(sizeof(TypePosition)*hashsize);
	H->count=0;
	return H;
}

/*the hash function that the hash table use. simply adding the characters timing their index*/
int h(TypePosition d){
	int i;
	int	sum=0;
	for(i=0;i<12;i++) {
		sum=sum+d->board[i]*i;
	}
	return sum%hashsize;
}

/*insert a position in the hash table, return 1 when insert is success,else 0*/
int INSERT(TypePosition d, Hash *H){
	int i;
	/*exit if table is full*/
	if (H->count>=hashsize){
		printf("Hash table is full\n");
		exit(0);
	}
	/*if the position is taken, move to the next slot*/
	i=h(d);
	while(H->Buckets[i]!=NULL&&Equal(H->Buckets[i], d)==1){
		i=(i+1)%hashsize;
	}

	/*entering the position*/
	if(H->Buckets[i]==NULL){
		H->Buckets[i]=d;
		H->count=H->count+1;
		return 1;
	}
	return 0;
}

/*finding the positions that one can reach by one move*/
TypePosition *reach(TypePosition d){
	/*the 4 directions: -1 west, 1 east, -6 north, 6 south*/
	int dirs[]={-1,1,-6,6};
	TypePosition *reachables=(TypePosition *)malloc(sizeof(TypePosition)*4);
	int i;
	for (i=0; i<4;i++){
		if(dirs[i]==1&&d->space==5){
		}else if(dirs[i]==-1&&d->space==6){
		}else if((dirs[i]+d->space)>=0&&(dirs[i]+d->space)<12){
		/*create new positions with the same board as input position*/
		reachables[i] = NEWPOSITION();
		ChangeBoard(reachables[i]->board, d->board);
		/*swap the space and the moved piece: the move*/
		reachables[i]->board[d->space]=reachables[i]->board[d->space+dirs[i]];
		reachables[i]->piece=reachables[i]->board[d->space];
		reachables[i]->board[d->space+dirs[i]]=' ';
		/*defining the other information of the position */
		reachables[i]->space=d->space+dirs[i];
		reachables[i]->back = d;
		reachables[i]->cost=d->cost+1;
		reachables[i]->dir=dirs[i];
		}
	}
	return reachables;
}

/*to display the process from the start to the final position using recursion*/
void Display(TypePosition final){
	if(final->back!=NULL){
		Display(final->back);
	}
	PRINT(final);
}

/*************************************************************************************/
/*START OF PROGRAM                                                                   */
/*************************************************************************************/
int main() {

	/*create the start position*/
	TypePosition start = NEWPOSITION();
	ChangeBoard(BOARD(start), "canamapanal ");
	start->space=11;
	start->cost=0;

	/*create the final position*/
	TypePosition final = NEWPOSITION();
	ChangeBoard(BOARD(final), "panamacanal ");

	/*create the Hash and insert start position*/
	Hash *H = createHash();
	INSERT(start,H);

	/*create the Queue and enqueue start position*/
	Queue *Q = createQueue();
	Enqueue(Q, start);

	/*keep searching for final until found==1||-1*/
	int found = 0;
	/*int for visualizing process*/
	int list=0;


	while (found==0) {
		TypePosition *reaches =(TypePosition *)malloc(sizeof(TypePosition)*4);
		TypePosition current = Dequeue(Q);
		reaches = reach(current);
		int j;
		for(j=0; j<4;j++){
			if (reaches[j]!=NULL) {

				/*process visualizing*/
				if(visualizer==1){
				list++;
				printf("\n[%d]--target:[210000]\n",list);
				printf("Q size:%d",Q->size);
				printf("H size:%d",H->count);
				}
				/*determine if final is found*/
				if (Equal(final, reaches[j])==0){
					found = 1;
					/*giving all other informations to final(back, dir, space, etc.)*/
					final = reaches[j];
					printf("\n------find Panama Canal!!!!------\n");
				}else{
					/*run insert, if insert is success, enq will equal 1, then do enqueue*/
					int enq=INSERT(reaches[j],H);
					if(enq==1){
						Enqueue(Q, reaches[j]);
					}
				}
			}
		}
	}

	/*display process*/
	printf("there are %d items in the HASH TABLE\n", H->count);
	printf("there are %d empty buckets in the HASH TABLE\n", hashsize-H->count);
	printf("since closed hash table is used, each buckets has only one position inserted\n");
	printf("there are %d items in the queue\n", Q->size);
	printf("the queue has ever stored %d items\n", Q->rear);
	Display(final);

	return 0;
}
