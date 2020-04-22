#ifndef __OBSERVABLES_H__
#define __OBSERVABLES_H__
#include <pyglasstools/MathAndTypes.h>
namespace abr = Aboria;

#include <pyglasstools/potential/PairPotential.h>

#include "../extern/pybind11/include/pybind11/pybind11.h"
namespace py = pybind11;

class PYBIND11_EXPORT Observable
{
    public:
        Observable() : name("NONE"), type("SCALAR"), islocal(true), isfield(false), useforce(false), dim(3) {};
        Observable(std::string _name, std::string _type, bool _islocal, bool _isfield, bool _useforce, int _dim) 
            : name(_name), type(_type), islocal(_islocal), isfield(_isfield), useforce(_useforce), dim(_dim)
            {
            };
        virtual void accumulate(const AboriaParticles::value_type& particle_i, 
                                const AboriaParticles::value_type& particle_j, 
                                const std::shared_ptr<PairPotential>& potential)
            {
            }
        virtual void accumulate(const AboriaParticles::value_type& particle_i, 
                                double cgval, unsigned int grid_id)
            {
            }
        virtual void accumulate(const AboriaParticles::value_type& particle_i, 
                                const AboriaParticles::value_type& particle_j, 
                                const std::shared_ptr<PairPotential>& potential,
                                double bondval, unsigned int grid_id)
            {
            }
        virtual void clear()
            {
            }
        virtual Eigen::MatrixXd getGlobalValue()
            {
                return Eigen::MatrixXd::Zero(1,1);
            }
        virtual std::vector< Eigen::MatrixXd > getField()
            {
                std::vector< Eigen::MatrixXd > temp(1,Eigen::MatrixXd::Zero(1,1));
                return temp;
            }

        std::string name;
        std::string type;
        bool islocal;
        bool isfield;
        bool useforce;
        int dim;
};


template< class AtomicObs >
class PYBIND11_EXPORT GlobalObservable : public Observable
{
    public:
        GlobalObservable() : Observable("NONE", "SCALAR", true, false, false,3), obs(3) {};
        GlobalObservable(std::string _name, std::string _type, bool _islocal, bool _useforce, int _dim) 
            : Observable(_name, _type, _islocal, false, _useforce, _dim), obs(_dim) 
            {
                if (type == "SCALAR")
                    val = Eigen::MatrixXd::Zero(1,1);
                else if (type == "VECTOR")
                    val = Eigen::MatrixXd::Zero(dim,1);
                else if (type == "2-TENSOR")
                    val = Eigen::MatrixXd::Zero(dim,dim);
                else if (type == "4-TENSOR")
                    val = Eigen::MatrixXd::Zero((int)3*(dim-1),(int)3*(dim-1));
                else
                    throw std::runtime_error("[ERROR] Type is unrecognized. Select from: SCALAR, VECTOR, 2-TENSOR, and 4-TENSOR");
            };
        virtual void accumulate(const AboriaParticles::value_type& particle_i, 
                                const AboriaParticles::value_type& particle_j, 
                                const std::shared_ptr<PairPotential>& potential)
            {
                val += obs.compute(particle_i, particle_j, potential);
            }
        virtual Eigen::MatrixXd getGlobalValue()
            {
                return val;
            }
        
        virtual std::vector< Eigen::MatrixXd > getField()
            {
                throw std::runtime_error("[ERROR] Observable Type is Global not Local");
                std::vector< Eigen::MatrixXd > temp(1,val);
                return temp;
            }
        void clear()
            {
                if (type == "SCALAR")
                    val = Eigen::MatrixXd::Zero(1,1);
                else if (type == "VECTOR")
                    val = Eigen::MatrixXd::Zero(dim,1);
                else if (type == "2-TENSOR")
                    val = Eigen::MatrixXd::Zero(dim,dim);
                else if (type == "4-TENSOR")
                    val = Eigen::MatrixXd::Zero((int)3*(dim-1),(int)3*(dim-1));
                else
                    throw std::runtime_error("[ERROR] Type is unrecognized. Select from: SCALAR, VECTOR, 2-TENSOR, and 4-TENSOR");
            }

        AtomicObs obs;
        Eigen::MatrixXd val;
};

template< class AtomicObs >
class PYBIND11_EXPORT LocalObservable : public Observable
{
    public:
        LocalObservable() : Observable("NONE", "SCALAR", true, false, false,3), obs(3) {};
        LocalObservable(std::string _name, std::string _type, bool _islocal, bool _useforce, int _dim, int _gridsize) 
            : Observable(_name, _type, _islocal, true, _useforce, _dim), obs(_dim) 
            {
                val.resize(_gridsize);
                if (type == "SCALAR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero(1,1));
                else if (type == "VECTOR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero(dim,1));
                else if (type == "2-TENSOR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero(dim,dim));
                else if (type == "4-TENSOR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero((int)3*(dim-1),(int)3*(dim-1)));
                else
                    throw std::runtime_error("[ERROR] Type is unrecognized. Select from: SCALAR, VECTOR, and TENSOR");
            };
        virtual void accumulate(const AboriaParticles::value_type& particle_i,
                                double cgval, unsigned int grid_id)
            {
                val[grid_id] += cgval*obs.compute(particle_i);
            }
        virtual void accumulate(const AboriaParticles::value_type& particle_i, 
                                const AboriaParticles::value_type& particle_j, 
                                const std::shared_ptr<PairPotential>& potential,
                                double bondval, unsigned int grid_id)
            {
                val[grid_id] += 0.5*bondval*obs.compute(particle_i, particle_j, potential);
            }
        virtual Eigen::MatrixXd getGlobalValue()
            {
                throw std::runtime_error("[ERROR] Observable Type is Local not Global");
                return Eigen::MatrixXd::Zero(1,1);
            }
        virtual std::vector< Eigen::MatrixXd > getField()
            {
                return val;
            }
        void clear()
            {
                if (type == "SCALAR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero(1,1));
                else if (type == "VECTOR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero(dim,1));
                else if (type == "2-TENSOR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero(dim,dim));
                else if (type == "4-TENSOR")
                    std::fill(val.begin(),val.end(),Eigen::MatrixXd::Zero((int)3*(dim-1),(int)3*(dim-1)));
                else
                    throw std::runtime_error("[ERROR] Type is unrecognized. Select from: SCALAR, VECTOR, 2-TENSOR");
            } 
        AtomicObs obs;
        std::vector< Eigen::MatrixXd > val;
};

void export_Observable(py::module& m)
{
    py::class_<Observable, std::shared_ptr<Observable> >(m,"Observable")
    .def(py::init< std::string, std::string, bool, bool, bool, int >()) 
    .def("getGlobalValue",&Observable::getGlobalValue)
    .def("getField",&Observable::getField)
    .def_readwrite("name", &Observable::name)
    .def_readwrite("type", &Observable::type)
    .def_readwrite("islocal", &Observable::islocal)
    .def_readwrite("useforce", &Observable::useforce)
    .def_readwrite("dim", &Observable::dim)
    ;
};

template<class T>
void export_GlobalObservable(py::module& m, const std::string& name)
{
    py::class_<T, Observable, std::shared_ptr<T> >(m,name.c_str())
    .def(py::init< std::string, std::string, bool, bool, int >()) 
    .def_readwrite("val", &T::val)
    ;
};

template<class T>
void export_LocalObservable(py::module& m, const std::string& name)
{
    py::class_<T, Observable, std::shared_ptr<T> >(m,name.c_str())
    .def(py::init< std::string, std::string, bool, bool, int, int >()) 
    .def_readwrite("val", &T::val)
    ;
};
/*If I need to import these methods. . . .
    //.def("accumulate", (void (T::*)(AboriaParticles::value_type)) &T::accumulate, "Accumulate value of local obs")
    //.def("accumulate", (void (T::*)(AboriaParticles::value_type,AboriaParticles::value_type)) &T::accumulate, "Accumulate value of pair obs")
    //.def("accumulate", (void (T::*)(AboriaParticles::value_type,AboriaParticles::value_type, const std::shared_ptr<PairPotential>&)) &T::accumulate, "Accumulate value of pair obs requiring force calc.")
    //.def("accumulate", (void (T::*)(AboriaParticles::value_type,double,unsigned int)) &T::accumulate, "Accumulate value of local obs")
    //.def("accumulate", (void (T::*)(AboriaParticles::value_type,AboriaParticles::value_type,std::shared_ptr<CoarseGrainFunction>)) &T::accumulate, "Accumulate value of pair obs")
    //.def("accumulate", (void (T::*)(AboriaParticles::value_type,AboriaParticles::value_type, const std::shared_ptr<PairPotential>&, std::shared_ptr<CoarseGrainFunction>)) &T::accumulate, "Accumulate value of pair obs requiring force calc.")
*/
#endif
