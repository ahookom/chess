#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include <string.h>

using namespace std;


//define piece colors and types by integers to be used in all functions
const int white=0; 
const int black=1;
const int pawn=1;
const int bishop=3;
const int knight=2;
const int rook=5;
const int queen=9;
const int king=100;
const bool TRUE=1;
const bool FALSE=0;
int check=FALSE;

const int STATS=16;
const int MAXMOVES=218;

const int EXIT=86;

//some useful game settings that are needed by many functions
int AIsetting=FALSE;
int win=FALSE;
bool player=0;
int error=FALSE;
int turn = 1;

//function prototypes
int printboard();
int moveanalysis(struct movestats moves[MAXMOVES], bool player);
int setcheck(bool player);
int countmoves(bool player);
int requestmove(int *x1, int *y1, int *x2, int *y2);
int legalmove(int player, int *x1, int *y1, int *x2, int *y2);
int gamemove();
int undomove();
int initialize_pieces();
void startingarrangement();
int countpieces();
int pathclear(int x1, int y1, int x2, int y2);
int isattacked(int defender, int i, int j);
int isdefended(int defender, int i, int j);
int pinned(bool player, int i, int j);
void printmovestats(int movecount, struct movestats moves[MAXMOVES]);
int pvalue(int i,int j);
int updateboard(int x1,int y1,int x2,int y2);
int weakestthreat(int opponent, int x1, int y1);
signed int positionimproved(int x1, int y1, int x2, int y2);
string move(int x1, int y1, int x2, int y2);


//definition of piece structure
struct piece {
bool empty;
int color;
int type;
int timesmoved;
bool justmoved;
};

//definition of a struct to hold analytic information on possible moves
struct movestats {
int startrow;
int startcol;
int endrow;
int endcol;
int piecevalue;
signed int currentspacesafety;
int pinned;
int attackedpiecevalue;
signed int endspacesafety;
signed int positionimproved;
signed int movescore;
int weakestthreateningpiece;
};

struct square{
int row;
int col;
};

//initializing a board array and each type of piece
struct piece *board[8][8];
struct piece empty;
struct piece wking;
struct piece wqueen;
struct piece wrookk;
struct piece wrookq;
struct piece wbishopl;
struct piece wbishopd;
struct piece wknightk;
struct piece wknightq;
struct piece wpawna;
struct piece wpawnb;
struct piece wpawnc;
struct piece wpawnd;
struct piece wpawne;
struct piece wpawnf;
struct piece wpawng;
struct piece wpawnh;

struct piece bking;
struct piece bqueen;
struct piece brookk;
struct piece brookq;
struct piece bbishopl;
struct piece bbishopd;
struct piece bknightk;
struct piece bknightq;
struct piece bpawna;
struct piece bpawnb;
struct piece bpawnc;
struct piece bpawnd;
struct piece bpawne;
struct piece bpawnf;
struct piece bpawng;
struct piece bpawnh;

//main program
int main(int argc, char * argv[]){

	initialize_pieces();
	startingarrangement();
	srand(time(NULL));
	char savestatus='x';
	string loadfile;
	if(argc<2){printf("\n\n\n\nWelcome to DaftChess!!\n\n\n\nStart from previous (y/n)? ");
		
		while((!(cin >> savestatus))||(savestatus!='y'&&savestatus!='Y'&&savestatus!='n'&&savestatus!='N')){
			cin.clear();
			cin.ignore();
			printf("Invalid entry. Try again: ");
		}

		if(savestatus!='n'&&savestatus!='N'){
			printf("Please type the filename to load: ");
			while(!(cin >> loadfile)){
				cin.clear();
				cin.ignore();
				printf("Invalid entry. Try again: ");
			}
		}
		
	}else if(argc==2)savestatus='Y';
	


//Board Initialization
	if(savestatus=='Y'||savestatus=='y'){
		ifstream restore;
		if(argc==2)restore.open( argv[1] );
		else restore.open( loadfile.c_str() );

		int x1,y1,x2,y2;

		if(restore.is_open()){

			while(!restore.eof()){

				restore >> x1 >> y1 >> x2 >> y2;
				updateboard(x1,y1,x2,y2);
				cout << move(x1,y1,x2,y2).c_str() << "\n";

			}

		restore.close();

		}else{
		printf("\nCouldn't open that file. Starting a new game.\n");
		}

	}else{

	remove("save.dat");

	}


//Set whether to play against the AI and whether AI is black or white
	char ai='x';
	printf("\n\nWant to play the AI (y/n)? ");

	while((!(cin >> ai))||(ai!='Y'&&ai!='y'&&ai!='n'&&ai!='N')){
		cin.clear();
		cin.ignore();
		printf("\nInvalid entry. Try again: ");
	}

	if(ai==('y'|'Y')){
		printf("\n\nAI plays white or black (w/b)? ");
		while(!(cin >> ai)||(ai!='W'&&ai!='w'&&ai!='B'&&ai!='b')){
			cin.clear();
			cin.ignore();
			printf("\nInvalid entry. Try again: ");
		}
		if(ai==('W'|'w'))AIsetting=1;
		else if(ai==('B'|'b'))AIsetting=2;
	}

//turn request cycle print board, ask for moves until you get a valid one, check if the game is ended, update whose turn it is and repeat
	while(win==0){
		
		printboard();

		setcheck(player);

		//making sure player has at least one legal move
		if(0==countmoves(player)){
		
			if(check!=0){
				for(int i=0;i<20;i++){
					if(player=0)printf("\nWhite Victory!!!");
					if(player=1)printf("\nGlorious Black Triumphs!!");
				}
				return 0;
			}else{
				printf("Player has no moves. Stalemate has been reached!");
				return 0;
			}

		}

		do{
		
		error = gamemove();

		if(error==EXIT)return 0;

		}while(error!=1);

	

	}
}


signed int positionimproved(int x1, int y1, int x2, int y2){

	int m = countmoves(player);

	struct piece* buffer;
	buffer=board[x2][y2];
	board[x2][y2]=board[x1][y1];
	board[x1][y1]=&empty;

	m=countmoves(player)-m;

	board[x1][y1]=board[x2][y2];
	board[x2][y2]=buffer;

	return m;

}


int undomove(){


}

int weakestthreat(int opponent, int x1, int y1){
	int mover = (opponent+1)%2;
	if(isattacked(mover,x1,y1)==0)return 0;
	int minattacker=100;
	int currentpiecevalue=0;
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			currentpiecevalue=pvalue(i,j);
			if(currentpiecevalue>0&&currentpiecevalue<minattacker&&legalmove(opponent,&i,&j,&x1,&y1)==1){
			minattacker==currentpiecevalue;
			}
		}
	}

	return minattacker;
}



//determine which pieces are attacking (i,j)
//find out if those pieces would be attacking a piece behind (i,j) if it is moved
//find out if any such pieces are undefended or are worth more than the attacking piece

int pinned(bool player, int i, int j){


	int timesattacked=isattacked(player,i,j);
	int currentattacker=1;
	int opponent=(player+1)%2;
	signed int x=i;
	signed int y=j;

	if (timesattacked==0)return FALSE;
	
	for(int k=0;k<8;k++){

		for(int l=0;l<8;l++){

			if(legalmove(opponent,&k,&l,&i,&j)==TRUE){
			
				if(!((board[k][l]->type<bishop)||board[k][l]->type==king)){

					signed int xinc = 0;
					if(i-k!=0) xinc = (i-k)/abs(i-k);
					signed int yinc = 0;
					if(j-l!=0) yinc = (j-l)/abs(j-l);
					x+=xinc;
					y+=yinc;
					
					while(x<8&&x>=0&&y<8&&y<=0){

						if(board[x][y]->color==player&&(pvalue(x,y)>pvalue(k,l)||isdefended(player,x,y)==0)){
							return TRUE;
						}else if(board[x][y]->empty==FALSE)break;

					}
					
				}

			}
			if(currentattacker==timesattacked)return FALSE;
			currentattacker++;
		
		}
	}

	return FALSE;

}


int moveanalysis(struct movestats moves[MAXMOVES],bool player){

int safemcount=0;

int opponent=player+1%2;
	for(int i=0;i<8;i++){

		for(int j=0;j<8;j++){
		
			for(int k=0;k<8;k++){

				for(int l=0;l<8;l++){

					if(legalmove(player, &i,&j,&k,&l)==1){

						struct piece* buffer;
						buffer=board[i][j];
						board[i][j]=&empty;
						moves[safemcount].endspacesafety=(isdefended(player,k,l))-isattacked(player,k,l);
						board[i][j]=buffer;
						


						//if(moves[safemcount].endspacesafety>=0){

						moves[safemcount].startrow=i;
						moves[safemcount].startcol=j;
						moves[safemcount].endrow=k;
						moves[safemcount].endcol=l;
						moves[safemcount].piecevalue=pvalue(i,j);
						moves[safemcount].currentspacesafety=isdefended(player,i,j)-isattacked(player,i,j);
						moves[safemcount].pinned=pinned(player,i,j);
						moves[safemcount].attackedpiecevalue=pvalue(k,l);

						moves[safemcount].positionimproved=positionimproved(i,j,k,l);
						moves[safemcount].weakestthreateningpiece=weakestthreat(opponent,i,j);

						safemcount++;
												

					}

				}
				

				

			}		
	
		}

	}
//assign movescores
printf("White has %d safe moves.\n",safemcount+1);
	for(int i=0;i<=safemcount;i++){

		int j=moves[i].startrow;
		int k=moves[i].startcol;
		moves[i].movescore=0;
		if(moves[i].endspacesafety<0)moves[i].movescore=-50;

		else if(moves[i].pinned==TRUE&&(moves[i].weakestthreateningpiece>moves[i].piecevalue||moves[i].piecevalue==1||moves[i].currentspacesafety>0))moves[i].movescore=-50;

		else{ 

		if(moves[i].attackedpiecevalue>moves[i].piecevalue||(moves[i].attackedpiecevalue>0&&moves[i].endspacesafety>=0))moves[i].movescore+=500;
		if((moves[i].currentspacesafety<0||(moves[i].weakestthreateningpiece<moves[i].piecevalue&&moves[i].weakestthreateningpiece>0))&&moves[i].endspacesafety>=0)moves[i].movescore+=100;
		moves[i].movescore+=moves[i].positionimproved;
		if(moves[i].endspacesafety<0)moves[i].movescore-=50;
		if(moves[i].endspacesafety==0)moves[i].movescore-=10;
		if(turn<=16&&board[j][k]->timesmoved<1&&moves[i].piecevalue<6)moves[i].movescore+=10;
		if(turn<=16&&(moves[i].startrow==0||moves[i].startrow==7)&&moves[i].piecevalue<5)moves[i].movescore+=10;
		if(board[j][k]->type==knight&&(moves[i].endcol==0||moves[i].endcol==7))moves[i].movescore-=5;
		}
		if(board[j][k]->type==queen&&turn<16)moves[i].movescore-=(32-turn*2);
		if(board[j][k]->type==king&&board[j][k]->timesmoved==0&&(moves[i].endcol==1||moves[i].endcol==5))moves[i].movescore+=25;
	}

	return 0;

}


int pvalue(int i, int j){

	if(board[i][j]->type!=2)return board[i][j]->type;
	else return 3;

}



void printmovestats(int movecount, struct movestats moves[MAXMOVES]){

	printf("Printing Moves.\n");
	for(int i=0;i<movecount;i++){
		cout << move(moves[i].startcol,moves[i].startrow,moves[i].endcol,moves[i].endrow).c_str() <<"  "<< moves[i].movescore << "  initial safety  " << moves[i].currentspacesafety << "  end safety  " << moves[i].endspacesafety << "\n";	            
	}

}



int pickmove(struct movestats moves[MAXMOVES],bool player, int *x1, int *y1,int*x2,int*y2){

	int moverow=0;
	int ties=0;
	int maxmoverating=moves[0].movescore;

	int movecount=countmoves(player);

	//for(int i=0;i<MAXMOVES;i++){

//	if(moves[i].startrow==0&&moves[i].startcol==0&&moves[i].endrow==0&&moves[i].endcol==0)break;	
//	else movecount++;

//	}

	
	for(int i=0;i<movecount;i++){
		if(moves[i].movescore>moves[moverow].movescore){
			moverow=i;
			ties=0;
			maxmoverating=moves[moverow].movescore;

		}
		else if(moves[i].movescore==moves[moverow].movescore){
		ties+=1;
		}
	}

	if(ties==0)return moverow;
	else if(ties!=0){

		int randpick=rand()%ties;
		int j=0;
	//	printf("There is a %d way tie for best move rated %d. AI will use the %d tied-move.\n",ties+1,maxmoverating,randpick+1);
		for(int i=0;i<movecount;i++){
			if(moves[i].movescore==maxmoverating){
				if(j==randpick)return i;

				j++;
			
			}
		}
		

	}

}



int requestmove(int *x1, int *y1,int*x2,int*y2){

	char oldletter='\0';
	int oldnumber='\0';
	char newletter='\0';
	int newnumber='\0';

	if(AIsetting==((int)player+1)){

		struct movestats moves[MAXMOVES];

		moveanalysis(moves, player);

		//printmovestats(countmoves(player), moves);

		int moverow=pickmove(moves,player, x1,y1,x2,y2);

		printf("AI picks to use move %d \n",moverow+1);

		*x1=moves[moverow].startrow;
		*y1=moves[moverow].startcol;
		*x2=moves[moverow].endrow;
		*y2=moves[moverow].endcol;


		printf("AI requests to move (%d, %d) to (%d, %d)",*x1,*y1,*x2,*y2);

	}else{

		printf("Player has %d legal moves. ",countmoves(player));
		if(player==0)printf("White player enter move: ");
		else if(player==1)printf("Black player enter move: ");

		while(!(cin >> oldletter)){
			cin.clear();
			cin.ignore();
			printf("Invalid entry. Try again: ");
		}
		if(oldletter=='x'||oldletter=='X')return EXIT;
		while(!(cin >> oldnumber)){
			cin.clear();
			cin.ignore();
			printf("Invalid entry. Try again: ");
		}
		while(!(cin >> newletter)){
			cin.clear();
			cin.ignore();
			printf("Invalid entry. Try again: ");
		}
		while(!(cin >> newnumber)){
			cin.clear();
			cin.ignore();
			printf("Invalid entry. Try again: ");
		}


		if((int)oldletter<73)*y1=(int)oldletter-65;
		if((int)oldletter>96)*y1=(int)oldletter-97;
		if((int)newletter<73)*y2=(int)newletter-65;
		if((int)newletter>96)*y2=(int)newletter-97;
		*x1=7-(oldnumber-1);
		*x2=7-(newnumber-1);
	//printf("looks like player wants to move from %d, %d to %d, %d",*x1,*y1,*x2,*y2);
	//printf("the piece at %d, %d is of type %d\n",*x1,*y1,board[*x1][*y1]->type);
	}
	if(legalmove(player, x1,y1,x2,y2)==1){printf("A legal move was requested.\n");return 1;}
	else{printf("That's not a legal move.\n\n"); return 0;}

}




int legalmove(int player, int*x1, int*y1, int*x2, int*y2){

//generic error handling
	if((*x1>7)||(*x2>7)||(*y1>7)||(*y2>7)||(*x1<0)||(*x2<0)||(*y1<0)||(*y2<0)){printf("Stay on the board!");return 3;}

	if(player!=board[*x1][*y1]->color){
//		printf("\nMove a piece that belongs to you.\n");
		return 0;
	}

	if(player==board[*x2][*y2]->color){
//		printf("\nYou can't move to that square because you've got a piece on it already.\n");
		return 0;
	}

//temporarily do the move to see if it puts the player into check
	struct piece* buffer;
	buffer=board[*x2][*y2];
	board[*x2][*y2]=board[*x1][*y1];
	board[*x1][*y1]=&empty;

	if(setcheck(player)>0){
//	printf("You can't move into check.\n");
	board[*x1][*y1]=board[*x2][*y2];
	board[*x2][*y2]=buffer;
	return 0;
	}
//undo the move
	board[*x1][*y1]=board[*x2][*y2];
	board[*x2][*y2]=buffer;


	if(board[*x1][*y1]->type==pawn){
		if(player==white){
			if(board[*x2][*y2]->empty==1&&(*x1-*x2)==1&&(*y1==*y2))return 1;
			if(abs(*y2-*y1)==1&&(*x1-*x2)==1&&(board[*x2][*y2]->color==black||(board[*x2][*y2]->empty==1&&*x2==2&&board[3][*y2]->color==black&&board[3][*y2]->justmoved==TRUE&&board[3][*y2]->type==pawn)))return 1;
			if(*x1==6&&board[*x2][*y2]->empty==1&&(*x1-*x2)==2&&(*y1==*y2)&&board[(*x1-1)][*y1]->empty==1)return 1;
		}
		if(player==black){
			if(board[*x2][*y2]->empty==1&&(*x2-*x1)==1&&(*y1==*y2))return 1;		
			if(abs(*y2-*y1)==1&&(*x2-*x1)==1&&(board[*x2][*y2]->color==white||(board[*x2][*y2]->empty==1&&*x2==5&&board[4][*y2]->color==white&&board[4][*y2]->justmoved==TRUE&&board[4][*y2]->type==pawn)))return 1;
			if(*x1==1&&(board[*x2][*y2]->empty==1)&&(*x2-*x1)==2&&(*y1==*y2)&&board[(*x1+1)][*y1]->empty==1)return 1;		
		}	
	else return 0;
	}

	if(board[*x1][*y1]->type==knight){
		if(abs(*x2-*x1)==1&&abs(*y2-*y1)==2){return 1;}
		if(abs(*x2-*x1)==2&&abs(*y2-*y1)==1){return 1;}
		else{return 0;}
	}

	if(board[*x1][*y1]->type==bishop){
		if((*x2-*x1==*y2-*y1)||(*x1+*y1==*x2+*y2)){
		if(pathclear(*x1,*y1,*x2,*y2)==1)return 1;
		}else return 0;
	}

	if(board[*x1][*y1]->type==rook){
		if((*x1==*x2||*y1==*y2)&&pathclear(*x1,*y1,*x2,*y2)==1)return 1;
	else return 0;
	}

	if(board[*x1][*y1]->type==queen){
		if((*x1==*x2||*y1==*y2)&&pathclear(*x1,*y1,*x2,*y2)==1)return 1;		
		else if(((*x2-*x1==*y2-*y1)||(*x1+*y1==*x2+*y2))&&(pathclear(*x1,*y1,*x2,*y2)==1))return 1;
	}

	if(board[*x1][*y1]->type==king){
	//	printf("piece at %d %d is a king", *x1,*y1);
		if(abs(*x1-*x2)+abs(*y1-*y2)==1)return 1;
		if((abs(*x1-*x2)+abs(*y1-*y2)==2)&&(*x1!=*x2)&&(*y1!=*y2))return 1;
		if((board[*x1][*y1]->timesmoved==0)&&(*x2==*x1)&&abs(*y2-*y1)==2){
		
			if(*y2>*y1&&board[*x1][7]->type==rook&&board[*x1][7]->timesmoved==0&&board[*x1][6]->empty==1&&board[*x1][5]->empty==1){
			//	printf("in castle test if loop\n");
				return 1;
			}
			else if(*y1>*y2&&board[*x1][0]->type==rook&&board[*x1][0]->timesmoved==0&&board[*x1][1]->empty==1&&board[*x1][2]->empty==1&&board[*x1][3]->empty==1){
			//	printf("in castle test elseif loop\n");
				return 1;
			}

		}
		else return 0;
	}

return 0;
}


int pathclear(int x1,int y1,int x2, int y2){

	if(x1==x2){

		if(y1<y2){
			for(int i=(y1+1);i<y2;i++){
				if(board[x1][i]->empty==0)return 0;
			}
		
		}
		
		else if(y1>y2){
			for(int i=(y1-1);i>y2;i--){
				if(board[x1][i]->empty==0)return 0;
			}

		}


	return 1;
	}

	if(x1!=x2){

		if(y1==y2){
			if(x2-x1<0){
				for(int i=(x1-1);i>x2;i--){
//				printf("Checking if legal upward move");
				if(board[i][y1]->empty==0)return 0;	

				}
			}

			else if(x2-x1>0){
				for(int i=(x1+1);i<x2;i++){
//				printf("Checking if legal downward move");
				if(board[i][y1]->empty==0)return 0;			
			}

			}
		return 1;
		}


		if(y1!=y2){
		signed int vshift = x2-x1;
		signed int hshift = y2-y1;
		signed int vpos;
		signed int hpos;
		if(vshift<0)vpos=-1;
		if(hshift<0)hpos=-1;
		if(vshift>0)vpos=1;	
		if(hshift>0)hpos=1;

		for(int i=(x1+vpos), j=(y1+hpos);(abs(i-x2)>0&&abs(j-y2)>0);i+=vpos, j+=hpos){
		
		if(board[i][j]->empty==0)return 0;
//		printf("square (%d, %d) is empty.\n",i, j);
		}
//		printf("path checked and is clear\n");
		return 1;
		}

	}

	return 0;
}


int updateboard(int x1, int y1, int x2, int y2){

//handle the rook movement if the move was a castle

if(board[x1][y1]->type==king&&(board[x1][y1]->timesmoved==0)&&(x2==x1)&&abs(y2-y1)==2){
	if(y2>y1&&board[x1][7]->type==rook&&board[x1][7]->timesmoved==0&&board[x1][6]->empty==1&&board[x1][5]->empty==1){
	//	printf("in castle test if loop\n");
		board[x1][5]=board[x1][7];
		board[x1][7]=&empty;
	}
	else if(y1>y2&&board[x1][0]->type==rook&&board[x1][0]->timesmoved==0&&board[x1][1]->empty==1&&board[x1][2]->empty==1&&board[x1][3]->empty==1){
	//	printf("in castle test elseif loop\n");
		board[x1][3]=board[x1][0];
		board[x1][0]=&empty;
	}
}

//moving the requested pieces themselves

	struct piece* buffer;
	buffer=board[x2][y2];
	board[x2][y2]=board[x1][y1];
	board[x1][y1]=&empty;

//en passant clearing the pawn that gets taken
	if(x2==2&&player==white&&board[3][y2]->color==black&&board[3][y2]->type==pawn&&board[3][y2]->justmoved==1){
		board[3][y2]=&empty;
	}else if(x2==5&&player==black&&board[4][y2]->color==white&&board[4][y2]->type==pawn&&board[4][y2]->justmoved==1){
		board[4][y2]=&empty;
	}

//updating to queen at the end of the board
	if(board[x2][y2]->type==pawn&&((player==0&&x2==0)||(player==1&&x2==7)))board[x2][y2]->type=queen;

//update the piece variables to keep track of the new move
	board[x2][y2]->timesmoved++;
	board[x2][y2]->justmoved=TRUE;
	
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			if(board[i][j]->color!=(player))board[i][j]->justmoved=FALSE;
		}
	}

	player=((player+1)%2);
	turn++;
}



//This is where I call to request a move until I get a legal one and then call move() to move the requested pieces

int gamemove(){
	int *x1=(int *)malloc(sizeof(int));
	int *y1=(int *)malloc(sizeof(int));
	int *x2=(int *)malloc(sizeof(int));
	int *y2=(int *)malloc(sizeof(int));


//get a move
	do{
	error=requestmove(x1,y1,x2,y2);
	if(error==EXIT)return EXIT;
	}while(error!=1);

	updateboard(*x1,*y1,*x2,*y2);
//record the move in the save file

	ofstream save;
	save.open("save.dat", ios::app);

	if(save.is_open()){

		save << "\n" << *x1 <<" "<< *y1 <<" "<< *x2 <<" "<< *y2;

		save.close();

	}
	else cout << "Unable to open file.\n";


//release the allocated memory
	free(x1);
	free(x2);
	free(y1);
	free(y2);

	return 1;
}

string move(int x1, int y1, int x2, int y2){
	string output;
	output[0]=(char)(x1+97);
	output[1]=(char)((8-y1)+48);
	output[2]=' ';
	output[3]=(char)(x2+97);
	output[4]=(char)((8-y2)+48);

	return output;
}

int printboard(){
	char wking[4]="\u2654";
	char wqueen[4]="\u2655";
	char wrook[4]="\u2656";
	char wbishop[4]="\u2657";
	char wknight[4]="\u2658";
	char wpawn[4]="\u2659";
	char bking[4]="\u265A";
	char bqueen[4]="\u265B";
	char brook[4]="\u265C";
	char bbishop[4]="\u265D";
	char bknight[4]="\u265E";
	char bpawn[4]="\u265F";

	char shade[8]="\033[46m";
	char dshade[8]="\033[0m";

	printf("\n\n         ");


if(player==0){

	for(int i=0;i<8;i++){
	printf("%s %c ",dshade,(char)(i+65));
	}
	printf("\n");

	for(int i=0;i<8;i++){
		printf("       %d ",(8-i));
			for(int j=0;j<8;j++){
				if((board[i][j]==0)||(board[i][j]->empty)==1){
					if((i+j)%2==1)printf("%s   %s",shade,dshade);
					else{printf("%s   ",dshade);
					}
				}
				else{
					if((i+j)%2==1)printf("%s",shade);
					printf(" ");
					if(board[i][j]->color==white){
						if(board[i][j]->type==king)printf("%s",wking);
						if(board[i][j]->type==queen)printf("%s",wqueen);
						if(board[i][j]->type==rook)printf("%s",wrook);
						if(board[i][j]->type==bishop)printf("%s",wbishop);
						if(board[i][j]->type==pawn)printf("%s",wpawn);
						if(board[i][j]->type==knight)printf("%s",wknight);
					}else{
						if(board[i][j]->type==king)printf("%s",bking);
						if(board[i][j]->type==queen)printf("%s",bqueen);
						if(board[i][j]->type==rook)printf("%s",brook);
						if(board[i][j]->type==bishop)printf("%s",bbishop);
						if(board[i][j]->type==pawn)printf("%s",bpawn);
						if(board[i][j]->type==knight)printf("%s",bknight);
					}
					printf(" ");
					if((i+j)%2==1)printf("%s",dshade);
					
				} 
		
			}
		printf("\n");
	}
}else if(player==1){

	for(int i=7;i>=0;i--){
	printf("%s %c ",dshade,(char)(i+65));
	}
	printf("\n");

	for(int i=7;i>=0;i--){
		printf("       %d ",(8-i));
			for(int j=7;j>=0;j--){
				if((board[i][j]==0)||(board[i][j]->empty)==1){
					if((i+j)%2==1)printf("%s   %s",shade,dshade);
					else{printf("%s   ",dshade);
					}
				}
				else{
					if((i+j)%2==1)printf("%s",shade);
					printf(" ");
					if(board[i][j]->color==white){
						if(board[i][j]->type==king)printf("%s",wking);
						if(board[i][j]->type==queen)printf("%s",wqueen);
						if(board[i][j]->type==rook)printf("%s",wrook);
						if(board[i][j]->type==bishop)printf("%s",wbishop);
						if(board[i][j]->type==pawn)printf("%s",wpawn);
						if(board[i][j]->type==knight)printf("%s",wknight);
					}else{
						if(board[i][j]->type==king)printf("%s",bking);
						if(board[i][j]->type==queen)printf("%s",bqueen);
						if(board[i][j]->type==rook)printf("%s",brook);
						if(board[i][j]->type==bishop)printf("%s",bbishop);
						if(board[i][j]->type==pawn)printf("%s",bpawn);
						if(board[i][j]->type==knight)printf("%s",bknight);
					}
					printf(" ");
					if((i+j)%2==1)printf("%s",dshade);
					
				} 
		
			}
		printf("\n");
	}
}
	printf("\n");
}

int countpieces(){
	int numwhite=0;
	int numblack=0;
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
		if(board[i][j]->color==white)numwhite++;
		if(board[i][j]->color==black)numblack++;		
		}
	}
}


int initialize_pieces(){
empty.empty=1;
empty.color=2;
empty.type=0;

wking.empty=0;
wking.color=white;
wking.type=king;
wqueen.empty=0;
wqueen.color=white;
wqueen.type=queen;
wrookq.empty=0;
wrookq.color=white;
wrookq.type=rook;
wrookk.empty=0;
wrookk.color=white;
wrookk.type=rook;
wbishopl.empty=0;
wbishopl.color=white;
wbishopl.type=bishop;
wbishopd.empty=0;
wbishopd.color=white;
wbishopd.type=bishop;
wknightq.empty=0;
wknightq.color=white;
wknightq.type=knight;
wknightk.empty=0;
wknightk.color=white;
wknightk.type=knight;
wpawna.empty=0;
wpawna.color=white;
wpawna.type=pawn;
wpawnb.empty=0;
wpawnb.color=white;
wpawnb.type=pawn;
wpawnc.empty=0;
wpawnc.color=white;
wpawnc.type=pawn;
wpawnd.empty=0;
wpawnd.color=white;
wpawnd.type=pawn;
wpawne.empty=0;
wpawne.color=white;
wpawne.type=pawn;
wpawnf.empty=0;
wpawnf.color=white;
wpawnf.type=pawn;
wpawng.empty=0;
wpawng.color=white;
wpawng.type=pawn;
wpawnh.empty=0;
wpawnh.color=white;
wpawnh.type=pawn;

bpawna.empty=0;
bpawna.color=black;
bpawna.type=pawn;
bpawnb.empty=0;
bpawnb.color=black;
bpawnb.type=pawn;
bpawnc.empty=0;
bpawnc.color=black;
bpawnc.type=pawn;
bpawnd.empty=0;
bpawnd.color=black;
bpawnd.type=pawn;
bpawne.empty=0;
bpawne.color=black;
bpawne.type=pawn;
bpawnf.empty=0;
bpawnf.color=black;
bpawnf.type=pawn;
bpawng.empty=0;
bpawng.color=black;
bpawng.type=pawn;
bpawnh.empty=0;
bpawnh.color=black;
bpawnh.type=pawn;
bknightk.empty=0;
bknightk.color=black;
bknightk.type=knight;
bknightq.empty=0;
bknightq.color=black;
bknightq.type=knight;
bbishopl.empty=0;
bbishopl.color=black;
bbishopl.type=bishop;
bbishopd.empty=0;
bbishopd.color=black;
bbishopd.type=bishop;
brookk.empty=0;
brookk.color=black;
brookk.type=rook;
brookq.empty=0;
brookq.color=black;
brookq.type=rook;
bqueen.empty=0;
bqueen.color=black;
bqueen.type=queen;
bking.empty=0;
bking.color=black;
bking.type=king;
}

void startingarrangement(){
	board[6][0]=&wpawna;
	board[6][1]=&wpawnb;
	board[6][2]=&wpawnc;
	board[6][3]=&wpawnd;
	board[6][4]=&wpawne;
	board[6][5]=&wpawnf;
	board[6][6]=&wpawng;
	board[6][7]=&wpawnh;

	board[1][0]=&bpawna;
	board[1][1]=&bpawnb;
	board[1][2]=&bpawnc;
	board[1][3]=&bpawnd;
	board[1][4]=&bpawne;
	board[1][5]=&bpawnf;
	board[1][6]=&bpawng;
	board[1][7]=&bpawnh;

	board[7][0]=&wrookq;
	board[7][7]=&wrookk;
	board[7][4]=&wking;
	board[7][3]=&wqueen;
	board[7][2]=&wbishopl;
	board[7][5]=&wbishopd;
	board[7][1]=&wknightq;
	board[7][6]=&wknightk;

	board[0][0]=&brookq;
	board[0][7]=&brookk;
	board[0][1]=&bknightq;
	board[0][6]=&bknightk;
	board[0][5]=&bbishopl;
	board[0][2]=&bbishopd;
	board[0][3]=&bqueen;
	board[0][4]=&bking;

	for(int i=2;i<6;i++){
		for(int j=0;j<8;j++){
			board[i][j]=&empty;
		}
	}
//set every piece as unmoved
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			board[i][j]->timesmoved=0;
			board[i][j]->justmoved=0;	
		}
	}

}


int countmoves(bool player){
	int counter=0;
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
			
				if(board[i][j]->color==player){
					for(int m=0;m<8;m++){
						for(int n=0;n<8;n++){
							if(legalmove(player,&i,&j,&m,&n))counter++;
						}
					}			

				}
		
			}

		}
	return counter;
}


int setcheck(bool player){

	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			if(board[i][j]->color==player&&board[i][j]->type==king){

			check=isattacked(player,i,j);

			}

		}

	}
	return check;
}

int isattacked(int defender, int i, int j){

	int attacks=0;

	int originalcolor = board[i][j]->color;

	bool opplayer = (defender+1)%2;
	bool isempty = board[i][j]->empty;

	if(originalcolor!=defender)board[i][j]->color = defender;
	if(isempty==1)board[i][j]->empty = 0;

	for(int k=0;k<8;k++){
		for(int l=0;l<8;l++){
			if(board[k][l]->color==opplayer){

			if(legalmove(opplayer,&k,&l,&i,&j)==1)attacks++;

			}

		}

	}

	board[i][j]->color = originalcolor;
	board[i][j]->empty = isempty;
	return attacks;


}

int isdefended(int defender, int i, int j){

	int defenses = 0;

	int originalcolor = board[i][j]->color;

	int opponent = (defender+1)%2;
	bool isempty = board[i][j]->empty;
	
	if(originalcolor!=opponent)board[i][j]->color = opponent;
	if(isempty==1)board[i][j]->empty==0;
	
	for(int k=0;k<8;k++){
		for(int l=0;l<8;l++){

			if(board[k][l]->color==defender){

			if(legalmove(defender,&k,&l,&i,&j)==1)defenses++;

			}

		}

	}

	board[i][j]->color=originalcolor;
	board[i][j]->empty = isempty;
	return defenses;

}



