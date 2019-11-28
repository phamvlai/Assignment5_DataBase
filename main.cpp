#include <iostream>
#include "Student.h"
#include "Faculty.h"
#include "BinaryTree.h"
#include "Database.h"
#include "Menu.h"

using namespace std;

bool exists(const string& name) {
	ifstream infile(name);
	return infile.good();
}

int main() {

	Menu *m = new Menu();

	if( exists("studentTable.bin") ) {
		cout << "Student table file already exists. Loading studentTable data from previous use." << endl;
		m->loadStu(); 		// Deserialize student table into database
	}
	else {
		cout << "First time running program, creating a new and empty studentTable." << endl;
		
	}
	if( exists("facultyTable.bin")) {
		cout << "Faculty table file already exists. Loading facultyTable data from previous use." << endl;
		m->loadFac(); 	// Deserialize faculty table into database
	}
	else {
		cout << "First time running program, createing a new and empty facultyTable." << endl;
	}

	m->execute();

	return 0;
}
