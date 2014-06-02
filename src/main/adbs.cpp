
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
	
	
//	result = get best from results
	
	return result;
}


vector<Table*> Database::getOptimalLDT(QueryGraph* queryGraph, Table* table) {
	
}


Database::~Database() {

}


Table::Table(string name, int columnCount) : name(name), columnCount(columnCount) { 
	this->columnStats = new EquiWidthStat(this);
}
bool Table::insert(int* tuple) { 
	int *d = ( int* )malloc(sizeof(int) * columnCount);
	for(int i=0; i<columnCount; i++){
		d[i]=tuple[i];
	} 
	data.push_back(d); 
	return true;
}

int Table::getNumberDifferentValuesofColumn(int column){ 
	if(this->columnStats != NULL){
		return this->columnStats->getNDVforColumn(column);
	}else{
		return 0;
	}
}





EquiWidthStat::EquiWidthStat(Table* table) : Stats::Stats() {
	this->widthCountForColumns = ( map<int, unsigned int>* )malloc( sizeof(map<int, unsigned int>) * table->getColumnCount());
	this->width = (int*)malloc( sizeof(int) * table->getColumnCount());
	
}

EquiWidthStat::~EquiWidthStat() {

}

void EquiWidthStat::setWidth(int width, int column){
		this->width[column] = width;
		this->widthCountForColumns[column].clear();		
}

unsigned int EquiWidthStat::getNDVforColumn(int column){
	unsigned int sum = 0;
	
	for(map<int, unsigned int>::iterator it = this->widthCountForColumns[column].begin(); it != this->widthCountForColumns[column].end(); it++){
		sum += ceil((*it).second/this->width[column]);
	}
	return sum;
}

int EquiWidthStat::getCountForValue(int column, int value){
	return (this->widthCountForColumns[column].find(value) == this->widthCountForColumns[column].end()) ? 0 : this->widthCountForColumns[column].at(floor(value/this->width[column]));
}

void EquiWidthStat::incrementWidthCount(int column, int value){
	if(this->widthCountForColumns[column].find(floor(value/this->width[column])) == this->widthCountForColumns[column].end()){
		this->widthCountForColumns[column].insert(pair<int, unsigned int>(floor(value/this->width[column]), 1));
	}else{
		this->widthCountForColumns[column].at(floor(value/this->width[column])) = (this->widthCountForColumns[column].at(floor(value/this->width[column]))+1);
	}
}

void EquiWidthStat::createColumnStatistics(Table* table, int column){
	int width = ceil(table->getNumberOfEntries()/this->partitions);
	this->setWidth(width, column);
	
	for(vector<int*>::iterator it = table->getDataBegin(); it != table->getDataEnd(); it++){
		this->incrementWidthCount(column, (*it)[column]);
	}
}



