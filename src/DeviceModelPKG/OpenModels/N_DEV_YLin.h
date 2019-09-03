//-------------------------------------------------------------------------
//   Copyright 2002-2019 National Technology & Engineering Solutions of
//   Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
//   NTESS, the U.S. Government retains certain rights in this software.
//
//   This file is part of the Xyce(TM) Parallel Electrical Simulator.
//
//   Xyce(TM) is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   Xyce(TM) is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with Xyce(TM).
//   If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Purpose        :
//
// Special Notes  :
//
// Creator        : Pete Sholander, SNL
//
// Creation Date  : 08/19/2019
//
//
//
//
//-----------------------------------------------------------------------------

#ifndef Xyce_N_DEV_YLin_h
#define Xyce_N_DEV_YLin_h

#include <N_DEV_fwd.h>
#include <N_DEV_Configuration.h>
#include <N_DEV_DeviceInstance.h>
#include <N_DEV_DeviceModel.h>
#include <N_DEV_DeviceMaster.h>
#include <N_UTL_Op.h>

namespace Xyce {
namespace Device {
namespace YLin {

class Model;
class Instance;

struct Traits : public DeviceTraits<Model, Instance>
{
  static const char *name() {return "LIN";}
  static const char *deviceTypeName() {return "LIN level 1";}
  static int numNodes() {return 2;}
  static int numOptionalNodes() {return 20;}
  static bool modelRequired() {return true;}
  static bool isLinearDevice() {return true;}

  static Device *factory(const Configuration &configuration, const FactoryBlock &factory_block);
  static void loadModelParameters(ParametricData<Model> &p);
  static void loadInstanceParameters(ParametricData<Instance> &p);
};

//-----------------------------------------------------------------------------
// Class         : Xyce::Device::YLin::Instance
// Purpose       :
// Special Notes :
// Creator       : Pete Sholander, SNL
// Creation Date : 8/19/2019
//-----------------------------------------------------------------------------

class Instance : public DeviceInstance
{
  friend class ParametricData<Instance>;              ///< Allow ParametricData to changes member values
  friend class Model;
  friend struct Traits;
  friend class Master;
  friend class resistorSensitivity;
  friend class resistorMatrixSensitivity;

public:
  Instance(
     const Configuration &     configuration,
     const InstanceBlock &     instance_block,
     Model &                   model,
     const FactoryBlock &      factory_block);

  /// Destroys this instance
  ~Instance() {}

private:
  Instance(const Instance &);
  Instance &operator=(const Instance &);

public:

  bool isLinearDevice() const;

  /// Gets the resistor model that owns this instance.
  Model &getModel()   { return model_;  }

  virtual void registerLIDs(const std::vector<int> & intLIDVecRef, const std::vector<int> & extLIDVecRef) /* override */;
  virtual void registerStateLIDs(const std::vector<int> & staLIDVecRef) /* override */;
  virtual void registerBranchDataLIDs(const std::vector<int> & branchLIDVecRef) /* override */;
  virtual void registerJacLIDs(const std::vector< std::vector<int> > & jacLIDVec) /* override */;

  virtual void loadNodeSymbols(Util::SymbolTable &symbol_table) const; // override

  virtual bool processParams() /* override */;
  virtual bool updateIntermediateVars() { return true; }
  virtual bool updatePrimaryState() { return true; }

  /// Return Jacobian stamp that informs topology of the layout of the
  /// resistor jacobian.
  virtual const std::vector< std::vector<int> > &jacobianStamp() const  /* override */ {
    return jacStamp;
  }

  virtual bool loadDAEFVector() /* override */;
  virtual bool loadDAEdFdx() /* override */;

  /// Load Q vector
  /// Since the Resistor does no charge storage, this is a no-op.
  virtual bool loadDAEQVector()
  {
    return true;
  }

  /// Load derivative of Q vector with respect to solution vector
  /// Since the Resistor does no charge storage, this is a no-op.
  virtual bool loadDAEdQdx()
  {
    return true;
  }

  virtual void setupPointers() /* override */;

private:
  static std::vector< std::vector<int> >  jacStamp; ///< All Resistor instances have a common Jacobian Stamp
  static void initializeJacobianStamp();

  Model &     model_;                 ///< Owning model

  // User-specified parameters:
  double      R;                      ///< Resistance (ohms)
  double      multiplicityFactor;     ///< multiplicity factor (M)

  ///<   NOT used, only here for compatibility in parsing
  ///<   netlist from simulators that support it
  double      dtemp;                  ///<Temp difference between instance and circuit, in C.
  bool        dtempGiven;             ///< Temp difference given in netlist

  // Derived parameters:
  double      G;                      ///< Conductance(1.0/ohms)
  double      i0;                     ///< Current(ohms)

  int         li_Pos;                 ///< Index for Positive Node
  int         li_Neg;                 ///< Index for Negative Node
  int         li_branch_data;         ///< Index for Lead Current and junction voltage (for power calculations)

  // Offset variables corresponding to the above declared indices.
  int         APosEquPosNodeOffset;   ///< Column index into force matrix of Pos/Pos conductance
  int         APosEquNegNodeOffset;   ///< Column index into force matrix of Pos/Neg conductance
  int         ANegEquPosNodeOffset;   ///< Column index into force matrix of Neg/Pos conductance
  int         ANegEquNegNodeOffset;   ///< Column index into force matrix of Neg/Neg conductance

#ifndef Xyce_NONPOINTER_MATRIX_LOAD
  // Pointers for Jacobian
  double *    f_PosEquPosNodePtr;
  double *    f_PosEquNegNodePtr;
  double *    f_NegEquPosNodePtr;
  double *    f_NegEquNegNodePtr;
#endif

};


//-----------------------------------------------------------------------------
// Class         : Xyce::Device::YLin::Model
// Purpose       :
// Special Notes :
// Creator       : Pete Sholander, SNL
// Creation Date : 8/19/2019
//-----------------------------------------------------------------------------
///
/// YLIN model class
///
class Model : public DeviceModel
{
  friend class ParametricData<Model>;               ///< Allow ParametricData to changes member values
  friend class Instance;                            ///< Don't force a lot of pointless getters
  friend struct Traits;
  friend class Master;                              ///< Don't force a lot of pointless getters

public:
  typedef std::vector<Instance *> InstanceVector;

  Model(
     const Configuration &       configuration,
     const ModelBlock &        model_block,
     const FactoryBlock &      factory_block);
  ~Model();

private:
  Model();
  Model(const Model &);
  Model &operator=(const Model &);

public:

  //---------------------------------------------------------------------------
  // Function      : Xyce::Device::YLin::Model::addInstance
  // Purpose       :
  // Special Notes :
  // Scope         : public
  // Creator       : David Baur
  // Creation Date : 8/12/2013
  //---------------------------------------------------------------------------
  ///
  /// Add an instance to the list of instances associated with this model
  ///
  /// @author David G. Baur  Raytheon  Sandia National Laboratories 1355
  /// @date   8/12/2013
  void addInstance(Instance *instance)
  {
    instanceContainer.push_back(instance);
  }

  void setupBaseInstanceContainer()
  {
    std::vector<Instance*>::iterator iter = instanceContainer.begin();
    std::vector<Instance*>::iterator end   = instanceContainer.end();
    for ( ; iter!=end; ++iter)
    {
      Xyce::Device::DeviceModel::baseInstanceContainer.push_back( static_cast<Xyce::Device::DeviceInstance *>(*iter) );
    }
  }

  virtual void forEachInstance(DeviceInstanceOp &op) const /* override */;

  virtual std::ostream &printOutInstances(std::ostream &os) const;

  virtual bool processParams() /* override */;
  virtual bool processInstanceParams() /* override */;

  bool readTouchStoneFile();
  void splitTouchStoneFileLine(const std::string& aLine, IO::TokenVector & parsedLine);
  void readTouchStoneFileLine(std::istream & in, std::string& line, int& lineNum);

private:
  InstanceVector      instanceContainer;            ///< List of owned resistor instances

  // variables specific to the YLIN model
  std::string            TSFileName_;       ///< Name of the Touchstone file
  bool                   TSFileNameGiven_;
  char                   TSCommentChar_;    ///< This is the ! character
  std::string            TSVersion_;        ///< Touchstone file format.  We only support "2.0".
  std::string            freqUnit_;         ///< Frequency unit in input Network Data.  Legal
                                            ///< values are Hz, kHz, MHz, and GHz. The default
                                            ///< value is GHz.
  double                 freqMultiplier_;
  char                   paramType_;        ///< 'S', Y' or 'Z'.  The default is 'S'
  std::string            dataFormat_;       ///< Format of the input network data.  Legal values
                                            ///< are: "RI", 'MA" or "DB".  The default is "MA".
  int                    numPorts_;         ///< Number of ports
  std::string            twoPortDataOrder_; ///< This is used for 2-port networks.
                                            ///< It is either "12_21 or "21_12"
  int                    numFreq_;          ///< Number of frequency points
  std::vector<double>    Z0Vec_;            ///< vector of impedances (for S-parameters)
  std::vector<double>    freqVec_;          ///< vector of the frequencies in the input Network Data
  std::vector<Teuchos::SerialDenseMatrix<int, std::complex<double> > > inputNetworkDataVec_;
};


//-----------------------------------------------------------------------------
// Class         : Xyce::Device::YLin::Master
// Purpose       :
// Special Notes :
// Creator       : Pete Sholander, SNL
// Creation Date : 8/19/2019
//-----------------------------------------------------------------------------
class Master : public DeviceMaster<Traits>
{
  friend class Instance;                            ///< Don't force a lot of pointless getters
  friend class Model;                               ///< Don't force a lot of pointless getters

public:

  //---------------------------------------------------------------------------
  // Function      : Xyce::Device::YLin::Master::Master
  // Purpose       :
  // Special Notes :
  // Scope         : public
  // Creator       : Pete Sholander, SNL
  // Creation Date : 8/19/2019
  //---------------------------------------------------------------------------
  Master(
     const Configuration &     configuration,
     const FactoryBlock &      factory_block,
     const SolverState &       solver_state,
     const DeviceOptions &     device_options)
   : DeviceMaster<Traits>(configuration, factory_block, solver_state, device_options)
  {}

  virtual void storeInstance(const FactoryBlock& factory_block,
                             InstanceType* instance);

  virtual bool updateState(double * solVec, double * staVec, double * stoVec)
  {
    return true;
  }

  virtual bool updateSecondaryState (double * staDerivVec, double * stoVec)
  {
    return true;
  }

  virtual bool loadDAEVectors (double * solVec, double * fVec, double * qVec,
                               double * bVec, double * leadF, double * leadQ,
                               double * junctionV)
  {
    return loadDAEVectors( solVec, fVec, qVec, bVec,
                           leadF, leadQ, junctionV, ALL );
  }

  virtual bool loadDAEVectors (double * solVec, double * fVec, double * qVec,
                               double * bVec, double * leadF, double * leadQ,
                               double * junctionV, int loadType);

  virtual bool loadDAEMatrices(Linear::Matrix & dFdx, Linear::Matrix & dQdx)
  {
    return loadDAEMatrices( dFdx, dQdx, ALL );
  }

  virtual bool loadDAEMatrices(Linear::Matrix & dFdx, Linear::Matrix & dQdx,
                               int loadType);

  virtual bool loadFreqDAEVectors(double frequency,
                                  std::complex<double>* solVec,
                                  std::vector<Util::FreqVecEntry>& fVec,
                                  std::vector<Util::FreqVecEntry>& bVec);

  virtual bool loadFreqDAEMatrices(double frequency,
                                   std::complex<double>* solVec,
                                   std::vector<Util::FreqMatEntry>& dFdx);

  private:
    InstanceVector      linearInstances_;            ///< List of owned linear resistor instances
    InstanceVector      nonlinearInstances_;         ///< List of owned nonlinear resistor instances
};

void registerDevice(const DeviceCountMap& deviceMap = DeviceCountMap(),
                    const std::set<int>& levelSet = std::set<int>());

} // namespace YLin
} // namespace Device
} // namespace Xyce

#endif // Xyce_N_DEV_YLin_h