/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Optimizer.cpp
 * Author: ltsach
 * 
 * Created on September 2, 2024, 10:51 AM
 */

#include "optim/IOptimizer.h"
#include "list/DLinkedList.h"
#include <string>
using namespace std;

IOptimizer::IOptimizer(double learning_rate):
m_fLearningRate(learning_rate){
    m_pGroupMap = new xmap<string, IParamGroup*>(&stringHash,
            0.75,
            nullptr,
            &xmap<string, IParamGroup*>::freeValue);
}

IOptimizer::IOptimizer(const IOptimizer& orig):
m_fLearningRate(orig.m_fLearningRate){
    // m_pGroupMap used to stay uninitialized => crash on first use/destruction.
    // The groups are created per-optimizer by create_group (and are registered by the
    // layers through compile), so the copy starts with an empty set of groups.
    m_pGroupMap = new xmap<string, IParamGroup*>(&stringHash,
            0.75,
            nullptr,
            &xmap<string, IParamGroup*>::freeValue);
}

IOptimizer::~IOptimizer() {
    if(m_pGroupMap != nullptr) delete m_pGroupMap;
}

void IOptimizer::step(){
    DLinkedList<string> gkeys = m_pGroupMap->keys();
    for(auto key: gkeys){
        IParamGroup* pGroup = m_pGroupMap->get(key);
        pGroup->step(m_fLearningRate);
    }
}
void IOptimizer::zero_grad(){
    DLinkedList<string> keys = m_pGroupMap->keys();
    for(auto key: keys){
        IParamGroup* pGroup = m_pGroupMap->get(key);
        pGroup->zero_grad();
    }
};