#include <sm/eigen/gtest.hpp>
#include <sm/eigen/NumericalDiff.hpp>
#include <aslam/backend/EuclideanExpression.hpp>
#include <sm/kinematics/rotations.hpp>
#include <sm/kinematics/quaternion_algebra.hpp>
#include <aslam/backend/RotationQuaternion.hpp>
#include <aslam/backend/EuclideanPoint.hpp>
#include <aslam/backend/RotationExpression.hpp>
#include <aslam/backend/EuclideanDirection.hpp>

using namespace aslam::backend;
using namespace sm::kinematics;

struct EuclideanExpressionNodeFunctor
{
  typedef Eigen::Vector3d value_t;
  typedef value_t::Scalar scalar_t;
  typedef Eigen::VectorXd input_t;
  typedef Eigen::MatrixXd jacobian_t;

  
  EuclideanExpressionNodeFunctor(EuclideanExpression dv) :
    _dv(dv) {}

  input_t update(const input_t & x, int c, scalar_t delta) { input_t xnew = x; xnew[c] += delta; return xnew; }

  EuclideanExpression _dv;

  Eigen::VectorXd operator()(const Eigen::VectorXd & dr)
  {
    
    Eigen::Vector3d p = _dv.toEuclidean();
    JacobianContainer J(3);
    _dv.evaluateJacobians(J);

    int offset = 0;
    for(size_t i = 0; i < J.numDesignVariables(); i++)
      {
	DesignVariable * d = J.designVariable(i);
	d->update(&dr[offset],d->minimalDimensions());
	offset += d->minimalDimensions();
      }

    p = _dv.toEuclidean();
    
 
    for(size_t i = 0; i < J.numDesignVariables(); i++)
      {
	DesignVariable * d = J.designVariable(i);
	d->revertUpdate();
      }

    return p;
   
  }
};


void testJacobian(EuclideanExpression dv)
{
  EuclideanExpressionNodeFunctor functor(dv);
  
  sm::eigen::NumericalDiff<EuclideanExpressionNodeFunctor> numdiff(functor);
  
  /// Discern the size of the jacobian container
  Eigen::Vector3d p = dv.toEuclidean();
  JacobianContainer Jc(3);
  JacobianContainer Jccr(3);
  dv.evaluateJacobians(Jc);
  dv.evaluateJacobians(Jccr, Eigen::Matrix3d::Identity());
   
  Eigen::VectorXd dp(Jc.cols());
  dp.setZero();
  Eigen::MatrixXd Jest = numdiff.estimateJacobian(dp);
 
  sm::eigen::assertNear(Jc.asSparseMatrix(), Jest, 1e-6, SM_SOURCE_FILE_POS, "Testing the quat Jacobian");

  sm::eigen::assertNear(Jccr.asSparseMatrix(), Jest, 1e-6, SM_SOURCE_FILE_POS, "Testing the quat Jacobian");
}





// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testPoint)
{
  try 
    {
      using namespace sm::kinematics;
      EuclideanPoint point(Eigen::Vector3d::Random());
      point.setActive(true);
      point.setBlockIndex(0);
      EuclideanExpression qr(&point);
      
      SCOPED_TRACE("");
      SCOPED_TRACE("");
      testJacobian(qr);
    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
}


// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testRotatedPoint)
{
  try 
    {
      using namespace sm::kinematics;
      RotationQuaternion quat(quatRandom());
      quat.setActive(true);
      quat.setBlockIndex(0);
      RotationExpression C(&quat);


      EuclideanPoint point(Eigen::Vector3d::Random());
      point.setActive(true);
      point.setBlockIndex(1);
      EuclideanExpression p(&point);
      
      EuclideanExpression Cp = C * p;
      
      SCOPED_TRACE("");
      testJacobian(Cp);

      sm::eigen::assertNear(Cp.toEuclidean(), C.toRotationMatrix() * p.toEuclidean(), 1e-14, SM_SOURCE_FILE_POS, "Testing the result is unchanged");
    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
}


// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testRotatedInverse)
{
  try 
    {
      using namespace sm::kinematics;
      RotationQuaternion quat(quatRandom());
      quat.setActive(true);
      quat.setBlockIndex(0);
      RotationExpression C(&quat);


      EuclideanPoint point(Eigen::Vector3d::Random());
      point.setActive(true);
      point.setBlockIndex(1);
      EuclideanExpression p(&point);
      
      EuclideanExpression Cp = C.inverse() * p;
      
      SCOPED_TRACE("");
      testJacobian(Cp);

      sm::eigen::assertNear(Cp.toEuclidean(), C.toRotationMatrix().transpose() * p.toEuclidean(), 1e-14, SM_SOURCE_FILE_POS, "Testing the result is unchanged");
    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
} 


// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testRotationExpression1)
{
  try 
    {
      using namespace sm::kinematics;
      RotationQuaternion quat0(quatRandom());
      quat0.setActive(true);
      quat0.setBlockIndex(0);
      RotationExpression C0(&quat0);

      using namespace sm::kinematics;
      RotationQuaternion quat1(quatRandom());
      quat1.setActive(true);
      quat1.setBlockIndex(1);
      RotationExpression C1(&quat1);


      EuclideanPoint point(Eigen::Vector3d::Random());
      point.setActive(true);
      point.setBlockIndex(2);
      EuclideanExpression p(&point);
      
      RotationExpression C01 = C0 * C1;
      EuclideanExpression C01p = C01 * p;
      
      SCOPED_TRACE("");
      testJacobian(C01p);

      sm::eigen::assertNear(C01p.toEuclidean(), C0.toRotationMatrix() * C1.toRotationMatrix() * p.toEuclidean(), 1e-14, SM_SOURCE_FILE_POS, "Testing the result is unchanged");
    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
} 

// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testRotationExpression2)
{
  try 
    {
      using namespace sm::kinematics;
      RotationQuaternion quat0(quatRandom());
      quat0.setActive(true);
      quat0.setBlockIndex(0);
      RotationExpression C0(&quat0);

      using namespace sm::kinematics;
      RotationQuaternion quat1(quatRandom());
      quat1.setActive(true);
      quat1.setBlockIndex(1);
      RotationExpression C1(&quat1);


      EuclideanPoint point(Eigen::Vector3d::Random());
      point.setActive(true);
      point.setBlockIndex(2);
      EuclideanExpression p(&point);
      
      EuclideanExpression C1p = C1 * p;
      EuclideanExpression C01p = C0 * C1p;
      
      SCOPED_TRACE("");
      testJacobian(C01p);

      sm::eigen::assertNear(C01p.toEuclidean(), C0.toRotationMatrix() * C1.toRotationMatrix() * p.toEuclidean(), 1e-14, SM_SOURCE_FILE_POS, "Testing the result is unchanged");
    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
} 

// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testEuclideanCrossproduct)
{
  try
    {
      using namespace sm::kinematics;
      EuclideanPoint point1(Eigen::Vector3d::Random());
      point1.setActive(true);
      point1.setBlockIndex(1);
      EuclideanExpression p1(&point1);

      EuclideanPoint point2(Eigen::Vector3d::Random());
      point2.setActive(true);
      point2.setBlockIndex(2);
      EuclideanExpression p2(&point2);

      EuclideanExpression p_cross = p1.cross(p2);

      SCOPED_TRACE("");
      testJacobian(p_cross);

    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
}

// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testEuclideanAddition)
{
  try
    {
      using namespace sm::kinematics;
      EuclideanPoint point1(Eigen::Vector3d::Random());
      point1.setActive(true);
      point1.setBlockIndex(1);
      EuclideanExpression p1(&point1);

      EuclideanPoint point2(Eigen::Vector3d::Random());
      point2.setActive(true);
      point2.setBlockIndex(2);
      EuclideanExpression p2(&point2);

      EuclideanExpression p_add = p1 + p2;

      SCOPED_TRACE("");
      testJacobian(p_add);

    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
}

// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testEuclideanSubtraction)
{
  try
    {
      using namespace sm::kinematics;
      EuclideanPoint point1(Eigen::Vector3d::Random());
      point1.setActive(true);
      point1.setBlockIndex(1);
      EuclideanExpression p1(&point1);

      EuclideanPoint point2(Eigen::Vector3d::Random());
      point2.setActive(true);
      point2.setBlockIndex(2);
      EuclideanExpression p2(&point2);

      EuclideanExpression p_diff = p1 - p2;

      SCOPED_TRACE("");
      testJacobian(p_diff);

    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
}

// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testVectorSubtraction)
{
  try
    {
      using namespace sm::kinematics;
      EuclideanPoint point1(Eigen::Vector3d::Random());
      point1.setActive(true);
      point1.setBlockIndex(1);
      EuclideanExpression p1(&point1);

      Eigen::Vector3d p2(Eigen::Vector3d::Random());

      EuclideanExpression p_diff = p1 - p2;

      SCOPED_TRACE("");
      testJacobian(p_diff);

    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
}

// Test that the jacobian matches the finite difference jacobian
TEST(EuclideanExpressionNodeTestSuites, testEuclideanDrection)
{
  try
    {
      using namespace sm::kinematics;
      Eigen::Vector3d p = Eigen::Vector3d::Random() * 10;
      EuclideanDirection point1(p);
      ASSERT_DOUBLE_MX_EQ(p, point1.toEuclidean(),1e-2, "Checking if the euclidean point is recovered correctly")

      point1.setActive(true);
      point1.setBlockIndex(1);
      EuclideanExpression p1 = point1.toExpression();

      //Eigen::Vector3d p2(Eigen::Vector3d::Random());
      
      //EuclideanExpression p_diff = p1 - p2;

      SCOPED_TRACE("");
      testJacobian(p1);

    }
  catch(std::exception const & e)
    {
      FAIL() << e.what();
    }
}
