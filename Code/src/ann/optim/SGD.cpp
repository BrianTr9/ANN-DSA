/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   SGD.cpp
 * Author: ltsach
 * 
 * Created on September 5, 2024, 5:30 PM
 */

#include "optim/SGD.h"
#include "list/DLinkedList.h"
#include <string>
using namespace std;
#include "optim/SGDParamGroup.h"

SGD::SGD(double lr):IOptimizer(lr){
}

SGD::SGD(const SGD& orig): IOptimizer(orig) { // the learning-rate was dropped (default 1e-4 used)
}

SGD::~SGD() {
}

IParamGroup* SGD::create_group(string name){
    // a group registered twice under the same name would leak: release the old one
    if(m_pGroupMap->containsKey(name)) delete m_pGroupMap->get(name);
    IParamGroup* pGroup = new SGDParamGroup();
    m_pGroupMap->put(name, pGroup);
    return pGroup;
}


