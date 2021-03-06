#include "stdio.h"
#include "constvar.h"
#include<stdlib.h>

#define LEX_RELOOP	0
#define LEX_DELIM	1
#define LEX_MUL		2
#define LEX_DIV		3
#define LEX_ADDMIN	4
#define LEX_DIGIT	5
#define LEX_LETTER_	6
#define LEX_SYMBOL	7


static char ReadAChar();
static int FoundRELOOP();
static int STR2INT();
static int FoundKeyword();
static int strcompare(char *sstr, char*tstr);

extern FILE *sFile;
static char prebuf=0;	//buffer to store the pre-read character
static char tokenStr[MAXTOKENLEN];	//token buffer
static int tokenLen;
int row = 1, col = 0;
MAP escapeList[10];  //用于转义字符查询
int LEN_ESCAPE = 0;


void initEscapeList(){
  escapeList[0].key='a';
  escapeList[0].value='\a';
  escapeList[1].key='b';
  escapeList[1].value='\b';
  escapeList[2].key='f';
  escapeList[2].value='\f';
  escapeList[3].key='n';
  escapeList[3].value='\n';
  escapeList[4].key='r';
  escapeList[4].value='\r';
  escapeList[5].key='t';
  escapeList[5].value='\t';
  escapeList[6].key='v';
  escapeList[6].value='\v';
  LEN_ESCAPE = 6 + 1;
}


TERMINAL nextToken()
{
	TERMINAL token;
	int state=0;
	char c,d;
	token.token=ERR;
	token.tokenVal.number=0;
	tokenLen=0;
	for (c=ReadAChar(sFile);c!=0;c=ReadAChar(sFile))
	{	tokenStr[tokenLen]=c;    // 记录读入符号的数组，最大由MAXTOKENLEN决定，为256
	    tokenLen++;
		if (tokenLen>=MAXTOKENLEN)
		{	printf("Token is too long! it limitted to %d \n", MAXTOKENLEN);
			break;
		}
		if (feof(sFile))
			state=LexTable[state][LEX_DELIM];  // 到达文件末尾
		else if (c=='<' || c=='>' || c=='=' || c=='!' || c=='&' || c=='|')
			state=LexTable[state][LEX_RELOOP];
		else if (c==' ' || c=='\t' || c=='\n')
			state=LexTable[state][LEX_DELIM];
		else if (c=='*')
			state=LexTable[state][LEX_MUL];
		else if (c=='/')
			state=LexTable[state][LEX_DIV];
		else if (c=='+' || c=='-')
			state=LexTable[state][LEX_ADDMIN];
		else if (c>='0' && c<='9')
			state=LexTable[state][LEX_DIGIT];
		else if ((c>='a' && c<='z')||(c>='A' && c<='Z')||(c=='_'))
			state=LexTable[state][LEX_LETTER_];
		else if (c=='(' || c==')' || c=='{' || c=='}' || c==',' || c==';' || c=='\'')
			state=LexTable[state][LEX_SYMBOL];
		else
		{	printf("Unknown symbol: %c\n",c);
			break;
		}
//  10* 状态表示接受状态，但多读了一个符号
//  20* 状态表示接受状态，但没多读符号
		if (state<100) continue;
		if (state>100 && state<200)
		{	prebuf=c;      // 此时为多读的字符，存放在prebuf中
			tokenLen--;    // id字符串中不算多读的字符
		}
		switch (state)     // 词法分析的输出
		{	case 101: token.token=FoundRELOOP();
					  break;
			case 102: token.token=SYN_DIV;
					  break;
			case 103: if (tokenStr[0]=='+') token.token=SYN_ADD;
					  else token.token=SYN_SUB;
					  break;
			case 104: token.token=SYN_NUM;
					  token.tokenVal.number=STR2INT();
					  break;
			case 105: tokenStr[tokenLen]='\0';
					  token.token=FoundKeyword();
					  token.tokenVal.str=tokenStr;
					  break;
			case 201: if (feof(sFile))
					  {//	  printf("Meet file end!\n");
						  token.token=-1;
						  break;
					  }
					  state=0; tokenLen=0;
					  continue;
			case 202: /*c=ReadAChar(sFile);
					  while (!feof(sFile) && !( c=='*' && (d=ReadAChar(sFile))=='/' ) )
						  c=d;
					  */
					  c=ReadAChar(sFile);
					  d=ReadAChar(sFile);
					  while(!feof(sFile) && !(( c=='*') && (d=='/'))){
                          c=d;
						  d=ReadAChar();
					  };
					  state=0; tokenLen=0;
					  continue;
			case 203: while ((c=ReadAChar(sFile))!='\n' && (!feof(sFile)));
					  state=0; tokenLen=0;
					  continue;
			case 204: token.token=SYN_MUL;
					  break;
			case 205: if (tokenStr[0]=='(') token.token=SYN_PAREN_L;
					  else if (tokenStr[0]==')') token.token=SYN_PAREN_R;
					  else if (tokenStr[0]=='{') token.token=SYN_BRACE_L;
					  else if (tokenStr[0]=='}') token.token=SYN_BRACE_R;
					  else if (tokenStr[0]==',') token.token=SYN_COMMA;
					  else if (tokenStr[0]==';') token.token=SYN_SEMIC;
					  else if (tokenStr[0]=='\'')
					       {    int temp;
						        token.token=SYN_LETTER;
								temp = (int)ReadAChar(sFile);
								/* 处理转义符号例如\n */
								if(temp != '\\')
								    token.tokenVal.number = temp;
								else{
                                    temp = (int)ReadAChar(sFile);
									int i;
									for(i = 0; i < LEN_ESCAPE; i++){
										if(escapeList[i].key==temp){
											token.tokenVal.number = escapeList[i].value;
											break;
										}
									}
									if(i == LEN_ESCAPE){//没有这种转义
									    token.tokenVal.number = temp;
										printf("sorry, we are not support escape of '%c', we will treat this as a single normal letter\n",temp);
									};
								}
								/* 字符表达形式错误 */
								if(ReadAChar(sFile) != '\''){
									printf("Error: lost a ' at row: %d, col: %d\n",row,col);
									exit(0);
								}
								
						   }
					  break;
			default: break;
		}//end of switch
		break;
	}
	return(token);
}

void renewLex()
{
	prebuf=0;
}

static char ReadAChar(FILE *sFile)
{
	char c;
	if (prebuf!=0)
	{
		c=prebuf;
		prebuf=0;
	}
	else if (!feof(sFile))
		c=fgetc(sFile);
	else
		c=0;
	if(c == '\n'){//便于输出错误的位置
		row++;
		col = 0;
	}
	col++;
	return(c);
}

static int FoundRELOOP()
{
	if (tokenStr[0]=='<' && tokenStr[1]!='=') return(SYN_LT);
	else if (tokenStr[0]=='<' && tokenStr[1]=='=') { prebuf=0; return(SYN_LE); }
	else if (tokenStr[0]=='>' && tokenStr[1]!='=') { return(SYN_GT);}
	else if (tokenStr[0]=='>' && tokenStr[1]=='=') { prebuf=0; return(SYN_GE); }
	else if (tokenStr[0]=='=' && tokenStr[1]!='=') {return(SYN_SET);}
	else if (tokenStr[0]=='=' && tokenStr[1]=='=') {prebuf=0;return(SYN_EQ);}
	else if (tokenStr[0]=='!' && tokenStr[1]!='=') {return(SYN_NOT);}
	else if (tokenStr[0]=='!' && tokenStr[1]=='=') {prebuf=0;return(SYN_NE);}
	else if (tokenStr[0]=='&' && tokenStr[1]=='&') {prebuf=0;return(SYN_AND);}
	else if (tokenStr[0]=='|' && tokenStr[1]=='|') {prebuf=0;return(SYN_OR);}
	else return(ERR);
}

static int STR2INT()
{
	int i=0,s=0,PlusOrMinus=1;
	if(tokenStr[0]=='+')
        i++;            //start calculate from the second char   
	if(tokenStr[0]=='-'){
		i++;
		PlusOrMinus = -1;
	};
	for (;i<tokenLen;i++)
		s=s*10+tokenStr[i]-'0';
	return(s * PlusOrMinus);
}

static int FoundKeyword()
{
	if (strcompare(tokenStr,"TRUE")) return(SYN_TRUE);
	if (strcompare(tokenStr,"FALSE")) return(SYN_FALSE);
	if (strcompare(tokenStr,"int")) return(SYN_INT);
	if (strcompare(tokenStr,"char")) return(SYN_CHAR);
	if (strcompare(tokenStr,"if")) return(SYN_IF);
	if (strcompare(tokenStr,"else")) return(SYN_ELSE);
	if (strcompare(tokenStr,"while")) return(SYN_WHILE);
	if (strcompare(tokenStr,"show")) return(SYN_SHOW);
	if (strcompare(tokenStr,"continue")) return(SYN_CONTINUE);
	if (strcompare(tokenStr,"break")) return(SYN_BREAK);
	return(SYN_ID);
}

static int strcompare(char *sstr, char*tstr)
{
	while (*sstr==*tstr && *sstr!='\0') { sstr++; tstr++; }
	if (*sstr=='\0' && *tstr=='\0')	return(1);
	else return(0);
}
