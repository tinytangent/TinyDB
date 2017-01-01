#include <iostream>
#include <map>
#include <boost/log/trivial.hpp>
#include "Expression/SuffixExpression.h"
#include "FieldTypes/FieldList.h"
#include "FieldTypes/FieldType.h"
#include "Storage/CachedStorageArea.h"
#include "Storage/AbstractStorageArea.h"
#include "Storage/AbstractFixedAllocator.h"
#include "Storage/FixedAllocator.h"
#include "Storage/RecordAllocator.h"
#include "Storage/BuddyDynamicAllocator.h"
#include "Parser/ASTNodes.h"
#include "Index/Index.h"
#include "Database.h"
#include "Table.h"

Table::Table(
    Database *database,
    const std::string& tableName,
    const boost::filesystem::path& fixedStorageFileName,
    const boost::filesystem::path& variableStorageFileName
) {
    this->database = database;
    //this->dynamicAllocator = dynamicAllocator;
    this->tableName = tableName;
    this->fixedStoragePath = fixedStorageFileName;
    this->variableStoragePath = variableStorageFileName;
    //TODO: initialize the storage area.
}

bool Table::addBinaryRecord(char * buffer)
{
    auto pos = fixedAllocator->allocate().getOffset();
    fixedStorageArea->setDataAt(pos, buffer, fieldList->getRecordFixedSize());
    fixedStorageArea->flush();
    for (auto index : associatedIndexes)
    {
        index->onAddRecord(pos, buffer);
    }
    //TODO: error handling
    return true;
}

std::vector<uint64_t> Table::findBinaryRecordInRange(FieldType* fieldType, int fieldOffset, char * rangeMin, char * rangeMax, int mode)
{
    //TODO : Implement bitmap lookup
    //std::cout << fixedAllocator << std::endl;
    std::vector<uint64_t> ret;
    char* temp = new char[fieldType->getConstantLength()];
    //TODO: Bug, fix it.
    uint64_t blockOffset = fixedAllocator->getBlockOffset(0, 1 + fixedAllocator->blocksReservedPerMacroBlock);
    for (int i = 0; i < fixedAllocator->blocksPerMacroBlock; i++)
    {
        auto status = fixedAllocator->getBlockStatus(0, i);
        if (
            status != RecordAllocator::BlockStatus::UNUSED &&
            status != RecordAllocator::BlockStatus::RESERVED )
        {
            for (int i = 0; i < 128 * 8; i++)
            {
                if (fixedAllocator->blockIsRecordUsed(blockOffset, i))
                {
                    uint64_t recordOffset = fixedAllocator->recordGetBlockOffset(blockOffset, i);
                    fixedStorageArea->getDataAt(recordOffset + fieldOffset, temp, fieldType->getConstantLength());
                    if (fieldType->isEqual(rangeMin, temp))
                    {
                        std::cout << "Found 1 record" << std::endl;
                        ret.push_back(recordOffset);
                    }
                }
            }
        }
    }
    return ret;
}

std::vector<uint64_t> Table::findBinaryRecordByTableScan(const ASTExpression * expression)
{
    SuffixExpression suffixExpression(expression);
    //TODO : Implement bitmap lookup
    //std::cout << fixedAllocator << std::endl;
    std::vector<uint64_t> ret;
    SuffixExpression *suffixExp = new SuffixExpression(expression);
    for (int i = 0; i < fixedAllocator->blocksPerMacroBlock; i++)
    {
        auto status = fixedAllocator->getBlockStatus(0, i);
        if (status == RecordAllocator::BlockStatus::UNUSED ||
            status == RecordAllocator::BlockStatus::RESERVED) continue;
        for (int j = 0; j < fixedAllocator->recordsPerBlock; j++)
        {
            auto blockOffset = fixedAllocator->getBlockOffset(0, i);
            if (!fixedAllocator->blockIsRecordUsed(blockOffset, j)) continue;
            uint64_t recordAddress = fixedAllocator->recordGetBlockOffset(blockOffset, j);
            if (expression == nullptr)
            {
                ret.push_back(recordAddress);
                continue;
            }
            std::map<std::string, SQLValue> values;
            for (auto& i : suffixExp->requiredContex)
            {
                auto& field = fieldList->getField(i);
                char* buffer = new char[field.fieldType->getConstantLength()];
                fixedStorageArea->getDataAt(recordAddress + field.fieldOffset,
                    buffer, field.fieldType->getConstantLength());
                values[field.fieldName] = field.fieldType->dataValue(buffer);
                delete buffer;
            }
            auto result = suffixExp->evaluate(values);
            //auto buffer = new char[fieldToUpdateType.fieldType->getConstantLength()];
            if (result.type == SQLValue::BOOLEAN && result.boolValue == true)
            {
                ret.push_back(recordAddress);
            }
        }
    }
    return ret;
}

boost::filesystem::path Table::getFixedStoragePath()
{
    return fixedStoragePath;
}

boost::filesystem::path Table::getVariableStoragePath()
{
    return variableStoragePath;
}

bool Table::initialize(ASTCreateTableStmtNode *astNode)
{
    fixedStorageArea = new CachedStorageArea(fixedStoragePath.string(), 4 * 1024 * 1024, 8192);
    dynamicStorageArea = new CachedStorageArea(variableStoragePath.string(), 4 * 1024 * 1024, 8192);
    dynamicAllocator = new BuddyDynamicAllocator(dynamicStorageArea);//, 1024 * 1024 * 1024, 128);
    dynamicAllocator->initialize();
    dynamicStorageArea->flush();
    std::list<ASTCreateTableFieldNode*> fieldNodes;
    std::vector<ASTFieldConstraintNode*> fieldConstraints;
    for (auto i : astNode->fields)
    {
        if (i->getType() == ASTNodeBase::NodeType::CREATE_TABLE_FIELD)
        {
            fieldNodes.push_back((ASTCreateTableFieldNode*)i);
            for (auto j : ((ASTCreateTableFieldNode*)i)->constraints)
            {
                j->tableName = ((ASTCreateTableFieldNode*)i)->name;
                fieldConstraints.push_back(j);
            }
        }
        else
        {
            assert(i->getType() == ASTNodeBase::NodeType::FIELD_CONSTRAINT);
            fieldConstraints.push_back((ASTFieldConstraintNode*)i);
        }
    }
    for (auto constraintNode : fieldConstraints)
    {
        Constraint::createFromASTNode(constraintNode);
    }
    fieldList = FieldList::fromASTNode(fieldNodes, dynamicAllocator);
    uint32_t bytesReserved = fieldList->getHeaderSize() + sizeof(uint32_t);
    bytesReserved = (bytesReserved / 4096 + 1) * 4096;
    fixedStorageArea->setDataAt(0, (char*)&bytesReserved, sizeof(uint32_t));
    fixedStorageArea->setDataAt(sizeof(uint32_t), fieldList->getHeaderData(), fieldList->getHeaderSize());
    fixedAllocator = new RecordAllocator(fixedStorageArea, fieldList->getRecordFixedSize());
    fixedAllocator->initialize();
    fixedStorageArea->flush();
    delete fixedAllocator;
    delete fixedStorageArea;
    delete dynamicAllocator;
    delete dynamicStorageArea;
    //TODO: initialize fixed storage area!
    //TODO: error handling.
    return true;
}

bool Table::drop()
{
    if (isOpened)
    {
        close();
    }
    return
        boost::filesystem::remove(fixedStoragePath) &&
        boost::filesystem::remove(variableStoragePath);
}

bool Table::open()
{
    if (isOpened)
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Opening an table that is already opened. " << fixedStoragePath;
        return false;
    }
    isOpened = true;
    uint32_t bytesReserved;
    //Initialize the fixed storage area.
    fixedStorageArea = new CachedStorageArea(fixedStoragePath.string(), 4 * 1024 * 1024, 8192);
    BOOST_LOG_TRIVIAL(debug) <<
        "Loading header info from fixed storage " << fixedStoragePath;

    //Parse the header information.
    uint32_t headerPageBytes;
    fixedStorageArea->getDataAt(0, (char*)&headerPageBytes, sizeof(headerPageBytes));
    BOOST_LOG_TRIVIAL(debug) << "Header info uses up to " << headerPageBytes << " bytes";
    char *buffer = new char[headerPageBytes];
    fixedStorageArea->getDataAt(0, buffer, headerPageBytes);
    dynamicStorageArea = new CachedStorageArea(variableStoragePath.string(), 4 * 1024 * 1024, 8192);
    dynamicAllocator = new BuddyDynamicAllocator(dynamicStorageArea);//, 1024 * 1024 * 1024, 128);
    dynamicAllocator->open();
    fieldList = FieldList::fromBuffer(buffer + sizeof(headerPageBytes), dynamicAllocator);
    delete[] buffer;
    BOOST_LOG_TRIVIAL(debug) << "Fixed part of each record is " << fieldList->getRecordFixedSize() << " bytes";

    fixedAllocator = new RecordAllocator(fixedStorageArea, fieldList->getRecordFixedSize());
    fixedStorageArea->getDataAt(0, (char*)&bytesReserved, sizeof(uint32_t));
    //TDOO: dynamci storage area.
    //TODO: error handling.
    return true;
}

bool Table::getIsOpened()
{
    return isOpened;
}

bool Table::close()
{
	fixedStorageArea->flush();
	dynamicAllocator->flush();
	dynamicStorageArea->flush();
    delete fixedAllocator;
    delete dynamicStorageArea;
    delete fixedStorageArea;
    //TODO: error handling.
    return true;
}

void Table::updateAssociatedIndexes()
{
    for (auto pair : database->getAllIndexes())
    {
        auto index = pair.second;
        if (index->tableName != this->tableName) continue;
        associatedIndexes.push_back(index);
    }
}

bool Table::addRecord(std::list<ASTSQLDataValue*> astFields)
{
    auto recordFixedPartSize = fieldList->getRecordFixedSize();
    auto fields = fieldList->getCompiledFields();
    auto fieldsCount = fields.size();
    auto buffer = new char[recordFixedPartSize];
    memset(buffer, 0, fieldList->nullBitmapSize);
    auto astListEntry = astFields.begin();
    for (int i = 0; i < fieldsCount; i++)
    {
        if ((*astListEntry)->dataType == ASTSQLDataValue::TYPE_NULL)
        {
            int byte = i / 8;
            int bit = i % 8;
            buffer[byte] |= 1 << bit;
            astListEntry++;
            continue;
        }
        auto field = fields[i].fieldType;
        auto temp = field->parseASTNode(*astListEntry, buffer + fields[i].fieldOffset);
        if (temp == -1)
        {
            delete[] buffer;
            return false;
        }
        astListEntry++;
    }
    return addBinaryRecord(buffer);
}

bool Table::deleteRecord(ASTExpression *expression)
{
    auto recordOffsets = findRecord(expression);
    for (auto address : recordOffsets)
    {
        fixedAllocator->free((*fixedStorageArea)[address]);
        for (auto index : associatedIndexes)
        {
            index->onDeleteRecord(address, nullptr);
        }
    }
    fixedStorageArea->flush();
    return true;
}

std::vector<uint64_t> Table::findRecord(const ASTExpression const *expression)
{
    std::vector<uint64_t> empty;
    /*if (expression != nullptr && expression->op != ASTExpression::EQUAL)
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Currently only supported condition is equal.";
        return empty;
    }
    if (expression != nullptr &&
        (expression->left->op != ASTExpression::NONE_COLUMN_NAME ||
        expression->right->op != ASTExpression::NONE_CONSTANT)
    ) {
        BOOST_LOG_TRIVIAL(error) <<
            "Currently only supported condition is columnName = value.";
        return empty;
    }*/
    /*std::string columnName = expression->left->identifier->name;
    ASTSQLDataValue *dataValue = expression->right->constant;
    bool columnFound = false;
    auto fields = fieldList->getCompiledFields();
    FieldList::CompiledField* field = nullptr;
    for (int i = 0; i < fields.size(); i++)
    {
        FieldList::CompiledField& f = fields[i];
        if (f.fieldName == columnName)
        {
            field = &f;
        }
    }
    std::cout << "Offset is: " << field->fieldOffset << std::endl;

    char *buffer = new char[field->fieldType->getConstantLength()];
    field->fieldType->parseASTNode(dataValue, buffer);*/
    auto ret = findBinaryRecordByTableScan(expression);
    //auto ret = findBinaryRecordInRange(field->fieldType, field->fieldOffset, buffer, buffer, 0);
    char* recordBuffer = new char[fieldList->getRecordFixedSize()];
    std::vector<int> printIndexes;

    for (int i = 0; i < fieldList->getCompiledFields().size(); i++)
    {
        if (i != 0) std::cout << "|";
        std::cout << fieldList->getCompiledFields()[i].fieldName;
        printIndexes.push_back(i);
    }
    std::cout << std::endl;
    for (auto recordOffset : ret)
    {
        fixedStorageArea->getDataAt(recordOffset, recordBuffer, fieldList->getRecordFixedSize());
        fieldList->printRecord(recordBuffer, printIndexes);
    }
    delete[] recordBuffer;
    //delete[] buffer;
    //Need to be rewritten
    /*char *buffer = new char[fieldList->getRecordFixedSize()];
    for (int index = 0; ; index++)
    {
        fixedStorageArea->getDataAt(index * fieldList->getRecordFixedSize(), buffer, fieldList->getRecordFixedSize());
        if (fieldList->getCompiledFields()[0].fieldType->compare(buffer, value) == 0)
            return index;
    }*/
    return ret;
}

bool Table::updateRecord(const std::string& fieldName,
    const ASTExpression const* expression,
    const ASTExpression const* whereExpression)
{
    auto resultSet = findRecord(whereExpression);
    SuffixExpression *suffixExp = new SuffixExpression(expression);
    auto fieldToUpdateType = fieldList->getField(fieldName);
    for (uint64_t recordAddress : resultSet)
    {
        std::map<std::string, SQLValue> values;
        for (auto& i : suffixExp->requiredContex)
        {
            auto& field = fieldList->getField(i);
            char* buffer = new char[field.fieldType->getConstantLength()];
            fixedStorageArea->getDataAt(recordAddress + field.fieldOffset,
                buffer, field.fieldType->getConstantLength());
            values[field.fieldName] = field.fieldType->dataValue(buffer);
            delete buffer;
        }
        auto result = suffixExp->evaluate(values);
        auto buffer = new char[fieldToUpdateType.fieldType->getConstantLength()];
        fieldToUpdateType.fieldType->parseSQLValue(result, buffer);
        fixedStorageArea->setDataAt(recordAddress + fieldToUpdateType.fieldOffset,
            buffer, fieldToUpdateType.fieldType->getConstantLength());
    }
    //fixedStorageArea->flush();
    return true;
}
