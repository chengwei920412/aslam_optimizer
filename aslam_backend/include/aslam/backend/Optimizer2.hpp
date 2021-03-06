#ifndef ASLAM_BACKEND_OPTIMIZER_2_HPP
#define ASLAM_BACKEND_OPTIMIZER_2_HPP


#include <boost/shared_ptr.hpp>
//#include <boost/function.hpp>
#include <sm/assert_macros.hpp>
#include <Eigen/Core>
#include "Optimizer2Options.hpp"
#include "backend.hpp"
#include "OptimizationProblemBase.hpp"
#include "OptimizerCallbackManager.hpp"
#include <aslam/Exceptions.hpp>
#include <sm/timing/Timer.hpp>
#include <boost/thread.hpp>
#include <sparse_block_matrix/linear_solver.h>
#include <aslam/backend/util/CommonDefinitions.hpp>
#include <aslam/backend/TrustRegionPolicy.hpp>
#include <aslam/backend/LevenbergMarquardtTrustRegionPolicy.hpp>
#include <aslam/backend/GaussNewtonTrustRegionPolicy.hpp>
#include <aslam/backend/DogLegTrustRegionPolicy.hpp>
#include <aslam/backend/util/OptimizerProblemManagerBase.hpp>

namespace sm {

  class ConstPropertyTree;

}
namespace aslam {
  namespace backend {
    class LinearSystemSolver;

    /**
     * \class Optimizer2
     *
     * A sparse Gauss-Newton/LM optimizer.
     *
     * The notation in this file follows Harley and Zisserman, Appendix 6.
     *
     * Some Additions to the standard algorithm:
     * Choice of Lambda:
     * H.B. Nielsen. Damping Parameter in Marquardts Method. Technical Report
     * IMM-REP-1999-05, Technical University of Denmark, 1999.
     *
     * Jacobian Normalisation:
     * Jean-Claude Trigeassou, Thierry Poinot & Sandrine Moreau (2003):
     * A Methodology for Estimation of Physical Parameters, Systems Analysis Modelling Simulation, 43:7, 925-943
     *
     * Erik Etien, Damien Halbert, and Thierry Poinot
     * Improved JilesAtherton Model for Least Square Identification Using Sensitivity Function Normalization
     * IEEE TRANSACTIONS ON MAGNETICS, VOL. 44, NO. 7, JULY 2008
     *
     */
    class Optimizer2 : public OptimizerProblemManagerBase {
    public:
      typedef aslam::backend::Timer Timer;
      typedef sparse_block_matrix::SparseBlockMatrix<Eigen::MatrixXd> SparseBlockMatrix;
      typedef Optimizer2Options Options;
      struct Status : public OptimizerStatus {
        SolutionReturnValue srv;
       private:
        void resetImplementation() override;
      };

      SM_DEFINE_EXCEPTION(Exception, aslam::Exception);

      Optimizer2(const Options& options = Options());
      Optimizer2(const sm::ConstPropertyTree& config, boost::shared_ptr<LinearSystemSolver> linearSystemSolver, boost::shared_ptr<TrustRegionPolicy> trustRegionPolicy);
      ~Optimizer2() override;

      /// \brief initialize the linear solver specified in the optimizer options.
      void initializeLinearSolver();
      
      void initializeTrustRegionPolicy();

      /// \brief Run the optimization
      SolutionReturnValue optimize();

      /// \brief Return the status
      const Status& getStatus() const override { return _status; }

      /// \brief Const getter for the optimizer options.
      const Options& getOptions() const override { return _options; }

      /// \brief Get the optimizer options.
      Options& options() { return _options; }

      /// \brief Set the optimizer options.
      void setOptions(const Options& options) { _options = options; }

      /// \brief Set the optimizer options.
      void setOptions(const OptimizerOptionsBase& options) override { static_cast<OptimizerOptionsBase&>(_options) = options; }

      /// \brief return the reduced system dx
      const Eigen::VectorXd& dx() const;

      /// The value of the objective function.
      double J() const;

      /// \brief compute the full covariance matrix. This is expensive.
      void computeCovariances(SparseBlockMatrix& outP, double lambda);

      /// \brief compute only the diagonal covariance blocks.
      void computeDiagonalCovariances(SparseBlockMatrix& outP, double lambda);

      /// \brief compute only the covariance blocks associated with the block indices passed as an argument
      void computeCovarianceBlocks(const std::vector<std::pair<int, int> >& blockIndices, SparseBlockMatrix& outP, double lambda);

      void computeHessian(SparseBlockMatrix& outH, double lambda);

      /// \brief Evaluate the error at the current state.
      double evaluateError(bool useMEstimator);

      /// \brief Get dense design variable i.
      DesignVariable* designVariable(size_t i);

      /// \brief how many dense design variables are involved in the problem
      size_t numDesignVariables() const;

      /// \brief print the internal timing information.
      void printTiming() const;

      /// \brief Do a bunch of checks to see if the problem is well-defined. This includes checking that every error term is
      ///        hooked up to design variables and running finite differences on error terms where this is possible.
      void checkProblemSetup() override;

      /// \brief Build the Gauss-Newton matrices.
      void buildGnMatrices(bool useMEstimator);

      /// Returns the linear solver
      template <class L>
      L* getSolver(bool assertNonNull = true){
        L* solver = dynamic_cast<L*>(_solver.get());
        if(assertNonNull) {
          SM_ASSERT_TRUE(Exception, solver != NULL, "The solver is null");
        }
        return solver;
      }

      /// \brief expose callback registry
      callback::Registry & callback(){
        return _callbackManager;
      }

      const Matrix * getJacobian() const;

      const LinearSystemSolver * getBaseSolver() const;

    private:

      /// \brief Zero the Gauss-Newton matrices.
      void zeroMatrices();

      /// \brief Revert the last state update.
      void revertLastStateUpdate();

      /// \brief Apply a state update.
      double applyStateUpdate();

      /// \brief issue callback for given event
      template<typename Event>
      void issueCallback();

      void optimizeImplementation() override;

      void initializeImplementation() override;

      /// \brief The dense update vector.
      Eigen::VectorXd _dx;

      /// \brief The previous value of the cost function.
      double _p_J;

      boost::shared_ptr<LinearSystemSolver> _solver;

      boost::shared_ptr<TrustRegionPolicy> _trustRegionPolicy;

      /// \brief the current set of options
      Options _options;

      /// \brief the current status
      Status _status;

      /// \brief A class that manages the optimizer callbacks
      callback::Manager _callbackManager;
    };

} // namespace backend
} // namespace aslam

#endif /* ASLAM_BACKEND_OPTIMIZER_HPP */
