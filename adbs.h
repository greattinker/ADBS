
#ifndef ADBS_H_
#define ADBS_H_

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

/**
 * Table object returned by Database::createTable()
 */
class Table {
public:
	Table(string name, int columnCount) : name(name), columnCount(columnCount), data(0) { }

	/**
	 * get name of table
	 */
	string getName() { return name; }
	
	/**
	 * get columnCount of table
	 */
	int getColumnCount() { return columnCount; }
	
	bool insert(int* tuple) { 
		int d[columnCount];
		for(int i=0; i<columnCount; i++){
			d[i]=tuple[i];
		} 
		data.push_back(d); 
		return true;
	}
	
	int* get(int position) { return data.at(position); }
	
	int getNumberDifferentValuesofColumn(int column){ return 1; }
	
	virtual ~Table() {};

protected:
	string name;
	vector<int*> data;
	int columnCount;
};

class QueryGraph {
public:
	QueryGraph() {};

	/**
	 * get vector of tables involved in the query plan
	 */
	vector<Table*> getTables() { return tables; };

	/**
	 * get joins involved in this query plan (undirected)
	 */
	vector<pair<Table*, Table*> > getJoins() { return joins; };
	
	/**
	 * get join columns in this query plan (undirected)
	 */
	vector<pair<int, int> > getJoinColumns() { return joinColumns; };

	/**
	 * add a join to the query plan
	 */
	void addJoin(Table* table1, Table* table2, int column1, int column2) {
		joins.push_back(make_pair(table1, table2));
		joinColumns.push_back(make_pair(column1, column2));
		if (find(tables.begin(), tables.end(), table1) == tables.end()) {
			tables.push_back(table1);
		}
		if (find(tables.begin(), tables.end(), table2) == tables.end()) {
			tables.push_back(table2);
		}
	}
	virtual ~QueryGraph() {};

protected:
	vector<Table*> tables;
	vector<pair<Table*, Table*> > joins;
	vector<pair<int, int> > joinColumns;
};

class Database {
public:
	Database();

	/**
	 * create a new table in the database
	 * name: name of table
	 * columnCount: amount of integer columns for this table
	 *
	 * return the new table object
	 */
	Table* createTable(string name, int columnCount);

	/**
	 * insert a new tuple
	 */
	bool insert(Table* table, int* tuple);

	/**
	 * calculate the optimal join order for the query graph
	 */
	vector<Table*> getJoinOrder(QueryGraph* queryGraph);
	vector<Table*> getOptimalLDT(QueryGraph* queryGraph, Table* table);
	virtual ~Database();
};



#endif /* ADBS_H_ */
