
#ifndef ADBS_H_
#define ADBS_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <math.h>  

using namespace std;

class Table;

class Statistics{
public:
	Statistics(Table *table){};
	virtual void createColumnStatistics(Table* table, int column);
	virtual int getNDVforColumn(int column);
};

class EquiWidthStat : Statistics{
public:
	void setWidth(int width, int column){
		this->width[column] = width;
		this->widthCountForColumns[column].clear();
//		this->widthCountForColumns[column].resize(this->width[column]);
		
	}

	int getNDVforColumn(int column){
//		return this->widthCountForColumns[column].at(floor(value/this->width[column]));
		unsigned int sum = 0;
		
		for(map<int, unsigned int>::iterator it = this->widthCountForColumns[column].begin(); it != this->widthCountForColumns[column].end(); it++){
			sum += ceil((*it).second/this->width[column]);
		}
		return sum;
	}
	
	int getCountForValue(int column, int value){
		return (this->widthCountForColumns[column].find(value) == this->widthCountForColumns[column].end()) ? 0 : this->widthCountForColumns[column].at(floor(value/this->width[column]));
	}
	
	void incrementWidthCount(int column, int value){
		if(this->widthCountForColumns[column].find(floor(value/this->width[column])) == this->widthCountForColumns[column].end()){
			this->widthCountForColumns[column].insert(pair<int, unsigned int>(floor(value/this->width[column]), 1));
		}else{
			this->widthCountForColumns[column].at(floor(value/this->width[column])) = (this->widthCountForColumns[column].at(floor(value/this->width[column]))+1);
		}
	}
	
	void createColumnStatistics(Table* table, int column){
		int width = ceil(table->getNumberOfEntries()/this->partitions);
		this->setWidth(width, column);
		
		for(vector<int*>::iterator it = table->getDataBegin(); it != table->getDataEnd(); it++){
			this->incrementWidthCount(column, (*it)[column]);
		}
	}
	
	
	
	EquiWidthStat(Table* table) : Statistics(table){
		this->widthCountForColumns = ( map<int, unsigned int>* )malloc( sizeof(map<int, unsigned int>) * table->getColumnCount());
		this->width = (int*)malloc( sizeof(int) * table->getColumnCount());
		
	}
	
protected:
//	vector<int>* widthCountForColumns;
	map<int, unsigned int>* widthCountForColumns;
	int* width;
	const int partitions = 1000;

};
/**
 * Table object returned by Database::createTable()
 */
class Table {
public:
	Table(string name, int columnCount) : name(name), columnCount(columnCount) { 
		this->columnStats = new EquiWidthStat(this);
	}
	
	

	/**
	 * get name of table
	 */
	string getName() { return name; }
	
	/**
	 * get columnCount of table
	 */
	int getColumnCount() { return columnCount; }
	
	bool insert(int* tuple) { 
		int *d = ( int* )malloc(sizeof(int) * columnCount);
		for(int i=0; i<columnCount; i++){
			d[i]=tuple[i];
		} 
		data.push_back(d); 
		return true;
	}
	
	int* get(int position) { 
		return data.at(position); 
	}
	
	int getNumberOfEntries(){ return data.size(); }
	
	int getNumberDifferentValuesofColumn(int column){ 
		if(this->columnStats != NULL){
			return this->columnStats->getNDVforColumn(column);
		}else{
			return 0;
		}
	}
	
	vector<int*>::iterator getDataBegin(){ return this->data.begin(); }
	vector<int*>::iterator getDataEnd(){ return this->data.end(); }
	
	virtual ~Table() {};

protected:
	string name;
	vector<int*> data;
	int columnCount;
	Statistics* columnStats;
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
