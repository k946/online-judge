#include<iostream>
#include<wait.h>
#include<string>
#include<fstream>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"dao.h"

using namespace std;

#define MAX_ERROR_NUMBER 5
#define WORKER_ID 1

int main(){
	dao d = dao();
	
	int error_number = 0;
	int sleepTime = 1;	
	int status;		//judge process retrun status
	char pId[13];	//codeId of char *
	int	iId;		//codeId of int
	char * p[] = {"./judge", "0", 0};	//prama of execute judge

	while(1){
		iId = d.getCodeId(WORKER_ID);
		if(iId != -1){
			sprintf(pId, "%d", iId);
			pid_t child = fork();
			p[1] = pId;
			if(child == 0){
				execvp(p[0],p);	
			}else if(child > 0){
				waitpid(child, &status, 0);
				
				if( WEXITSTATUS(status) == 0 ){
					d.deleteCritical_SectionID(iId);
					sleepTime = 1;
					error_number = 0;
				}else{
					error_number++;
					ofstream log("Database.error", ios::out | ios::app);
					log << "CodeId: " << iId <<"  Erorr status: " << WEXITSTATUS(status) << endl;
					log.close();

					//over MAX_ERROR_NUMBER close judge worker
					// and clean workerID at database
					if(error_number == MAX_ERROR_NUMBER){
						d.cleanWorkerId(WORKER_ID);
						exit(0);
					}
				}
			}
		}	
		sleep(sleepTime);
		sleepTime = (sleepTime + 1) % 30 ;
	}

	return 0;
}
