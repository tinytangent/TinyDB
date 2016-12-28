#include <string>
#include <vector>

class ASTNodeBase;

class SQLParser
{
public:
    static std::vector<ASTNodeBase*> parse(const std::string& sql);
    static std::vector<ASTNodeBase*> parseStream(std::istream& sql);
};
