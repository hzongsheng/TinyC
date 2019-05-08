[TOC]

# 代码阅读笔记
***
## Structer of program:
   * SyntaxAnalysis() --> nextToken()

## Explanation of the Data-Structer:
   > TERMINAL:终结符
   * token: 为每一个符号都指定的int值，定义在constvar.h 中
   * tokenvalue: 存放具体的数字值或者字符值
   > nexttoken:   

## problems record:
   0. > sysmbol 缺少符号'\[', '\]' :遇到该符号报错
      * 修改LexicalAnalysis.c 方法nexttoken 中判断 symbol的表达式
      * 增加。。。。。。。。。。。。。。。。。。。中case205的判断
      * 增加constvar.h  中对'[',']'常量的定义
   
   1. > 由两个符号组成的关系运算符解释不正确:输出整个关系运算符后，又将后一个运算符输出
      * 将FindRELoop中将所有的prebuf赋值为0,