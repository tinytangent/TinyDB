#include <string>

class ASTNodeBase;

class SQLParser
{
public:
    static ASTNodeBase* parse(const std::string& sql);
};
