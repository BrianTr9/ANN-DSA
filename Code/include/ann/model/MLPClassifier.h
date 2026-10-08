/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.h to edit this template
 */

/* 
 * File:   Model.h
 * Author: ltsach
 *
 * Created on September 1, 2024, 5:09 PM
 */

#ifndef MODEL_H
#define MODEL_H

#include "tensor/xtensor_lib.h"
#include "list/DLinkedList.h"
#include "layer/ILayer.h"
#include "layer/FCLayer.h"
#include "model/IModel.h"
#include "config/Config.h"

class MLPClassifier: public IModel {
public:
    MLPClassifier(string cfg_filename, string sModelName="MLPClassifier");
    MLPClassifier(string cfg_filename, string sModelName, ILayer** seq, int size);
    // The model OWNS raw ILayer pointers: the old copy-constructor copied the pointers
    // (shallow) and the destructor of each copy then deleted the same layers => double free.
    MLPClassifier(const MLPClassifier& orig) = delete;
    MLPClassifier& operator=(const MLPClassifier& orig) = delete;
    ~MLPClassifier();
    
    //for the inference mode:
    double_tensor predict(double_tensor X, 
                bool make_decision=false);
    double_tensor predict(
                DataLoader<double, double>* pLoader,
                bool make_decision=false);
    double_tensor evaluate(DataLoader<double, double>* pLoader);
    
    //for the training mode:
    void compile(
                IOptimizer* pOptimizer,
                ILossLayer* pLossLayer, 
                IMetrics* pMetricLayer);
    bool save(string model_path="");
    bool load(string model_path, bool use_name_in_file=false);
    
    
    void set_working_mode(bool trainable);
    int get_num_classes(){
        // number of classes = Nout of the LAST fully-connected layer.
        // (It used to take the layer at size-2, which is only right when the model ends with
        //  "FC + Softmax"; with another tail it cast a non-FC layer to FCLayer*.)
        for(auto it = m_layers.bbegin(); it != m_layers.bend(); ++it){
            if((*it)->get_type() == LayerType::FC)
                return ((FCLayer*)(*it))->getNout();
        }
        return 0;
    };

protected:
    double_tensor forward(double_tensor X);
    void backward();
    
protected:
    DLinkedList<ILayer*> m_layers;
    
private:
};

#endif /* MODEL_H */

