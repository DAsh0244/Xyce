//-------------------------------------------------------------------------
//   Copyright 2002-2020 National Technology & Engineering Solutions of
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
// Purpose        : Builder for PCE specific linear objects
//
// Special Notes  :
//
// Creator        : Eric Keiter, SNL
//
// Creation Date  : 06/27/2019
//
//
//
//
//-----------------------------------------------------------------------------

#ifndef  Xyce_LAS_PCEBUILDER_H
#define  Xyce_LAS_PCEBUILDER_H

// ---------- Standard Includes ----------

#include <string>
#include <vector>

// ----------   Xyce Includes   ----------
#include <N_LAS_fwd.h>

#include <N_LAS_Builder.h>

#include <Teuchos_RCP.hpp>

// ---------- Forward Declarations ----------


class N_PDS_ParMap;

namespace Xyce {
namespace Linear {

//-----------------------------------------------------------------------------
// Class         : PCEBuilder
// Purpose       :
//
// Special Notes : Unlike some other block-analysis builders, this builder 
//                 supports two different block structures.  
//
//                 (1) The PCE expansion.  This is the main one, which 
//                 creates the block system objects used by the nonlinear 
//                 solver.  This structure is used by all the default 
//                 functions such as "createMatrix", "createVector" etc.
//
//                 (2) The quadrature points, which are needed in order 
//                 to evaluate the integrals that are computed to set up 
//                 the matrix and vector entries into the PCE block system 
//                 described by (1).  This structure is used by functions 
//                 and data with the word "quad" in the name, such as 
//                 "createQuadVector".  The only part of the code that 
//                 will use these objects is the PCELoader.
//
// Creator       : Eric Keiter, SNL
// Creation Date : 06/27/2019
//-----------------------------------------------------------------------------
class PCEBuilder : public Builder
{
 public:

  // Default Constructor
  PCEBuilder( const int Size, const int quadPointsSize);

  // Destructor
  ~PCEBuilder() {}

  // Vector and Matrix creators

  // Vector factory 
  Vector * createVector() const;
  BlockVector * createQuadVector() const;

  // State Vector factory 
  Vector * createStateVector() const;

  // Store Vector factory 
  Vector * createStoreVector() const;

  // Lead Current Vector factory 
  Vector * createLeadCurrentVector() const;

  // Matrix factory
  Matrix * createMatrix() const;
  BlockMatrix * createQuadMatrix() const;

  bool generateMaps( const Teuchos::RCP<N_PDS_ParMap>& BaseMap, 
                     const Teuchos::RCP<N_PDS_ParMap>& oBaseMap );

  bool generateStateMaps( const Teuchos::RCP<N_PDS_ParMap>& BaseStateMap );
  bool generateStoreMaps( const Teuchos::RCP<N_PDS_ParMap>& BaseStoreMap );
  bool generateLeadCurrentMaps( const Teuchos::RCP<N_PDS_ParMap>& BaseLeadCurrentMap );

  bool generateGraphs( 
    const Graph& pceGraph,
    const Graph& baseFullGraph );

  // Return maps for sampling linear system.
  Teuchos::RCP<const N_PDS_ParMap> getSolutionMap() const
  { return( PCEMap_ ); }

  Teuchos::RCP<N_PDS_ParMap> getSolutionMap()
  { return( PCEMap_ ); }

  Teuchos::RCP<N_PDS_ParMap> getSolutionOverlapMap() const
  { return oPCEMap_; }

  // Return the base map for each block in the expanded maps (a.k.a. time-domain maps)
  Teuchos::RCP<const N_PDS_ParMap> getBaseSolutionMap() const
  { return BaseMap_; }

  Teuchos::RCP<const N_PDS_ParMap> getBaseStateMap() const
  { return BaseStateMap_; }

  Teuchos::RCP<const N_PDS_ParMap> getBaseStoreMap() const
  { return BaseStoreMap_; }
  
  Teuchos::RCP<const N_PDS_ParMap> getBaseLeadCurrentMap() const
  { return BaseLeadCurrentMap_; }

  // Return GID offset for blocks for construction of Loader
  int getPCEOffset()
  { return offset_; }

  int getPCEStateOffset()
  { return stateOffset_; }

  int getPCEStoreOffset()
  { return storeOffset_; }
  
  int getPCELeadCurrentOffset()
  { return leadCurrentOffset_; }

  int getNumBlockRows ()
  { return numBlockRows_; }

private:
  const int numBlockRows_;
  const int numQuadPoints_;
  int numSolVariables_, numStateVariables_;
  int numStoreVariables_;
  int numLeadCurrentVariables_;

  int offset_, stateOffset_;
  int storeOffset_;
  int leadCurrentOffset_;

  // PCE maps for block vectors (BV):
  Teuchos::RCP<N_PDS_ParMap> BaseMap_, oBaseMap_;

  Teuchos::RCP<N_PDS_ParMap> BaseStateMap_;
  Teuchos::RCP<N_PDS_ParMap> BaseStoreMap_;
  Teuchos::RCP<N_PDS_ParMap> BaseLeadCurrentMap_;

  Teuchos::RCP<Graph> pceGraph_;
  Teuchos::RCP<Graph> baseFullGraph_;
  Teuchos::RCP<Graph> blockGraph_;
  Teuchos::RCP<Graph> quadBlockGraph_;

  std::vector<std::vector<int> > blockPattern_;
  std::vector<std::vector<int> > quadBlockPattern_;

  Teuchos::RCP<N_PDS_ParMap> PCEMap_, oPCEMap_;
  Teuchos::RCP<N_PDS_ParMap> quadMap_, oquadMap_;

  Teuchos::RCP<N_PDS_ParMap> PCEStateMap_;
  Teuchos::RCP<N_PDS_ParMap> quadStateMap_;

  Teuchos::RCP<N_PDS_ParMap> PCEStoreMap_;
  Teuchos::RCP<N_PDS_ParMap> quadStoreMap_;

  Teuchos::RCP<N_PDS_ParMap> PCELeadCurrentMap_;
  Teuchos::RCP<N_PDS_ParMap> quadLeadCurrentMap_;
};

} // namespace Linear
} // namespace Xyce

#endif
