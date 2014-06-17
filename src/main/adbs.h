
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

class Stats{
public:
	Stats(){ };
	~Stats(){ };
	virtual void createColumnStatistics(Table* table, int column) = 0;
	virtual unsigned int getNDVforColumn(int column) = 0;
};

/**
 * DB-Statistics implemented as Equi-Width-Histogram
 */
class EquiWidthStat : public Stats{
public:
	EquiWidthStat(Table* table);	
	~EquiWidthStat();	
	void setWidth(int width, int column);
	unsigned int getNDVforColumn(int column);
	int getCountForValue(int column, int value);
	void incrementWidthCount(int column, int value);
	void createColumnStatistics(Table* table, int column);
	
protected:
	map<int, unsigned int>* widthCountForColumns;
	int* width;
};



/**
 * Table object returned by Database::createTable()
 */
class Table {
public:
	Table(string name, int columnCount);

	/**
	 * get name of table
	 */
	string getName() { return name; };
	
	/**
	 * get columnCount of table
	 */
	int getColumnCount() { return columnCount; };
	
	bool insert(int* tuple);
	
	int* get(int position) { return data.at(position); };
	
	int getNumberOfEntries(){ return data.size(); };
	
	int getNDVofColumn(int column);
	
	void createStatisticsForColumn(int column) { this->columnStats->createColumnStatistics(this, column); };
	
	int getGreatestColumnValue(int column) { return this->greatestColumnValue[column]; };
	
	int getSmallestColumnValue(int column) { return this->smallestColumnValue[column]; };
	
	vector<int*>::iterator getDataBegin(){ return this->data.begin(); };
	vector<int*>::iterator getDataEnd(){ return this->data.end(); };
	
	virtual ~Table() {};

protected:
	string name;
	vector<int*> data;
	int columnCount;
	Stats* columnStats;
	
	int* greatestColumnValue;
	int* smallestColumnValue;
};


class PrecedenceGraphNode;


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

class PrecedenceGraphNode {
public:
	PrecedenceGraphNode() {
		this->representedNode = NULL;
		this->parent = NULL;
	 };

	/**
	 * get vector of tables involved in the query plan
	 */
	float getT() { return t; };

	/**
	 * get vector of tables involved in the query plan
	 */
	float getCost() { return c; };

	/**
	 * get vector of tables involved in the query plan
	 */
	float getRank() { return r; };

	void addTable(Table* t){ tables.push_back(t); };

	void importTables(vector<Table*> importTables){ tables = importTables; };

	/**
	 * get joins involved in this query plan (undirected)
	 */
	vector<PrecedenceGraphNode> getChildren() { return children; };

	/**
	 * get joins involved in this query plan (undirected)
	 */
	void importChildren(vector<PrecedenceGraphNode> importChildren) { children=importChildren; };

	/**
	 * get joins queryGraphToPrecedenceGraphinvolved in this query plan (undirected)
	 */
	vector<Table*> getTables() { return tables; };
	
	/**
	 * get join columns in this query plan (undirected)
	 */
	PrecedenceGraphNode* getParent() { return parent; };

	/**
	 * get join columns in this query plan (undirected)
	 */
	void setParent(PrecedenceGraphNode* node) { parent = node; };
	void removeParent(){ parent = NULL;};
	/**
	 * add a join to the query plan
	 */
	void addChild(PrecedenceGraphNode child) {
		children.push_back(child);
	};
	/**
	 * add a join to the query plan
	 */
	void clearChildren() {
		children.clear();
	};
	
	void removeChild(PrecedenceGraphNode child) {
		for(vector<PrecedenceGraphNode>::iterator it = children.begin(); it != children.end(); ++it){
			if((*it).getTables().front()->getName() == child.getTables().front()->getName())
				children.erase(it);
		}
	};

	void setRepresentedNode(PrecedenceGraphNode* node){ representedNode = node; };
	PrecedenceGraphNode* getRepresentedNode() { return representedNode; };

	virtual ~PrecedenceGraphNode() {};

protected:
	float t;
	float c;
	float r;
	vector<PrecedenceGraphNode> children;
	PrecedenceGraphNode* representedNode;
	PrecedenceGraphNode* parent;
	vector<Table*> tables;
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
	vector<Table*> getOptimalLDT(PrecedenceGraphNode* root);
	PrecedenceGraphNode queryGraphToPrecedenceGraph(vector<Table*>* tables, vector<pair<Table*, Table*> >* joins , Table* table);
	bool isNotChain(PrecedenceGraphNode* root);
	PrecedenceGraphNode* getSubtreeRoot(PrecedenceGraphNode* currentRoot, PrecedenceGraphNode* currentNode);
	void normalize(PrecedenceGraphNode* node);
	vector<Table*> denormalize(PrecedenceGraphNode* node);
	bool sortDescendingByRank(PrecedenceGraphNode i, PrecedenceGraphNode j) { return (j.getRank()<i.getRank()); };
	virtual ~Database();
};



#endif /* ADBS_H_ */
