#include<iostream>
#include<stdio.h>
#include<string>
#include"dao.h"	
#include"code.h"

using namespace std;

code::code(int id){
	codeId = id;
}

code::code(){
}

bool code::init(){
	dao d = dao();
	questionId = d.queryQuestionIdById(codeId);		
	timeLimit = d.queryTimeLimitByQuestionId(questionId) + 1000;
	memoryLimit = d.queryMemoryLimitByQuestionId(questionId);
	language = d.queryLanguageById(codeId);
	
	bool hasCode;
	switch(language){
		case 0:
				hasCode	= d.queryCodeById(codeId, "user/userCode.c");
				break;
		case 1:	
				hasCode	= d.queryCodeById(codeId, "user/userCode.cpp");
				break;
		case 2:	
				hasCode	= d.queryCodeById(codeId, "user/userCode.java");
				break;
	}
	
	if(timeLimit > 0 && memoryLimit > 0 && hasCode){
		//cout << timeLimit << "\t" << memoryLimit << "\t" << hasCode << endl;
		return true;
	}else{
		return false;
	}
}

int code::getCodeId(){
	return codeId;
}

void code::setCodeId(int id){
	codeId = id;
}

int code::getLanguage(){
	return language;
}

int code::getTimeLimit(){
	return timeLimit;
}

int code::getMemoryLimit(){
	return memoryLimit;
}


int code::getQuestionId(){
	return questionId;
}


int code::getUseTime(){
	return useTime;
}
int code::getUseMemory(){
	return useMemory;
}
void code::setUseTime(int time){
	useTime = time;
}
void code::setUseMemory(int memory){
	useMemory = memory;
}








