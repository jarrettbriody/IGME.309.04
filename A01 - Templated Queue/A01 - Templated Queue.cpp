
#include "pch.h"
#include <iostream>
#include "queue.h"
#include "Sword.h"

void TestFunction() {
	Queue<int> q = Queue<int>();
	cout << "Queue is empty: " << q.IsEmpty() << endl;

	//add 20 ints to the queue, print queue every five entries
	cout << "-----PUSHING-----" << endl << endl;
	for (size_t i = 1; i < 21; i++)
	{
		q.Push(i * 10);
		if (i % 5 == 0) {
			q.Print();
			cout << endl;
		}
	}

	//remove 5 ints from the queue, print each time
	cout << "-----POPPING-----" << endl << endl;
	for (size_t i = 1; i < 6; i++)
	{
		q.Pop();
		q.Print();
		cout << endl;
	}

	//push 20 more ints into the queue, testing dynamic sizing, print every 5
	cout << "-----PUSHING-----" << endl << endl;
	for (size_t i = 1; i < 21; i++)
	{
		q.Push(i * 10);
		if (i % 5 == 0) {
			q.Print();
			cout << endl;
		}
	}
	cout << "Queue is empty: " << q.IsEmpty() << endl << endl;


	//push a custom sword object into a new queue, print
	cout << "-----PUSHING CUSTOM OBJECT-----" << endl << endl;
	Queue<Sword> q2 = Queue<Sword>();
	Sword s = Sword("Excalibre", 2, 10);
	q2.Push(s);
	q2.Print();

	//copy that custom sword queue to another using copy ctor
	cout << "-----COPYING CUSTOM OBJECT QUEUE WITH COPY CONSTRUCTOR-----" << endl << endl;
	Queue<Sword> q3 = Queue<Sword>(&q2);
	q3.Print();

	//copy the custom sword queue to another using equals assignment
	cout << "-----COPYING CUSTOM OBJECT QUEUE WITH EQUAL ASSIGNMENT OPERATOR-----" << endl << endl;
	Queue<Sword> q4 = Queue<Sword>();
	q4 = q2;
	q4.Print();
}

int main()
{
	TestFunction();
	cout << "Memory Leaks End of Main: " << _CrtDumpMemoryLeaks() << endl;
	return 0;
}
