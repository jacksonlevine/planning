#include <stdlib.h>
#include <optional>
#include <string>
#include <vector>
enum ResultType
{
	PVARSERROR,
	PVARSRESULT
};
class Result
{
private:
	std::optional<std::string> _thing;
public:
	ResultType type;
	std::string value();
	void setValue(std::string val);
	Result();
};

class ListResult
{
private:
	std::optional<std::vector<std::string>> _things;
public:
	ResultType type;
	std::vector<std::string> values();
	void setValues(std::vector<std::string> vals);
	ListResult();
};

class PVarsContext
{
public:
	static std::string tableName;
};

bool setDbVariable(const char* variable1, const char* value1);
Result getDbVariable(const char* key);
ListResult getDbTable(std::string tableName);
bool deleteDbVariable(const std::string key);
