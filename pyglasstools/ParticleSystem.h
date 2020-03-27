#ifndef __SYSTEM_DATA_H__
#define __SYSTEM_DATA_H__

#include "extern/pybind11/include/pybind11/pybind11.h"
#include "extern/pybind11/include/pybind11/stl.h"
#include "PairPotential.h"
#include "SimBox.h"
#include <Aboria.h>
namespace py = pybind11;
using namespace Aboria;

ABORIA_VARIABLE(velocity, vdouble3, "velocity");
ABORIA_VARIABLE(mass, double, "mass");
ABORIA_VARIABLE(diameter, double, "diameter");
typedef typename Particles< std::tuple<velocity, diameter, mass> >::position position;        

//template<class PairPotential>
class PYBIND11_EXPORT ParticleSystem
{
    public:
        std::shared_ptr<SimBox> m_simbox;
        std::shared_ptr<PairPotential> m_potential;
        Particles< std::tuple<velocity, diameter, mass> > m_particles;
        
        ParticleSystem( std::shared_ptr< SimBox > simbox,std::shared_ptr< PairPotential > potential,
                        unsigned int numparticles, std::vector<double> atomdiameter, 
                        std::vector<double> atommass, std::vector< std::vector<double> > atomposition, 
                        std::vector< std::vector<double> > atomvelocity)
                        :   m_simbox(simbox), m_potential(potential), m_particles(numparticles), 
                            m_numparticles(numparticles), m_atomposition(atomposition), m_atomvelocity(atomvelocity)  
        {
            get<diameter>(m_particles) = atomdiameter;
            get<mass>(m_particles) = atommass;

            for(unsigned int i=0; i < numparticles; i++)
            {
                get<position>(m_particles[i]) = vdouble3(atomposition[i][0], atomposition[i][1], atomposition[i][2]);
                get<velocity>(m_particles[i]) = vdouble3(atomvelocity[i][0], atomvelocity[i][1], atomvelocity[i][2]);
            }
            vdouble3 boxmax = vdouble3(m_simbox->getUpperBound(0), m_simbox->getUpperBound(1), m_simbox->getUpperBound(2));
            vdouble3 boxmin = vdouble3(m_simbox->getLowerBound(0), m_simbox->getLowerBound(1), m_simbox->getLowerBound(2));
            vbool3 periodic = vbool3((bool)m_simbox->getPeriodic(0),(bool)m_simbox->getPeriodic(1),(bool)m_simbox->getPeriodic(2));
            m_particles.init_neighbour_search(boxmin, boxmax, periodic);
        };
        ~ParticleSystem(){};

        void setMass(std::vector<double> atommass)
        {
            if (m_numparticles != atommass.size() )
                throw std::invalid_argument("[ERROR]: Size of mass array mismatch with # of particles!");
            else
                get<mass>(m_particles) = atommass;
        };
        std::vector<double> getMass()
        {
            return get<mass>(m_particles); 
        };
        
        void setDiameter(std::vector<double> atomdiameter)
        {
            if (m_numparticles != atomdiameter.size() )
                throw std::invalid_argument("[ERROR]: Size of diameter array mismatch with # of particles!");
            else
                get<diameter>(m_particles) = atomdiameter;
        };
        std::vector<double> getDiameter()
        {
            return get<diameter>(m_particles); 
        };
        
        void setAtomPosition(std::vector< std::vector<double> > atomposition)
        {
            if (m_numparticles != atomposition.size() )
                throw std::invalid_argument("[ERROR]: Size of position array mismatch with # of particles!");
            else
            {
                m_atomposition = atomposition;
                for(unsigned int i=0; i < m_numparticles; i++)
                {
                    get<position>(m_particles[i]) = vdouble3(atomposition[i][0],atomposition[i][1],atomposition[i][2]);
                }
            }
        };
        
        std::vector< std::vector<double> > getAtomPosition()
        {
            return m_atomposition; 
        };
        
        void setAtomVelocity(std::vector< std::vector<double> > atomvelocity)
        {
            if (m_numparticles != atomvelocity.size() )
                throw std::invalid_argument("[ERROR]: Size of velocity array mismatch with # of particles!");
            else
            {
                m_atomvelocity = atomvelocity;
                for(unsigned int i=0; i < m_numparticles; i++)
                {
                    get<velocity>(m_particles[i]) = vdouble3(atomvelocity[i][0],atomvelocity[i][1],atomvelocity[i][2]);
                }
            }
        };

        std::vector< std::vector<double> > getAtomVelocity()
        {
            return m_atomvelocity; 
        };
        
        unsigned int getN()
        {
            return m_numparticles; 
        };
        std::vector<unsigned int> getNeighbors(std::vector<double> point, double radius)
        {

            std::vector<unsigned int> particleID;
            for(    auto particle = euclidean_search(  m_particles.get_query(), vdouble3(point[0],point[1],point[2]), radius); 
                    particle != false; ++particle)
            {
                particleID.push_back(get<id>(*particle));
            }
            return particleID;
        };
    private: 
        //Atomic properties, fed in to the system
        unsigned int m_numparticles;
        
        //positions and velocities in terms of vectors
        std::vector< std::vector<double> > m_atomposition;        
        std::vector< std::vector<double> > m_atomvelocity;        
};

//an export function here
void export_ParticleSystem(py::module& m)
{
    py::class_<ParticleSystem, std::shared_ptr<ParticleSystem> >(m,"ParticleSystem")
    .def(py::init<  std::shared_ptr< SimBox >, std::shared_ptr< PairPotential >,
                     unsigned int, std::vector<double>, std::vector<double>, 
                    std::vector< std::vector<double> >, std::vector< std::vector<double> > >())
    .def("getMass", &ParticleSystem::getMass)
    .def("setMass", &ParticleSystem::setMass)
    .def("getDiameter", &ParticleSystem::getDiameter)
    .def("setDiameter", &ParticleSystem::setDiameter)
    .def("getAtomPosition", &ParticleSystem::getAtomPosition)
    .def("setAtomPosition", &ParticleSystem::setAtomPosition)
    .def("getAtomVelocity", &ParticleSystem::getAtomVelocity)
    .def("setAtomVelocity", &ParticleSystem::setAtomVelocity)
    .def("getNeighbors", &ParticleSystem::getNeighbors)
    ;
};
#endif //__SYSTEM_DATA_H__
