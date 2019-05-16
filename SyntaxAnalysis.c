#include "stdio.h"
#include "stdlib.h"
#include "constvar.h"
#include<string.h>

extern TERMINAL nextToken();
extern void renewLex();
extern void initEscapeList();
static int match (int t);
static int strcompare(char *sstr, char *tstr);	
static IDTABLE* InstallID();		
static IDTABLE* LookupID();			
static void FreeExit();
static int cast2int(EXPVAL exp);		
static char cast2char(EXPVAL exp);		
static EXPVAL calculateExp(EXPVAL val1, int calop,EXPVAL val2);
static int Prod_FUNC();
static int Prod_S();
static int Prod_D();
static int Prod_L(int type);
static int Prod_T();
static int Prod_A();
static int Prod_B();
static int Prod_B1(int bval);
static int Prod_TB();
static int Prod_TB1(int bval);
static int Prod_FB();
static EXPVAL Prod_E();
static EXPVAL Prod_E1(EXPVAL val);
static EXPVAL Prod_TE();
static EXPVAL Prod_TE1(EXPVAL val);
static EXPVAL Prod_F();

extern FILE *sFile;
static TERMINAL lookahead;
static int curtoken_num;
static char curtoken_str[MAXTOKENLEN];
static IDTABLE *IDTHead=NULL;
static int run_status=1;	//0不执行，1正常执行，2跳过执行
                          //在if-else中：3由break产生的不执行，4由continue产生的不执行
static char printCharList[62][10];//锟绞凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟节斤拷token锟斤拷应锟斤拷锟斤拷锟斤拷转锟斤拷为锟斤拷应锟斤拷锟街凤拷
extern int row, col;

void initPrintCharList(){ //锟斤拷始锟斤拷printCharList锟斤拷锟斤拷
	strcpy(printCharList[1], " int");
	strcpy(printCharList[2]," id");
	strcpy(printCharList[3]," letter");
	strcpy(printCharList[11]," <");
	strcpy(printCharList[12]," >");
	strcpy(printCharList[13]," <=");
	strcpy(printCharList[14]," >=");
	strcpy(printCharList[15]," ==");
	strcpy(printCharList[16]," !=");
	strcpy(printCharList[17]," +");
	strcpy(printCharList[18]," -");
	strcpy(printCharList[19]," *");
	strcpy(printCharList[20]," /");
	strcpy(printCharList[21]," (");
	strcpy(printCharList[22]," )");
	strcpy(printCharList[23]," {");
	strcpy(printCharList[24]," }");
	strcpy(printCharList[25]," ,");
	strcpy(printCharList[26]," ;");
	strcpy(printCharList[27]," =");
	strcpy(printCharList[28]," '");
	strcpy(printCharList[29]," \"");
	strcpy(printCharList[30]," continue");
	strcpy(printCharList[31]," break");
	strcpy(printCharList[51]," &&");
	strcpy(printCharList[52]," ||");
	strcpy(printCharList[53]," !");
	strcpy(printCharList[54]," TRUE");
	strcpy(printCharList[55]," FALSE");
	strcpy(printCharList[56]," int");
	strcpy(printCharList[57]," char");
	strcpy(printCharList[58]," if");
	strcpy(printCharList[59]," else");
	strcpy(printCharList[60]," while");
	strcpy(printCharList[61]," show");
	return;
}

void SyntaxAnalysis()
{
initPrintCharList();
initEscapeList();
#if defined(AnaTypeLex)
//testing lexical analysis
	TERMINAL token;
	token=nextToken();
	while (token.token!=ERR)
	{	if (token.token==SYN_NUM)
			printf("LEX: %d,   %d \t\tnum\n",token.token,token.tokenVal.number);
		else if (token.token==SYN_ID)
			printf("LEX: %d,   %s \t\tid\n",token.token,token.tokenVal.str);
		else if (token.token==SYN_LETTER)
		  printf("LEX: %d,   %c \t\tletter\n",token.token,(char)token.tokenVal.number);
		else
			printf("LEX: %d, %s\n",token.token, printCharList[token.token]);
		token=nextToken();
	}
#else
//syntax analysis
	lookahead=nextToken();
	if (Prod_FUNC()==ERR)
		printf("PROGRAM HALT!\n");
//	FreeExit();

#endif
}

static int match (int t)
{
	char *p,*q;
	if (lookahead.token == t)
	{	if (t==SYN_NUM)
			curtoken_num=lookahead.tokenVal.number;
		else if (t==SYN_LETTER)
		  curtoken_num=(int)lookahead.tokenVal.number;
		else if (t==SYN_ID)
			for (p=lookahead.tokenVal.str,q=curtoken_str;(*q=*p)!='\0';p++,q++);
		lookahead = nextToken( );
	}
	else{
		lookahead = nextToken();
		printf("Error  row: %d , col: %d:  expect a \"%s\" before %s\n", row,col,printCharList[t],printCharList[lookahead.token]);
		FreeExit();
	}
	return(0);
}

static int strcompare(char *sstr, char *tstr)
{
	while (*sstr==*tstr && *sstr!='\0') { sstr++; tstr++; }
	if (*sstr=='\0' && *tstr=='\0')	return(0);
	else return(ERR);
}

static IDTABLE* InstallID()
{
	IDTABLE *p,*q;
	char *a,*b;
	p=IDTHead; q=NULL;
	while (p!=NULL && strcompare(curtoken_str,p->name)==ERR)
	{
		q=p;
		p=p->next;
	}
	if (p!=NULL)
		return(NULL);
	else             
	{
		p=(IDTABLE*)malloc(sizeof(IDTABLE));
		if (q==NULL)
			IDTHead=p;
		else       
			q->next=p;
		p->next=NULL;
		for (a=curtoken_str,b=p->name;(*b=*a)!='\0';a++,b++);
		return(p);
	}
}

static IDTABLE* LookupID()
{
	IDTABLE *p;
	p=IDTHead;
	while (p!=NULL && strcompare(curtoken_str,p->name)==ERR)
		p=p->next;
	if (p==NULL)
		return(NULL);
	else
		return(p);
}

static void FreeExit()
{
	IDTABLE *p,*q;
	p=IDTHead;
	while ((q=p)!=NULL)
	{	p=p->next;
		#if defined(AnaTypeSyn)
		printf("NAME:%s, TYPE:%d, ",q->name,q->type);
		if (q->type==ID_INT)
			printf("VALUE:%d\n",q->val.intval);
		else if (q->type==ID_CHAR)
			printf("VALUE:%c\n",q->val.charval);
		else
			printf("\n");
		#endif
		free(q);
	}
	printf("\a");
	printf("prees any key to exit");getchar();exit(0);
}

static int cast2int(EXPVAL exp)
{
	if (exp.type==ID_INT)
		return(exp.val.intval);
	else if (exp.type==ID_CHAR )
		return((int)(exp.val.charval));
}

static char cast2char(EXPVAL exp)
{
		return((char)cast2int(exp));
}

static EXPVAL calculateExp(EXPVAL val1, int calop, EXPVAL val2){
    int result = cast2int(val1);
		int result1 = cast2int(val2);
    switch (calop)
		{
		case SYN_ADD: 
			  result += result1;
			  break;
		case SYN_SUB:
		    result -= result1;
				break;
		case SYN_MUL:
		    result *= result1;
				break;
		case SYN_DIV:
		    result /= result1;
				break;
		};

    EXPVAL val;
		if(val1.type==ID_CHAR && val2.type==ID_CHAR){
				val.type=ID_CHAR;
				val.val.charval=(char)result;
			}
		else
			{
				val.type=ID_INT;
				val.val.intval=result;
			}

		return val;
}

static int Prod_FUNC()
{
	IDTABLE *p;
	match(SYN_ID);
	if (strcompare(curtoken_str,"main")==ERR) FreeExit();
	p=InstallID();
    if(p)p->type=ID_FUN;
	#if defined(AnaTypeSyn)
	printf("SYN: FUNC-->main() {S}\n");
	#endif
	match(SYN_PAREN_L);
	match(SYN_PAREN_R);
	match(SYN_BRACE_L);
	Prod_S();
	match(SYN_BRACE_R);
	return(0);
}

static int Prod_S()
{
	long file_index;
	EXPVAL exp;
	int bval;
	if (lookahead.token==SYN_INT || lookahead.token==SYN_CHAR)
	{  // S --> D S ; D --> T id[=E] L;
		#if defined(AnaTypeSyn)
		printf("SYN: S-->D S\n");
		#endif
		Prod_D();
		Prod_S();
	}
	else if (lookahead.token==SYN_ID)
	{   // S --> A S ; A --> id=E;
		#if defined(AnaTypeSyn)
		printf("SYN: S-->A S\n");
		#endif
		Prod_A();
		Prod_S();
	}
	else if (lookahead.token==SYN_SHOW)
	{   // S-->show(E); S
		#if defined(AnaTypeSyn)
		printf("SYN: S-->show(E); S\n");
		#endif
 		match(SYN_SHOW);
		match(SYN_PAREN_L);
		exp=Prod_E();
		match(SYN_PAREN_R);
		match(SYN_SEMIC);
		if (run_status==1)
			if (exp.type==ID_INT )
				printf("%d",exp.val.intval);
			else if (exp.type==ID_CHAR )
				printf("%c",exp.val.charval);
		Prod_S();
	}
	else if (lookahead.token==SYN_IF)
	{   // S-->if (B) {S} [else {S}] S
		#if defined(AnaTypeSyn)
		printf("SYN: S-->if (B) {S} [else {S}] S\n");
		#endif
		int status_record = run_status;
		int isExeIf , isExeElse ;
		match(SYN_IF);
		match(SYN_PAREN_L);
		bval=Prod_B();
		match(SYN_PAREN_R);
	  if(run_status == 1 && bval == 1)isExeIf = 1;
		else isExeIf = -1;
		if(run_status == 1 && bval == 0)isExeElse = 1;
		else isExeElse = -1;
		if(isExeIf == 1)run_status=1;
		else run_status = -1;
    match(SYN_BRACE_L);
		Prod_S();
		match(SYN_BRACE_R);
		if(lookahead.token==SYN_ELSE)
		{
			if(run_status != 0 && run_status != 2){
		  	if(isExeElse == 1)run_status=1;
			  else run_status=-1;
			};
			match(SYN_ELSE);
			match(SYN_BRACE_L);
			Prod_S();
			match(SYN_BRACE_R);
		};

    if( run_status != 0 && run_status != 2)
		  run_status=status_record;
		Prod_S();
	}
	/*      WHILE         WHILE        WHILE             */
	else if (lookahead.token==SYN_WHILE)
	{   //SYN: S-->while(B) {S} S
		#if defined(AnaTypeSyn)
		printf("SYN: S-->while(B) {S} S\n");
		#endif
		/* record origin values */
		int status_record = run_status;
		int origin_row = row;
		int origin_col = col - 1;
		file_index=ftell(sFile) - 1;
		match(SYN_WHILE); //change loc with last statement
		match(SYN_PAREN_L);
		bval=Prod_B();
		match(SYN_PAREN_R);
		if (run_status==2)run_status=0; //elimilate amgiguous situation that status=2 cased by continue
		if (run_status==1 && bval==0)run_status=0;// should't be executed, same value,but meaning is different  from last statement
		match(SYN_BRACE_L);
	  Prod_S();
		match(SYN_BRACE_R);
		while(run_status == 1 || run_status == 2){
			run_status = 1;// bool expression need run_status equals to 1
			fseek(sFile,file_index,SEEK_SET);//seek_set: head of file
			renewLex();         //set prebuf equals to 0
			lookahead.token = 60;
			row = origin_row;
			col = origin_col;
      match(SYN_WHILE);
			match(SYN_PAREN_L);
			bval=Prod_B();
			match(SYN_PAREN_R);
			if (bval!=1)run_status=0;
			match(SYN_BRACE_L);
			Prod_S();
			match(SYN_BRACE_R);
			
		};//end of while
    run_status = status_record; // set status to its origin value
		Prod_S();
	}
	else if(lookahead.token==SYN_CONTINUE)
	{
		match(SYN_CONTINUE);
		match(SYN_SEMIC);
		if(run_status == 1)run_status=2;
		Prod_S();
	}
	else if(lookahead.token==SYN_BREAK)
	{
		match(SYN_BREAK);
		match(SYN_SEMIC);
		if(run_status == 1)run_status=0;
		Prod_S();
	}
	else
	{   // S --> empty
		#if defined(AnaTypeSyn)
		printf("SYN: S--> \n");
		#endif
	}
	return(0);
}

static int Prod_D()
{
//  D --> T id [=E] L | kong
	int type;
	IDTABLE *p;
	EXPVAL exp;
	#if defined(AnaTypeSyn)
	printf("SYN: D-->T id [=E] L;\n");
	#endif
	type=Prod_T();
	match(SYN_ID);
	p=InstallID();
	if(p == NULL){
		printf("Error  row: %d, col: %d:  '%s' has defined before",row,col,curtoken_str);
		FreeExit();
	}
	if (lookahead.token==SYN_SET)
	{
		match(SYN_SET);
		exp=Prod_E();
		if (run_status==1)
		{	if (type==ID_INT)
		  {
				p->val.intval=cast2int(exp);
			  p->type=ID_INT;
			}
			else 
			    if (type==ID_CHAR)
				   {
						 p->val.charval=cast2char(exp);
						 p->type=ID_CHAR;
					 }
		}
	}
	Prod_L(type);
	match(SYN_SEMIC);
	return(0);
}

static int Prod_L(int type)
{
//  L --> ,id[=E] L | kong
	IDTABLE *p;
	EXPVAL exp;
	if (lookahead.token==SYN_COMMA)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: L-->, id [=E] L\n");
		#endif
		match(SYN_COMMA);
		match(SYN_ID);
		p=InstallID();
		if(p == NULL){
		printf("Error  row: %d, col: %d:  '%s' has defined before",row,col,curtoken_str);
		FreeExit();
	}
		if (lookahead.token==SYN_SET)
		{
			match(SYN_SET);
			exp=Prod_E();
			if (run_status==1)
			{	if (type==ID_INT)
			  {
					p->val.intval=cast2int(exp);
				  p->type=ID_INT;
				}
				else if (type==ID_CHAR)
				    {
	    				p->val.charval=cast2char(exp);
							p->type=ID_CHAR;
						}
			}
		}
		Prod_L(type);
	}
	else
	{
		#if defined(AnaTypeSyn)
		printf("SYN: L--> \n");
		#endif
	}
	return(0);
}

static int Prod_T()
{
	if (lookahead.token==SYN_INT)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: T-->int\n");
		#endif
		match(SYN_INT);
		return(ID_INT);
	}
	else if (lookahead.token==SYN_CHAR)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: T-->char\n");
		#endif
		match(SYN_CHAR);
		return(ID_CHAR);
	}
	else
		FreeExit();
	return(0);
}

static int Prod_A()
{
//  A --> id = E
	IDTABLE *p;
	EXPVAL exp;
	#if defined(AnaTypeSyn)
	printf("SYN: A-->id=E;\n");
	#endif
	match(SYN_ID);
	p=LookupID();
	match(SYN_SET);
	exp=Prod_E();
	match(SYN_SEMIC);
	if (run_status==1)
	{	if (p->type==ID_INT)
			p->val.intval=cast2int(exp);
		else if (p->type==ID_CHAR)
			p->val.charval=cast2char(exp);
	}
	return(0);
}

static int Prod_B()
{
//  B --> TB B1
	int bval1,bval2;
	#if defined(AnaTypeSyn)
	printf("SYN: B-->id=TB B1\n");
	#endif
	bval1=Prod_TB();
	bval2=Prod_B1(bval1);
	return(bval2);
}

static int Prod_B1(int bval1)
{
//  B1 --> 锟斤拷||锟斤拷 TB B1 | 锟斤拷
	int bval2;
	if (lookahead.token==SYN_OR)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: B1-->|| B1\n");
		#endif
		match(SYN_OR);
		bval2=Prod_TB();
		bval1=(run_status==1 && (bval1==1 || bval2==1)) ? 1 : 0;
		bval2=Prod_B1(bval1);
		return(bval2);
	}
	else
	{
		#if defined(AnaTypeSyn)
		printf("SYN: B1--> \n");
		#endif
		return(bval1);
	}
}

static int Prod_TB()
{
//  TB --> && FB TB1 | kong
	int bval1,bval2;
	#if defined(AnaTypeSyn)
	printf("SYN: TB-->FB TB1\n");
	#endif
	bval1=Prod_FB();
	bval2=Prod_TB1(bval1);
	return(bval2);
}

static int Prod_TB1(int bval1)
{   
//  TB1 --> && FB TB1 | kong
	int bval2;
	if (lookahead.token==SYN_AND)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: TB1-->&& TB1\n");
		#endif
		match(SYN_AND);
		bval2=Prod_FB();
		bval1=(run_status==1 && (bval1==1 && bval2==1)) ? 1 : 0;
		bval2=Prod_TB1(bval1);
		return(bval2);
	}
	else
	{
		#if defined(AnaTypeSyn)
		printf("SYN: TB1--> \n");
		#endif
		return(bval1);
	}
}

static int Prod_FB()
{

	int bval;
	EXPVAL val1,val2;
	int ival1,ival2;
	if (lookahead.token==SYN_NOT)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: FB-->!B\n");
		#endif
		match(SYN_NOT);
		bval=Prod_B();
		return(run_status==1 ? 1-bval : 0);
	}
	else if (lookahead.token==SYN_TRUE)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: FB-->TRUE\n");
		#endif
		match(SYN_TRUE);
		return(run_status==1 ? 1 : 0);
	}
	else if (lookahead.token==SYN_FALSE)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: FB-->FALSE\n");
		#endif
		match(SYN_FALSE);
		return(run_status==1 ? 0 : 0);
	}
	else if (lookahead.token==SYN_ID || lookahead.token==SYN_NUM || lookahead.token==SYN_PAREN_L)
	{
		val1=Prod_E();
		if (run_status==1) ival1=cast2int(val1);
		if (lookahead.token==SYN_LT)
		{
			#if defined(AnaTypeSyn)
			printf("SYN: FB-->E<E\n");
			#endif
			match(SYN_LT);
			val2=Prod_E();
			if (run_status==1)
			{	ival2=cast2int(val2);
				return(ival1<ival2 ? 1 : 0);
			}
			else
				return(0);
		}
		else if (lookahead.token==SYN_LE)
		{
			#if defined(AnaTypeSyn)
			printf("SYN: FB-->E<=E\n");
			#endif
			match(SYN_LE);
			val2=Prod_E();
			if (run_status==1)
			{	ival2=cast2int(val2);
				return(ival1<=ival2 ? 1 : 0);
			}
			else
				return(0);
		}
		else if (lookahead.token==SYN_GT)
		{
			#if defined(AnaTypeSyn)
			printf("SYN: FB-->E>E\n");
			#endif
			match(SYN_GT);
			val2=Prod_E();
			if (run_status==1)
			{	ival2=cast2int(val2);
				return(ival1>ival2 ? 1 : 0);
			}
			else
				return(0);
		}
		else if (lookahead.token==SYN_GE)
		{
			#if defined(AnaTypeSyn)
			printf("SYN: FB-->E>=E\n");
			#endif
			match(SYN_GE);
			val2=Prod_E();
			if (run_status==1)
			{	ival2=cast2int(val2);
				return(ival1>=ival2 ? 1 : 0);
			}
			else
				return(0);
		}
		else if (lookahead.token==SYN_EQ)
		{
			#if defined(AnaTypeSyn)
			printf("SYN: FB-->E==E\n");
			#endif
			match(SYN_EQ);
			val2=Prod_E();
			if (run_status==1)
			{	ival2=cast2int(val2);
				return(ival1==ival2 ? 1 : 0);
			}
			else
				return(0);
		}
		else if (lookahead.token==SYN_NE)
		{
			#if defined(AnaTypeSyn)
			printf("SYN: FB-->E!=E\n");
			#endif
			match(SYN_NE);
			val2=Prod_E();
			if (run_status==1)
			{	ival2=cast2int(val2);
				return(ival1!=ival2 ? 1 : 0);
			}
			else
				return(0);
		}
		else
		{
			#if defined(AnaTypeSyn)
			printf("SYN: FB-->E\n");
			#endif
			if (run_status==1)
				return(ival1!=0 ? 1 : 0);
			else
				return(0);
		}

	}
	else
	{	FreeExit();
		return(0);
	}
}

static EXPVAL Prod_E()
{
	EXPVAL val1,val2;
	#if defined(AnaTypeSyn)
	printf("SYN: E-->TE E1\n");
	#endif
	val1=Prod_TE();
	val2=Prod_E1(val1);
	return(val2);
}

static EXPVAL Prod_E1(EXPVAL val1)
{
/* 锟斤拷锟斤拷F锟斤拷值锟斤拷锟斤拷锟斤拷锟斤拷应锟侥凤拷锟脚斤拷锟叫加硷拷锟斤拷锟斤拷
 * 同Prod_TE1
 */

	EXPVAL val2,val;
	int i1,i2;
	char c1,c2;
	if (lookahead.token==SYN_ADD)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: E1-->+TE E1\n");
		#endif
		match(SYN_ADD);
		val2=Prod_TE();
		if (run_status==1)
      val = calculateExp(val1, SYN_ADD, val2);
		val=Prod_E1(val);
	}
	else if (lookahead.token==SYN_SUB)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: E1-->-TE E1\n");
		#endif
		match(SYN_SUB);
		val2=Prod_TE();
		if (run_status==1)
		  val = calculateExp(val1, SYN_SUB, val2);
		val=Prod_E1(val);
	}
	else
	{
		#if defined(AnaTypeSyn)
		printf("SYN: E1--> \n");
		#endif
		val=val1;
	}
	return(val);
}

static EXPVAL Prod_TE()
{
	EXPVAL val1,val2;
	#if defined(AnaTypeSyn)
	printf("SYN: TE-->F TE1\n");
	#endif
	val1=Prod_F();
	val2=Prod_TE1(val1);
	return(val2);
}

static EXPVAL Prod_TE1(EXPVAL val1)
{
	EXPVAL val2,val;
	int i1,i2;
	char c1,c2;
	if (lookahead.token==SYN_MUL)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: TE1-->*F TE1\n");
		#endif
		match(SYN_MUL);
		val2=Prod_F();
		if (run_status==1)
			val=calculateExp(val1, SYN_MUL, val2);		
		val=Prod_TE1(val);
	}
	else if (lookahead.token==SYN_DIV)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: TE1-->/F TE1\n");
		#endif
		match(SYN_DIV);
		val2=Prod_F();
		if (run_status==1)
		  val = calculateExp(val1, SYN_DIV, val2);
		val=Prod_TE1(val);
	}
	else
	{
		#if defined(AnaTypeSyn)
		printf("SYN: TE1--> \n");
		#endif
		val=val1;
	}
	return(val);
}

static EXPVAL Prod_F()
{

	EXPVAL val;
	static IDTABLE *p;  
	if (lookahead.token==SYN_NUM)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: F-->num\n");
		#endif
		match(SYN_NUM);
		val.type=ID_INT;
		val.val.intval=curtoken_num;
	}
	else if (lookahead.token==SYN_ID)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: F-->id\n");
		#endif
		match(SYN_ID);
		p=LookupID();
		if(p==NULL){printf("Error row: %d ,col: %d:  '%s' haven't been defined", row,col,curtoken_str);FreeExit();};
		val.type=p->type; 
		val.val=p->val;
	}
	else if(lookahead.token==SYN_PAREN_L)
	{
			#if defined(AnaTypeSyn)
			printf("SYN: F-->(B)\n");
			#endif
			match(SYN_PAREN_L);
			//change int type to EXPVAL
			if(Prod_B() == 1){
				val.type = ID_INT;
				val.val.intval = 1;
			}
			else{
				val.type = ID_INT;
				val.val.intval = 0;
			}

			match(SYN_PAREN_R);
  }
	else if (lookahead.token==SYN_LETTER)
	{
		#if defined(AnaTypeSyn)
		printf("SYN: F-->letter\n");
		#endif
		match(SYN_LETTER);
		val.type=ID_CHAR;
		val.val.charval=(char)curtoken_num;
	}
	else{
		printf("Error  row: %d, col: %d:  expect a expression before %s \n", row, col,printCharList[lookahead.token]);
		FreeExit();
	};
	return(val);
}
