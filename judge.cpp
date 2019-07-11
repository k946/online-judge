#include<iostream>
#include<wait.h>
#include<map>
#include<string>
#include<string.h>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/ptrace.h>
#include<sys/syscall.h>
#include<sys/user.h>
#include<sys/reg.h>
#include<asm/unistd_64.h>
#include"pramaters.h"
#include"code.h"
#include"dao.h"
using namespace std;

map<int, int> allow_syscall;
map<int, int> refuse_signal;

dao d = dao();
code c = code();

int compile(int language, int codeId);
int run(int codeId);
bool readFile(char * path, string &buffer, string add2Last);
bool isAnswer(int questionId);
int trace_child(pid_t pid);
void allow_syscall_init();
void refuse_signal_init();

int main(int argv, char * args[]){
	int status;
	string buffer="";
	cout << "codeID: " << args[1] << endl;
	c.setCodeId( atoi(args[1]) );
	if( !c.init() )
		return 1;	
		
	int codeId = c.getCodeId();
	int language = c.getLanguage();
	int questionId = c.getQuestionId();
	int timeLimit = c.getTimeLimit();
	int memoryLimit = c.getMemoryLimit();

	cout << "codeID: " << codeId << "  Language: " << language << "  QuetionID: " << questionId << "   TimeLimit: " << timeLimit << "         MemoryLimit: " << memoryLimit << endl;
	

	status = compile(codeId, language);
	if( status != 0){
		d.updata(codeId, "status", COMPILE_ERROR);

		readFile("codeError.info", buffer, "<br>");
		d.insertErrorInfo(c.getCodeId(), buffer.c_str());	
		return 0;
	}

	d.updata(codeId, "status", COMPILE_PASS);
	d.queryInputByQuestionId(questionId);
	
	// status =0--normal  -1--error  >0--signal
 	status = run(codeId);
	if( status == 14){		//SIGALRM 
		d.updata(codeId, "status", OVER_TIME);		
		return 0;	
	}else if(status == 11){	//SIGSEGV segmentation error
		d.updata(codeId, "status", OVER_MEMORY);
		return 0;
	}else{
		d.updata(codeId, "status", RUNTIME_ERROR);		
		cout << "Error: " << status << endl;
		readFile("codeError.info", buffer, "<br>");
		d.insertErrorInfo(c.getCodeId(), buffer.c_str());
		return 0;
	}

	if( !isAnswer(questionId) ){
		d.updata(codeId, "status", ANSWER_ERROR);
		return 0;
	}

	d.updata(codeId, "status", ACCEPT);
	d.insertResourceUsed(codeId, c.getUseTime(), c.getUseMemory());
	
	return 0;
}


int compile(int codeId, int language){
	pid_t child = fork();
	int status = 0;	

	if(child == 0){
		//child process		
		freopen("codeError.info", "w", stderr);
		alarm(5);
		d.updata(codeId, "status", COMPILING);
		execvp(compile_prama[language][0], compile_prama[language]);
	}else if(child > 0){
		//father process		
		waitpid(child, &status, 0);

		return status;
	}else{
		exit(10);
	}
}

bool readFile(char * path, string &buffer, string add2Last){
	ifstream file(path, ios::binary);
	char s[10240];
	
	if(add2Last == ""){
		file.seekg(0, file.end);
		int length = file.tellg();
		if(length >= 10240){
			length = 10239;	
		}
		file.seekg(0, file.beg);
	
		file.read(s,length);

		buffer = s;	
	}else{
		file >> s;
		while( !file.eof() ){
			buffer += s + add2Last;
			file >> s;			
		}
	}
	

	if(buffer.length())
		return true;
}


int run(int codeId){
	int status = 0;
	int timeLimit = c.getTimeLimit();
	int memoryLimit = c.getMemoryLimit();
	pid_t child = fork();	

	if(child == 0){
		//child process	
		struct rlimit rli;
		
		d.updata(codeId, "status", RUNING);	
		
		//redirect I/O
		freopen("user/userOutput.d", "w", stdout);
		freopen("user/userInput.d", "r", stdin);
		freopen("codeError.info", "w", stderr);
		
		//limit runing time
		rli.rlim_cur = rli.rlim_max = timeLimit/1000;
		setrlimit(RLIMIT_CPU, &rli);
		
		//limit runing time
		alarm( timeLimit/1000 );
	
		//limit memory
		rli.rlim_cur = rli.rlim_max = memoryLimit * 1024 * 1024 ;
		setrlimit(RLIMIT_AS, &rli);

		//trace sub-process
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);	
		//chroot("user");		
		execvp("user/userCode", 0);
	}else if(child > 0){
		//father process		
		status = trace_child(child);	
	}else{
		//create process fail	
		exit(10);
	}
	
	return status;
}

bool isAnswer(int questionId){
	string userOutput = "";
	string answer = "";	
	
	if ( !d.queryOutputByQuestionId(questionId, answer) )
		return false;

	readFile("user/userOutput.d", userOutput, "");
/*
	for(int i = 0; i < answer.length(); i++){
		printf("%d  ", answer[i]);
	}
	cout << endl;
	for(int i = 0; i < userOutput.length(); i++){
		printf("%d  ", userOutput[i]);
	}
*/
	cout << "answer: \n" << answer << "user: \n" << userOutput ;
	if( answer.compare(userOutput) == 0 ){
		return true;	
	} 
	return false;
		
}


int trace_child(pid_t pid){
	
	int status = 2 << 30;
	long orig_eax;
	int sig;
	int timeMS;
	int memoryKB;
	int timeLimit = c.getTimeLimit();
	int memoryLimit = c.getMemoryLimit();
	allow_syscall_init();
	refuse_signal_init();

	struct rusage rup;
	while(1){
		
		wait4(pid, &status, 0, &rup);

		//runtime error
		if( WIFSTOPPED(status) ){
			sig = WSTOPSIG(status);
			if( refuse_signal[sig] ){
				cout << "error signal: " << sig << endl;
				ptrace(PTRACE_KILL, pid, 0, 0);
				return sig;
			}

		}

		//cout << "status: " << WEXITSTATUS(status) << endl;
			
		if( WIFEXITED(status) ){
			timeMS = rup.ru_utime.tv_sec * 1000 + rup.ru_utime.tv_usec / 1000;
			timeMS += rup.ru_stime.tv_sec * 1000 + rup.ru_stime.tv_usec / 1000 ;

			memoryKB = rup.ru_minflt * (getpagesize() / 1024) ; 
			cout << "time: " << timeMS << endl;
			cout << "memory: " << memoryKB << endl;

			c.setUseTime(timeMS);
			c.setUseMemory(memoryKB / 1024);
			return 0;
		}

		orig_eax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);	
		if( !allow_syscall[orig_eax] && orig_eax != -1 ){
			ofstream log("Database.error", ios::out | ios::app);
			log <<  "codeID: " << c.getCodeId() << "   Error syscall: " << orig_eax << endl;
			log.close();
			switch(orig_eax){
				case __NR_alarm : alarm(0);
				break;
			}
			ptrace(PTRACE_KILL, pid, 0, 0);	
			return -1;			
		}	
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);	
	}

}

void allow_syscall_init(){
	int size = sizeof(a_syscall)/sizeof(a_syscall[0]);
	for(int i = 0; i < size; i++){
		allow_syscall[ a_syscall[i] ] = 1;
	}
}

void refuse_signal_init(){
	int size = sizeof(r_signal)/sizeof(r_signal[0]);
	for(int i = 0; i < size; i++){
		refuse_signal[ r_signal[i] ] = 1;
	}
}






