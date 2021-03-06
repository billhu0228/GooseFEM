/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseFEM

*/

#ifndef GOOSEFEM_VECTOR_HPP
#define GOOSEFEM_VECTOR_HPP

#include "Vector.h"

namespace GooseFEM {

inline Vector::Vector(const xt::xtensor<size_t, 2>& conn, const xt::xtensor<size_t, 2>& dofs)
    : m_conn(conn), m_dofs(dofs)
{
    m_nelem = m_conn.shape(0);
    m_nne = m_conn.shape(1);
    m_nnode = m_dofs.shape(0);
    m_ndim = m_dofs.shape(1);
    m_ndof = xt::amax(m_dofs)() + 1;

    GOOSEFEM_ASSERT(xt::amax(m_conn)() + 1 <= m_nnode);
    GOOSEFEM_ASSERT(m_ndof <= m_nnode * m_ndim);
}

inline size_t Vector::nelem() const
{
    return m_nelem;
}

inline size_t Vector::nne() const
{
    return m_nne;
}

inline size_t Vector::nnode() const
{
    return m_nnode;
}

inline size_t Vector::ndim() const
{
    return m_ndim;
}

inline size_t Vector::ndof() const
{
    return m_ndof;
}

inline xt::xtensor<size_t, 2> Vector::dofs() const
{
    return m_dofs;
}

inline void
Vector::copy(const xt::xtensor<double, 2>& nodevec_src, xt::xtensor<double, 2>& nodevec_dest) const
{
    GOOSEFEM_ASSERT(xt::has_shape(nodevec_src, {m_nnode, m_ndim}));
    GOOSEFEM_ASSERT(xt::has_shape(nodevec_dest, {m_nnode, m_ndim}));

    xt::noalias(nodevec_dest) = nodevec_src;
}

inline void
Vector::asDofs(const xt::xtensor<double, 2>& nodevec, xt::xtensor<double, 1>& dofval) const
{
    GOOSEFEM_ASSERT(xt::has_shape(nodevec, {m_nnode, m_ndim}));
    GOOSEFEM_ASSERT(dofval.size() == m_ndof);

    dofval.fill(0.0);

    #pragma omp parallel for
    for (size_t m = 0; m < m_nnode; ++m) {
        for (size_t i = 0; i < m_ndim; ++i) {
            dofval(m_dofs(m, i)) = nodevec(m, i);
        }
    }
}

inline void
Vector::asDofs(const xt::xtensor<double, 3>& elemvec, xt::xtensor<double, 1>& dofval) const
{
    GOOSEFEM_ASSERT(xt::has_shape(elemvec, {m_nelem, m_nne, m_ndim}));
    GOOSEFEM_ASSERT(dofval.size() == m_ndof);

    dofval.fill(0.0);

    #pragma omp parallel for
    for (size_t e = 0; e < m_nelem; ++e) {
        for (size_t m = 0; m < m_nne; ++m) {
            for (size_t i = 0; i < m_ndim; ++i) {
                dofval(m_dofs(m_conn(e, m), i)) = elemvec(e, m, i);
            }
        }
    }
}

inline void
Vector::asNode(const xt::xtensor<double, 1>& dofval, xt::xtensor<double, 2>& nodevec) const
{
    GOOSEFEM_ASSERT(dofval.size() == m_ndof);
    GOOSEFEM_ASSERT(xt::has_shape(nodevec, {m_nnode, m_ndim}));

    #pragma omp parallel for
    for (size_t m = 0; m < m_nnode; ++m) {
        for (size_t i = 0; i < m_ndim; ++i) {
            nodevec(m, i) = dofval(m_dofs(m, i));
        }
    }
}

inline void
Vector::asNode(const xt::xtensor<double, 3>& elemvec, xt::xtensor<double, 2>& nodevec) const
{
    GOOSEFEM_ASSERT(xt::has_shape(elemvec, {m_nelem, m_nne, m_ndim}));
    GOOSEFEM_ASSERT(xt::has_shape(nodevec, {m_nnode, m_ndim}));

    nodevec.fill(0.0);

    #pragma omp parallel for
    for (size_t e = 0; e < m_nelem; ++e) {
        for (size_t m = 0; m < m_nne; ++m) {
            for (size_t i = 0; i < m_ndim; ++i) {
                nodevec(m_conn(e, m), i) = elemvec(e, m, i);
            }
        }
    }
}

inline void
Vector::asElement(const xt::xtensor<double, 1>& dofval, xt::xtensor<double, 3>& elemvec) const
{
    GOOSEFEM_ASSERT(dofval.size() == m_ndof);
    GOOSEFEM_ASSERT(xt::has_shape(elemvec, {m_nelem, m_nne, m_ndim}));

    #pragma omp parallel for
    for (size_t e = 0; e < m_nelem; ++e) {
        for (size_t m = 0; m < m_nne; ++m) {
            for (size_t i = 0; i < m_ndim; ++i) {
                elemvec(e, m, i) = dofval(m_dofs(m_conn(e, m), i));
            }
        }
    }
}

inline void
Vector::asElement(const xt::xtensor<double, 2>& nodevec, xt::xtensor<double, 3>& elemvec) const
{
    GOOSEFEM_ASSERT(xt::has_shape(nodevec, {m_nnode, m_ndim}));
    GOOSEFEM_ASSERT(xt::has_shape(elemvec, {m_nelem, m_nne, m_ndim}));

    #pragma omp parallel for
    for (size_t e = 0; e < m_nelem; ++e) {
        for (size_t m = 0; m < m_nne; ++m) {
            for (size_t i = 0; i < m_ndim; ++i) {
                elemvec(e, m, i) = nodevec(m_conn(e, m), i);
            }
        }
    }
}

inline void
Vector::assembleDofs(const xt::xtensor<double, 2>& nodevec, xt::xtensor<double, 1>& dofval) const
{
    GOOSEFEM_ASSERT(xt::has_shape(nodevec, {m_nnode, m_ndim}));
    GOOSEFEM_ASSERT(dofval.size() == m_ndof);

    dofval.fill(0.0);

    for (size_t m = 0; m < m_nnode; ++m) {
        for (size_t i = 0; i < m_ndim; ++i) {
            dofval(m_dofs(m, i)) += nodevec(m, i);
        }
    }
}

inline void
Vector::assembleDofs(const xt::xtensor<double, 3>& elemvec, xt::xtensor<double, 1>& dofval) const
{
    GOOSEFEM_ASSERT(xt::has_shape(elemvec, {m_nelem, m_nne, m_ndim}));
    GOOSEFEM_ASSERT(dofval.size() == m_ndof);

    dofval.fill(0.0);

    for (size_t e = 0; e < m_nelem; ++e) {
        for (size_t m = 0; m < m_nne; ++m) {
            for (size_t i = 0; i < m_ndim; ++i) {
                dofval(m_dofs(m_conn(e, m), i)) += elemvec(e, m, i);
            }
        }
    }
}

inline void
Vector::assembleNode(const xt::xtensor<double, 3>& elemvec, xt::xtensor<double, 2>& nodevec) const
{
    GOOSEFEM_ASSERT(xt::has_shape(elemvec, {m_nelem, m_nne, m_ndim}));
    GOOSEFEM_ASSERT(xt::has_shape(nodevec, {m_nnode, m_ndim}));

    xt::xtensor<double, 1> dofval = this->AssembleDofs(elemvec);
    this->asNode(dofval, nodevec);
}

inline xt::xtensor<double, 1> Vector::AsDofs(const xt::xtensor<double, 2>& nodevec) const
{
    xt::xtensor<double, 1> dofval = xt::empty<double>({m_ndof});
    this->asDofs(nodevec, dofval);
    return dofval;
}

inline xt::xtensor<double, 1> Vector::AsDofs(const xt::xtensor<double, 3>& elemvec) const
{
    xt::xtensor<double, 1> dofval = xt::empty<double>({m_ndof});
    this->asDofs(elemvec, dofval);
    return dofval;
}

inline xt::xtensor<double, 2> Vector::AsNode(const xt::xtensor<double, 1>& dofval) const
{
    xt::xtensor<double, 2> nodevec = xt::empty<double>({m_nnode, m_ndim});
    this->asNode(dofval, nodevec);
    return nodevec;
}

inline xt::xtensor<double, 2> Vector::AsNode(const xt::xtensor<double, 3>& elemvec) const
{
    xt::xtensor<double, 2> nodevec = xt::empty<double>({m_nnode, m_ndim});
    this->asNode(elemvec, nodevec);
    return nodevec;
}

inline xt::xtensor<double, 3> Vector::AsElement(const xt::xtensor<double, 1>& dofval) const
{
    xt::xtensor<double, 3> elemvec = xt::empty<double>({m_nelem, m_nne, m_ndim});
    this->asElement(dofval, elemvec);
    return elemvec;
}

inline xt::xtensor<double, 3> Vector::AsElement(const xt::xtensor<double, 2>& nodevec) const
{
    xt::xtensor<double, 3> elemvec = xt::empty<double>({m_nelem, m_nne, m_ndim});
    this->asElement(nodevec, elemvec);
    return elemvec;
}

inline xt::xtensor<double, 1> Vector::AssembleDofs(const xt::xtensor<double, 2>& nodevec) const
{
    xt::xtensor<double, 1> dofval = xt::empty<double>({m_ndof});
    this->assembleDofs(nodevec, dofval);
    return dofval;
}

inline xt::xtensor<double, 1> Vector::AssembleDofs(const xt::xtensor<double, 3>& elemvec) const
{
    xt::xtensor<double, 1> dofval = xt::empty<double>({m_ndof});
    this->assembleDofs(elemvec, dofval);
    return dofval;
}

inline xt::xtensor<double, 2> Vector::AssembleNode(const xt::xtensor<double, 3>& elemvec) const
{
    xt::xtensor<double, 2> nodevec = xt::empty<double>({m_nnode, m_ndim});
    this->assembleNode(elemvec, nodevec);
    return nodevec;
}

inline xt::xtensor<double, 1> Vector::AllocateDofval() const
{
    xt::xtensor<double, 1> dofval = xt::empty<double>({m_ndof});
    return dofval;
}

inline xt::xtensor<double, 2> Vector::AllocateNodevec() const
{
    xt::xtensor<double, 2> nodevec = xt::empty<double>({m_nnode, m_ndim});
    return nodevec;
}

inline xt::xtensor<double, 3> Vector::AllocateElemvec() const
{
    xt::xtensor<double, 3> elemvec = xt::empty<double>({m_nelem, m_nne, m_ndim});
    return elemvec;
}

inline xt::xtensor<double, 3> Vector::AllocateElemmat() const
{
    xt::xtensor<double, 3> elemmat = xt::empty<double>({m_nelem, m_nne * m_ndim, m_nne * m_ndim});
    return elemmat;
}

inline xt::xtensor<double, 1> Vector::AllocateDofval(double val) const
{
    xt::xtensor<double, 1> dofval = xt::empty<double>({m_ndof});
    dofval.fill(val);
    return dofval;
}

inline xt::xtensor<double, 2> Vector::AllocateNodevec(double val) const
{
    xt::xtensor<double, 2> nodevec = xt::empty<double>({m_nnode, m_ndim});
    nodevec.fill(val);
    return nodevec;
}

inline xt::xtensor<double, 3> Vector::AllocateElemvec(double val) const
{
    xt::xtensor<double, 3> elemvec = xt::empty<double>({m_nelem, m_nne, m_ndim});
    elemvec.fill(val);
    return elemvec;
}

inline xt::xtensor<double, 3> Vector::AllocateElemmat(double val) const
{
    xt::xtensor<double, 3> elemmat = xt::empty<double>({m_nelem, m_nne * m_ndim, m_nne * m_ndim});
    elemmat.fill(val);
    return elemmat;
}

} // namespace GooseFEM

#endif
