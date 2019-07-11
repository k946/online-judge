/*
** 该程序为临时程序，用于测试各种LINUX系统调用
*/


#include<unistd.h>
#include<stdio.h> 
#include<string>
#include<stdlib.h>
#include<wait.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<iostream>
#include<sys/ptrace.h>
#include<sys/syscall.h>
#include<sys/user.h>
#include<map>
#include<sys/reg.h>

using namespace std;


//测试trace系统调用
void trace_child(pid_t pid){
	int status = 2 << 30;
	long orig_eax, eax;
	int insyscall = 0;

	struct rusage rup;
	while(1){
		
		wait4(pid, &status, 0, &rup);

		if( WEXITSTATUS(status) == 11 ){
			ptrace(PTRACE_KILL, pid, 0, 0);
			cout << "runtime error: " << WEXITSTATUS(status) << endl;
			break;		
		}else if( WEXITSTATUS(status) == 14 ){
			cout << "over time: " << WEXITSTATUS(status) << endl;
			ptrace(PTRACE_KILL, pid, 0, 0);
			break;
		}else{
			cout << "SIG: " << WEXITSTATUS(status) << endl;
		}
			
		//cout << "status: " << WEXITSTATUS(status) << endl;
			
		if( WIFEXITED(status) ){
			int time = rup.ru_utime.tv_sec * 1000 + rup.ru_utime.tv_usec / 1000;
			time += rup.ru_stime.tv_sec * 1000 + rup.ru_stime.tv_usec / 1000 ;
			cout << "time: " << time << endl;
			cout << "memory: " << rup.ru_minflt * (getpagesize() / 1024) << endl;
			break;
		}

		orig_eax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);		
		if(orig_eax != -1)		
			cout << "syscall: " << orig_eax << endl;
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);

	}

}

int main(){
	char * const prama[] = {"./a",0};	
	pid_t child = fork();

	
	if( child  == 0){
		struct rlimit rli;
		rli.rlim_cur = rli.rlim_max = 1;
		setrlimit(RLIMIT_CPU, &rli);
		
		rli.rlim_cur = rli.rlim_max = 128 * 1024 * 1024 ;
		setrlimit(RLIMIT_AS, &rli);

		alarm(1);

		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		setuid(0);
		cout << "UID: " << getuid() << endl;		
		//cout << chroot(".") << endl;
		//cout << chdir("user") << endl;			

		cout << execvp(prama[0],prama) << endl;
	}else if(child > 0){
		cout << "pid: " << child << endl;
		trace_child(child);
		printf("creation!\n");
	}

	
	return 0;
}


