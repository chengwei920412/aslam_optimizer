#include <numpy_eigen/boost_python_headers.hpp>
#include <aslam/backend/ScalarNonSquaredErrorTerm.hpp>
#include <aslam/backend/DesignVariable.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost::python;
using namespace aslam::backend;


void exportScalarNonSquaredErrorTerm()
{
  
  class_<ScalarNonSquaredErrorTerm, boost::shared_ptr<ScalarNonSquaredErrorTerm>, boost::noncopyable>("ScalarNonSquaredErrorTerm", no_init)
    .def("evaluateError", &ScalarNonSquaredErrorTerm::evaluateError)
    .add_property("weight", &ScalarNonSquaredErrorTerm::getWeight, &ScalarNonSquaredErrorTerm::setWeight)
    .def("setMEstimatorPolicy", &ScalarNonSquaredErrorTerm::setMEstimatorPolicy)
    .def("clearMEstimatorPolicy", &ScalarNonSquaredErrorTerm::clearMEstimatorPolicy)
    .def("getMEstimatorWeight", &ScalarNonSquaredErrorTerm::getMEstimatorWeight)
    .def("getCurrentMEstimatorWeight", &ScalarNonSquaredErrorTerm::getCurrentMEstimatorWeight)
    .def("getMEstimatorName", &ScalarNonSquaredErrorTerm::getMEstimatorName)
    .def("numDesignVariables", &ScalarNonSquaredErrorTerm::numDesignVariables)
    .def("designVariable", (DesignVariable* (ScalarNonSquaredErrorTerm::*) (size_t i))&ScalarNonSquaredErrorTerm::designVariable, return_internal_reference<>())
    .def("getWeightedError", (double (ScalarNonSquaredErrorTerm::*)(bool) const)&ScalarNonSquaredErrorTerm::getWeightedError, "getWeightedError(bool useMEstimator)")
    .def("getWeightedErrorWithMEstimator", (double (ScalarNonSquaredErrorTerm::*)(void) const)&ScalarNonSquaredErrorTerm::getWeightedError)
    .def("getRawError", &ScalarNonSquaredErrorTerm::getRawError)
    .def("getTime", &ScalarNonSquaredErrorTerm::getTime)
    ;

}
