#pragma once

#include <string>

using namespace std;

//test class for testing objects in queue
class Sword
{
private:
	string name;
	int length;
	int damage;
public:
	friend ostream& operator<<(ostream& ostr, Sword const& swd) {
		ostr << "Name: " << swd.name << endl
			<< "Caliber: " << swd.length << endl
			<< "Speed: " << swd.damage << endl << endl;
		return ostr;
	}
	Sword(string n = "Unnamed", int l = 0, int d = 0);
	void ChangeValues(string n, int l, int d);
	~Sword();
};

