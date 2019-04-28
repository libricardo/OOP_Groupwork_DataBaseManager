#include "DataBase.h"
#include <assert.h>

DataBase::DataBase(const std::string &DBName): __name(DBName)
{

}

DataBase::~DataBase()
{
	for (auto it: mTable)
		delete it.second;
}

void DataBase::createTable(const std::string &command) {
	using namespace std;
	vector<string> param, not_null;
	vector<pair<string, int> > _attrType;
	string pri_key = "\0";
	auto res = ParamSpliter::split_createTable(command, param, not_null, pri_key);
	assert(res == TABLE_CREATE); // !!!

	for (int i = 1; i < param.size(); i += 2)
	{
		pair<string, int> _attr;
		_attr.first = param[i];
		_attr.second = attrTypeMap.at(param[i + 1]);
		_attrType.push_back(_attr);
	}
	mTable[param[0]] = new DataTable(param[0], _attrType, pri_key, not_null);
}

void DataBase::dropTable(const std::string &Name) {
	try {
		auto it = mTable.find(Name);
		if (it != mTable.end()) {
			delete it->second;
			mTable.erase(it);
		} else
			throw false;
	}
	catch (bool) {
		// deletion error
	}
}

void DataBase::showTable(const std::string &Name) {
	try {
		auto it = mTable.find(Name);
		if (it != mTable.end()) {

			auto &_attrTable = mTable[Name]->getAttrTable();
			for (auto e: _attrTable)
			{
				std::cout << e.first << std::endl;
			}

		} else
			throw(false);
	}
	catch (bool) {
		// show table error
	}
}

void DataBase::showTableAll() { // temporarily function
	for (auto it: mTable) {
		std::cout << it.first << std::endl;
	}
}

void DataBase::insertData(const std::vector<std::string> &param)
{
	//using namespace Params;
	if (mTable.count(param[0]) || param.size() % 2 == 1)
	{
		auto _table = mTable[ param[0] ];
		std::vector<ATTRIBUTE> _attributes;
		for (int i = 1; i < param.size(); i += 2)
		{
			auto _attr_type = _table->getTypeof(param[i]) ; // pay attention to the legality
			Base *pt = NULL;
			switch (_attr_type)
			{
				case INT :
				{
					int val = 0;
					if (Params::str2int(param[i + 1], val))
						pt = new dataInt(val);
					break;
				}
				case DOUBLE :
				{
					double val = 0;
					if (Params::str2double(param[i + 1], val))
						pt = new dataDouble(val);
					break;
				}
				case STRING :
				{
					pt = new dataString(param[i + 1]);
					break;
				}
			}
			if (pt != NULL)
			{
				_attributes.push_back( ATTRIBUTE(param[i], pt) );
			}
		}
		_table->insert(_attributes);
	}
	else
	{
		// show insert error
	}
}

void DataBase::selectData(const std::vector<std::string> &param)
{
	using namespace std;
	string _attrName = param[0];
	string _tableName = param[1];

	DataTable* _table = mTable[_tableName];
	list<Data*> _dataList;
	vector<Base*> _attrList;

	if (param.size() == 2)
		_table->getDataWhere(param[2], _dataList);
	else
		_table->getDataWhere("", _dataList);

	_table->select(_attrName, _attrList, _dataList);
	// ?? what to do next ?? //
	for (auto it: _attrList)
	{
		cout << it->getTypename() << endl;
	}
}

void DataBase::updateData(const std::vector<std::string> &param)
{
	// only set one attribute
	using namespace std;
	//using namespace Params;
	string _tableName = param[0];
	string _attrName  = param[1];
	string _attrVStr  = param[2];
	DataTable *_table = mTable[_tableName];
	Base *_attrVal = NULL;
	switch (_table->getTypeof(_attrName))
	{
		case INT:
		{
			int val;
			if (Params::str2int(_attrVStr, val))
				_attrVal = new dataInt(val);
			break;
		}
		case DOUBLE:
		{
			double val;
			if (Params::str2double(_attrVStr, val))
				_attrVal = new dataDouble(val);
			break;
		}
		case STRING:
		{
			_attrVal = new dataString(_attrVStr);
			break;
		}
		default:
		{
			// throw an error, wrong type
			break;
		}
	}
	ATTRIBUTE _attribute = ATTRIBUTE(_attrName, _attrVal);

	static list<Data*> _dataList;
	if (param.size() == 4)
		_table->getDataWhere(param[3], _dataList);
	else
		_table->getDataWhere("", _dataList);

	_table->update(_attribute, _dataList);
}

void DataBase::deleteData(const std::vector<std::string> &param)
{
	using namespace std;

	string _tableName = param[0];
	DataTable *_table = mTable[_tableName];

	static list<Data*> _dataList;
	if (param.size() == 2)
		_table->getDataWhere(param[1], _dataList);
	else
		_table->getDataWhere("", _dataList);

	_table->remove(_dataList);
}