#include<iostream>
#include<fstream>
#include<cstring>
#include<stdlib.h>
using namespace std;

struct Ans
{
    string sym;
    string strToken;
    int line;
}ans[1024],temp;

enum object{constant=1,variable=2,procedure=3};

struct
{
    string name;
    enum object kind;
    int value;
    int address;
    int level;
    int size1;
    int size2;
    int size;
}table[1024];

struct 
{
    string F;
    int L;
    int A;
}midcode[1024],I;


int sub=0,n=0;
int level=0,address=0,dx=3;
int tnum=0,mnum=0;
int stack[1024];

void prog();
void block();
void condecl();
void _const();
void vardecl();
void proc();
void body();
void statement();
void lexp();
void exp();
void term();
void factor();
void enter_table(int,string,enum object,int,int,int,int);
void gen(int,string,int,int);
int find_table(int);
int get_sl(int,int);
void interpreter();

string Concat(string &strToken,char ch)
{
    strToken+=ch;
    return strToken;
}

bool IsLetter(char ch)
{
    if((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z'))
        return true;
    else
        return false;
}

bool IsDigit(char ch)
{
    if(ch>='0'&&ch<='9')
        return true;
    else
        return false;
}

int IsReserve(string *reserved_word,string &strToken)
{
    int count;
    for(count=0;count<15;count++)
        if(reserved_word[count]==strToken)
            return 1;
    return 0;
}

void enter_table(int i,string name,enum object kind,int level=0,int address=0,int value=0,int size1=0,int size2=0)
{
    table[i].name=name;
    table[i].kind=kind;
    table[i].value=value;
    table[i].level=level;
    table[i].address=address;
    table[i].size1=size1;
    table[i].size2=size2;
    table[i].size=table[i].size1+table[i].size2;
}

void gen(int i,string f,int l,int a)
{
    midcode[i].F=f;
    midcode[i].L=l;
    midcode[i].A=a;
}

int find_table(string name,int lev)
{
    int index;
    for(index=0;index<tnum;index++)
        if(table[index].name==name&&table[index].level==lev)
            return index;
    for(index=0;index<tnum;index++)
        if(table[index].name==name)
            return index;
    return -1;
}

int find1(string name,int lev)
{
    int index;
    for(index=0;index<tnum;index++)
        if(table[index].name==name&&table[index].level==lev)
            return index;
    return -1;
}

void prog()
{
    temp=ans[sub];
    if(temp.strToken!="program")
    {
        sub++;
        temp=ans[sub];
        if(temp.strToken!=";")
        {
            cout<<"第"<<temp.line<<"行program拼写错误"<<endl;
        }
        else
        {
            cout<<"第"<<temp.line<<"行program缺失"<<endl;
            sub--;
        }
    }
    else
    {
        sub++;
    }
    temp=ans[sub];
    if(temp.sym!="id")
    {
        cout<<"第"<<temp.line<<"行缺少标识符或标识符错误"<<endl;
    }
    else
    {
        sub++;
    }
    temp=ans[sub];
    if(temp.strToken!=";")
    {
        cout<<"第"<<temp.line-1<<"行缺少;"<<endl;
    }
    else
    {
        sub++;
    }
    block();
}

void block()
{
    dx=3;
    int cx1=mnum;
    int curnum=0;
    gen(mnum,"JMP",0,0);
    mnum++;
    temp=ans[sub];
    if(temp.sym=="id")
    {
        cout<<"第"<<temp.line<<"行缺少const/var"<<endl;
    }
    if(temp.strToken=="const")
    {
        sub++;
        _const();
        curnum++;
        while(temp.strToken==",")
        {
            sub++;
            _const();
            curnum++;
            temp=ans[sub];
        }
        if(temp.strToken==";")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            temp=ans[sub-1];
            cout<<"第"<<temp.line<<"行缺少;"<<endl;
        }
        temp=ans[sub];
    }
    if(temp.strToken=="var")
    {
        sub++;
        temp=ans[sub];
        if(temp.sym=="id")
        {
            if(find1(temp.strToken,level)==-1)
            {
                enter_table(tnum,temp.strToken,variable,level,dx);
                tnum++;
                dx++;
                curnum++;
            }
            else
                cout<<"第"<<temp.line<<"行变量重复定义"<<endl;
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少标识符或标识符错误"<<endl;
        }
        while(temp.sym=="id")
        {
            cout<<"第"<<temp.line<<"行缺少,"<<endl;
            enter_table(tnum,temp.strToken,variable,level,dx);
            tnum++;
            dx++;
            sub++;
            temp=ans[sub];
        }
        while(temp.strToken==",")
        {
            sub++;
            temp=ans[sub];
            if(temp.sym=="id")
            {
                if(find1(temp.strToken,level)==-1)
                {
                    enter_table(tnum,temp.strToken,variable,level,dx);
                    tnum++;
                    dx++;
                    curnum++;
                }
                else
                    cout<<"第"<<temp.line<<"行变量重复定义"<<endl;
                sub++;
                temp=ans[sub];
                while(temp.sym=="id")
                {
                    cout<<"第"<<temp.line<<"行缺少,"<<endl;
                    enter_table(tnum,temp.strToken,variable,level,dx);
                    tnum++;
                    dx++;
                    sub++;
                    temp=ans[sub];
                }
            }
            else
            {
                cout<<"第"<<temp.line<<"行缺少标识符或标识符错误"<<endl;
                if(ans[sub+1].strToken==",")
                {
                    sub++;
                    temp=ans[sub];
                }
            }
        }
        if(temp.strToken==";")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<temp.line-1<<"行缺少;"<<endl;
        }
    }
    while(temp.strToken=="procedure")
    {
        Ans t;
        int size=0;
        sub++;
        temp=ans[sub];
        if(temp.sym!="id")
        {
            if(temp.strToken=="(")
            {
                cout<<"第"<<temp.line<<"行缺少标识符"<<endl;
            }
            else
            {
                cout<<"第"<<temp.line<<"行标识符拼写错误"<<endl;
            }
        }
        else
        {
            t=temp;
            sub++;
            temp=ans[sub];
        }
        if(temp.strToken=="(")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少("<<endl;
        }
        if(temp.sym=="id")
        {         
            if(find1(temp.strToken,level)==-1)
            {
                enter_table(tnum,temp.strToken,variable,level,dx);
                tnum++;
                dx++;
                size++;
            }
            else
                cout<<"第"<<temp.line<<"行变量重复定义"<<endl;
            sub++;
            temp=ans[sub];
            while(temp.sym=="id")
            {
                cout<<"第"<<temp.line<<"行缺少,"<<endl;
                enter_table(tnum,temp.strToken,variable,level,dx);
                tnum++;
                dx++;
                sub++;
                temp=ans[sub];
            }
            while(temp.strToken==",")
            {
                sub++;
                temp=ans[sub];
                if(temp.sym=="id")
                {
                    if(find1(temp.strToken,level)==-1)
                    {
                        enter_table(tnum,temp.strToken,variable,level,dx);
                        tnum++;
                        dx++;
                        size++;
                    }
                    else
                        cout<<"第"<<temp.line<<"行变量重复定义"<<endl;
                    sub++;
                    temp=ans[sub];
                    while(temp.sym=="id")
                    {
                        cout<<"第"<<temp.line<<"行缺少,"<<endl;
                        enter_table(tnum,temp.strToken,variable,level,dx);
                        tnum++;
                        dx++;
                        sub++;
                        temp=ans[sub];
                    }
                }
                else
                {
                    cout<<"第"<<temp.line<<"行缺少标识符或标识符错误"<<endl;
                    if(ans[sub+1].strToken==",")
                    {
                        sub++;
                        temp=ans[sub];
                    }
                }
            }
        }
        if(temp.strToken==")")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少)"<<endl;
        }
        if(temp.strToken==";")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<temp.line-1<<"行缺少;"<<endl;
        }
        if(find1(t.strToken,level)==-1)
        {
            enter_table(tnum,t.strToken,procedure,level,mnum,0,size,curnum);
            tnum++;
        }
        else
            cout<<"第"<<temp.line<<"行过程重复定义"<<endl;
        level++;
        int cur_dx=dx;
        block();
        level--;
        dx=cur_dx;
        temp=ans[sub];
        if(temp.strToken==";")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            break;
        }
    }
    midcode[cx1].A=mnum;
    gen(mnum,"INT",0,dx);
    mnum++;
    body();
    gen(mnum,"OPR",0,0);
    mnum++;
}

void _const()
{
    temp=ans[sub];
    if(temp.sym!="id")
    {
        sub++;
        temp=ans[sub];
        if(temp.sym=="integer")
        {
            cout<<"第"<<temp.line<<"行标识符拼写错误"<<endl;
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少标识符"<<endl;
            sub--;
        }
    }
    else
    {
        sub++;
    }
    temp=ans[sub];
    if(temp.sym!="fuzhi"&&temp.sym!="integer")
    {
        cout<<"第"<<temp.line<<"行赋值号错误"<<endl;
        sub++;
    }
    else if(temp.sym!="fuzhi"&&temp.sym=="integer")
    {
        cout<<"第"<<temp.line<<"行缺少赋值号"<<endl;
    }
    else
    {
        sub++;
    }
    temp=ans[sub];
    if(temp.sym!="integer")
    {
        cout<<"第"<<temp.line<<"行未赋值"<<endl;
    }
    else
    {
        if(find1(ans[sub-2].strToken,level)==-1)
        {
            enter_table(tnum,ans[sub-2].strToken,constant,level,0,stoi(temp.strToken));
            tnum++;
        }
        else
            cout<<"第"<<temp.line<<"行常量重复定义"<<endl;
        sub++;
    }
    temp=ans[sub];
}

void body()
{
    temp=ans[sub];
    if(temp.strToken!="begin")
    {
        cout<<"第"<<ans[sub-1].line<<"行begin/const/var/procedure/if/while/call/read/write缺失或拼写错误或其他错误"<<endl;
    }
    else
    {
        sub++;
        statement();
        while(temp.strToken==";")
        {
            sub++;
            statement();
        }
        while(temp.strToken==";"||temp.sym=="id"||temp.strToken=="if"||temp.strToken=="while"||temp.strToken=="call"||temp.strToken=="read"||temp.strToken=="write"||temp.strToken=="begin")
        {
            if(temp.strToken==";"||temp.strToken=="begin")
            {
                sub++;
                statement();
            }
            else
            {
                cout<<"第"<<temp.line-1<<"行缺少;"<<endl;
                statement();
            }
        }
        if(temp.strToken=="end")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<ans[sub-1].line<<"行end与begin不匹配或其他错误"<<endl;
        }
    }
}

void statement()
{
    temp=ans[sub];
    if(temp.sym=="id")
    {
        int index=find_table(temp.strToken,level);
        if(index==-1)
            cout<<"变量"<<temp.strToken<<"未定义"<<endl;
        else if(table[index].kind!=variable)
            cout<<temp.strToken<<"不是一个变量"<<endl;
        sub++;
        temp=ans[sub];
        if(temp.sym!="fuzhi"&&temp.sym=="integer")
        {
            cout<<"第"<<temp.line<<"行缺少赋值号"<<endl;
            exp();
            if(index!=-1)
            {
                gen(mnum,"STO",level-table[index].level,table[index].address);
                mnum++;
            }
        }
        else if(temp.sym!="fuzhi"&&temp.sym!="integer")
        {
            cout<<"第"<<temp.line<<"行赋值号错误"<<endl;
            sub++;
            exp();
            if(index!=-1)
            {
                gen(mnum,"STO",level-table[index].level,table[index].address);
                mnum++;
            }
        }
        else
        {
            sub++;
            temp=ans[sub];  
            exp();
            if(index!=-1)
            {
                gen(mnum,"STO",level-table[index].level,table[index].address);
                mnum++;
            }
        }
    }
    else if(temp.strToken=="if")
    {
        sub++;
        lexp();
        if(temp.strToken=="then")
        {
            int cx2=mnum;
            gen(mnum,"JPC",0,0);
            mnum++;
            sub++;
            statement();
            midcode[cx2].A=mnum;
            if(temp.strToken=="else")
            {
                sub++;
                int cx1=mnum;
                gen(mnum,"JMP",0,0);
                mnum++;
                statement();
                midcode[cx1].A=mnum;
            }
        }
        else
        {
            cout<<"第"<<temp.line<<"行then缺失"<<endl;
            int cx2=mnum;
            gen(mnum,"JPC",0,0);
            mnum++;
            statement();
            midcode[cx2].A=mnum;
            if(temp.strToken=="else")
            {
                sub++;
                int cx1=mnum;
                gen(mnum,"JMP",0,0);
                mnum++;
                statement();
                midcode[cx1].A=mnum;
            }
        }
    }
    else if(temp.strToken=="while")
    {
        int jmp_addr=mnum;
        sub++;
        lexp();
        if(temp.strToken=="do")
        {
            sub++;
            temp=ans[sub];
            int cx2=mnum;
            gen(mnum,"JPC",0,0);
            mnum++;
            statement();
            gen(mnum,"JMP",0,jmp_addr);
            mnum++;
            midcode[cx2].A=mnum;
        }
        else
        {
            cout<<"第"<<temp.line-1<<"行while与do不匹配"<<endl;
            int cx2=mnum;
            gen(mnum,"JPC",0,0);
            mnum++;
            statement();
            gen(mnum,"JMP",0,jmp_addr);
            mnum++;
            midcode[cx2].A=mnum;
        }
    }
    else if(temp.strToken=="call")
    {
        sub++;
        temp=ans[sub];
        if(temp.sym=="id")
        {
            int index=find_table(temp.strToken,level);
            if(index==-1)
            {
                cout<<"标识符"<<temp.strToken<<"未定义"<<endl;
            }
            else if(table[index].kind==procedure)
            {
                
            }
            else
            {
                cout<<temp.strToken<<"不是过程名"<<endl;
            }
            sub++;
            temp=ans[sub];
            if(temp.strToken=="(")
            {
                int i=3;
                sub++;
                temp=ans[sub];
                if(temp.strToken==")")
                {
                    sub++;
                    temp=ans[sub];
                }
                else
                {
                    exp();
                    gen(mnum,"STO",level-table[index].level,table[index].size2+i);
                    mnum++;
                    i++;
                    while(temp.strToken==",")
                    {
                        sub++;
                        temp=ans[sub];
                        exp();
                        gen(mnum,"STO",level-table[index].level,table[index].size2+i);
                        mnum++;
                        i++;
                    }
                    if(temp.strToken==")")
                    {
                        sub++;
                        temp=ans[sub];
                    }
                    else
                    {
                        cout<<"第"<<temp.line<<"行缺少)"<<endl;
                    }
                }
                gen(mnum,"CAL",level-table[index].level,table[index].address);
                mnum++;
            }
            else
            {
                cout<<"第"<<temp.line<<"行缺少("<<endl;
                if(temp.strToken==")")
                {
                    sub++;
                    temp=ans[sub];
                }
                else
                {
                    exp();
                    while(temp.strToken==",")
                    {
                        sub++;
                        temp=ans[sub];
                        exp();
                    }
                    if(temp.strToken==")")
                    {
                        sub++;
                        temp=ans[sub];
                    }
                    else
                    {
                        cout<<"第"<<temp.line<<"行缺少)"<<endl;
                    }
                }
            }
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少标识符"<<endl;
            if(temp.strToken=="(")
            {
                sub++;
                temp=ans[sub];
                if(temp.strToken==")")
                {
                    sub++;
                    temp=ans[sub];
                }
                else
                {
                    exp();
                    while(temp.strToken==",")
                    {
                        sub++;
                        temp=ans[sub];
                        exp();
                    }
                    if(temp.strToken==")")
                    {
                        sub++;
                        temp=ans[sub];
                    }
                    else
                    {
                        cout<<"第"<<temp.line<<"行缺少)"<<endl;
                    }
                }
            }
            else
            {
                cout<<"第"<<temp.line<<"行缺少("<<endl;
                if(temp.strToken==")")
                {
                    sub++;
                    temp=ans[sub];
                }
                else
                {
                    exp();
                    while(temp.strToken==",")
                    {
                        sub++;
                        temp=ans[sub];
                        exp();
                    }
                    if(temp.strToken==")")
                    {
                        sub++;
                        temp=ans[sub];
                    }
                    else
                    {
                        cout<<"第"<<temp.line<<"行缺少)"<<endl;
                    }
                }
            }
        }
    }
    else if(temp.strToken=="read")
    {
        gen(mnum,"OPR",0,16);
        mnum++;
        sub++;
        temp=ans[sub];
        if(temp.strToken=="(")
        {
            sub++;
            temp=ans[sub];
            if(temp.sym!="id"&&temp.strToken!=")")
            {
                cout<<"第"<<temp.line<<"行标识符错误"<<endl;
                sub++;
                temp=ans[sub];
            }
            else if(temp.sym!="id"&&temp.strToken==")")
            {
                cout<<"第"<<temp.line<<"行缺少标识符"<<endl;
            }
            else
            {
                int index=find_table(temp.strToken,level);
                if(index==-1)
                {
                    cout<<"变量"<<temp.strToken<<"未定义"<<endl;
                }
                else
                {
                    gen(mnum,"STO",level-table[index].level,table[index].address);
                    mnum++;
                }
                sub++;
                temp=ans[sub];
                if(temp.sym=="mop"||temp.sym=="aop")
                {
                    cout<<"第"<<temp.line<<"行不能出现表达式"<<endl;
                }
                while(temp.strToken!=")"&&temp.strToken!=";"&&temp.strToken!=",")
                {
                    sub++;
                    temp=ans[sub];
                }
            }
            while(temp.sym=="id")
            {
                cout<<"第"<<temp.line<<"行缺少,"<<endl;
                sub++;
                temp=ans[sub];
            }
            while(temp.strToken==",")
            {
                sub++;
                temp=ans[sub];
                if(temp.sym=="id")
                {
                    gen(mnum,"OPR",0,16);
                    mnum++;
                    int index=find_table(temp.strToken,level);
                    if(index==-1)
                    {
                        cout<<"变量"<<temp.strToken<<"未定义"<<endl;
                    }
                    else
                    {
                        gen(mnum,"STO",level-table[index].level,table[index].address);
                        mnum++;
                    }
                    sub++;
                    temp=ans[sub];
                    while(temp.sym=="id")
                    {
                        cout<<"第"<<temp.line<<"行缺少,"<<endl;
                        sub++;
                        temp=ans[sub];
                    }
                }
                else
                {
                    cout<<"第"<<temp.line<<"行缺少标识符或标识符错误"<<endl;
                    if(ans[sub+1].strToken==","||ans[sub+1].strToken==")")
                    {
                        sub++;
                        temp=ans[sub];
                    }
                }
            }
            if(temp.strToken==")")
            {
                sub++;
                temp=ans[sub];
            }
            else
            {
                cout<<"第"<<temp.line<<"行缺少右括号"<<endl;
            }
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少左括号"<<endl;
            if(temp.sym!="id"&&temp.strToken!=")")
            {
                cout<<"第"<<temp.line<<"行缺少标识符或标识符错误"<<endl;
                sub++;
                temp=ans[sub];
            }
            else
            {
                int index=find_table(temp.strToken,level);
                if(index==-1)
                {
                    cout<<"变量"<<temp.strToken<<"未定义"<<endl;
                }
                else
                {
                    gen(mnum,"STO",level-table[index].level,table[index].address);
                    mnum++;
                }
                sub++;
                temp=ans[sub];
            }
            while(temp.sym=="id")
            {
                cout<<"第"<<temp.line<<"行缺少,"<<endl;
                sub++;
                temp=ans[sub];
            }
            while(temp.strToken==",")
            {
                sub++;
                temp=ans[sub];
                if(temp.sym=="id")
                {
                    int index=find_table(temp.strToken,level);
                    if(index==-1)
                    {
                        cout<<"变量"<<temp.strToken<<"未定义"<<endl;
                    }
                    else
                    {
                        gen(mnum,"STO",level-table[index].level,table[index].address);
                        mnum++;
                    }
                    sub++;
                    temp=ans[sub];
                    while(temp.sym=="id")
                    {
                        cout<<"第"<<temp.line<<"行缺少,"<<endl;
                        sub++;
                        temp=ans[sub];
                    }
                }
                else
                {
                    cout<<"第"<<temp.line<<"行缺少标识符或标识符错误"<<endl;
                    if(ans[sub+1].strToken==",")
                    {
                        sub++;
                        temp=ans[sub];
                    }
                }
            }
            if(temp.strToken==")")
            {
                sub++;
                temp=ans[sub];
            }
            else
            {
                cout<<"第"<<temp.line<<"行缺少右括号"<<endl;
            }
        }
    }
    else if(temp.strToken=="write")
    {
        sub++;
        temp=ans[sub];
        if(temp.strToken=="(")
        {
            sub++;
            temp=ans[sub];
            exp();
            gen(mnum,"OPR",0,14);
            mnum++;
            gen(mnum,"OPR",0,15);
            mnum++;
            while(temp.strToken==",")
            {
                sub++;
                exp();
                gen(mnum,"OPR",0,14);
                mnum++;
                gen(mnum,"OPR",0,15);
                mnum++;
            }
            if(temp.strToken==")")
            {
                sub++;
                temp=ans[sub];
            }
            else
            {
                cout<<"第"<<temp.line<<"行缺少右括号"<<endl;
            }
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少左括号"<<endl;
            exp();
            while(temp.strToken==",")
            {
                sub++;
                exp();
            }
            if(temp.strToken==")")
            {
                sub++;
                temp=ans[sub];
            }
            else
            {
                cout<<"第"<<temp.line<<"行缺少右括号"<<endl;
            }
        }
    }
    else
    {
        body();
    }
}

void lexp()
{
    temp=ans[sub];
    if(temp.strToken=="odd")
    {
        sub++;
        temp=ans[sub];
        exp();
        gen(mnum,"OPR",0,6);
        mnum++;
    }
    else
    {
        exp();
        if(temp.sym=="lop")
        {
            Ans a=temp;
            sub++;
            temp=ans[sub];
            exp();
            if(a.strToken=="=")
            {
                gen(mnum,"OPR",0,8);
                mnum++;
            }
            else if(a.strToken=="<>")
            {
                gen(mnum,"OPR",0,9);
                mnum++;
            }
            else if(a.strToken=="<")
            {
                gen(mnum,"OPR",0,10);
                mnum++;
            }
            else if(a.strToken==">=")
            {
                gen(mnum,"OPR",0,11);
                mnum++;
            }
            else if(a.strToken==">")
            {
                gen(mnum,"OPR",0,12);
                mnum++;
            }
            else if(a.strToken=="<=")
            {
                gen(mnum,"OPR",0,13);
                mnum++;
            }
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少比较运算符"<<endl;
            exp();
        }
    }
}

void exp()
{
    temp=ans[sub];
    if(temp.sym=="aop")
    {
        Ans a=temp;
        sub++;
        term();
        if(a.strToken=="-")
        {
            gen(mnum,"OPR",0,1);
            mnum++;
        }
    }
    else
    {
        term();
    }
    while(temp.sym=="aop")
    {
        Ans a=temp;
        sub++;
        term();
        if(a.strToken=="+")
        {
            gen(mnum,"OPR",0,2);
            mnum++;
        }
        else if(a.strToken=="-")
        {
            gen(mnum,"OPR",0,3);
            mnum++;
        }
    }
}

void term()
{
    temp=ans[sub];
    factor();
    while(temp.sym=="mop")
    {
        Ans a=temp;
        sub++;
        factor();
        if(a.strToken=="*")
        {
            gen(mnum,"OPR",0,4);
            mnum++;
        }
        else if(a.strToken=="/")
        {
            gen(mnum,"OPR",0,5);
            mnum++;
        }
    }
}

void factor()
{
    temp=ans[sub];
    if(temp.sym=="id")
    {
        int index=find_table(temp.strToken,level);
        if(index==-1)
        {
            cout<<"变量"<<temp.strToken<<"未定义"<<endl;
        }
        else
        {
            if(table[index].kind==constant)
            {
                gen(mnum,"LIT",0,table[index].value);
                mnum++;
            }
            else if(table[index].kind==variable)
            {
                gen(mnum,"LOD",level-table[index].level,table[index].address);
                mnum++;
            }
            else
            {
                cout<<temp.strToken<<"为过程名，错误"<<endl;
            }
        }
        sub++;
        temp=ans[sub];
    }
    else if(temp.sym=="integer")
    {
        gen(mnum,"LIT",0,stoi(temp.strToken));
        mnum++;
        sub++;
        temp=ans[sub];
    }
    else if(temp.strToken=="(")
    {
        sub++;
        exp();
        if(temp.strToken==")")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少)"<<endl;
        }
    }
    else if(temp.strToken==")")
    {
        cout<<"第"<<temp.line<<"行表达式错误"<<endl;
    }
    else if(temp.strToken==";")
    {
        cout<<"第"<<temp.line<<"行未赋值"<<endl;
    }
    else
    {
        cout<<"第"<<temp.line<<"行缺少("<<endl;
        exp();
        if(temp.strToken==")")
        {
            sub++;
            temp=ans[sub];
        }
        else
        {
            cout<<"第"<<temp.line<<"行缺少)"<<endl;
        }
    }
}

int get_sl(int B,int level)
{
    int res_B=B;
    while(level>0)
    {
        res_B=stack[res_B];
        level--;
    }
    return res_B;
}

void interpreter()
{
    int B=0,T=0,P=0;
    I=midcode[P];
    P+=1;
    while(P!=0)
    {
        if(I.F=="JMP")
        {
            P=I.A;
        }
        else if(I.F=="JPC")
        {
            if(stack[T]==0)
                P=I.A;
            T--;
        }
        else if(I.F=="INT")
        {
            T+=I.A-1;
        }
        else if(I.F=="LOD")
        {
            T+=1;
            stack[T]=stack[get_sl(B,I.L)+I.A];
        }
        else if(I.F=="STO")
        {
            stack[get_sl(B,I.L)+I.A]=stack[T];
            T--;
        }
        else if(I.F=="LIT")
        {
            T++;
            stack[T]=I.A;
        }
        else if(I.F=="CAL")
        {
            T++;
            stack[T]=get_sl(B,I.L);
            stack[T+1]=B;
            stack[T+2]=P;
            B=T;
            //T+=2;
            P=I.A;
        }
        else if(I.F=="OPR")
        {
            if(I.A==0)
            {
                T=B-1;
                P=stack[T+3];
                B=stack[T+2];
            }
            else if(I.A==1)
            {
                stack[T]=-stack[T];
            }
            else if(I.A==2)
            {
                T--;
                stack[T]=stack[T]+stack[T+1];
            }
            else if(I.A==3)
            {
                T--;
                stack[T]=stack[T]-stack[T+1];
            }
            else if(I.A==4)
            {
                T--;
                stack[T]=stack[T]*stack[T+1];
            }
            else if(I.A==5)
            {
                T--;
                stack[T]=stack[T]/stack[T+1];
            }
            else if(I.A==6)
            {
                stack[T]=stack[T]%2;
            }
            else if(I.A==8)
            {
                T--;
                stack[T]=stack[T]==stack[T+1];
            }
            else if(I.A==9)
            {
                T--;
                stack[T]=stack[T]!=stack[T+1];
            }
            else if(I.A==10)
            {
                T--;
                stack[T]=stack[T]<stack[T+1];
            }
            else if(I.A==11)
            {
                T--;
                stack[T]=stack[T]>=stack[T+1];
            }
            else if(I.A==12)
            {
                T--;
                stack[T]=stack[T]>stack[T+1];
            }
            else if(I.A==13)
            {
                T--;
                stack[T]=stack[T]<=stack[T+1];
            }
            else if(I.A==14)
            {
                cout<<stack[T];
            }
            else if(I.A==15)
            {
                cout<<endl;
            }
            else if(I.A==16)
            {
                int x;
                cin>>x;
                T++;
                stack[T]=x;
            }
        }
        I=midcode[P];
        if(P==0)
            break;
        P++;
    }
}

int main()
{
    string reserved_word[15]={"program","const","var","procedure","begin","end","if","then","else","while","do","call","read","write","odd"};
    int row=1,col=0;
    char ch;
    string strToken="";
    ifstream infile;
    infile.open("3.txt",ios::in);
    if(!infile)
        cout<<"Open Error!"<<endl;
    ch=infile.get();
    col++;
    while(infile.peek()!=EOF)
    {
        strToken="";
        if(ch=='(')
        {
            ans[n].sym="leftpa";
            ans[n].strToken="(";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch==')')
        {
            ans[n].sym="rightpa";
            ans[n].strToken=")";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch=='+')
        {
            ans[n].sym="aop";
            ans[n].strToken="+";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch=='-')
        {
            ans[n].sym="aop";
            ans[n].strToken="-";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch=='*')
        {
            ans[n].sym="mop";
            ans[n].strToken="*";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch=='/')
        {
            ans[n].sym="mop";
            ans[n].strToken="/";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch=='=')
        {
            ans[n].sym="lop";
            ans[n].strToken="=";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch=='<')
        {
            ch=infile.get();
            col++;
            if(ch=='=')
            {
                ans[n].sym="lop";
                ans[n].strToken="<=";
                ans[n].line=row;
                n++;
                ch=infile.get();
                col++;
            }
            else if(ch=='>')
            {
                ans[n].sym="lop";
                ans[n].strToken="<>";
                ans[n].line=row;
                n++;
                ch=infile.get();
                col++;
            }
            else
            {
                ans[n].sym="lop";
                ans[n].strToken="<";
                ans[n].line=row;
                n++;
            }
        }
        else if(ch=='>')
        {
            ch=infile.get();
            col++;
            if(ch=='=')
            {
                ans[n].sym="lop";
                ans[n].strToken=">=";
                ans[n].line=row;
                n++;
                ch=infile.get();
                col++;
            }
            else
            {
                ans[n].sym="lop";
                ans[n].strToken=">";
                ans[n].line=row;
                n++;
            }
        }
        else if(ch==';')
        {
            ans[n].sym="fenhao";
            ans[n].strToken=";";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch==',')
        {
            ans[n].sym="douhao";
            ans[n].strToken=",";
            ans[n].line=row;
            n++;
            ch=infile.get();
            col++;
        }
        else if(ch==':')
        {
            ch=infile.get();
            col++;
            if(ch=='=')
            {
                ans[n].sym="fuzhi";
                ans[n].strToken=":=";
                ans[n].line=row;
                n++;
                ch=infile.get();
                col++;
            }
            else
            {
                cout<<row<<"行"<<col<<"列后面缺少="<<endl;
            }
        }
        else if(ch==' '||ch=='\n')
        {
            if(ch=='\n')
            {
                row++;
                col=0;
            }
            ch=infile.get();
            col++;
        }
        else if(IsDigit(ch))
        {
            while(IsDigit(ch))
            {
                Concat(strToken,ch);
                ch=infile.get();
                col++;
            }
            if(!IsLetter(ch))
            {
                ans[n].sym="integer";
                ans[n].strToken=strToken;
                ans[n].line=row;
                n++;
            }
            else
            {
                cout<<row<<"行"<<col<<"列标识符不能以数字开头"<<endl;
            }
        }
        else if(IsLetter(ch))
        {
            while(IsLetter(ch)||IsDigit(ch))
            {
                Concat(strToken,ch);
                ch=infile.get();
                col++;
            }
            if(IsReserve(reserved_word,strToken))
            {
                ans[n].sym="reserved_word";
                ans[n].strToken=strToken;
                ans[n].line=row;
                n++;
            }
            else
            {
                ans[n].sym="id";
                ans[n].strToken=strToken;
                ans[n].line=row;
                n++;
            }
        }
        else
        {
            ch=infile.get();
            col++;
        }
    }
    for(int i=0;i<n;i++)
    {
        cout<<ans[i].line<<'\t'<<ans[i].sym<<'\t'<<ans[i].strToken<<endl;
    }
    infile.close();
    prog();
    for(int i=0;i<mnum;i++)
    {
        cout<<i<<'.'<<'('<<midcode[i].F<<','<<midcode[i].L<<','<<midcode[i].A<<')'<<endl;
    }
    interpreter();
    return 0;
}