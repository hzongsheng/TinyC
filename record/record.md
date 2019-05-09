[TOC]

# 代码阅读笔记
***
## Explaination of Syntax Expressions
   * **函数由main(){S}基本结构组成**
   * **语句 S，包含:**
      * **声明语句 D(即文法分析中用非终结符D表示)** 
         - int var;
      * **赋值语句 A** 
         - var=1;
      * **打印语句 show(算术表达式 E)** 
         -  show(var);
      * **条件判断语句 if(布尔表达式语句 B){...}else{...}, 其中else部分可选 例如**
         - if(var1 >= var2){show(var1);}else{show(var2);}
         - if(TRUE){var1 = var2;}
      * **while循环语句 while(布尔表达式语句 B){S}** 
         - while(FALSE){show(var1 + var2);}
   * **算术表达式 E ,可表达为**:
      * 基本元素 F 
         - var1
      * 由基本元素 F 通过加 + ,减 - ,乘 * ,除 /连接构成
         - var1 + var2 * var3 / var4;
      * 由基本元素或者其通过运算符相连的表达式外加括号构成, 例如
         - (var1)  
         - var1 * (var2 + var3)
   * **布尔表达式 B**
      * TRUE, FALSE直接表示 F_B
         - if(TRUE){...}; 
      * 两个算术表达式 E_1,E_2通过双目逻辑符号>, <, ==, >=, <=, !=,或者布尔表达式 B 通过单目运算符 ! 形成!B所形成的表达式 F_B,
         - var1 != var2;
         - !var1
      * 布尔表达式F_B 通过布尔运算符&&, ||,连接形成的表达式
         - var1 != var2 || !BExp && TRUE
   * **基本元素 F**
      * 数字,num 由且仅由0~9组成
         - 0
         - 66
      * 大小写英文字母, Letter_ 下划线构成的字
         - sample_name
         - Sample_Name
         - _Name


## Structer of program
   * 词法分析
```flow
​```flow
st=>start: main
e=>end: return
SyntaxAnalysis=>operation: SyntaxAnalysis()
nextToken=>operation: nextToken()

st->nextToken->SyntaxAnalysis->e
```
   * 语法分析
```flow
​```flow
st=>start: main
e=>end: return
SyntaxAnalysis=>operation: SyntaxAnalysis()
nextToken=>operation: nextToken()
Prod_FUNC=>condition: Prod_FUNC()==ERR
FreeExit=>operation: FreeExit()

st->SyntaxAnalysis->nextToken->Prod_FUNC
Prod_FUNC(yes)->FreeExit->e
Prod_FUNC(no)->e
```
## Explanation of the Data-Structer
   > TERMINAL:终结符
   * token: 为每一个符号都指定的int值，定义在constvar.h 中
   * tokenvalue: 存放具体的数字值或者字符值
   > nexttoken:   

## problems record

   > 由两个符号组成的关系运算符解释不正确:输出整个关系运算符后，又将后一个运算符输出
   * 解决方法: 简单地将FindRELoop中将所有的prebuf赋值为0。即判断为两个符号组成的关系运算符后, 下一次开始读取分析的符号位置为prebuf的后一个字符。而当判断为单个字符所组成的运算符后，下一个分析的位置的字符应该为prebuf所记录的字符，不应该调用ReadAChar中的fgetc()函数，所以置prebuf为0
   > 单个字符构成的运算符后丢失一个符号: 例如!FALSE 分析后变为符号! 和 id ALSE
   * 将FindRELoop中判断SYN_NOT的语句的prebuf赋值语句删去。错误原因为未能注意到单字符组成的运算符，解决方法的原理在前一条已经解释过。
