#include "Parser/ASTNodes.h"
#include "FieldType.h"
#include "FieldList.h"

FieldList::FieldList()
{

}

void FieldList::addField(const std::string& fieldName, const std::string& typeName, ASTNodeBase *astNode)
{
    UncompiledField field;
    field.fieldName = fieldName;
    field.typeName = typeName;
    field.astNode = astNode;
    uncompiledFieldLists.push_back(field);
}

void FieldList::compile()
{

}

FieldList* FieldList::fromASTNode(std::list<ASTCreateTableFieldNode*> fieldNodes)
{
    FieldList* ret = new FieldList();
    for (auto i : fieldNodes)
    {
        CompiledField field;
        field.fieldName = i->name;
        field.fieldType = FieldType::getType(i->dataType->name)->construct(i);
        ret->compiledField.push_back(field);
    }
    for (int i = 0; i < ret->compiledField.size(); i++)
    {
        CompiledField& filed = ret->compiledField[i];
        ret->headerSize += filed.fieldName.size() + 1;
        ret->headerSize += filed.fieldType->getTypeName().size() + 1;
        ret->headerSize += 4;
        ret->headerSize += filed.fieldType->getHeaderLength();
        if (filed.fieldType->hasConstantLength())
        {
            ret->recordFixedSize += filed.fieldType->getConstantLength();
        }
        else
        {
            ret->recordFixedSize += FIXED_SIZE_MAX_BYTES + sizeof(uint32_t);
        }
    }
    ret->headerData = new char[ret->headerSize];
    char *headerPos = ret->headerData;
    for (int i = 0; i < ret->compiledField.size(); i++)
    {
        CompiledField& filed = ret->compiledField[i];
        auto fieldType = filed.fieldType;
        memcpy(headerPos, filed.fieldName.c_str(), filed.fieldName.size() + 1);
        headerPos += filed.fieldName.size() + 1;
        memcpy(headerPos, fieldType->getTypeName().c_str(), fieldType->getTypeName().size() + 1);
        headerPos += fieldType->getTypeName().size() + 1;
        uint32_t extraDataSize = fieldType->getHeaderLength();
        memcpy(headerPos, (char*)&extraDataSize, sizeof(extraDataSize));
        headerPos += sizeof(extraDataSize);
        fieldType->writeHeader(headerPos);
    }
    return ret;
}

FieldList* FieldList::fromBuffer()
{
    return NULL;
}

int FieldList::getRecordFixedSize()
{
    return recordFixedSize;
}

int FieldList::getHeaderSize()
{
    return headerSize;
}

char* FieldList::getHeaderData()
{
    return headerData;
}

const std::vector<FieldList::CompiledField>& FieldList::getCompiledFields()
{
    return compiledField;
}
