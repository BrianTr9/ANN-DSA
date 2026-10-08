/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IDeck.h
 * Author: LTSACH
 *
 * Created on 27 August 2020, 10:32
 */

#ifndef IDECK_H
#define IDECK_H
#include <sstream>
using namespace std;


class Underflow: public std::exception{
private:
    string desc;
    string message; //the buffer returned by what() must outlive the call
public:
    Underflow(string desc){
        this->desc = desc;
        stringstream os;
        os << "Underflow: " << this->desc;
        this->message = os.str();
    }
    // (it used to return os.str().c_str() of a local stringstream => dangling pointer)
    const char * what () const throw (){
        return message.c_str();
    }
};


template<class T>
class IDeck{
public:
    virtual ~IDeck(){};
    virtual void push(T item)=0;
    virtual T pop()=0;
    virtual T& peek()=0;
    virtual bool empty()=0;
    virtual int size()=0;
    virtual void clear()=0;
    virtual bool remove(T item)=0;
    virtual bool contains(T item)=0;
    virtual string toString(string (*item2str)(T&)=0 )=0;
};

#endif /* IDECK_H */

