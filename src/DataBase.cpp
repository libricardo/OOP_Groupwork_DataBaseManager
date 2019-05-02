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

void DataBase::CreateTable(const std::string &command) {
	using namespace std;
	vector<string> param, not_null;
	vector<pair<string, int> > _attrType;
	string pri_key = "\0";
	auto res = ParamSpliter::split_create_table(command, param, not_null, pri_key);
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

void DataBase::DropTable(const std::string &Name) {
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

void DataBase::ShowTableCol(const std::string &Name) {
	try {
		auto it = mTable.find(Name);
		if (it != mTable.end()) {

			mTable[Name]->PrintAttributeTable();

		} else
			throw(false);
	}
	catch (bool) {
		// show table error
	}
}

void DataBase::ShowTableAll() { // temporarily function
	std::cout << "Tables_in_" << __name << std::endl;
	for (auto it: mTable) {
		std::cout << it.first << std::endl;
	}
}

void DataBase::InsertData(const std::vector<std::string> &param)
{
	//using namespace stralgo;
	if (mTable.count(param[0]) && param.size() % 2 == 1)
	{
		auto _table = mTable[ param[0] ];
		std::vector<ATTRIBUTE> _attributes;
		int n = param.size() / 2;
		for (int i = 1; i <= n; i ++)
		{
			auto _attr_type = _table->GetTypeof(param[i]) ; // pay attention to the legality
			Value *pt = NULL;
			switch (_attr_type)
			{
				case INT :
				{
					int val = 0;
					if (stralgo::str2int(param[i + n], val))
						pt = new dataInt(val);
					break;
				}
				case DOUBLE :
				{
					double val = 0;
					if (stralgo::str2double(param[i + n], val))
						pt = new dataDouble(val);
					break;
				}
				case STRING :
				{
					pt = new dataString(param[i + n]);
					break;
				}
			}
			if (pt != NULL)
			{
				_attributes.push_back( ATTRIBUTE(param[i], pt) );
			}
		}
		_table->Insert(_attributes);
	}
	else
	{
		// show insert error
	}
}

void DataBase::SelectData(const std::vector<std::string> &param)
{
	using namespace std;
	string _attrName = param[0];
	string _tableName = param[1];

	DataTable* _table = mTable[_tableName];
	vector<Data*> _dataList;
	vector< pair<string, vector<Value*> > > _attrList; // ? static ?
	_attrList.clear();

	if (param.size() == 3)
		_table->GetDataWhere(param[2], _dataList);
	else
		_table->GetDataWhere("", _dataList);

	if (_attrName == "*")
	{
		auto &_attrTable = _table->GetAttributeTable();
		for (auto _attr: _attrTable)
		{
			_attrName = _attr.first;
			_attrList.resize(_attrList.size() + 1);
			_attrList.back().first = _attrName;
			_table->Select(_attrName, _dataList, _attrList.back().second);
		}
	}
	else
	{
		_attrList.resize(1);
		_attrList[0].first = _attrName;
		_table->Select(_attrName, _dataList, _attrList[0].second);
	}

	int n = _attrList[0].second.size();
	if (!n)
		return;

	for (int i = 0; i < _attrList.size(); i ++)
		cout << _attrList[i].first << "\t";
	//cout << _attrList.back().first << endl;
	cout << endl;

	for (int i = 0; i < n; i ++)
	{
		for (int j = 0; j < _attrList.size(); j ++)
			cout << *(_attrList[j].second[i]) << "\t";
		//cout << (_attrList.back().second[i]) << endl;
		cout << endl;
	}
}

void DataBase::UpdateData(const std::vector<std::string> &param)
{
	// only set one attribute
	using namespace std;
	//using namespace stralgo;
	string _tableName = param[0];
	string _attrName  = param[1];
	string _attrVStr  = param[2];
	DataTable *_table = mTable[_tableName];
	Value *_attrVal = NULL;
	switch (_table->GetTypeof(_attrName))
	{
		case INT:
		{
			int val;
			if (stralgo::str2int(_attrVStr, val))
				_attrVal = new dataInt(val);
			break;
		}
		case DOUBLE:
		{
			double val;
			if (stralgo::str2double(_attrVStr, val))
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

	static vector<Data*> _dataList;
	if (param.size() == 4)
		_table->GetDataWhere(param[3], _dataList);
	else
		_table->GetDataWhere("", _dataList);

	_table->Update(_attribute, _dataList);
}

void DataBase::DeleteData(const std::vector<std::string> &param)
{
	using namespace std;

	string _tableName = param[0];
	DataTable *_table = mTable[_tableName];

	static vector<Data*> _dataList;
	if (param.size() == 2)
		_table->GetDataWhere(param[1], _dataList);
	else
		_table->GetDataWhere("", _dataList);

	_table->Remove(_dataList);
}