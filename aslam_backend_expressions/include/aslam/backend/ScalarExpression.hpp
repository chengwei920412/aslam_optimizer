#ifndef ASLAM_SCALAR_EXPRESSION_HPP
#define ASLAM_SCALAR_EXPRESSION_HPP

#include <Eigen/Core>
#include <boost/shared_ptr.hpp>
#include <aslam/backend/JacobianContainer.hpp>
#include <aslam/backend/Differential.hpp>
#include <set>

namespace aslam {
  namespace backend {

    using std::sqrt;
    using std::log;

    class ScalarExpressionNode;
    
    class ScalarExpression
    {
    public:
      enum { Dimension = 1 };
      typedef double Value;
      typedef double value_t;
      typedef ScalarExpressionNode node_t;

      ScalarExpression( double value );
      ScalarExpression(ScalarExpressionNode * designVariable);
      ScalarExpression(boost::shared_ptr<ScalarExpressionNode> designVariable);
      ~ScalarExpression();
      
      double toScalar() const;
      double toValue() const { return toScalar(); }
      double evaluate() const { return toScalar(); }

      void evaluateJacobians(JacobianContainer & outJacobians) const;
      void evaluateJacobians(JacobianContainer & outJacobians, const Eigen::MatrixXd & applyChainRule) const;
      void getDesignVariables(DesignVariable::set_t & designVariables) const;

      boost::shared_ptr<ScalarExpressionNode> root() const { return _root; }
      bool isEmpty() const { return false; }  //TODO feature: support empty scalar expression

      ScalarExpression operator+(const ScalarExpression & s) const;
      ScalarExpression operator-(const ScalarExpression & s) const;
      ScalarExpression operator*(const ScalarExpression & s) const;
      ScalarExpression operator/(const ScalarExpression & s) const;
      ScalarExpression operator+(double s) const;
      ScalarExpression operator-(double s) const;
      ScalarExpression operator-() const;
      ScalarExpression operator*(double s) const;
      ScalarExpression operator/(double s) const;

    private:
      /// \todo make the default constructor private.
      ScalarExpression();

      boost::shared_ptr<ScalarExpressionNode> _root;

      friend class EuclideanExpression;

    };
    
    ScalarExpression sqrt(const ScalarExpression& e);
    ScalarExpression log(const ScalarExpression& e);

  } // namespace backend
} // namespace aslam


#endif /* ASLAM_SCALAR_EXPRESSION_HPP */
