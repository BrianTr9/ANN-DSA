#include "ann/functions.h"
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <limits>



xt::xarray<double> softmax(xt::xarray<double> X, int axis){
    xt::svector<unsigned long> shape = X.shape();
    axis = positive_index(axis, shape.size());
    shape[axis] = 1;
    
    xt::xarray<double> Xmax = xt::amax(X, axis);
    X = xt::exp(X - Xmax.reshape(shape));
    xt::xarray<double> SX = xt::sum(X, axis); SX = SX.reshape(shape);
    X = X/SX;
    
    return X;
}

/*
 */
double cross_entropy(xt::xarray<double> Ypred, xt::xarray<double> Ygt, bool mean_reduced){
    int nsamples = Ypred.shape()[0];
    xt::xarray<double> logYpred = xt::log(Ypred + 1e-7);
    xt::xarray<double> R = -Ygt * logYpred;
    R = xt::sum(R, -1);
    
    xt::xarray<double>  sum = xt::sum(R);
    if(mean_reduced) return (sum/nsamples)[0];
    else return sum[0];
}

/*
 */
double cross_entropy(xt::xarray<double> Ypred, xt::xarray<unsigned long> ygt, bool mean_reduced){
    int nclasses = Ypred.shape()[1];
    xt::xarray<double> Ytarget = onehot_enc(ygt, nclasses);
    
    return cross_entropy(Ypred, Ytarget, mean_reduced);
}

xt::xarray<double> onehot_enc(xt::xarray<unsigned long> x, int nclasses){
    int nsamples = x.shape()[0];
    xt::xarray<double> Y = xt::zeros<double>({nsamples, nclasses});
    for(int r=0; r < nsamples; r++){
        int c = x[r];
        xt::view(Y, r, c) = 1.0;
    }
    return Y;
}

int stringHash(string& str, int size) {
    long long int sum = 0;
    // (unsigned char: a plain char can be negative for non-ASCII text => negative bucket index)
    for (int idx = 0; idx < (int)str.length(); idx++) sum += (unsigned char)str[idx];
    return (int)(sum % size);
}

// trim from start (in place)
string ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return s;
}

// trim from end (in place)
string rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}
string trim(std::string &s) {
    rtrim(s);
    ltrim(s);
    return s;
}

string to_lower(const string& str){
    string lowercase = "";
    // std::tolower requires a value representable as unsigned char (or EOF)
    for (char ch : str) lowercase += (char)std::tolower((unsigned char)ch);
    return lowercase;
}

void estimate_params(xt::xarray<double> X, xt::xarray<double>& mu, xt::xarray<double>& sigma){
    mu = xt::mean(X, 0);
    // xt::stddev(X, 0) does NOT take "0" as an axis (it is taken as the evaluation
    // strategy): it returned ONE scalar std over all the elements, so the features were not
    // standardised per column. Pass the axis explicitly: one std per feature.
    // (population std, same definition as xt::stddev; computed with the axis-aware mean)
    sigma = xt::sqrt(xt::mean(xt::square(X - mu), 0));
    // a constant feature has std = 0 => division by zero (inf/NaN) in normalize(): use 1
    sigma = xt::where(xt::equal(sigma, 0.0), 1.0, sigma);
    return;
}
xt::xarray<double> normalize(xt::xarray<double> X, xt::xarray<double> mu, xt::xarray<double> sigma){
    return (X - mu)/sigma;
}


ulong_tensor confusion_matrix(ulong_tensor y_true, ulong_tensor y_pred,  int nclasses){
    //int nclasses = xt::amax(y_true)[0] + 1;
    int nsamples = y_true.shape()[0];
    
    ulong_tensor C = xt::zeros<ulong>({nclasses, nclasses});
    ulong_tensor indices = xt::arange<ulong>(0, nsamples);
    for(auto idx: indices){
        ulong r = y_true[idx];
        ulong c = y_pred[idx];
        xt::view(C, r, c) += 1;
    }
    return C;
}
xt::xarray<ulong> class_count(xt::xarray<ulong> confusion){
    xt::xarray<ulong> count = xt::sum(confusion, -1);
    return count;
}

double_tensor calc_classifcation_metrics(ulong_tensor y_true, ulong_tensor y_pred,  int nclasses){
    double_tensor lookup = xt::zeros<double>({NUM_CLASS_METRICS});
    
    ulong_tensor C = confusion_matrix(y_true, y_pred, nclasses);
    ulong nsamples = xt::sum(C)[0];
    ulong ncorrect = xt::sum(xt::diagonal(C))[0];
    
    double_tensor label_per_class = xt::cast<double>(xt::sum(C, 1));
    double_tensor pred_per_class = xt::cast<double>(xt::sum(C, 0));
    double_tensor diag = xt::cast<double>(xt::diagonal(C));
    
    // A class may be absent from y_true or never predicted: the plain divisions below
    // produced 0/0 = NaN, and NaN then contaminated every macro/weighted average.
    // Convention (as in scikit-learn with zero_division=0): an undefined ratio is 0.
    auto safe_div = [](const double_tensor& num, const double_tensor& den) {
        double_tensor R = xt::zeros<double>(num.shape());
        for (size_t i = 0; i < num.size(); i++)
            R.flat(i) = (den.flat(i) > 0) ? num.flat(i)/den.flat(i) : 0.0;
        return R;
    };
    
    double_tensor prec_per_class = safe_div(diag, pred_per_class);
    double_tensor recall_per_class = safe_div(diag, label_per_class);
    double_tensor weights = (nsamples > 0)? double_tensor(label_per_class/double(nsamples))
                                          : double_tensor(xt::zeros<double>(label_per_class.shape()));
    double_tensor f1_measures = safe_div(2*diag, pred_per_class + label_per_class);

    lookup[ulong(ACCURACY)] = (nsamples > 0)? double(ncorrect)/nsamples : 0.0;
    lookup[ulong(PRECISION_MACRO)] = xt::mean(prec_per_class)[0];
    lookup[ulong(PRECISION_WEIGHTED)] = xt::sum(weights*prec_per_class)[0];
    lookup[ulong(RECALL_MACRO)] = xt::mean(recall_per_class)[0];
    lookup[ulong(RECALL_WEIGHTED)] = xt::sum(weights*recall_per_class)[0];
    lookup[ulong(F1_MEASURE_MACRO)] = xt::mean(f1_measures)[0];
    lookup[ulong(F1_MEASURE_WEIGHTED)] = xt::sum(weights*f1_measures)[0];
    
    return lookup;
}