#ifndef _ARRAY_
#define _ARRAY_

#include <iostream>
using namespace std;

template <class T>
class Array {
	void assertIndex(int index);
public:
	T	 *array;
	int  size;
	
	Array(int sz);
 	~Array();

	void	append(T e);
	void	append(Array& a);
	void	append(Array& a, int from);
	void	insert(T e, int index);
	T		remove(int index);
	T&		operator[] (int index);
	void	setSize(int sz);
};

typedef Array<double> DOUBLE_ARRAY;
typedef Array<int>    INT_ARRAY;


template <class T>
ostream& operator <<(ostream& os, Array<T>& a);

template <class T>
Array<T>::Array(int sz) {
	size = sz;
	array = new T[size];
}

template <class T>
Array<T>::~Array() {
	delete [] array;
}

template <class T>
void Array<T>::append(T e) {
	insert(e, size);
}

template <class T>
void Array<T>::insert(T e, int index) {
	if(index != size)
		assertIndex(index);

	T *newarray = new T[size+1];
	int i;

	for(i=0; i<index; i++)
		newarray[i] = array[i];

	newarray[index] = e;

	for(i=index; i<size; i++)
		newarray[i+1] = array[i];

	delete [] array;
	array = newarray;
	size++;
}

template <class T>
T& Array<T>::operator[] (int index) {
	assertIndex(index);
	return array[index];
}

template <class T>
void Array<T>::setSize(int sz) {
	size = sz;
	delete [] array;
	array = new T[size];
}


template <class T>
void Array<T>::append(Array<T>& a) {
	append(a, 0);
}

/*
 * Appends a starting from the a's index 'from'
 * to the and of this array.
 */
template <class T>
void Array<T>::append(Array<T>& a, int from) {
	T *newarray = new T[size+a.size-from];

	for(int i=0; i<size; i++)
		newarray[i] = array[i];

	for(i=from; i<a.size; i++)
		newarray[size+i-from] = a[i];

	delete [] array;
	array = newarray;
	size += a.size-from;
}

template <class T>
T Array<T>::remove(int index) {
	assertIndex(index);

	T *newarray = new T[size-1];
	int i;

	for(i=0; i<index; i++)
		newarray[i] = array[i];

	for(i=index+1; i<size; i++)
		newarray[i-1] = array[i];

	T ret = array[index];
	delete [] array;
	array = newarray;
	size--;
	return ret;
}

template <class T>
void Array<T>::assertIndex(int index) {
	if(index<0 || index>=size) {
		cerr << "Array index out of bounds: "<<index<<endl;
		exit(-1);
	}
}

template <class T>
ostream& operator <<(ostream& os, Array<T>& a) {
	for(int i=0; i<a.size; i++)
		os << a[i] << ' ';
	return os<<endl;
}

#endif