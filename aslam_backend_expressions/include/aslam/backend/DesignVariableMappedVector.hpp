#ifndef ASLAM_BACKEND_DESIGN_VARIABLE_MAPPED_VECTOR_HPP
#define ASLAM_BACKEND_DESIGN_VARIABLE_MAPPED_VECTOR_HPP

#include <aslam/Exceptions.hpp>
#include <aslam/backend/JacobianContainer.hpp>
#include <aslam/backend/DesignVariable.hpp>
#include "VectorExpressionNode.hpp"
#include "VectorExpression.hpp"

namespace aslam {
  namespace backend {
    template<int D>
    class DesignVariableMappedVector : public DesignVariable, public VectorExpressionNode<D>
    {
    public:
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW
      typedef Eigen::Matrix<double, D, 1> vector_t;

      /// \brief initialize with a pointer to the mapped memory. This must be 
      ///        a contiguous block of size D.
      DesignVariableMappedVector(double * mappedMemory);
      
      /// \brief initialize from an existing map.
      DesignVariableMappedVector(Eigen::Map< vector_t > v);

      virtual ~DesignVariableMappedVector();

      vector_t value() const;

      VectorExpression<D> toExpression();

      void updateMap(double * p);
    protected:
      /// \brief Revert the last state update.
      virtual void revertUpdateImplementation();
      /// \brief Update the design variable.
      virtual void updateImplementation(const double * dp, int size);
      /// \brief what is the number of dimensions of the perturbation variable.
      virtual int minimalDimensionsImplementation() const;

      virtual vector_t evaluateImplementation() const;
      virtual void evaluateJacobiansImplementation(JacobianContainer & outJacobians) const;
      virtual void evaluateJacobiansImplementation(JacobianContainer & outJacobians, const Eigen::MatrixXd & applyChainRule) const;
      virtual void getDesignVariablesImplementation(DesignVariable::set_t & designVariables) const;

    private:
      Eigen::Map< vector_t > _v;
      vector_t _p_v;
      
    };


  } // namespace backend
} // namespace aslam

#include "implementation/DesignVariableMappedVector.hpp"

#endif /* ASLAM_BACKEND_DESIGN_VARIABLE_MAPPED_VECTOR_HPP */
