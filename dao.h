#include<mysql/mysql.h>
#include<string>

using namespace std;

class dao{
public:

	bool connect(MYSQL * mysql);

	int getCodeId(int workerID);
	int queryTimeLimitByQuestionId(int id);
	int queryMemoryLimitByQuestionId(int id);
	int queryQuestionIdById(int id);
	int queryLanguageById(int id);

	void deleteCritical_SectionID(int id);
	void cleanWorkerId(int id);
	void updata(int codeID, char const * column ,int value);
	
	bool queryCodeById(int id,char * savePath);
	bool queryInputByQuestionId(int id);
	bool queryOutputByQuestionId(int id, string &buffer);
	
	void insertResourceUsed(int codeID, int timeMS, int memoryM);
	void insertErrorInfo(int codeID, char const * compileErrorInformation);
};



