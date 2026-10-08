/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   AdagradParamGroup.cpp
 * Author: ltsach
 * 
 * Created on October 7, 2024, 9:59 PM
 */

#include "optim/AdaParamGroup.h"

AdaParamGroup::AdaParamGroup(double decay): m_decay(decay) {
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pSquaredGrads = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pCounter = nullptr; //was uninitialized
}

AdaParamGroup::AdaParamGroup(const AdaParamGroup& orig): m_decay(orig.m_decay) {
    // (It used to leave every member uninitialized.)
    // parameters/gradients belong to the layers => share the pointers;
    // the squared-gradient accumulators belong to this group => deep copy.
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pSquaredGrads = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pCounter = orig.m_pCounter;
    DLinkedList<string> keys = orig.m_pParams->keys();
    for(auto key: keys){
        m_pParams->put(key, orig.m_pParams->get(key));
        m_pGrads->put(key, orig.m_pGrads->get(key));
        m_pSquaredGrads->put(key, new xt::xarray<double>(*orig.m_pSquaredGrads->get(key)));
    }
}

AdaParamGroup::~AdaParamGroup() {
    if(m_pParams != nullptr) delete m_pParams;
    if(m_pGrads != nullptr) delete m_pGrads;
    if(m_pSquaredGrads != nullptr) delete m_pSquaredGrads; //also frees the accumulators (freeValue)
}

void AdaParamGroup::register_param(string param_name, xt::xarray<double>* ptr_param, xt::xarray<double>* ptr_grad){
    m_pParams->put(param_name, ptr_param);
    m_pGrads->put(param_name, ptr_grad);
    //prepare squared-grads: an accumulator with the SAME SHAPE as the parameter, filled with 0.
    //(it used to be an empty tensor, only "fixed" by zero_grad zeroing it before every step)
    if(m_pSquaredGrads->containsKey(param_name)) delete m_pSquaredGrads->get(param_name);
    m_pSquaredGrads->put(param_name, new double_tensor(xt::zeros<double>(ptr_param->shape())));
}
void AdaParamGroup::register_sample_count(unsigned long long* pCounter){
    m_pCounter = pCounter;
}
void AdaParamGroup::zero_grad(){
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys){
        xt::xarray<double>* pGrad = m_pGrads->get(key);
        xt::xarray<double>* pParam = m_pParams->get(key);
        *pGrad = xt::zeros<double>(pParam->shape());
        // NOTE: the squared-gradient accumulator must NOT be reset here: zero_grad runs
        // before every mini-batch, so resetting it threw away the optimizer's history
        // and made the update degenerate to ~ lr*sign(grad).
    }
    //reset sample_counter
    if(m_pCounter != nullptr) *m_pCounter = 0;
}

void AdaParamGroup::step(double lr){
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys){
        xt::xarray<double>& grad_P = *m_pGrads->get(key);
        xt::xarray<double>& squared_grad = *m_pSquaredGrads->get(key);
        squared_grad = m_decay*squared_grad + (1 - m_decay)*grad_P*grad_P;
        xt::xarray<double>& P = *m_pParams->get(key);
        
        P = P - lr*grad_P/(xt::sqrt(squared_grad) + 1e-7);
    }
}