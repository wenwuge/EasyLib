#ifndef __LDD_COURIER_EVENT_LIST_H_
#define __LDD_COURIER_EVENT_LIST_H_

#include <assert.h>


namespace ldd{
namespace courier{

template<typename Key, class Comparator>
class EventList{
private:
	class Node;
public:
	EventList(Comparator cmp);
	~EventList(){	
		DelNode(head_);
	}

	void Insert(const Key& key);
	void PopFront();
	Key& Front();
	bool Empty();

private:	

  // No copying allowed
  EventList(const EventList& a);
  void operator=(const EventList& b);
	
	Node* NewNode(const Key& key);
	void DelNode(const Node* n);
	bool KeyIsAfterNode(const Key& key, Node* n) const;
	Node* FindGreaterOrEqual(const Key& key);
private:
	Comparator const compare_;
	Node* head_;

};   // class list


template<typename Key, class Comparator>
class EventList<Key, Comparator>::Node {
public:
	explicit Node(const Key& k) : key_(k), next_(NULL) {}

	void SetNext(Node* x){
		next_=x;
	}

	Node* Next(){
		return next_;
	}
	Key key_;
private:
	Node* next_;
}; 	// class Node


///// list Implementation 
template<typename Key, class Comparator>
EventList<Key, Comparator>::EventList(Comparator cmp) 
: compare_(cmp), head_( NewNode(0) ) {
	head_->SetNext(NULL);
}


template<typename Key, class Comparator>
typename EventList<Key,Comparator>::Node* EventList<Key, Comparator>::NewNode(const Key& key){
	Node* p = new Node(key);
	p->SetNext(NULL);
	return p;
}

template<typename Key, class Comparator>
void EventList<Key, Comparator>::DelNode(const Node* n){
	delete n;
}


template<typename Key, class Comparator>
bool EventList<Key,Comparator>::KeyIsAfterNode(const Key& key, Node* n) const {
	// NULL n is considered infinite
	return (n != NULL) && (compare_(n->key_, key) < 0);
}


template<typename Key, class Comparator>
typename EventList<Key,Comparator>::Node* EventList<Key,Comparator>::FindGreaterOrEqual(const Key& key){
	Node* x = head_;

	while(true){
		Node* next = x->Next();
		if(KeyIsAfterNode(key, next)){
			x = next;
		}else {
			return x;
		}
	}
}

template<typename Key, class Comparator>
void  EventList<Key, Comparator>::Insert(const Key& key){
	Node* x= FindGreaterOrEqual(key);
	Node* c= NewNode(key);

	c->SetNext(x->Next());
	x->SetNext(c);
}

template<typename Key, class Comparator>
bool  EventList<Key, Comparator>::Empty(){
	return ((head_->Next() == NULL) ? true : false) ;
}

template<typename Key, class Comparator>
Key&  EventList<Key, Comparator>::Front(){
	assert(head_->Next() != NULL);
	return head_->Next()->key_;
}

template<typename Key, class Comparator>
void  EventList<Key, Comparator>::PopFront(){
	if(head_->Next() == NULL)
		return;
	Node* x = head_->Next();
	head_->SetNext(x->Next());		
	DelNode(x);
}


}//namespace courier
}//namespace ldd



#endif // __LDD_COURIER_EVENT_LIST_H_






