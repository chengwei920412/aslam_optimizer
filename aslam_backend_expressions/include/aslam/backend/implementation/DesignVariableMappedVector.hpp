
namespace aslam {
  namespace backend {

    template<int D>
    DesignVariableMappedVector<D>::DesignVariableMappedVector(double * v) : _v(v), _p_v(_v)
    {

    }

    template<int D>
    DesignVariableMappedVector<D>::DesignVariableMappedVector(Eigen::Map< vector_t > v) : _v(v), _p_v(_v)
    {

    }


    template<int D>
    DesignVariableMappedVector<D>::~DesignVariableMappedVector()
    {

    }

    template<int D>
    typename DesignVariableMappedVector<D>::vector_t DesignVariableMappedVector<D>::value() const
    {
      return _v;
    }

    template<int D>
    VectorExpression<D> DesignVariableMappedVector<D>::toExpression()
    {
      return VectorExpression<D>(this);
    }


    template<int D>
    void DesignVariableMappedVector<D>::updateMap(double * p)
    {
        new (&_v) Eigen::Map< vector_t >(p);
        // \todo make these debug asserts
        SM_ASSERT_EQ(Exception, _v.data(), p, "The remap syntax didn't work");
        SM_ASSERT_EQ(Exception, _v.size(), D, "The remap syntax didn't work");
        _p_v = _v;
        
    }

    /// \brief Revert the last state update.
    template<int D>
    void DesignVariableMappedVector<D>::revertUpdateImplementation()
    {
      _v = _p_v;
    }

    /// \brief Update the design variable.
    template<int D>
    void DesignVariableMappedVector<D>::updateImplementation(const double * dp, int size)
    {
      SM_ASSERT_EQ_DBG(aslam::InvalidArgumentException, size, D, "Update dimension doesn't match the state dimension");
      Eigen::Map< const vector_t > dv(dp);
      _p_v = _v;
      _v += dv;
    }


    /// \brief what is the number of dimensions of the perturbation variable.
    template<int D>
    int DesignVariableMappedVector<D>::minimalDimensionsImplementation() const
    {
      return D;
    }


    template<int D>
    typename DesignVariableMappedVector<D>::vector_t DesignVariableMappedVector<D>::evaluateImplementation() const
    {
      return _v;
    }

    template<int D>
    void DesignVariableMappedVector<D>::evaluateJacobiansImplementation(JacobianContainer & outJacobians) const
    {
      SM_ASSERT_EQ_DBG(aslam::InvalidArgumentException, outJacobians.rows(), D, "The Jacobian container dimension doesn't match the state. Are you missing a chain rule?");
      outJacobians.add(const_cast< DesignVariableMappedVector<D>* >(this), Eigen::Matrix<double,D,D>::Identity());
    }

    template<int D>
    void DesignVariableMappedVector<D>::evaluateJacobiansImplementation(JacobianContainer & outJacobians, const Eigen::MatrixXd & applyChainRule) const
    {
      SM_ASSERT_EQ_DBG(aslam::InvalidArgumentException, applyChainRule.cols(), D, "The chain rule matrix dimension doesn't match the state.");
      outJacobians.add(const_cast< DesignVariableMappedVector<D>* >(this), applyChainRule);
    }

    template<int D>
    void DesignVariableMappedVector<D>::getDesignVariablesImplementation(DesignVariable::set_t & designVariables) const
    {
      designVariables.insert( const_cast<DesignVariableMappedVector<D> *>(this));
    }



  } // namespace backend
} // namespace aslam

