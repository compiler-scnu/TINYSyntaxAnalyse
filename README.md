# TINYSyntaxAnalyse
## TINY语言扩充语法分析

扩充的语法规则有：实现 while、do while、for、if语句、+= 加法赋值运算符号（类似于C语言的+=）、求余%、乘方^、<=(小于等于) <>(不等于)运算符号，具体文法规则自行构造。

(1) While-stmt --> while(exp)  stmt-sequence  endwhile

(2) Dowhile-stmt-->do  stmt-sequence  while(exp); 

(3) for-stmt-->for identifier:=simple-exp  to  simple-exp  do  stmt-sequence enddo    步长递增1

(4) for-stmt-->for identifier:=simple-exp  downto  simple-exp  do  stmt-sequence enddo    步长递减1

(5) += 加法赋值运算符号、求余%、乘方^、<=(小于等于)、<>(不等于)运算符号的文法规则请自行组织。

(6) 把TINY语言原有的if语句书写格式

```c
if_stmt-->if exp then stmt-sequence end  |  | if exp then stmt-sequence else stmt-sequence end 
```

改写为：

```c
if_stmt-->if(exp) stmt-sequence else stmt-sequence | if(exp) stmt-sequence
```
