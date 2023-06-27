#include <stdlib.h>
#include <optional>
#include <string>
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

class PVarsContext
{
public:
	static std::string tableName;
};

bool setDbVariable(const char* variable1, const char* value1);
Result getDbVariable(const char* key);

