#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include "Database.h"
#include "Student.h"
#include "Faculty.h"
#include "BinaryTree.h"
#include "GenStack.h"
#include "queue.h"
#include "listnode.h"

using namespace std;

Database::Database() {
	studentTable = new BinarySearchTree<Student>();
	studentRollStack = new GenStack<BinarySearchTree<Student>*>(5);
	facultyTable = new BinarySearchTree<Faculty>();
	facultyRollStack = new GenStack<BinarySearchTree<Faculty>*>(5);
}


Database::~Database() {

}

void Database::PrintAllStu() { 	// Prints student in order of ID#
	if (studentTable->getSize() == 0) {
		cout << "There are no students on record." << endl;
	}
	else {
		studentTable->print();
	}
}

void Database::PrintAllFac() { 	// Prints faculty in order of ID#
	if (facultyTable->getSize() == 0) {
		cout << "There are no advisors on record." << endl;
	}
	else {
		facultyTable->print();
	}
}

Student Database::FindStu(int stuID) { 	// Finds a student by their ID #
	if(studentTable->getSize() == 0) {
		cout << "There are no students on record." << endl;
	}
	else {
		Student s;
		s.setID(stuID);
		Student S = studentTable->search(s);
		cout << S << endl;
		return S;
	}

}

Faculty Database::FindFac(int facID) { 	// Finds a faculty by their ID#
	if(facultyTable->getSize() == 0) {
		cout << "Faculty table is empty." << endl;
	}
	else {
		Faculty f;
		f.setID(facID);
		Faculty F = facultyTable->search(f);
		cout << F << endl;
		return F;
	}

}

Faculty Database::FindFacByStu(int stuID) { // Find a faculty by the student's ID#
	if(facultyTable->getSize() == 0) {
		cout << "Faculty table is empty." << endl;
	}
	if(studentTable->getSize() == 0) {
		cout << "Student table is empty." << endl;
	}
	if(!studentTable->isEmpty() && !facultyTable->isEmpty()) { // Checks to make sure both the student and faculty table is not empty
		Student s;
		s.setID(stuID);
		Student S = studentTable->search(s);
		return FindFac(S.getAdvisor());
	}
}

void Database::FindStusByFac(int facID) { // Finds the student
	if(facultyTable->getSize() == 0) {
		cout << "Faculty table is empty." << endl;
	}
	if(studentTable->getSize() == 0) {
		cout << "Student table is empty." << endl;
	}

	if(!studentTable->isEmpty() && !facultyTable->isEmpty()) {
		Faculty f;
		f.setID(facID);
		Faculty F = facultyTable->search(f);

		for (int j =0; j<F.getAdviseeListSize(); j++) {
			int i = F.deleteFromAdviseeList();
			if( i!=0){
				FindStu(i);
			}
			else{
				cout << "There is an issue, the faculty has no advisees" << endl;
			}
			F.addToAdviseeList(i);
		}
	}
}

void Database::AddStu() { 	// Creates a new student and adds them to the BST
	studentRollStack->push(studentTable); // In case of a rollback
	facultyRollStack->push(facultyTable); // In case of a rollback

	string holder, stuName, stuYear, stuMajor;
	int stuID, advID;
	double stuGPA;

	cout<< "Please enter student's name." << endl;
	getline(cin,stuName);

	cout<< "Please enter the student's ID number." << endl;
	getline(cin,holder);
	stuID = atoi(holder.c_str());

	bool checkID = true;
	while(checkID) {
		Student tempS;
		tempS.setID(stuID);
		if(studentTable->contains(tempS)) {
			cout << "That ID number already exists in the system, please choose another." << endl;
			getline(cin, holder);
			stuID = atoi(holder.c_str());
		}
		else {
			checkID = false;
		}
	}

	cout << "Please enter the student's GPA." << endl;
	getline(cin,holder);
	stuGPA = atof(holder.c_str());

	cout << "Please enter Student's year (Freshman, Sophmore, etc)" << endl;
	getline(cin,stuYear);

	cout << "Please enter the student's major." << endl;
	getline(cin,stuMajor);

	cout << "Please enter the student's advisor's ID number." << endl;
	getline(cin,holder);
	advID = atoi(holder.c_str());


	Student s(stuName, stuID, stuGPA, stuYear, stuMajor, advID);
	studentTable->add(s);



	if(!facultyTable->isEmpty()) { 	//Find advisor using id#, add student to advisee list
		Faculty temp;
		temp.setID(advID);
		if(facultyTable->contains(temp)) {
			Faculty f = FindFac(advID);
			f.addToAdviseeList(stuID);

			cout << "Print faculty advisee list: " << endl;
			f.adviseeList->print();
		}
		else {
			cout << "No faculty member with that ID in the system." << endl;
			cout << "Would you like to create a faculty member with that ID " << advID << "?" << endl;
			string answer;
			getline(cin, answer);
			if(answer == "yes") {
				AddFac();
			}
			if(answer == "no") {
				cout << "You will now be redirected to main menu." << endl;
			}
			else {
				cout << "Please type 'yes' or 'no'." << endl;
			}
		}
	}
	else {
		cout << "No faculty members yet." << endl;
		cout << "Would you like to create a faculty member with that ID (" << advID << ")?" << endl;
		string answer;
		getline(cin, answer);
		if(answer == "yes") {
			AddFac();
		}
		if(answer == "no") {
			cout << "You will now be redirected to main menu." << endl;
		}
		else {
			cout << "Please type 'yes' or 'no'." << endl;
		}
	}
}

void Database::bfsS() {
	studentTable->bfs();
}

void Database::bfsF() {
	facultyTable->bfs();
}

int Database::DeleteStu(int stuID) { 	//Finds a student by their ID and deletes them from the tree


	studentRollStack->push(studentTable); // For the Rollback
	facultyRollStack->push(facultyTable); // For the Rollback

	Student s;
	s.setID(stuID);
	Student S = studentTable->search(s);

	if(S.getName().compare(".") == 1)
		return 0;

	studentTable->remove(S);
	RemoveAdvisee(stuID);
	return 1;

}

void Database::AddFac() { 	//Creates a new faculty and adds them to the BST
	studentRollStack->push(studentTable);
	facultyRollStack->push(facultyTable); // For the Rollback

	string holder, facName, facLevel, facDepartment;
	int facID, adviseeID;
	DLinkedList<int> *adviseeList = new DLinkedList<int>(); // Fixes size error by creating a new adviseeList instead of just declaring *adviseeList

	cout<< "Please enter the faculty's name." << endl;
	getline(cin,facName);

	cout<< "Please enter the faulty's ID number." << endl;
	getline(cin,holder);
	facID = atoi(holder.c_str());

	bool checkID = true;
	while(checkID) {
		Faculty tempF;
		tempF.setID(facID);
		if(facultyTable->contains(tempF)) {
			cout << "That ID number already exists in the system. Please enter another." << endl;
			getline(cin, holder);
			facID = atoi(holder.c_str());
		}
		else {
			checkID = false;
		}
	}

	cout << "Please enter the faculty member's level? (Assisstant, associate, etc)" << endl;
	getline(cin,facLevel);

	cout << "Please enter the faculty's department?" << endl;
	getline(cin,facDepartment);

	cout << "What are the ID numbers of the faculty member's advisees? (Please hit enter after each number). When done finished, please enter Done." << endl;
	bool check = true;
	while(check) {
		getline(cin,holder);
		if(holder == "Done") {
			check = false;
		}
		else {
			adviseeID = atoi(holder.c_str());
			Student s;
			s.setID(adviseeID);
			if(studentTable->contains(s)) {
				adviseeList->insertBack(adviseeID);// Replaces list with the last id to be added
			}
			else {
				cout << "The student you are trying to add to the advisee list (ID number: " << adviseeID << " ) does not exist in the system." << endl;
			}
		}
	}

	adviseeList->print();

	Faculty f(facName, facID, facLevel, facDepartment, adviseeList);
	facultyTable->add(f);

}

int Database::DeleteFac(int facID, int advTransferID) { 	// Finds a faculty by their ID # and deletes them from the tree, takes their advisees and assign another advisor
	studentRollStack->push(studentTable); // For the rollback
	facultyRollStack->push(facultyTable); // For the rollback

	Faculty f;
	f.setID(facID);
	Faculty F = facultyTable->search(f);

	if(F.getName().compare(".") == 1)
		return 0;

	facultyTable->remove(F);
	f.setID(advTransferID);
	Faculty advTransfer = facultyTable->search(f);

	while(F.getAdviseeListSize() != 0) {
		int frontID = F.deleteFromAdviseeList()	;
		ChangeStuAdvisor( frontID, advTransferID);
	}

	facultyTable->remove(F);
	return 1;

}

int Database::ChangeStuAdvisor(int stuID, int facID) { 	// Finds a student by ID#, removes student from their advisor, finds a new advisor by ID,

	//adds the student to the new advisor

	studentRollStack->push(studentTable);  // For the rollback
	facultyRollStack->push(facultyTable); // For the rollback

	Student s;
	s.setID(stuID);
	Student S = studentTable->search(s);

	if(S.getName().compare(".") == 1)
		return 0;

	Faculty f;
	f.setID(S.getAdvisor());
	Faculty newF = facultyTable->search(f);
	if(newF.getName().compare(".") == 1)
		return 0;
	if(S.getAdvisor() != 0){
		S.setAdvisor(0);

	}

	S.setAdvisor(facID);
	newF.addToAdviseeList(stuID);

}

int Database::RemoveAdvisee(int stuID) { 	// Finds a student by ID, removes them from their advisor


	studentRollStack->push(studentTable); // For the Rollback
	facultyRollStack->push(facultyTable); // For the Rollback

	Student s;
	s.setID(stuID);
	Student S = studentTable->search(s);
	if(S.getName().compare(".") == 1)
		return 0;

	Faculty f;
	f.setID(S.getAdvisor());
	Faculty F = facultyTable->search(f);
	if(F.getName().compare(".") == 1)
		return 0;

	Faculty oldF = facultyTable->search(F);
	if(oldF.getName().compare(".") == 1)
		return 0;

	oldF.deleteFromAdviseeList(stuID);

}

void Database::Rollback() { // Implements the Rollback feature

	if(!studentRollStack->isEmpty()) {
		studentTable = studentRollStack->pop();
		facultyTable = facultyRollStack->pop();
	}
	else {
		cout << "Rollback feature cannot be used, please make a structure change." << endl;
	}
}


void Database::Exit() {

}


void Database::serializeStudents(string outFile) {
	ofstream myfile(outFile, ios::out | ios::trunc | ios::binary);

	if (!myfile.is_open()) {
		cout << "Error opening file." << endl;
	}
	else {
		GenQueue<TreeNode<Student>*> *q;
		q = studentTable->bfs();

		TreeNode<Student>* current;

		while(!q->isEmpty()) {
			current = q->remove();
			unsigned len;
			string n = current->data.getName();
			len = n.size();
			myfile.write(reinterpret_cast<const char*>(&len), sizeof(len));
			myfile.write(n.c_str(), len);

			int i = current->data.getID();
			myfile.write(reinterpret_cast<const char*>(&i), sizeof(int));

			double g = current->data.getGPA();
			myfile.write(reinterpret_cast<const char*>(&g), sizeof(double));

			string l = current->data.getLevel();
			len = l.size();
			myfile.write(reinterpret_cast<const char*>(&len), sizeof(len));
			myfile.write(l.c_str(), len);

			string m = current->data.getMajor();
			len = m.size();
			myfile.write(reinterpret_cast<const char*>(&len), sizeof(len));
			myfile.write(m.c_str(), len);

			int a = current->data.getAdvisor();
			myfile.write(reinterpret_cast<const char*>(&a), sizeof(int));
		}

		myfile.close();
	}
}

void Database::serializeFaculty(string outFile) {
	ofstream myfile(outFile, ios::out | ios::trunc | ios::binary);

	if (!myfile.is_open()) {
		cout << "Error opening file." << endl;
	}

	GenQueue<TreeNode<Faculty>*> *q;
	q = facultyTable->bfs();

	TreeNode<Faculty>* current;

	while(!q->isEmpty()) {
		current = q->remove();

		unsigned len;

		string n = current->data.getName();
		len = n.size();
		myfile.write(reinterpret_cast<const char*>(&len), sizeof(len));
		myfile.write(n.c_str(), len);

		int i = current->data.getID();
		myfile.write(reinterpret_cast<const char*>(&i), sizeof(int));

		string l = current->data.getLevel();
		len = l.size();
		myfile.write(reinterpret_cast<const char*>(&len), sizeof(len));
		myfile.write(l.c_str(), len);

		string d = current->data.getDepartment();
		len = d.size();
		myfile.write(reinterpret_cast<const char*>(&len), sizeof(len));
		myfile.write(d.c_str(), len);

		int marker = current->data.getAdviseeListSize();

		myfile.write(reinterpret_cast<const char*>(&marker), sizeof(int));
		DLinkedList<int>* a = current->data.getAdviseeList(); 		//Write in each advisee ID from advisee list

		while(!a->isEmpty()) {
			int i = a->removeFront();
			myfile.write(reinterpret_cast<const char*>(&i), sizeof(int));
		}
	}
	myfile.close();
}

BinarySearchTree<Student>* Database::deserializeStudents(string inFile) { 	// Initializes studentTable to empty before reading in stored copy of student BST
	studentTable = new BinarySearchTree<Student>();
	ifstream file(inFile, ios::in | ios::binary);

	if(!file.is_open()) {
		cout << "Error opening file." << endl;
	}
	else {
		int c = file.peek();
		while(c != EOF) {
			string tName, tLevel, tMajor;
			int tID, tAdv;
			double tGPA;
			unsigned len;
			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			if(len >0){
				char* buffer = new char[len];
				file.read(buffer, len);
				tName.append(buffer, len);
				delete[] buffer;
			}
			cout << tName << ", ";

			file.read((char*)&tID, sizeof(int));
			cout << tID << ", ";
			file.read((char*)&tGPA, sizeof(double));
			cout << tGPA << ", ";


			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			if(len >0){
				char* buffer = new char[len];
				file.read(buffer, len);
				tLevel.append(buffer, len);
				delete[] buffer;
			}
			cout << tLevel << ", ";


			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			if(len >0){
				char* buffer = new char[len];
				file.read(buffer, len);
				tMajor.append(buffer, len);
				delete[] buffer;
			}
			cout << tMajor << ", ";

			file.read((char*)&tAdv, sizeof(int));
			cout << tAdv << endl;

			Student s(tName, tID, tGPA, tLevel, tMajor, tAdv);

			studentTable->add(s);
			c = file.peek();
		}
	}
	studentTable->print();
	file.close();

	return studentTable;
}

BinarySearchTree<Faculty>* Database::deserializeFaculty(string inFile) { 	// Initialize faculty table to empty before reading in faculty from stored BST
	facultyTable = new BinarySearchTree<Faculty>();
	ifstream file(inFile, ios::in | ios::binary);

	if(!file.is_open()) {
		cout << "Error opening file." << endl;
	}
	else {
		string tName, tLevel, tDepartment;
		int tID, marker, tsID;
		DLinkedList<int>* taList = new DLinkedList<int>();

		int c = file.peek();
		while(c != EOF) {
			unsigned len;
			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			if(len >0){
				char* buffer = new char[len];
				file.read(buffer, len);
				tName.append(buffer, len);
				delete[] buffer;
			}
			cout << tName << ", ";

			file.read((char*)&tID, sizeof(int));
			cout << tID << ", ";

			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			if(len >0){
				char* buffer = new char[len];
				file.read(buffer, len);
				tLevel.append(buffer, len);
				delete[] buffer;
			}
			cout << tLevel << ", ";

			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			if(len >0){
				char* buffer = new char[len];
				file.read(buffer, len);
				tDepartment.append(buffer, len);
				delete[] buffer;
			}
			cout << tDepartment << ", ";
			file.read((char*)&marker, sizeof(int));

			if(marker != 0) {
				for(int i=0; i<marker; i++) {
					file.read((char*)&tsID, sizeof(int));
					taList->insertBack(tsID);
				}
			}

			taList->print();

			Faculty f(tName, tID, tLevel, tDepartment, taList);
			facultyTable->add(f);
			c = file.peek();
		}
	}
	file.close();

	facultyTable->print();

	return facultyTable;
}
