#include "databaseext.h"

QueryResult DatabaseExt::selectAllFrom(const std::string &tableName, const Expr &expr, const std::string* file_name, const std::vector<std::string>& group_by_column)
{
    auto queryResult = dynamic_cast<QueryResultSelect*>(Database::selectAllFrom(tableName, expr).result());
    assert(queryResult != nullptr);
	return QueryResult(new QueryResultSelectInto(*queryResult, file_name, group_by_column));
}

QueryResult DatabaseExt::selectFrom(const std::string &tableName, const std::vector<std::string> &keyNames, const Expr &expr, const std::string* file_name, const std::vector<std::string>& group_by_column)
{
    auto queryResult = dynamic_cast<QueryResultSelect*>(Database::selectFrom(tableName, keyNames, expr).result());
    assert(queryResult != nullptr);
    return QueryResult(new QueryResultSelectInto(*queryResult, file_name, group_by_column));
}

void DatabaseExt::load(const std::string &tableName, const std::vector< std::map<std::string, Variant> > &entries)
{
    assertDatabaseSelected();
    assertTableExist(tableName);

    for (size_t i = 0; i < entries.size(); i++)
    {
        _tables[tableName].insertInto(entries[i]);
    }
}