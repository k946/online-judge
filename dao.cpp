#include<string>
#include<iostream>
#include<fstream>
#include<stdio.h>
#include"dao.h"

using namespace std;

bool dao::connect(MYSQL * mysql){
	mysql_init(mysql);
	if( !mysql_real_connect(mysql, "120.79.56.xx", 
		"sa", "123456", "test", 0, NULL ,0) ){
		FILE * error = fopen("Database.error", "w");
		fprintf(error, "%s\n", mysql_error(mysql));
		fclose(error);
		exit(1);
		return false;
	}
	return true;
}

int dao::getCodeId(int workerID){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	cout << "----------------------" << endl;
	sprintf(sql, "select codeID from Critical_Section where  workerID = %d or workerID is NULL and error < 2;", workerID);
	cout<<sql<<endl;
	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
		
	MYSQL_ROW row = mysql_fetch_row(result);
	
	if(NULL != row){
		int codeID = atoi( row[0] );
		sprintf(sql, "update critical_section set workerID = %d where codeID = %d;", workerID , codeID );
		cout<<sql<<endl;
		mysql_query( &mysql, sql );
		
		mysql_free_result(result);
		mysql_close(&mysql);
		
		return codeID;
	}else{
		return -1;
	}
	
}

int dao::queryTimeLimitByQuestionId(int id){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	sprintf(sql, "select timeLimit from question where questionID = %d", id);
	cout<<sql<<endl;
	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	MYSQL_ROW row = mysql_fetch_row(result);

	if(NULL != row){
		mysql_free_result(result);
		mysql_close(&mysql);
		return atoi(row[0]);
	}

	exit(2);
	
}

int dao::queryMemoryLimitByQuestionId(int id){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	sprintf(sql, "select memoryLimit from question where questionID = %d", id);
	cout<<sql<<endl;
	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	
	MYSQL_ROW row = mysql_fetch_row(result);
	if(NULL != row){
		mysql_free_result(result);
		mysql_close(&mysql);
		return atoi(row[0]);
	}


	exit(3);
	
}

int dao::queryQuestionIdById(int id){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	sprintf(sql, "select questionID from code where codeID = %d", id);
	cout<<sql<<endl;

	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	
	MYSQL_ROW row = mysql_fetch_row(result);
	if(NULL != row){
		mysql_free_result(result);
		mysql_close(&mysql);
		return atoi(row[0]);
	}

	exit(4);
	
}

int dao::queryLanguageById(int id){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	sprintf(sql, "select language from code where codeID = %d", id);
	cout<<sql<<endl;

	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	
	MYSQL_ROW row = mysql_fetch_row(result);
	if(NULL != row){
		mysql_free_result(result);
		mysql_close(&mysql);
		return atoi(row[0]);
	}
	
	exit(5);
	
}


void dao::deleteCritical_SectionID(int id){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	sprintf(sql, "delete from critical_section where codeID = %d", id);	
	cout<<sql<<endl;
	mysql_query(&mysql, sql);
	mysql_close(&mysql);	
}

void dao::cleanWorkerId(int workerId){
	int error_number = 0;	
		
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];

	sprintf(sql, "select error from critical_section where workerId = %d", workerId);
	cout<<sql<<endl;

	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	
	MYSQL_ROW row = mysql_fetch_row(result);
	if(NULL != row){	
		error_number = atoi(row[0]);
	}

	
	
	sprintf(sql, "update critical_section set error = %d where workerID = %d", error_number + 1 ,workerId);	
	cout<<sql<<endl;
	mysql_query(&mysql, sql);
	
	sprintf(sql, "update critical_section set workerID = 0 where workerID = %d", workerId);	
	cout<<sql<<endl;
	mysql_query(&mysql, sql);
	mysql_close(&mysql);

}

void dao::updata(int codeID, char const * column ,int value){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	sprintf(sql, "update code set %s = %d where codeID = %d", column , value, codeID);	
	cout<<sql<<endl;
	mysql_query(&mysql, sql);
	mysql_close(&mysql);		
}

void dao::insertResourceUsed(int codeID, int timeMS, int memoryM){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[127];
	sprintf(sql, "insert into acceptcode value(%d, %d, %d)", codeID, timeMS, memoryM);
	cout<<sql<<endl;
	mysql_query(&mysql, sql);
	mysql_close(&mysql);	

}

void dao::insertErrorInfo(int codeID, char const * compileError){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	char sql[2048];
	sprintf(sql, "insert into error_info value(%d, '%s')", codeID, compileError);
	cout<<sql<<endl;	
	mysql_query(&mysql, sql);
	mysql_close(&mysql);
}
	
bool dao::queryCodeById(int id,char *savePath){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);

	ofstream file(savePath);

	char sql[127];
	sprintf(sql, "select code from code where codeID = %d", id);
	cout<<sql<<endl;
	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	MYSQL_ROW row = mysql_fetch_row(result);

	if(NULL != row){
		file << row[0];
		cout << "read code:\n" << row[0] <<endl;
		file.close();
		mysql_free_result(result);
		mysql_close(&mysql);
		return true;
	}else{
		cout << "no data!!" ;
		exit(7);
	}
	
}


bool dao::queryInputByQuestionId(int id){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);
	ofstream file("user/userInput.d");//userCode.c used to sava code 
	char sql[127];
	sprintf(sql, "select input from question where questionID = %d", id);
	cout<<sql<<endl;
	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	MYSQL_ROW row = mysql_fetch_row(result);

	if(NULL != row){
		file << row[0];

		file.close();
		mysql_free_result(result);
		mysql_close(&mysql);
		return true;
	}
		
	exit(8);
}


bool dao::queryOutputByQuestionId(int id, string &buffer){
	MYSQL mysql;
	if ( !connect(&mysql) )
		exit(1);

	char sql[127];
	sprintf(sql, "select output from question where questionID = %d", id);
	cout<<sql<<endl;
	mysql_query( &mysql, sql );
	MYSQL_RES * result = mysql_store_result( &mysql );
	MYSQL_ROW row = mysql_fetch_row(result);

	if(NULL != row){
		buffer.append(row[0]);
		mysql_free_result(result);
		mysql_close(&mysql);
		return true;
	}

	exit(9);
	

}

