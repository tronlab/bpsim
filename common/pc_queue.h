/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#ifndef PC_QUEUE_H
#define PC_QUEUE_H

#include <deque>
#include <map>
#include <iostream>
#include <cassert>
#include <stdint.h>

using namespace std;

template <class T>
class PC_Queue {

	std::map<uint64_t,std::deque<T> > q;
  public:
	//PC_Queue();
	//~PC_Queue();
	void push( uint64_t, T );
	T pop( uint64_t );
	void print();
	void print( uint64_t );
	void print_size();
};

template <class T>
void PC_Queue<T>::push( uint64_t pc, T item ){
	assert( q[pc].size() < 100 );
	q[pc].push_back( item );
}

template <class T>
T PC_Queue<T>::pop( uint64_t pc ){
	T item = q[pc].front();
	q[pc].pop_front();
	return item;
}



template <class T>
void PC_Queue<T>::print( uint64_t pc ){
	deque<uint64_t> &keys = q[pc];
	cout << pc << ": ";
	for( unsigned i=0; i<keys.size(); ++i ){
		cout << keys[i] << ",";
	}
	cout << endl;
}

template <class T>
void PC_Queue<T>::print(){
	map<uint64_t,deque<uint64_t> >::iterator it;
	for( it=q.begin(); it!=q.end(); ++it ){
		print( it->first );
	}
	cout << endl;
}

template <class T>
void PC_Queue<T>::print_size(){
	/*
	map<uint64_t,deque<T> >::iterator it;
	for( it=q.begin(); it!=q.end(); ++it ){
		cout << it->first << ": " << it->second.size() << endl;
	}
	cout << endl;
	*/
}


#endif /* ifndef PC_QUEUE_H */

