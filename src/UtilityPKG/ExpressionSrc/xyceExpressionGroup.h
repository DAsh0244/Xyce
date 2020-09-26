

#ifndef xyceExpressionGroup_H
#define xyceExpressionGroup_H

#include<string>
#include<complex>
#include<unordered_map>

#include<newExpression.h>
#include <ExpressionType.h>
#include <expressionGroup.h>
#include <N_UTL_ExtendedString.h>

namespace Xyce {
namespace Util {

#define CONSTCtoK    (273.15)  

//-----------------------------------------------------------------------------
// Class         : xyceExpressionGroup
//
// Purpose       : This is the group class for connecting the new 
//                 expression library to Xyce
//
// Special Notes : For now, this is a lightweight class, which will hopefully be
//                 temporary.  In this class, there will (probably) be a
//                 different unique instance for each newExpression class
//                 instance.   This is necessary because in the current design,
//                 the newExpression object is hiding behind N_UTL_Expression,
//                 and so each instance of N_UTL_Expression will allocate a
//                 newExpression object, and will also create a xyceExpressionGroup
//                 object to use with that object.
//
//                 Long term, there should be a better way to do this.  One drawback
//                 of having a unique group for each expression is that for expressions
//                 that have external dependencies, we want to make sure that variables
//                 don't get updated multiple times.  There are use cases where this
//                 mistake would be easy to do.
//
//                 Long term, it should be possible to have a single group
//                 used by all expressions.  It should also contain all the machinery
//                 necessary to obtain the values that it needs for voltages, currents,
//                 parameters, etc.
//
// Creator       : Eric Keiter
// Creation Date : 2/12/2020
//-----------------------------------------------------------------------------
class xyceExpressionGroup : public baseExpressionGroup
{
public:

  xyceExpressionGroup ();
  ~xyceExpressionGroup () {};
  virtual bool resolveExpression (Xyce::Util::newExpression & exp);

  virtual bool isOption (const std::string & optionStr)
  {
    std::string tmp = optionStr;
    Xyce::Util::toUpper(tmp);
    return false; //  FIX THIS
  }

  virtual bool getSolutionSdt(const std::string & nodeName, double & retval )
  {
    bool success=true;
    std::string tmp = nodeName;
    Xyce::Util::toUpper(tmp);
    retval = 0.0;
    return success; // FIX THIS
  }

  virtual bool getSolutionDdt(const std::string & nodeName, double & retval )
  {
    bool success=true;
    std::string tmp = nodeName;
    Xyce::Util::toUpper(tmp);
    retval = 0.0;
    return success; // FIX THIS
  }

  virtual bool setSolutionVal(const std::string & nodeName, const double & val )
  {
    bool success=true;
    std::string tmp = nodeName;
    Xyce::Util::toUpper(tmp);

    std::vector<std::string>::iterator it = std::find(names_.begin(), names_.end(), tmp);
    if (it != names_.end())
    {
      int index = it - names_.begin();
      dvals_[index] = val;
    }

    return success; // FIX THIS
  }

  virtual bool getSolutionVal(const std::string & nodeName, double & retval );
#if 0
  {
    bool success=true;
    retval = 0.0;
    std::string tmp = nodeName;
    Xyce::Util::toUpper(tmp);

    std::vector<std::string>::iterator it = std::find(names_.begin(), names_.end(), tmp);
    if (it != names_.end())
    {
      int index = it - names_.begin();
      retval = dvals_[index];
      std::cout << "Solution variable " << nodeName << " found by the xyceExpresionGroup! value = " << retval << std::endl;
    }
    else // not found
    {
      std::cout << "ERROR.  Solution variable " << nodeName << " not found by the xyceExpresionGroup!" << std::endl;
    }

    return success; // FIX THIS
  }
#endif

  virtual bool getGlobalParameterVal (const std::string & nodeName, double & retval );
#if 0
  {
    bool success=true;
    retval = 0.0;
    std::string tmp = nodeName;
    Xyce::Util::toUpper(tmp);

    std::vector<std::string>::iterator it = std::find(names_.begin(), names_.end(), tmp);
    if (it != names_.end())
    {
      int index = it - names_.begin();
      retval = dvals_[index];
    }

    return success; // FIX THIS
  }
#endif

  // ERK NOTE:  Need to have a "notify" (or something) for .STEP loops.  
  // Important for time-dependent expressions.
  //
  // Also, for "time" related quantities, should these have names 
  // like "getCurrTimeStep", rather than "getTimeStep"?  Do we ever need much else?
  virtual bool setTimeStep (double dt) { dt_ = dt; return true; } // WAG
  virtual bool setTimeStepAlpha (double a) { alpha_ = a; return true; }

  virtual double getTimeStep () { return dt_; } // WAG
  virtual double getTimeStepAlpha () { return alpha_; }
  virtual double getTimeStepPrefac () { return (getTimeStepAlpha() / getTimeStep ()) ; } // FIX

  virtual bool setTime(double t) { time_ = t; return true; } 
  virtual bool setTemp(double t) { temp_ = t; return true; } 
  virtual bool setVT  (double v) { VT_ = v; return true; } 
  virtual bool setFreq(double f) { freq_ = f; return true; } 

  virtual double getTime() { return time_;} 
  virtual double getTemp() { return temp_;} 
  virtual double getVT  () { return VT_;} 
  virtual double getFreq() { return freq_;} 

  // in a real Xyce group, need something like this:
  //solver_state.bpTol_ = analysis_manager.getStepErrorControl().getBreakPointLess().tolerance_;
  virtual double getBpTol() { return 0.0; }

  virtual bool getFunction    (const std::string & name, Xyce::Util::newExpression & exp);
  virtual bool getParam       (const std::string & name, Xyce::Util::newExpression & exp);
  virtual bool getGlobalParam (const std::string & name, Xyce::Util::newExpression & exp);

  void setNames ( const std::vector<std::string> & names )
  {
    names_ = names;
    for (int ii=0;ii<names_.size();ii++)
    {
      Xyce::Util::toUpper(names_[ii]);
    }
  }

  void setVals( const std::vector<double> & vals )
  {
    dvals_ = vals;
  }

  void setVals( const std::vector<std::complex<double> > & vals )
  {
    cvals_ = vals;
  }

  void addFunction (const std::string & name, Xyce::Util::newExpression & exp);

  const std::vector<std::string> & getNames() { return names_; }

private:

  // don't know if these are best way ...
  std::vector<std::string> names_;
  std::vector< double> dvals_;
  std::vector< std::complex<double> > cvals_;

  std::unordered_map <std::string, Xyce::Util::newExpression  >  functions_;

  double time_, temp_, VT_, freq_;
  double dt_, alpha_;
};

}
}

#endif

