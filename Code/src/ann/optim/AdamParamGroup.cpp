/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   AdamParamGroup.cpp
 * Author: ltsach
 * 
 * Created on October 8, 2024, 1:43 PM
 */

#include "optim/AdamParamGroup.h"

AdamParamGroup::AdamParamGroup(double beta1, double beta2):
    m_beta1(beta1), m_beta2(beta2){
    //Create some maps:
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pFirstMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pSecondMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    //
    m_pCounter = nullptr;
    m_step_idx = 1;
    m_beta1_t = m_beta1;
    m_beta2_t = m_beta2;
}

AdamParamGroup::AdamParamGroup(const AdamParamGroup& orig):
    m_beta1(orig.m_beta1), m_beta2(orig.m_beta2){
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pFirstMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pSecondMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    //copy:
    //  + parameters and gradients belong to the layers => share the pointers
    //  + the moments belong to this group => DEEP copy (they are freed by this group's maps;
    //    the former "*m_pFirstMomment = *orig.m_pFirstMomment" shared them => double free)
    DLinkedList<string> keys = orig.m_pParams->keys();
    for(auto key: keys){
        m_pParams->put(key, orig.m_pParams->get(key));
        m_pGrads->put(key, orig.m_pGrads->get(key));
        m_pFirstMomment->put(key, new xt::xarray<double>(*orig.m_pFirstMomment->get(key)));
        m_pSecondMomment->put(key, new xt::xarray<double>(*orig.m_pSecondMomment->get(key)));
    }
    //
    m_pCounter = orig.m_pCounter;
    m_step_idx = orig.m_step_idx;
    m_beta1_t = orig.m_beta1_t;
    m_beta2_t = orig.m_beta2_t;
}

AdamParamGroup::~AdamParamGroup() {
    // parameters/gradients belong to the layers: free only the maps
    if(m_pParams != nullptr) delete m_pParams;
    if(m_pGrads != nullptr) delete m_pGrads;
    // the moments are owned by the group (freeValue deletes the tensors)
    if(m_pFirstMomment != nullptr) delete m_pFirstMomment;
    if(m_pSecondMomment != nullptr) delete m_pSecondMomment;
}

void AdamParamGroup::register_param(string param_name, 
        xt::xarray<double>* ptr_param,
        xt::xarray<double>* ptr_grad){
    //YOUR CODE IS HERE
    // (this method used to be empty => Adam had no parameter to update, i.e., the model never learned)
    m_pParams->put(param_name, ptr_param);
    m_pGrads->put(param_name, ptr_grad);
    
    // moments: same shape as the parameter, initialized with 0.
    // If the name is registered twice, release the old buffers first (no leak).
    if(m_pFirstMomment->containsKey(param_name)) delete m_pFirstMomment->get(param_name);
    if(m_pSecondMomment->containsKey(param_name)) delete m_pSecondMomment->get(param_name);
    m_pFirstMomment->put(param_name, new xt::xarray<double>(xt::zeros<double>(ptr_param->shape())));
    m_pSecondMomment->put(param_name, new xt::xarray<double>(xt::zeros<double>(ptr_param->shape())));
}
void AdamParamGroup::register_sample_count(unsigned long long* pCounter){
    m_pCounter = pCounter;
}

void AdamParamGroup::zero_grad(){
    //YOUR CODE IS HERE
    // Reset ONLY the gradients (and the sample counter). The moments are the optimizer's
    // memory across mini-batches and must be kept.
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys){
        xt::xarray<double>* pGrad = m_pGrads->get(key);
        xt::xarray<double>* pParam = m_pParams->get(key);
        *pGrad = xt::zeros<double>(pParam->shape());
    }
    if(m_pCounter != nullptr) *m_pCounter = 0;
}

void AdamParamGroup::step(double lr){
    //YOUR CODE IS HERE
    const double epsilon = 1e-7; //same stabilizer as the rest of the library
    
    //bias-correction factors: 1 - beta^t  (m_beta1_t = beta1^t, m_beta2_t = beta2^t)
    const double correction1 = 1.0 - m_beta1_t;
    const double correction2 = 1.0 - m_beta2_t;
    
    DLinkedList<string> keys = m_pParams->keys();
    for(auto key: keys){
        xt::xarray<double>& P = *m_pParams->get(key);
        xt::xarray<double>& grad_P = *m_pGrads->get(key);
        xt::xarray<double>& m = *m_pFirstMomment->get(key);
        xt::xarray<double>& v = *m_pSecondMomment->get(key);
        
        // biased first and second moment estimates
        m = m_beta1*m + (1.0 - m_beta1)*grad_P;
        v = m_beta2*v + (1.0 - m_beta2)*grad_P*grad_P;
        
        // bias-corrected estimates
        xt::xarray<double> m_hat = m/correction1;
        xt::xarray<double> v_hat = v/correction2;
        
        P = P - lr*m_hat/(xt::sqrt(v_hat) + epsilon);
    }
    
    //UPDATE step_idx:
    m_step_idx += 1;
    m_beta1_t *= m_beta1;
    m_beta2_t *= m_beta2;
}
