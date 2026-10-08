/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::Iterator it;
     for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::BWDIterator it;
     for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////

class BWDIterator {
private:
    DLinkedList<T>* pList;
    Node* pNode;

public:
    
    BWDIterator(DLinkedList<T>* pList = 0, bool bbegin = true) {
    if(bbegin){
      if(pList != 0) this->pNode = pList->tail->prev;
      else pNode = 0;
    }
    else{
      if(pList != 0) this->pNode = pList->head;
      else pNode = 0;
    }
    this->pList = pList;
}
 
    BWDIterator& operator=(const BWDIterator& iterator) {
        this->pNode = iterator.pNode;
        this->pList = iterator.pList;
        return *this;
    }
  
    void remove(void (*removeItemData)(T) = nullptr) {
        if (!pNode || pNode == pList->head || pNode == pList->tail) {
            return;  
        }

        pNode->prev->next = pNode->next;
        pNode->next->prev = pNode->prev;
        // After the removal, the next ++/-- (which moves backward) must reach
        // the node that preceded the removed one => stand on the following node.
        Node* pNext = pNode->next;

        if (removeItemData != nullptr)
            removeItemData(pNode->data);

        delete pNode;   // the node used to leak
        pNode = pNext;
        pList->count -= 1;
    }

    T& operator*() {
        return pNode->data;
    }

    bool operator!=(const BWDIterator& iterator) {
        return pNode != iterator.pNode;
    }

    BWDIterator& operator++() {
        pNode = pNode->prev;  
        return *this;             
    }
    
    BWDIterator operator++(int) {
        BWDIterator iterator = *this;  
        pNode=pNode->prev;                     
        return iterator;               
    }

    BWDIterator& operator--() {
        pNode = pNode->prev;  
        return *this;             
    }
    
    BWDIterator operator--(int) {
        BWDIterator iterator = *this;  
        pNode=pNode->prev;                     
        return iterator;               
    }
}; 
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; 
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // TODO
    this->head = new Node();
    this->tail = new Node();
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
    this->itemEqual = itemEqual;
    this->deleteUserData = deleteUserData;

}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    // TODO
    this->head = new Node();
    this->tail = new Node();
    this->head->next = this->tail;
    this->tail->prev = this->head;
    copyFrom(list);
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // TODO
    if (this != &list)
    {
        copyFrom(list);
    }
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    // TODO
    removeInternalData();
    delete this->head;
    delete this->tail;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    // TODO
    Node *newNode = new Node(e);
    Node *pLast = this->tail->prev;
    pLast->next = newNode;
    newNode->prev = pLast;
    newNode->next = this->tail;
    this->tail->prev = newNode;
    this->count++;
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    // TODO
    if (index < 0 || index > this->count)
        throw out_of_range("Index is out of range!");
    if (index == this->count)
    {
        add(e);
        return;
    }
    Node *pNode = getPreviousNodeOf(index);
    Node *newNode = new Node(e);
    newNode->next = pNode->next;
    pNode->next->prev = newNode;
    pNode->next = newNode;
    newNode->prev = pNode;
    this->count++;
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    // TODO
    if (index < 0 || index >= this->count)
        throw out_of_range("Index is out of range!");
    Node *pNode = this->head;
    if (index==0) return this->head;
    if (index < this->count / 2)
    {
        for (int i = 0; i < index; i++)
            pNode = pNode->next;
    }
    else
    {
        pNode = this->tail;
        for (int i = this->count; i >= index; i--)
            pNode = pNode->prev;
    }
    return pNode;

}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    // TODO
    if (index < 0 || index >= this->count)
        throw out_of_range("Index is out of range!");
    Node *pNode = getPreviousNodeOf(index);
    Node *pRemove = pNode->next;
    pNode->next = pRemove->next;
    pRemove->next->prev = pNode;
    T data = pRemove->data;
    delete pRemove;
    this->count--;
    return data;

}

template <class T>
bool DLinkedList<T>::empty()
{
    // TODO
    return this->count == 0;
}

template <class T>
int DLinkedList<T>::size()
{
    // TODO
    return this->count;
}

template <class T>
void DLinkedList<T>::clear()
{
    // TODO
    removeInternalData();
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    // TODO
    if (index < 0 || index >= this->count)
        throw out_of_range("Index is out of range!");
    Node *pNode = getPreviousNodeOf(index);
    return pNode->next->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    // TODO
    Node *pNode = this->head->next;
    for (int i = 0; i < this->count; i++)
    {
        if (equals(pNode->data, item, this->itemEqual))
        {
            return i;
        }
        pNode = pNode->next;
    }
    return -1;

}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // TODO
    int index = indexOf(item);
    if (index == -1) return false;
    Node *pNode = getPreviousNodeOf(index);
    if (removeItemData) removeItemData(pNode->next->data);
    removeAt(index);
    return true;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    // TODO
    return indexOf(item) != -1;
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    // TODO
    string result = "[";
    Node *pNode = this->head->next;
    while (pNode != this->tail)
    {
        if (item2str)
            result += item2str(pNode->data);
        else
        {
            stringstream ss;
            ss << pNode->data;
            result += ss.str();
        }
        pNode = pNode->next;
        if (pNode != this->tail)
            result += ", ";
    }
    result += "]";
    return result;

}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    // TODO
    removeInternalData();
    this->count = list.count;
    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;

    Node *pNode = list.head->next;
    Node *pNewNode = this->head;
    while (pNode != list.tail)
    {
        Node *newNode = new Node(pNode->data);
        pNewNode->next = newNode;
        newNode->prev = pNewNode;
        pNewNode = newNode;
        pNode = pNode->next;
    }
    pNewNode->next = this->tail;
    this->tail->prev = pNewNode;

}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    // TODO
    if (this->head->next == this->tail)
        return;
    if (this->deleteUserData)
            this->deleteUserData(this);
    Node *pNode = this->head->next;
    while (pNode != this->tail)
    {
        Node *pNext = pNode->next;
        delete pNode;
        pNode = pNext;
    }
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
}

#endif /* DLINKEDLIST_H */
