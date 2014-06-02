
#include "adbs.h"

int main(int argc, char* argv[]) {
	Database database;
	Table* table1 = database.createTable("Table 1", 1);
	Table* table2 = database.createTable("Table 2", 3);
	Table* table3 = database.createTable("Table 3", 1);
	Table* table4 = database.createTable("Table 4", 1);

	int data[3];
	for (int i = 0; i < 10000; i++) {
		data[0] = i;
		database.insert(table1, data);
	}
	for (int i = 0; i < 50000; i++) {
		data[0] = i * 5;
		data[1] = i;
		data[2] = i % 4000;
		database.insert(table2, data);
	}
	for (int i = 0; i < 4000; i++) {
		data[0] = i;
		database.insert(table3, data);
	}
	for (int i = 0; i < 8000; i++) {
		data[0] = i;
		database.insert(table4, data);
	}
	
//	cout << table4->getNumberOfPhrases() << endl;
	
	QueryGraph queryGraph;
	queryGraph.addJoin(table2, table1, 0, 0);
	queryGraph.addJoin(table2, table3, 1, 0);
	queryGraph.addJoin(table2, table4, 2, 0);
	vector<Table*> joinOrder = database.getJoinOrder(&queryGraph);

	cout << "Join Order:" << endl;
	for (vector<Table*>::iterator it = joinOrder.begin(); it != joinOrder.end(); it++) {
		cout << (*it)->getName() << endl;
	}

}


