#include<map>
using namespace std;

#define COMPILING 1
#define COMPILE_ERROR 2
#define COMPILE_PASS 3
#define RUNING 4
#define RUNTIME_ERROR 5
#define OVER_TIME 6
#define OVER_MEMORY 7
#define ANSWER_ERROR 8
#define ACCEPT 9

char *  compile_c[] = {"gcc", "-static" , "-o", "user/userCode", "user/userCode.c", 0};
char *  compile_cpp[] = {"g++", "-Wall", "-static","-o", "user/userCode", "user/userCode.cpp", 0};
char *  compile_java[] ={"javac", "-J-Xms32", "-J-Xmx256m", "user/userCode.java", 0};


char * * compile_prama[] = {compile_c, compile_cpp, compile_java};

int a_syscall[] = {0 ,1 ,2 ,3 ,5 ,8 ,9 ,10, 11, 12, 21, 59, 63, 89,158, 231};

int r_signal[] = {2, 3, 4, 7, 8, 9, 11, 14, 24, 31};

