

#define AnaTypeLex	1
//#define AnaTypeSyn	1
#define MAXTOKENLEN	256

typedef union {
	int number;
	char *str;
} TOKENVAL;

typedef struct {
	int token;
	TOKENVAL tokenVal;
} TERMINAL;

typedef union {
	int intval;
	char charval;
} EXPVALUE;

typedef struct expValue{
	int type;
	EXPVALUE val;
} EXPVAL;

typedef struct idNode{
	char name[MAXTOKENLEN];
	int type;
	EXPVALUE val;
	struct idNode *next;
} IDTABLE;

//词法分析DFA转换表
static int LexTable[6][8]=
//  10* 状态表示接受状态，但多读了一个符号，需要缓存
//  20* 状态表示接受状态，但没多读符号
 /*  relop  delim    *      /      +|_   digit letter_ symbol*/
   {{   1,   201,   204,     2,     3,     4,     5,   205},  //0
	{ 101,   101,   101,   101,   101,   101,   101,   101},  //1
	{ 102,   102,   202,   203,   102,   102,   102,   102},  //2
	{ 103,   103,   103,   103,   103,     4,   103,   103},  //3
	{ 104,   104,   104,   104,   104,     4,   104,   104},  //4
	{ 105,   105,   105,   105,   105,     5,     5,   105}}; //5


//用于词法分析输出，及语法分析
#define ERR			-1
#define SYN_NUM		1		// int整数
#define SYN_ID		2		// id
#define SYN_LT		11		// <
#define SYN_GT		12		// >
#define SYN_LE		13		// <=
#define SYN_GE		14		// >=
#define SYN_EQ		15		// ==
#define SYN_NE		16		// !=
#define SYN_ADD		17		// +
#define SYN_SUB		18		// -
#define SYN_MUL		19		// *
#define SYN_DIV		20		// /
#define SYN_PAREN_L	21		// (
#define SYN_PAREN_R	22		// )
#define SYN_BRACE_L	23		// {
#define SYN_BRACE_R	24		// }
#define SYN_COMMA	25		// ,
#define SYN_SEMIC	26		// ;
#define SYN_SET		27		// =
#define SYN_SQUARE_BRACKETS_L 28      // [
#define SYN_SQUARE_BRACKETS_R 29      // ]
#define SYN_AND		51		// &&
#define SYN_OR		52		// ||
#define SYN_NOT		53		// !
#define SYN_TRUE	54		// TRUE
#define SYN_FALSE	55		// FALSE
#define SYN_INT		56		// int
#define SYN_CHAR	57		// char
#define SYN_IF		58		// if
#define SYN_ELSE	59		// else
#define SYN_WHILE	60		// while
#define SYN_SHOW	61		// show
/*
static char printCharList[62][10];
strcpy(printCharList[1], " int");
strcpy(printCharList[2]," id");
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
*/
//用于符号表中类型
#define ID_FUN		1		// 函数类型
#define ID_INT		2		// int类型
#define ID_CHAR		3		// char类型
