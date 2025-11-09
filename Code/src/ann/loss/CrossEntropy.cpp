/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   CrossEntropy.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"

#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction) : ILossLayer(reduction) {}

CrossEntropy::CrossEntropy(const CrossEntropy& orig) : ILossLayer(orig) {}

CrossEntropy::~CrossEntropy() {}

double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t) {
  // Todo CODE YOUR
  m_aCached_Ypred=X;
  m_aYtarget=t;
  return cross_entropy(X, t, m_eReduction == REDUCE_MEAN);
}
xt::xarray<double> CrossEntropy::backward() {
  // Todo CODE YOUR
  xt::xarray<double> DY = -m_aYtarget/(m_aCached_Ypred+1e-7);
  if (m_eReduction == REDUCE_MEAN) {
    DY = DY / m_aYtarget.shape()[0];
  }
  return DY;
  
}