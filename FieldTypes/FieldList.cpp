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
    for(int i = 0; i < uncompiledFieldLists.size(); i++)
    {
        CompiledField field;
        field.fieldName = uncompiledFieldLists[i].fieldName;
        field.fieldType = FieldType::getType(
            uncompiledFieldLists[i].typeName
        )->construct(uncompiledFieldLists[i].astNode);
        compiledField.push_back(field);
    }
    for(int i = 0; i < compiledField.size(); i++)
    {
        headerSize += compiledField[i].fieldName.size() + 1;
        headerSize += compiledField[i].fieldType->getTypeName().size() + 1;
        headerSize += 4;
        headerSize += compiledField[i].fieldType->getHeaderLength();
        if(compiledField[i].fieldType->hasConstantLength())
        {
            recordFixedSize += compiledField[i].fieldType->getConstantLength();
        }
        else
        {
            recordFixedSize += FIXED_SIZE_MAX_BYTES + sizeof(uint32_t);
        }
    }
    headerData = new char[headerSize];
    char *headerPos = headerData;
    for(int i = 0; i < compiledField.size(); i++)
    {
        auto fieldType = compiledField[i].fieldType;
        memcpy(headerPos, compiledField[i].fieldName.c_str(), compiledField[i].fieldName.size() + 1);
        headerPos += compiledField[i].fieldName.size() + 1;
        memcpy(headerPos, fieldType->getTypeName().c_str(), fieldType->getTypeName().size() + 1);
        headerPos += fieldType->getTypeName().size() + 1;
        uint32_t extraDataSize = fieldType->getHeaderLength();
        memcpy(headerPos, (char*)&extraDataSize, sizeof(extraDataSize));
        headerPos += sizeof(extraDataSize);
        fieldType->writeHeader(headerPos);
    }
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
