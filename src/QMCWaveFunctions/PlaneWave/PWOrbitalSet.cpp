//////////////////////////////////////////////////////////////////
// -*- C++ -*-
#include "Message/Communicate.h"
#include "QMCWaveFunctions/PlaneWave/PWOrbitalSet.h"
#include "Numerics/MatrixOperators.h"

namespace qmcplusplus {
  PWOrbitalSet::~PWOrbitalSet() {
#if !defined(ENABLE_SMARTPOINTER)
    if(OwnBasisSet&&myBasisSet) delete myBasisSet;
#endif
  }

  void PWOrbitalSet::reset()
  {
  }

  void PWOrbitalSet::setOrbitalSetSize(int norbs)
  {
  }

  void PWOrbitalSet::resetTargetParticleSet(ParticleSet& P) {
    cout << "resetTargetParticleSet not yet coded." << endl;
    OHMMS::Controller->abort();
  }
  void 
  PWOrbitalSet::evaluate(const ParticleSet& P, int iat, ValueVector_t& psi)
  {
    //Evaluate every orbital for particle iat.
    //Evaluate the basis-set at these coordinates:
    myBasisSet->evaluate(P,iat);
    MatrixOperators::product(C,myBasisSet->Zv,&psi[0]);
  }

  void 
  PWOrbitalSet::evaluate(const ParticleSet& P, int iat, 
          ValueVector_t& psi, GradVector_t& dpsi, ValueVector_t& d2psi)
  {
    //Evaluate the orbitals and derivatives for particle iat only.
    myBasisSet->evaluateAll(P,iat);
    MatrixOperators::product(C,myBasisSet->Z,Temp);
    const ValueType* restrict tptr=Temp.data();
    for(int j=0; j< OrbitalSetSize; j++, tptr+=PW_MAXINDEX) {
      psi[j]    =tptr[PW_VALUE];
      d2psi[j]  =tptr[PW_LAP];
      dpsi[j]=GradType(tptr[PW_GRADX],tptr[PW_GRADY],tptr[PW_GRADZ]);
    }
  }
    
  void 
  PWOrbitalSet::evaluate(const ParticleSet& P, int first, int last,
        ValueMatrix_t& logdet, GradMatrix_t& dlogdet, ValueMatrix_t& d2logdet)
  {
    for(int iat=first,i=0; iat<last; iat++,i++){
      myBasisSet->evaluateAll(P,iat);
      MatrixOperators::product(C,myBasisSet->Z,Temp);
      const ValueType* restrict tptr=Temp.data();
      for(int j=0; j< OrbitalSetSize; j++,tptr+=PW_MAXINDEX) {
        logdet(j,i)= tptr[PW_VALUE];
        d2logdet(i,j)= tptr[PW_LAP];
        dlogdet(i,j)=GradType(tptr[PW_GRADX],tptr[PW_GRADY],tptr[PW_GRADZ]);
      }
    }
  }
}
