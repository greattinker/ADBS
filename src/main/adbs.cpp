
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
	vector<pair<int, int> > joinColumns = queryGraph->getJoinColumns();
//	PrecedenceGraph precedenceGraph;
	
	vector<pair<int, vector<Table*> > > R;
	
	vector<Table*> *results = ( vector<Table*>* )malloc( sizeof( vector<Table*> ) * tables.size() );
	
//	testing all possible root relations
	int i = 0;
	for(vector<Table*>::iterator it = tables.begin(); it != tables.end(); ++it){
//		construct precedence graph rooted at node i
/*		tables.erase(it);
		precedenceGraph = queryGraphToPrecedenceGraph(tables, queryGraph->getJoins(), *it, precedenceGraph);

		R.push_back(getOptimalLDT(&precedenceGraph));
*/
//		results[i] = getOptimalLDT(queryGraph, (*it));
		vector<pair<Table*, Table*> > joins = queryGraph->getJoins();
		vector<Table*> tablelist = queryGraph->getTables();
		PrecedenceGraphNode root = queryGraphToPrecedenceGraph(&tablelist, &joins , (*it));
cout << "root is:"+(*it)->getName() << endl;
cout << root.getChildren().size() << endl;
cout << "!!!!!!!!!!!" << endl;
		getOptimalLDT(root);
	}
	
	
//	result = get best from results
//	for test geht the first
	//result = R.front().second;

	return result;
}

bool Database::isNotChain(PrecedenceGraphNode* root){
	vector<PrecedenceGraphNode> children = root->getChildren();
	for(vector<PrecedenceGraphNode>::iterator it = children.begin(); it != children.end(); ++it){
		if(!(*it).getChildren().empty())
			return true;
	}

	return false;
}
/**
* vorsicht die funktionen sind noch nicht ganz richtig im sinne des ikkbz algo.
* eigentlich müsste man einen knoten finden der chains als kinder hat, ich laufe hier
* bis zu den blättern durch, funktioniert auch (also hier noch nicht weil es nicht fertig implementiert ist)
* aber ist eigentlich nicht richtig, würde es nochmals "richtig" implementieren, aber das schaff ich heute nicht mehr ^^
*/
PrecedenceGraphNode* Database::getSubtreeRoot(PrecedenceGraphNode* node){
	if(node->getChildren().empty()) return node->getParent();

	vector<PrecedenceGraphNode> children = node->getChildren();
	for(vector<PrecedenceGraphNode>::iterator it = children.begin(); it != children.end(); ++it){
		return getSubtreeRoot(&(*it));
	}
}

PrecedenceGraphNode* Database::normalize(PrecedenceGraphNode* node){
	//replace node by a compund relation cnode
	//case one: root has more then 1 child
	if(node->getChildren().size()>1){
		//do nice stuff :)
	}
	else{
		//only 1 child, merge node and child
		PrecedenceGraphNode compoundNode;
		//test rank functions for ascending order
	}
}

vector<Table*> Database::getOptimalLDT(PrecedenceGraphNode root) {
	while(isNotChain(&root)){
		vector<PrecedenceGraphNode> children = root.getChildren();
		for(vector<PrecedenceGraphNode>::iterator it = children.begin(); it != children.end(); ++it){
			if(!(*it).getChildren().empty()){
				PrecedenceGraphNode* subtreeRoot = getSubtreeRoot(&(*it));
				normalize(subtreeRoot);
			}
		}
		
		//merge chains in ascending order
	}

	//denormalize

	return root.getTables();
}


PrecedenceGraphNode Database::queryGraphToPrecedenceGraph(vector<Table*>* tables, vector<pair<Table*, Table*> >* joins , Table* table){
	PrecedenceGraphNode node;
	node.setTable(table);

for(vector<Table*>::iterator it = tables->begin(); it != tables->end(); ++it){
	if((*it) == table){
		tables->erase(it);
		break;
	}
cout << (*it)->getName() << endl;
}

cout << "currend node is "+table->getName() << endl;


	for(vector<pair<Table*, Table*> >::iterator it = joins->begin(); it != joins->end(); ++it){
		
		if((*it).first == table){
			if(std::find(tables->begin(), tables->end(), (*it).second) != tables->end()){
				PrecedenceGraphNode child = queryGraphToPrecedenceGraph(tables, joins , (*it).second);
cout << "returned from "+child.getTables().front()->getName() << endl;
				node.addChild(child);
				child.setParent(&node);
			}
		}
		else if((*it).second == table){
			if(std::find(tables->begin(), tables->end(), (*it).first) != tables->end()){
				PrecedenceGraphNode child = queryGraphToPrecedenceGraph(tables, joins , (*it).first);
cout << "returned from "+child.getTables().front()->getName() << endl;
				node.addChild(child);
				child.setParent(&node);
			}
		}
	}	
cout << "over" << endl;
	return node;
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



