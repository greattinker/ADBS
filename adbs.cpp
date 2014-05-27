
#include "adbs.h"

Database::Database() {

}

Table* Database::createTable(string name, int columnCount) {
	return new Table(name, columnCount);
}

bool Database::insert(Table* table, int* tuple) {
	table->insert(tuple);
	
	return true;
}

vector<Table*> Database::getJoinOrder(QueryGraph* queryGraph) {
	vector<Table*> result;
	
	vector<Table*> tables = queryGraph->getTables();
	vector<pair<Table*, Table*> > joins = queryGraph->getJoins();
	vector<pair<int, int> > joinColumns = queryGraph->getJoinColumns();
	
	vector<Table*> *results = ( vector<Table*>* )malloc( sizeof( vector<Table*> ) * tables.size() );
	
//	testing all possible root relations
	int i = 0;
	for(vector<Table*>::iterator it = tables.begin(); it != tables.end(); ++it){
		results[i] = getOptimalLDT(queryGraph, (*it));
	}
	
	
//	result = get best from results
	
	return result;
}


vector<Table*> Database::getOptimalLDT(QueryGraph* queryGraph, Table* table) {
	
}


Database::~Database() {

}

