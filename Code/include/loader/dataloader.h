/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "tensor/xtensor_lib.h"
#include "loader/dataset.h"

using namespace std;

template<typename DType, typename LType>
class DataLoader{
public:
    class Iterator; //forward declaration for class Iterator
    
private:
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int nbatch;
    ulong_tensor item_indices;
    int m_seed;
    
public:
    DataLoader(Dataset<DType, LType>* ptr_dataset, 
            int batch_size, bool shuffle=true, 
            bool drop_last=false, int seed=-1)
                : ptr_dataset(ptr_dataset), 
                batch_size(batch_size), 
                shuffle(shuffle),
                drop_last(drop_last), //was never stored => uninitialized, behaviour was random
                m_seed(seed){
            // batch_size <= 0 would divide by zero below (and loop forever while iterating)
            if(batch_size <= 0) throw std::invalid_argument("DataLoader: batch_size must be > 0");
            nbatch = ptr_dataset->len()/batch_size;
            item_indices = xt::arange(0, ptr_dataset->len());
            // seed < 0: do NOT touch the random generator; seed >= 0: reproducible shuffling
            if(m_seed >= 0) xt::random::seed(m_seed);
    }
    virtual ~DataLoader(){}
    
    //New method: from V2: begin
    int get_batch_size(){ return batch_size; }
    int get_sample_count(){ return ptr_dataset->len(); }
    int get_total_batch(){return int(ptr_dataset->len()/batch_size); }
    
    //New method: from V2: end
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// START: Section                                                     //
    /////////////////////////////////////////////////////////////////////////
public:
     Iterator begin() {
        // 'shuffle' used to be accepted but ignored: re-shuffle the sample order
        // each time a new pass (epoch) over the data starts.
        if(shuffle) xt::random::shuffle(item_indices);
        return Iterator(this, 0);
    }

    Iterator end() {
        return Iterator(this, item_indices.size());
    }
    
    class Iterator {
    private:
        DataLoader<DType, LType>* loader;
        unsigned long cur;
    public:
        Iterator(DataLoader<DType, LType>* loader, unsigned long startIndex)
            : loader(loader), cur(startIndex) {
                if (cur + loader->batch_size > loader->item_indices.size()) {
                    cur = loader->item_indices.size();
                }
            }

        bool operator!=(const Iterator& other) const {
            return cur != other.cur;
        }

        Iterator& operator++() {
            this->cur += loader->batch_size;
            cur = std::min(cur, loader->item_indices.size());
            if (cur + loader->batch_size > loader->item_indices.size()) {
                cur = loader->item_indices.size();
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            this->cur += loader->batch_size;
            cur = std::min(cur, loader->item_indices.size());
            if (cur + loader->batch_size > loader->item_indices.size()) {
                cur = loader->item_indices.size();
            }
            return temp;
        }
   
        
        Batch<DType, LType> operator*() const {
            unsigned long endIndex = std::min(cur + loader->batch_size, loader->item_indices.size());
            if (loader->drop_last==false && loader->item_indices.size() - endIndex < loader->batch_size) {
                endIndex = loader->item_indices.size();
            } 
            
            
            //cout<<"batch_size: "<<loader->batch_size<<endl;
            //cout<<"cur: "<<cur<<endl;
            //cout<<"endIndex: "<<endIndex<<endl;
            //cout<<"item_indices: "<<loader->item_indices<<endl;
            //cout<<"len: "<<loader->ptr_dataset->len()<<endl;
            //cout<<"drop_last: "<<loader->drop_last<<endl;
            
            
            auto newdatashape = loader->ptr_dataset->get_data_shape();
            newdatashape[0] = endIndex - cur;
            auto data = loader->ptr_dataset->getitem(0).getData();
            data.resize(newdatashape);

            
            auto newlabelshape = loader->ptr_dataset->get_label_shape();
            auto labels = loader->ptr_dataset->getitem(0).getLabel();
            if (newlabelshape.size() != 0) {
            newlabelshape[0] = endIndex - cur;
            labels.resize(newlabelshape);
            }
           
            
            
            for (unsigned long i = cur; i < endIndex; i++) {
                auto dataLabel = loader->ptr_dataset->getitem(loader->item_indices[i]);
                xt::view(data, i - cur) = dataLabel.getData();
                if (loader->ptr_dataset->get_label_shape().size() != 0) {
                    xt::view(labels, i - cur) = dataLabel.getLabel();
                }
            }

            return Batch<DType, LType>(data, labels);
            
              
        } 
    
    };
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// END: Section                                                       //
    /////////////////////////////////////////////////////////////////////////
};



#endif /* DATALOADER_H */

