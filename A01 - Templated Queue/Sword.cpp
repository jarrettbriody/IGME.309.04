#include "pch.h"
#include "Sword.h"


Sword::Sword(string n, int l, int d)
{
	name = n;
	length = l;
	damage = d;
}

void Sword::ChangeValues(string n, int l, int d)
{
	name = n;
	length = l;
	damage = d;
}

Sword::~Sword()
{
}
