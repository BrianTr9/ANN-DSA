/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   Softmax.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:46 PM
 */

#include "layer/Softmax.h"

#include <filesystem>  //require C++17

#include "ann/functions.h"
#include "sformat/fmt_lib.h"
namespace fs = std::filesystem;

Softmax::Softmax(int axis, string name) : m_nAxis(axis) {
  if (trim(name).size() != 0)
    m_sName = name;
  else
    m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax& orig): m_nAxis(orig.m_nAxis) {
    // (m_nAxis used to stay uninitialized in a copy)
    m_sName = "Softmax_" + to_string(++m_unLayer_idx);
    m_aCached_Y = orig.m_aCached_Y;
}

Softmax::~Softmax() {}

xt::xarray<double> Softmax::forward(xt::xarray<double> X) {
  // Todo CODE YOUR
  m_aCached_Y = softmax(X, m_nAxis);
  return m_aCached_Y;
}

xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
  // Todo CODE YOUR
  //xt::xarray<double> DZ=xt::diag(m_aCached_Y)-xt::linalg::outer(m_aCached_Y,xt::transpose(m_aCached_Y));
  if (DY.dimension() == 1){
    xt::xarray<double> DZ=xt::linalg::dot(xt::diag(m_aCached_Y)-xt::linalg::outer(m_aCached_Y,xt::transpose(m_aCached_Y)), DY); 
    return DZ;
  }

    xt::xarray<double> Y = this->m_aCached_Y;
    xt::xarray<double> diagY = diag_stack(Y);
    xt::xarray<double> outerY = outer_stack(Y, Y);
    xt::xarray<double> jacobian = diagY - outerY;

    xt::xarray<double> DZ = matmul_on_stack(jacobian, DY);
    return DZ; 
}

string Softmax::get_desc() {
  string desc = fmt::format("{:<10s}, {:<15s}: {:4d}", "Softmax",
                            this->getname(), m_nAxis);
  return desc;
}