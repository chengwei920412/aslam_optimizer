#ifndef ASLAM_BACKEND_MAPPED_DV_QUAT_HPP
#define ASLAM_BACKEND_MAPPED_DV_QUAT_HPP


#include <Eigen/Core>
#include <aslam/backend/DesignVariable.hpp>
#include "RotationExpression.hpp"
#include "RotationExpressionNode.hpp"

namespace aslam {
  namespace backend {
    
    class MappedRotationQuaternion : public RotationExpressionNode, public DesignVariable
    {
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
      MappedRotationQuaternion(double * q);

      virtual ~MappedRotationQuaternion();

      /// \brief Revert the last state update.
      virtual void revertUpdateImplementation();

      /// \brief Update the design variable.
      virtual void updateImplementation(const double * dp, int size);

      /// \brief the size of an update step
      virtual int minimalDimensionsImplementation() const;

      RotationExpression toExpression();

        void set( const Eigen::Vector4d & q){ _q = q; _p_q = q; }
    private:
      virtual Eigen::Matrix3d toRotationMatrixImplementation();
      virtual void evaluateJacobiansImplementation(JacobianContainer & outJacobians) const;
      virtual void evaluateJacobiansImplementation(JacobianContainer & outJacobians, const Eigen::MatrixXd & applyChainRule) const;
      virtual void getDesignVariablesImplementation(DesignVariable::set_t & designVariables) const;

      /// Returns the content of the design variable
      virtual void getParametersImplementation(Eigen::MatrixXd& value) const;

      /// Sets the content of the design variable
      virtual void setParametersImplementation(const Eigen::MatrixXd& value);

        Eigen::Map<Eigen::Vector4d> _q;
        Eigen::Vector4d _p_q;
        Eigen::Matrix3d _C;
      
    };

  } // namespace backend
} // namespace aslam

#endif /* ASLAM_BACKEND_DV_QUAT_HPP */