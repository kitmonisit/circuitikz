/***************************************************************************
 *   Copyright (C) 2007 by Massimo Redaelli   *
 *   mredaelli@elet.polimi.it   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/



#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

typedef struct nodo
{
	char x_coord[5],y_coord[5];
	char component_to_previous[30];
	char label_component[30];
	struct nodo * next;
} NODO;

typedef struct etichette_nodi
{
	char label[2],x_co_old[5],y_co_old[5];
} NODE_LABEL;

using namespace std;

int menu ( char openfile[] );
int file_create ( int choose,char openfile[] );
int scan_file ( char openfile[] );
int cerca_tipo ( char linea_testo[] );
int menu_trasformazione ( int in_file,int out_file,char openfile[] );
int trasforma_pgf_pst ( char openfile[] );
int trasforma_pgf_spice ( char openfile[] );
int trasforma_pst_pgf ( char openfile[] );
int trasforma_pst_spice ( char openfile[] );
int trasforma_spice_pgf ( char openfile[] );
int trasforma_spice_pst ( char openfile[] );
void riconosci_componente ( string linea,NODO * comp,int inizio );
void crea_lista ( string linea,int inizio,int fine );
int traduttore_char_int ( char num[5] );

void pauseAndWait()
{
	char c;
	cin >> c;
}

void clearscr()
{
	cout << endl << endl << endl;
}

int main ()
{
	ifstream provafile;
	char file[20];
	int scelta;
	int tipo;
inizio_file:
	fflush ( stdin );
	cout << "Che file vuoi aprire?\n(si deve trovare nella cartella del programma)\n";
	cin.getline ( file,20 );
	provafile.open ( file );
	if ( provafile.is_open() )
	{
		cout<<"Il file esiste e puo' essere elaborato\n";
	}
	else
	{
		cout<<"Controllare il nome del file\n";
		pauseAndWait();
		clearscr();
		goto inizio_file;
	};
	scelta=menu ( file );
	file_create ( scelta,file );
	tipo=scan_file ( file );
	menu_trasformazione ( tipo,scelta,file );
	pauseAndWait();
}

int trasforma_pgf_pst ( char openfile[] )
{
	ifstream readfile;
	ofstream writefile;
	readfile.open ( openfile );
	string s, linea;
	int c=0,inizio,inizio_cod;
	int counter=0;
	if ( readfile.is_open() ) cout<<"File in lettura aperto\n";else cout<<"Errorissimo!";
	writefile.open ( "trasformato.tex" );
	if ( writefile.is_open() ) cout<<"File in scrittura aperto\n";else cout<<"Errorissimo!";
	cout<<"Trasformerai un file di tipo CircuiTikz in un file di tipo Pst-Circ.\n";
	writefile<<"\\begin{pspicture}";
	writefile.close();
	while ( getline ( readfile, linea ) )
		s += linea;
	while ( s[counter]!='\0' ) {counter++;};
	for ( inizio=0; inizio<counter; inizio++ )
	{
		if ( s[inizio]=='\\'&&s[inizio+1]=='d'&&s[inizio+2]=='r'&&s[inizio+3]=='a'&&s[inizio+4]=='w' ) break;
	};
	inizio_cod=inizio+5;
	while ( s[inizio_cod]==' '||s[inizio_cod]=='\t' ) inizio_cod++;
	pauseAndWait();
	clearscr();
	crea_lista ( s,inizio_cod,counter );
};

void crea_lista ( string linea,int inizio,int fine )
{
	ofstream writefile;
	NODO * next_node=NULL;
	NODO * nodo,* head,* scorrere;
	NODE_LABEL stringa_nodi[30];
	int c,counter_x,counter_y,init,y_max_int,y_min_int,x_max_int,x_min_int,carattere=65,x_co_int=0,y_co_int=0,vector_count=0,cycle_counter=0;
	char x_co[5],y_co[5],x_max[5],y_max[5],x_min[5],y_min[5];
	for ( init=0; init<5; init++ )
	{
		x_max[init]='\0';
		y_max[init]='\0';
		x_min[init]='\0';
		y_min[init]='\0';
	};
	x_max[0]='0';
	y_max[0]='0';
	x_min[0]='0';
	y_min[0]='0';
	for ( c=inizio; c<fine; c++ )
	{
		counter_x=0;
		counter_y=0;
		for ( init=0; init<5; init++ )
		{
			x_co[init]='\0';
			y_co[init]='\0';
		};
		if ( linea[c]=='(' )
		{
			c++;
			while ( linea[c]!=',' )
			{
				x_co[counter_x]=linea[c];
				counter_x++;
				c++;
			};
			c++;
			while ( linea[c]!=')' )
			{
				y_co[counter_y]=linea[c];
				counter_y++;
				c++;
			};
			nodo=new NODO;
			for ( init=0; init<5; init++ )
			{
				nodo->x_coord[init]='\0';
				nodo->y_coord[init]='\0';
			};
			nodo->component_to_previous[0]='\0';
			strcpy ( nodo->x_coord,x_co );
			strcpy ( nodo->y_coord,y_co );
			nodo->next=next_node;
			next_node=nodo;
		};
		if ( linea[c]=='t'&&linea[c+1]=='o'&&linea[c+2]=='[' )
		{
			riconosci_componente ( linea,nodo,c+3 );
		}
		else if ( linea[c]=='-'&&linea[c+1]=='-' )
		{
			strcpy ( nodo->component_to_previous,"wire" );
		};
	};
	head=nodo;
	while ( nodo!=NULL )
	{
		if ( strcmp ( nodo->x_coord,"0" ) >=0&&strcmp ( nodo->x_coord,x_max ) >0 ) strcpy ( x_max,nodo->x_coord );
		else if ( strcmp ( nodo->x_coord,"0" ) <0&&strcmp ( nodo->x_coord,x_min ) <0 ) strcpy ( x_min,nodo->x_coord );
		if ( strcmp ( nodo->y_coord,"0" ) >=0&&strcmp ( nodo->y_coord,y_max ) >0 ) strcpy ( y_max,nodo->y_coord );
		else if ( strcmp ( nodo->y_coord,"0" ) <0&&strcmp ( nodo->y_coord,y_min ) <0 ) strcpy ( y_min,nodo->y_coord );
		nodo=nodo->next;
	};
	y_max_int=traduttore_char_int ( y_max );
	y_min_int=traduttore_char_int ( y_min );
	x_max_int=traduttore_char_int ( x_max );
	x_min_int=traduttore_char_int ( x_min );
	writefile.open ( "trasformato.tex",ios::app );
	if ( x_min_int<=0&&y_min_int<=0 ) writefile<<"("<<abs ( x_min_int ) +abs ( x_max_int ) <<","<<abs ( y_min_int ) +abs ( y_max_int ) <<")\n";
	else if ( x_min_int<=0&&y_min_int>0 ) writefile<<"("<<abs ( x_min_int ) +abs ( x_max_int ) <<","<<abs ( y_max_int ) <<")\n";
	else if ( x_min_int>0&&y_min_int<=0 ) writefile<<"("<<abs ( x_max_int ) <<","<<abs ( y_min_int ) +abs ( y_max_int ) <<")\n";
	writefile.close();
	nodo=head;
	while ( nodo!=NULL )
	{
		int dupli=0;
		strcpy ( x_co,nodo->x_coord );
		strcpy ( y_co,nodo->y_coord );
		scorrere=nodo->next;
		while ( scorrere!=NULL )
		{
			if ( strcmp ( scorrere->x_coord,x_co ) ==0&&strcmp ( scorrere->y_coord,y_co ) ==0 )
			{
				dupli=1;
				break;
			}
			else scorrere=scorrere->next;
		};
		x_co_int=traduttore_char_int ( x_co );
		y_co_int=traduttore_char_int ( y_co );
		if ( dupli!=1 )
		{
			strcpy ( stringa_nodi[vector_count].x_co_old,x_co );
			strcpy ( stringa_nodi[vector_count].y_co_old,y_co );
			stringa_nodi[vector_count].label[0]= ( char ) ( vector_count+65 );
			stringa_nodi[vector_count].label[1]='\0';
			vector_count++;
			writefile.open ( "trasformato.tex",ios::app );
			writefile<<"\\pnode(";
			if ( x_min_int<0 )
			{
				writefile<<abs ( x_min_int ) +x_co_int;
			}
			else if ( x_min_int>=0 )
			{
				writefile<<x_co;
			};
			writefile<<",";
			if ( y_min_int<0 )
			{
				writefile<< ( abs ( y_min_int ) +y_co_int );
			}
			else if ( y_min_int>=0 )
			{
				writefile<<y_co;
			};
			writefile<<"){"<< ( char ) carattere<<"}\n";
			writefile.close();
			carattere++;
			nodo=nodo->next;
		}
		else nodo=nodo->next;
	};

	cycle_counter=vector_count;
	nodo=head;
	while ( nodo->next!=NULL )
	{
		if ( ( nodo->next )->component_to_previous[0]!='\0' )
		{
			for ( vector_count=0; vector_count<cycle_counter; vector_count++ )
			{
				if ( strcmp ( stringa_nodi[vector_count].x_co_old,nodo->x_coord ) ==0&&strcmp ( stringa_nodi[vector_count].y_co_old,nodo->y_coord ) ==0 )
				{
					writefile.open ( "trasformato.tex",ios::app );
					writefile<<"\\"<<nodo->next->component_to_previous<<"("<<stringa_nodi[vector_count].label<<")";
					break;
				};
			};
			for ( vector_count=0; vector_count<cycle_counter; vector_count++ )
			{
				if ( strcmp ( stringa_nodi[vector_count].x_co_old,nodo->next->x_coord ) ==0&&strcmp ( stringa_nodi[vector_count].y_co_old,nodo->next->y_coord ) ==0 )
				{
					writefile<<"("<<stringa_nodi[vector_count].label<<")\n";
					writefile.close();
					break;
				};
			};
			nodo=nodo->next;
		}
		else {nodo=nodo->next;};
	};
	writefile.open ( "trasformato.tex",ios::app );
	writefile<<"\\end{pspicture}";
	writefile.close();
};

int traduttore_char_int ( char num[5] )
{
	int tot_int=0,tot,count=0,counter=0,c;
	if ( num[0]!=45 ) tot=num[0]+num[1]+num[2]+num[3]+num[4];else tot=0- ( num[1]+num[2]+num[3]+num[4] );
	if ( tot>57 )
	{
		for ( counter=0; counter<5; counter++ )
		{
			if ( ( int ) num[counter]==0 ) break;
		};
		count=counter;
		for ( c=0; c<counter; c++ )
		{
			tot_int=tot_int+ ( num[count-1]-48 ) *pow ( 10,c );
			count--;
		};
		return tot_int;
	}
	else if ( tot>=48&&tot<=57 ) return ( int ) tot-48;
	else if ( tot<=-57 )
	{
		for ( counter=0; counter<4; counter++ )
		{
			if ( ( int ) num[counter+1]==0 ) break;
		};
		count=counter;
		for ( c=0; c<counter; c++ )
		{
			tot_int=tot_int+ ( num[count-1]-48 ) *pow ( 10,c );
			count--;
		};
		return tot_int;
	}
	else if ( tot<=-48&&tot>=-57 ) return ( ( int ) tot+48 );
};

int menu_trasformazione ( int in_file,int out_file,char openfile[] )
{
	if ( in_file==1 )
	{
		if ( out_file==2 ) trasforma_pgf_pst ( openfile );
		else if ( out_file==3 ) trasforma_pgf_spice ( openfile );
	}
	else if ( in_file==2 )
	{
		if ( out_file==1 ) trasforma_pst_pgf ( openfile );
		else if ( out_file==3 ) trasforma_pst_spice ( openfile );
	}
	else if ( in_file==3 )
	{
		if ( out_file==1 ) trasforma_spice_pgf ( openfile );
		else if ( out_file==2 ) trasforma_spice_pst ( openfile );
	};
}

void riconosci_componente ( string linea,NODO * comp,int inizio )
{
	char etichetta[20];
	for ( int c=0; c<20; c++ )
	{
		etichetta[c]='\0';
	};
	int counter=0;
	while ( linea[inizio]==' ' ) inizio++;
	switch ( linea[inizio] )
	{
		case 'v':
			strcpy ( comp->component_to_previous,"Ucc" );
			break;
		case 'V':
			strcpy ( comp->component_to_previous,"Ucc" );
			break;
		case 'R':
			strcpy ( comp->component_to_previous,"resistor" );
			break;
		case 'r':
			strcpy ( comp->component_to_previous,"resistor" );
			break;
		case 'I':
			strcpy ( comp->component_to_previous,"Icc" );
			break;
		case 'i':
			strcpy ( comp->component_to_previous,"Icc" );
			break;
	};
	inizio++;
	while ( linea[inizio]!='$' ) inizio++;
	inizio++;
	while ( linea[inizio]!='$' )
	{
		etichetta[counter]=linea[inizio];
		counter++;
		inizio++;
	}
	strcpy ( comp->label_component,etichetta );
}

int cerca_tipo ( char linea_testo[] )
{
	int n=0;
	char circuitikz[18];
	strcpy ( circuitikz,"\\begin{circuitikz" );
	circuitikz[17]='\0';
	char pst[18]="\\begin{pspicture}";
	char inizio[18];
	strncpy ( inizio,linea_testo,17 );
	inizio[17]='\0';
	if ( strcmp ( inizio,circuitikz ) ==0 )
	{
		return 1;
	}
	else if ( strcmp ( inizio,pst ) ==0 )
	{
		return 2;
	}
	else
	{
		return 3;
	};
}

int scan_file ( char openfile[] )
{
	char n[200];
	int tipo=0;
	ifstream file;
	file.open ( openfile );
	while ( !file.eof() )
	{
		file.getline ( n,200 );
		tipo=cerca_tipo ( n );
		if ( tipo==1 )
		{
			cout<<"Il file e' del tipo CircuiTikz\n";
			break;
		}
		else if ( tipo==2 )
		{
			cout<<"Il file e' del tipo Pst-Circ\n";
			break;
		};
	};
	return tipo;
	file.close();
}
int file_create ( int choose,char openfile[] )
{
	ifstream myfile;
	ofstream newfile;
	myfile.open ( openfile );
	cout<<"File aperto\n";
	switch ( choose )
	{
		case 1:
			newfile.open ( "trasformato.tex" );
			break;
		case 2:
			newfile.open ( "trasformato.tex" );
			break;
		case 3:
			newfile.open ( "trasformato.cir" );
			break;
	};
	newfile.close();
	myfile.close();
}

int menu ( char openfile[] )
{
	int n=0;
	int scelta=0;
	int inizio=0;
	char ext[3];
	while ( n<20 )
	{
		if ( openfile[n]=='.' )
		{
			inizio=n;
			ext[0]=openfile[inizio+1];
			ext[1]=openfile[inizio+2];
			ext[2]=openfile[inizio+3];
			break;
		}
		else {n++;}
	};
	if ( ext[0]=='c' && ext[1]=='i' && ext[2]=='r' )
	{
	inizio_Spice:
		clearscr();
		cout<< "Scegli in che formato vuoi trasformare il tuo file Spice:\n";
		cout<< "1. CircuiTikz\n";
		cout<< "2. Pst-Circ\n";
		cout<< "Scelta: ";
		cin>> scelta;
		if ( scelta!=2&&scelta!=1 )
		{
			cout<<"Errore Scelta!\n";
			scelta=0;
			pauseAndWait();
			goto inizio_Spice;
		}
	}
	else if ( ext[0]=='t' && ext[1]=='e' && ext[2]=='x' )
	{
	Select:
		scelta=scan_file ( openfile );
		pauseAndWait();
		if ( scelta!=2&&scelta!=1 )
		{
			cout<<"Errore interno!\n";
			scelta=0;
			pauseAndWait();
			goto Select;
		}
		if ( scelta==1 )
		{
		inizio_Pgf:
			clearscr();
			cout<< "Scegli in che formato vuoi trasformare il tuo file CircuiTikz:\n";
			cout<< "2. Pst-Circ\n";
			cout<< "3. Spice\n";
			cout<< "Scelta: ";
			cin>> scelta;
			if ( scelta!=2&&scelta!=3 )
			{
				cout<<"Errore Scelta!\n";
				scelta=0;
				pauseAndWait();
				goto inizio_Pgf;
			}
		}
		else
		{
		inizio_Pst:
			clearscr();
			cout<< "Scegli in che formato vuoi trasformare il tuo file Pst-Circ:\n";
			cout<< "1. CircuiTikz\n";
			cout<< "3. Spice\n";
			cout<< "Scelta: ";
			cin>> scelta;
			if ( scelta!=3&&scelta!=1 )
			{
				cout<<"Errore Scelta!\n";
				scelta=0;
				pauseAndWait();
				goto inizio_Pst;
			}
		};
	};
	return scelta;
};

int trasforma_pgf_spice ( char openfile[] )
{
};
int trasforma_pst_pgf ( char openfile[] )
{
};
int trasforma_pst_spice ( char openfile[] )
{
};
int trasforma_spice_pgf ( char openfile[] )
{
};
int trasforma_spice_pst ( char openfile[] )
{
};