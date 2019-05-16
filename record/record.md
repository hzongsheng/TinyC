[TOC]

# TinyC代码阅读笔记
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
      * 结构：
             B
      |    |    |     |
      TB || TB || TB  ||  B1...
    |    |    |
    F && F && F 空...
      
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
   * Prod_FUNC
```flow
​```flow
st=>start: Prod_FUNC( )
end=>end: return
matain=>operation: maintain:
                   符号表IDTHead
                   curtoken_num
                   curtoken_str[ ]
installid=>operation: InstallID( )
                      建立当前符号在符号表中的项
installid
st->matain->installid->end
```
## Explanation of the Data-Structer
   > TERMINAL:终结符
   * token: 为每一个符号都指定的int值，定义在constvar.h 中
   * tokenvalue: 存放具体的数字值或者字符值

   > EXPVAL: 表达式值
   * int type: 记录表达式值的类型，int，char
   * EXPVALUE val: 记录相应的值

## Explanation of Static Values
   > run_status: 
   * 0 程序不执行
   * 1 程序正常执行
   * 2 跳过当前结构继续执行
   * 初始化为1
   * 作用：通过已知的status结合当前的条件决定下一步要执行的动作为继续执行,还是不执行,或者是跳过本单元继续执行，实现控制流语句。

   > curtoken_num curtoken_str[]:
   * 表示当前分析的token的int型值或者char值
   * 在每次移动token，即执行match函数时，更新此两个值

## Usage of Variable Value in Functions
   > Prod_D:
   * type: 
      记录产生式 D --> T id [=E] L中T的类型(char, int)
   * exp:
      记录赋值的值,int型的，char型的

   > match:  
   * 用途:  匹配一个符号，当此符号为id或者num时将当前的token赋值为此符号,用于建立符号表表项
   然后lookahead向前移动为下一个token
##　Functions
   > Prod_F
   * 无需参数
   * 返回EXPVAL型的值

   > Prod_TE1:执行乘除运算
   * int或者char型被乘数的值， 或者被除数的值
   * 返回 乘除运算后的值或者原值
   * 当其中至少一个为int时，将返回两个值的int型值相乘，相除的结果
   * 当两个均为char时，返回ascaii码相乘，相除结果

   > Prod_E1:执行加减运算
   * int或者char型被加数的值，或者被减数的值
   * 返回 运算后的值或者原值
   * 功能同Prod_TE1
   * ......

   > Prod_TB1: 执行 &&　与运算
   * 
   > Prod_B1: 

## problems record

   > 由两个符号组成的关系运算符解释不正确:输出整个关系运算符后，又将后一个运算符输出
   * 解决方法: 简单地将FindRELoop中将所有的prebuf赋值为0。即判断为两个符号组成的关系运算符后, 下一次开始读取分析的符号位置为prebuf的后一个字符。而当判断为单个字符所组成的运算符后，下一个分析的位置的字符应该为prebuf所记录的字符，不应该调用ReadAChar中的fgetc()函数，所以置prebuf为0

   > 单个字符构成的运算符后丢失一个符号: 例如!FALSE 分析后变为符号! 和 id ALSE
   * 将FindRELoop中判断SYN_NOT的语句的prebuf赋值语句删去。错误原因为未能注意到单字符组成的运算符，解决方法的原理在前一条已经解释过。
  
   > 输出表达式a=a+1出错
   * 此法分析读取完表达式右边的a后将"+"和"1"视为"+1"，将加号视作正号，导致归约出错。同时将数字视作有正负却没在STR2INT函数中体现
   * 修改词法分析器，将STR2INT函数修改为可以处理负数的函数。从而使得TinyC支持负数，但负数符号必须紧挨着数字，与数字隔开将把符号"+"视作加号

   > Prod_D()出现segmentFault错误
   * 由于当当前token已经存在于符号表中的时候，调用函数InstallID()返回NULL，所以需要加判断,为NULL时，返回不能重复定义的消息

   > 增加对char的支持
   >> 词法部分
   * 在sysmbol中增加符号类型英文单引号" ' "，在nextToken函数中symbol判断中增加此类型
   * 增加判断出 ' 后处于状态205的处理，读入下一个字符作为具体值，当作int存入token.tokenVal.number中，再读如下一个 ' ,如果不是' 抛出错误。否则输出letter类型符号结果，

   >> 语法部分
   * 修改Prod_F函数中对char数据的处理
   * 赋值语句中对char型值的处理(其实没有要处理的)
   * 增加对char型变量的算术运算(无需改动)
   * 增加对char型变量的逻辑运算(无需改动)
   * 增加show语句对char的支持(无需改动)
   * 将运算的逻辑单元独立为calculateExp()函数，重构代码
   * 增加对转义符的支持：当识别出单引号符号时，再读入一个符号，如果为"\"，则再读入一个符号，查表得出相应的转义符值，作为读入的符号值。
   
   >> show语句未能打印出变量值，表达式的type为一个随机数 
   * Prod_D 和 Prod_L 对建立ID表项后未添加type

   > 逻辑表达式参与算术计算
   * 修改文法，将F的产生式由F-->id|num|letter|(E) 改为 F-->id|num|letter|(B)
   * 将Prod_F中 B 的返回值存入EXPVAL型变量val的intval中，产生式返回val
   * 
  
   > 支持continue，break
   >> if语句
   * 修改constvar.h ，增加SYN_CONTINUE, SYN_BREAK
   * 修改词法分析，关键字判断
   * if语句需要达到的目标是两个:
      1. 判断并决定是否执行if，else块
      2. 使得continue，break语句产生的对run_status的影响可以传递，而其他的status改变最终需要回到入口时的状态
   * 方法：
      1. 在程序入口判断出if，else是否需要执行，将结果存入isExeIf，isExeElse中，在执行Prod_S函数前，置相应status为0，或者-1。以决定其中的语句是否执行。
      2. 为了避免和break语句产生的对status影响相冲突，判断为不执行时选择的status值为-1，而不是0。最后为了回到入口状态的status，选择在入口记录status的值，以用于在程序出口除由continue和break引起的status状态的改变的恢复。
   >> while语句
   * while语句需要实现的功能为:
      1. 结果成立继续执行，否则，退出
      2. break语句执行后，其后的程序内语句均不执行，并结束循环
      3. continue执行后，其后的程序内语句均不执行，但需要回到条件判断表达式是否成立，是则继续，否则退出
   * 入口时程序执行的条件为status为1且bval为1，否则均不执行，为了避免与continue语句产生影响混淆，将入口status为2时赋值为0，status为1但bval为0时赋值为0，表示不执行
   * 结构上为先执行一次while循环体的翻译执行，在判断是否继续重复执行，将重复执行单元放在一个循环中，进入循环条件为status为1或者2，由于入口便为2的情况已经被重新置为0，所以此循环体中status为2的原因只可能为由continue所造成的。重新判断bval，为0便置status为0，结束循环
      
   > 此法分析中case202 while语句死循环，怀疑为编译器问题，改变表达后问题解决