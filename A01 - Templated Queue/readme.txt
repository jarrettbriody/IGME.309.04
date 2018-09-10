The GetSize requirement in the rubric is named GetCount in my queue, GetSize in my queue is another
function which gets the amount of entries the array can currently hold in total.

All functions were implemented and are showcased in the main test function.

The extra credit was NOT attempted.

----------------------------

In the test function, 20 ints are added to the queue, which forces the queue to dynamically resize from
10 to 20 indicies. 

5 ints are then removed from the queue, which shifts all remaining entries down
one spot in the queue.

20 more ints are added to the queue, forcing another dynamic resizing, and essentially showcasing
the shifting of the entries after the previous five were popped.

A separate queue is made using a custom object and is then printed.

A separate queue is made with the copy constructor using the previous queue.

A separate queue is made with the equal assignment operator using the previous queue.