
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
	for(vector<Table*>::iterator it = tables.begin(); it != tables.end(); ++it)
	{
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
		results[i] = this->getOptimalLDT(&root);
		
		i++;
	}
	
	
//	result = get best from results
//	for test geht the first
	//result = R.front().second;

	return results[1];
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
PrecedenceGraphNode* Database::getSubtreeRoot(PrecedenceGraphNode* currentRoot, PrecedenceGraphNode* currentNode){
	if(currentNode->getChildren().empty()) return currentRoot;

	vector<PrecedenceGraphNode> children = currentNode->getChildren();
	for(vector<PrecedenceGraphNode>::iterator it = children.begin(); it != children.end(); ++it)
	{
		PrecedenceGraphNode* currentRoot = (children.size() > 1)? getSubtreeRoot(currentRoot, &(*it)) : getSubtreeRoot(&(*it), &(*it));
		//if( currentRoot != node) return n;
	}

	return currentRoot;
}

void Database::normalize(PrecedenceGraphNode* node){
	//if isNotChain is false then node only has leafs as children
	//remember all childnodes of node are actually chains
	//consider the case that the subtree is a chain allready, which means subtreeroot only has 1 child
	cout << "\tnormalisiere " << node->getTables().front()->getName() << endl;
	//HIER!!! frag mich doch nicht was hier falsch läuft >< in Zeile 151 sagt er, ja ich habe nur 1 kind, aber hier sagt er, ich hab 2 kinder.. dabei zeigt der Zeiger doch wohl auf das gleiche objekt
//	cout << "\tchildren size" << node->getChildren().size() << endl;
	if(isNotChain(node))
	{		
		vector<PrecedenceGraphNode> children = node->getChildren();
		node->clearChildren();
		for(vector<PrecedenceGraphNode>::iterator it = children.begin(); it != children.end(); ++it)
		{
			if((*it).getChildren().size() > 1)
			{	
				//if it's no leaf, then create a compoundNode and add it to the nodes children		
				PrecedenceGraphNode compoundNode;
				PrecedenceGraphNode child = node->getChildren().front();
				//for recursive rank computation, because the rank computation runs until a node has no parent
				child.removeParent();
						
				compoundNode.setRepresentedNode(&child);
				compoundNode.setParent(node);

				//compute rank of compundNode
				//compoundNode.computeRank();

				(*it).getParent()->addChild(compoundNode);
			}
			else{
				//child of node is a leaf, then just add it to the children of node again
				(*it).getParent()->addChild((*it));
			}
		}
	}
	else if(node->getChildren().size() == 1)
	{
		cout << "\tnormalisiere chain" << node->getTables().front()->getName() << endl;
		PrecedenceGraphNode compoundNode;
		compoundNode.setRepresentedNode(node);
		node->getParent()->clearChildren();
		node->getParent()->addChild(compoundNode);
	}
}

vector<Table*> Database::denormalize(PrecedenceGraphNode* node) 
{
	vector<Table*> result(0); 
	vector<Table*> parentTables(0);
	
//	cout << "ere" << endl;
	if(node == NULL){ 
//		cout << "node is NULL" << endl; 
		return vector<Table*>(); 
	}
	
	if(node->getRepresentedNode() == NULL){
		result = node->getTables();
	}else{
		result = node->getRepresentedNode()->getTables();
//		parentTables = this->denormalize(node->getRepresentedNode()->getParent());
	}
		parentTables = this->denormalize(node->getParent());
	
//	cout << "here" << endl;
	
	if(parentTables.size() > 0){
		result.reserve(result.size() + parentTables.size());
		result.insert(result.end(), parentTables.begin(), parentTables.end());
	}
	
	return result;
}

vector<Table*> Database::getOptimalLDT(PrecedenceGraphNode* root) {
	cout << "call getOptimalLDT" << endl;
	while(isNotChain(root))
	{
//		cout << "while isnotchain-loop" << endl;
		vector<PrecedenceGraphNode> children = root->getChildren();
		root->clearChildren();
		//merge and normalize all children of root node
		for(vector<PrecedenceGraphNode>::iterator it = children.begin(); it != children.end(); ++it)
		{
			if(!((*it).getChildren().size() == 0))
			{
				PrecedenceGraphNode* subtreeRoot = getSubtreeRoot(&(*it), &(*it));
//				cout << subtreeRoot->getTables().front()->getName() << endl;
				this->normalize(subtreeRoot);
			
				//merge chains in descending order so we can just pop the first node from the back of the list
				vector<PrecedenceGraphNode> mergeList = subtreeRoot->getChildren();
				//mergeList.sort(mergeList.begin(),mergelist.end(), sortDescendingByRank);
				subtreeRoot->clearChildren();
				//sort children by rank! however it's not implemented yet^^
				PrecedenceGraphNode* currentNode = subtreeRoot;
				
//				cout << "\t\tmuss " << mergeList.size() << " Knoten mergen" << endl;
				
				while(mergeList.size() > 0){
					//cout << "merge jetzt " << currentNode->getTables().front()->getName() << endl;
					PrecedenceGraphNode lastChild = mergeList.back();
					mergeList.pop_back();
					
//					cout << "\t\t\tmuss noch " << mergeList.size() << " Knoten mergen" << endl;
					
					currentNode->addChild(lastChild);
					
//					cout << "\t\t\tlastChild ist " << lastChild.getTables().front()->getName() << endl;
					
					currentNode = &lastChild;
					
//					cout << "\t\t\tund damit auch currentNode " << currentNode->getTables().front()->getName() << endl;
				}
//				cout << "\t\tdas sollte table 2 sein " << subtreeRoot->getTables().front()->getName()  << endl;
//				cout << "\t\tdas sollte 1 sein " << subtreeRoot->getChildren().size()  << endl;
//				cout << "\t\tsollte ebenfasll 1 sein " << (*it).getChildren().size()  << endl;
//				cout << "\t\tdas sollte table 4 sein " << subtreeRoot->getChildren().front().getTables().front()->getName()  << endl;
			}
		}
	}

	//denormalize

	cout << "denormalize call" << endl;
	cout << "root children " << root->getChildren().size() << endl;
	return this->denormalize(root);
}


PrecedenceGraphNode Database::queryGraphToPrecedenceGraph(vector<Table*>* tables, vector<pair<Table*, Table*> >* joins , Table* table){
	PrecedenceGraphNode node;
	node.addTable(table);

	for(vector<Table*>::iterator it = tables->begin(); it != tables->end(); ++it){
		if((*it) == table){
			tables->erase(it);
			break;
		}
		cout << (*it)->getName() << endl;
	}

//	cout << "currend node is "+table->getName() << endl;


	for(vector<pair<Table*, Table*> >::iterator it = joins->begin(); it != joins->end(); ++it){
		
		if((*it).first == table){
			if(std::find(tables->begin(), tables->end(), (*it).second) != tables->end()){
				PrecedenceGraphNode child = queryGraphToPrecedenceGraph(tables, joins , (*it).second);
//				cout << "returned from "+child.getTables().front()->getName() << endl;
				node.addChild(child);
				child.setParent(&node);
			}
		}
		else if((*it).second == table)
		{
			if(std::find(tables->begin(), tables->end(), (*it).first) != tables->end())
			{
				PrecedenceGraphNode child = queryGraphToPrecedenceGraph(tables, joins , (*it).first);
//				cout << "returned from "+child.getTables().front()->getName() << endl;
				node.addChild(child);
				child.setParent(&node);
			}
		}
	}	
//	cout << "over" << endl;
	return node;
}


Database::~Database() {

}


Table::Table(string name, int columnCount) : name(name), columnCount(columnCount) { 
	this->greatestColumnValue = ( int* )malloc(sizeof(int) * columnCount);
	this->smallestColumnValue = ( int* )malloc(sizeof(int) * columnCount);
	for(int i = 0; i<columnCount; i++){
		this->greatestColumnValue[i] = 0;
		this->smallestColumnValue[i] = 0;
	}
	
	this->columnStats = new EquiWidthStat(this);
}

bool Table::insert(int* tuple) { 
	int *d = ( int* )malloc(sizeof(int) * columnCount);
	for(int i=0; i<columnCount; i++){
		d[i]=tuple[i];
		if(tuple[i] > this->greatestColumnValue[i]){ this->greatestColumnValue[i] = tuple[i]; }
		if(tuple[i] < this->smallestColumnValue[i]){ this->smallestColumnValue[i] = tuple[i]; }
	} 
	data.push_back(d); 
	return true;
}

int Table::getNDVofColumn(int column){ 
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
	if(table->getNumberOfEntries() == 0) return;
	
//	int width = ceil(table->getNumberOfEntries()/this->partitions);
	int width = ceil((table->getGreatestColumnValue(column) + (-1) * table->getSmallestColumnValue(column)) / table->getNumberOfEntries());
	if(width == 0) width = 1;
	this->setWidth(width, column);
	
	for(vector<int*>::iterator it = table->getDataBegin(); it != table->getDataEnd(); it++){
		this->incrementWidthCount(column, (*it)[column]);
	}
}



