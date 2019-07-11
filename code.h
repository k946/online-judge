class code{
private:
	int codeId;
	int timeLimit;
	int memoryLimit;
	int questionId;
	int language;
	int useTime;
	int useMemory;

public:
	code(int id);
	code();
	bool init();
	int getCodeId();
	void setCodeId(int id);
	int getLanguage();
	int getTimeLimit();
	int getMemoryLimit();
	int getQuestionId();
	int getUseTime();
	int getUseMemory();
	void setUseTime(int time);
	void setUseMemory(int memory);
};
