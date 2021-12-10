//////////////////////////////////////////////////////////////////
//								 								//
//		Projekt 3 - Prace s datovymi strukturami			 	//
//								 								//
//			  	Matus Vrablik			 						//
//								 								//
//			  27.November 2019			 						//
//								 								//
//////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
typedef struct
{
	int rows;
	int cols;
	unsigned int **cells;
} Map;

bool ismap(Map *m,int row,int col)
{
	if (row<=m->rows && row>0 && col<=m->cols&& col>0) return true;
	else return false;
}

void map_init(Map *m)
{
	if (m!=NULL)
	{
		m->cells=malloc(m->rows*sizeof(unsigned int*));
		for (int i=0;i++<m->rows;)
			m->cells[i]=malloc(m->cols*sizeof(unsigned int));
	}
	else fprintf(stderr,"Inicializaton failed");
}

void free_map(Map *m)
{
	for (int i=0;i++<m->rows;)
		free(m->cells[i]);
	free(m->cells);
}

bool isnum(char s[])
{
	bool isnumber=true;
	for(int i=0;i<(signed)strlen(s);i++)
	{	if (!isdigit(s[i])) 
		{	isnumber=false;//scanf %d -> ak nenacita da 0
			i=strlen(s);
		}
	}
	if (isnumber) return true;
	else return false;
}

int load_num(FILE *f)
{
	char c,s[20];
	fscanf(f,"%c",&c);
	int i=0;
	while (c!=' '&&c!='\n'&&c!=EOF)
	{
		s[i++]=c;	
		fscanf(f,"%c",&c);
	}	
	s[i]='\0';
	if (isnum(s)) return atoi(s);
	else return 99;
}

bool isborder(Map *m,int r,int c,int border)
{
	switch(border)
	{
		case 1 : if ((m->cells[r][c] % 2)>0) return true;
			 else return false;
		case 2 : if (((m->cells[r][c] % 4) - (m->cells[r][c] % 2))>0) return true;
			 else return false;
		case 3 : if ((m->cells[r][c] / 4)>0) return true;
			 else return false;
		default: return true;
	}
}

bool triangle_down(int row,int col)
{
	int triangle=((row%2)+(col%2));
	if (triangle==1) return false;
	else return true;
}

bool is_edge(Map *m,int row,int col)
{
	if (row==1 || row==m->rows) return true;
	else if (col==1 || col==m->cols) return true;
		else return false;
}

int which_side_in(Map *m,int row,int col,int prev_row,int prev_col)
{
	if (is_edge(m,row,col)&&(prev_row==0 && prev_col==0))
	{	if (col-1==0)
			if (!isborder(m,row,col,1)) return 1;
			else if (((row-1==0)&&(triangle_down(row,col)))||((row+1==m->rows+1)&&(!triangle_down(row,col))))
				if (!isborder(m,row,col,3)) return 3;
				else return -13;//-> vertical edge is border
			     else return -11;//-> left edge is border
		else
			if (col+1==m->cols+1)
				if (!isborder(m,row,col,2)) return 2;
				else if (((row-1==0)&&(triangle_down(row,col)))||((row+1==m->rows+1)&&(!triangle_down(row,col))))
					if (!isborder(m,row,col,3)) return 3;
					else return -13;//-> vertical edge is border
				     else return -12;//-> right edge is border
			else 
				if (((row-1==0)&&(triangle_down(row,col)))||((row+1==m->rows+1)&&(!triangle_down(row,col))))
					if (!isborder(m,row,col,3)) return 3;
					else return -13;//-> vertical edge is border
				else return -3;//-> row,col is not an edge of map
	} 
	else if (!(prev_row==0 && prev_col==0))
		{	int r=row-prev_row,c=col-prev_col;
			if (r!=0)
				if (!isborder(m,row,col,3)) return 3;
				else return 0;
			else 
			{	if (c>0)
				{	if (!isborder(m,row,col,1)) return 1;
					else return 0;
				}
				else 
				{	if (c<0)
					{	if (!isborder(m,row,col,2)) return 2;
						else return 0;
					}
				}
			}
		}
	     else return 0;
	return 0;
}

int get_rc(int max,char rc)
{
	if (rc=='r')
	{	if (max % 2 ==0 ) return 1;
		else return 2;}
	else if (max % 2==0) return 2;
		 else return 1;
}

int start_border(Map *m,int r,int c,int prev_row,int prev_col,int leftright)
{
	switch (which_side_in(m,r,c,prev_row,prev_col))
	{	case 1:if (leftright==1)
				if (triangle_down(r,c)) return 3;
				else return 2;
			else if (triangle_down(r,c)) return 2;
				else return 3;
		case 2:if (leftright==1)
				if (triangle_down(r,c)) return 1;
				else return 3;
			else if (triangle_down(r,c)) return 3;
				else return 1;
		case 3:if (leftright==1)
				if (triangle_down(r,c)) return 2;
				else return 1;
			else if (triangle_down(r,c)) return 1;
				else return 2;
		default:return 0;
	}
}

void find_exit(Map *m,int row,int col,int prev_row,int prev_col,int path)
{
	while (ismap(m,row,col))
	{	int go_border=start_border(m,row,col,prev_row,prev_col,path);
		if (go_border==0) 
		{	printf("unable to enter\n");
			row=0;
		}
		else 
		{ for(int i=0;i++<3;)
		    {	if (!isborder(m,row,col,go_border)) 
			{	printf("%d,%d\n",row,col);
				prev_row=row;
				prev_col=col;
				switch (go_border)
				{	case 1:col-=1;
						break;
					case 2:col+=1;
						break;
					case 3:if (triangle_down(row,col)) row-=1;
						else row+=1;
						break;
				}					 
				i=3;
				find_exit(m,row,col,prev_row,prev_col,path);
				row=0;
			}
			if (path==1)
			{	if(triangle_down(row,col)) go_border-=1;
				else go_border+=1;
			}
			else 
			{	if (triangle_down(row,col)) go_border+=1;
				else go_border-=1;
			}
			if (go_border>3) go_border-=3; 
			if (go_border<1) go_border+=3;
		    }
		}
	}
}

void arg_err()
{
	fprintf(stderr,"ERROR: Either one or more of the arguments have wrong form\n"
		"       or there is wrong number of arguments\nFor more information type \"./proj3 --help\"\n");
}

bool testing(FILE *f)
{
	Map test;
	int r=load_num(f),c=load_num(f);
	bool Invalid=false;
	if (r==99 || c==99) return false;
	else 
	{	map_init(&test);
		for (int i=0;i++<r;)
		{	for (int j=0;j++<c;)
			{	test.cells[i][j]=load_num(f);
				int cell=test.cells[i][j];
				if (cell==99 || cell>7 || cell<0) 
				{	Invalid=true;
					printf("invalid\n");
					j=c;
				}
				char ch;
				fscanf(f,"%c",&ch);
				if (j<c && !strcmp(&ch,"\n"))
				{	Invalid=true;
					j=c;
				}
			}
			if (Invalid) i=r;
		}
		free_map(&test);
		if (Invalid) return false;
		else return true;
		//int end;
		//fscanf(f,"%d",&end);
		//printf("%d\n",end);
		//if (end!=0) return false;
		//else return true;
	}
	return true;
}

int main(int argc, char *argv[])
{
	switch (argc)
	{	case 2:if (strcmp(argv[1],"--help")==0) 
			{	printf("this is guide \n");
				return 0;
			}
			else 
			{	arg_err();
				return 1;
			}
		case 3:if (strcmp(argv[1],"--test")==0)
			{	FILE *f=fopen(argv[2],"r");
				if (f==NULL) fprintf(stderr,"ERROR: File does not exist\n");
				else if (testing(f)) printf("Valid\n");
					else printf("Invalid\n"); 
				fclose(f);
				return 0;
			}
			else
			{	arg_err();
				return 1;
			}
		case 5 :{FILE *f=fopen(argv[4],"r+");
			 if (f==NULL) 
			 {	printf("problem\n");
				return 2;
			 }
			 else
			 {	Map map;
				map.rows=load_num(f);
				map.cols=load_num(f);
				int start_row=atoi(argv[2]),start_col=atoi(argv[3]);
				map_init(&map);
				if (ismap(&map,start_row,start_col))
				{	
					for (int i=0;i++<map.rows;)
						for (int j=0;j++<map.cols;)
							map.cells[i][j]=load_num(f);
					int way;
					if (strcmp(argv[1],"--rpath")==0) way=2;
					if (strcmp(argv[1],"--lpath")==0) way=1;
					int prev_row=0,prev_col=0;
					find_exit(&map,start_row,start_col,prev_row,prev_col,way);
				}
				else fprintf(stderr,"Starting field out of map boundaries");
				free_map(&map);
			 	fclose(f);
			 }
			 return 0;
			}
		default :arg_err();
			 return 1;
			 
	}
}
