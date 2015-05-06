#ifndef POOL
#define POOL

#include <vector>

using namespace std;

template <class T>

class Pool
{

private:

	int liveCount;
	vector<T*> objects;
	 
public:

	Pool() {
		liveCount = 0;
		objects = vector<T*>();
	}

	T* new_object() {

		T* obj;

		if (liveCount < objects.size())
		{
			obj = objects[liveCount];
		}
		else {
			obj = new T();
			objects.push_back(obj);
		}

		liveCount++;

		return obj;

	}

	void mark_dead(int index) {
		T* tmp = objects[index];
		objects[index] = objects[--liveCount];
		objects[liveCount] = tmp;
	}

	T* at(int index) {
		return objects[index];
	}

	int count() {
		return liveCount;
	}

};

#endif