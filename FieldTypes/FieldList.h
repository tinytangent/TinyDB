#include <string>
#include <vector>

class FieldType;
class ASTNodeBase;

class FieldList
{
public:
    struct CompiledField {
        std::string fieldName;
        FieldType* fieldType;
    };
protected:
    struct UncompiledField {
        std::string fieldName;
        std::string typeName;
        ASTNodeBase *astNode;
    };
    std::vector<UncompiledField> uncompiledFieldLists;
protected:
    std::vector<CompiledField> compiledField;
    int headerSize = 0;
    int recordFixedSize = 0;
    char *headerData = nullptr;
public:
    const int FIXED_SIZE_MAX_BYTES = 32;
    FieldList();
    void addField(const std::string &fieldName, const std::string &typeName, ASTNodeBase *astNode);
    void compile();
    //void deserialize(char* buffer);
    int getRecordFixedSize();
    int getHeaderSize();
    char* getHeaderData();
};
