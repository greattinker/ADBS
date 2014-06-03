
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
//	int i = 0;
//	for(vector<Table*>::iterator it = tables.begin(); it != tables.end(); ++it){
//		results[i] = getOptimalLDT(queryGraph, (*it));
//	}

	for(vector<Table*>::iterator it = tables.begin(); it != tables.end(); ++it){
		queryGraphToPrecedenceGraph(joins , (*it));
		//results[i] = getOptimalLDT(queryGraph, (*it));
	}
	
	
//	result = get best from results
	
	return result;
}


vector<Table*> Database::getOptimalLDT(QueryGraph* queryGraph, Table* table) {
	
}


PrecedenceGraphNode Database::queryGraphToPrecedenceGraph(vector<pair<Table*, Table*> > joins , Table* table){
	PrecedenceGraphNode node(table);
	for(vector<pair<Table*, Table*> >::iterator it = joins.begin(); it != joins.end(); ++it){
		if((*it).first == table){
			PrecedenceGraphNode child = queryGraphToPrecedenceGraph(joins , (*it).first);
			node.addChild(&child);
			child.setParent(&node);
		}
		else if((*it).second == table){
			PrecedenceGraphNode child = queryGraphToPrecedenceGraph(joins , (*it).second);
			node.addChild(&child);
			child.setParent(&node);
		}
	}	

	return node;
}

Database::~Database() {

}

