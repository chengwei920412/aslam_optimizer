// It is extremely important to use this header
// if you are using the numpy_eigen interface
#include <numpy_eigen/boost_python_headers.hpp>

void exportBackend();
void exportBackendDesignVariables();
// The title of this library must match exactly
BOOST_PYTHON_MODULE(libaslam_backend_python)
{
  // fill this in with boost::python export code
  exportBackend();


}