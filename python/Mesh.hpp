/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseFEM

================================================================================================= */

#include <Eigen/Eigen>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

#include <pyxtensor/pyxtensor.hpp>

#include "../include/GooseFEM/GooseFEM.h"

// =================================================================================================

namespace py = pybind11;

// =================================================================================================

void init_Mesh(py::module &m)
{

py::class_<GooseFEM::Mesh::Renumber>(m, "Renumber")

  .def(py::init<const xt::xarray<size_t>&>(), "Renumber", py::arg("dofs"))

  .def("get", &GooseFEM::Mesh::Renumber::get, "Renumber list")
  .def("index", &GooseFEM::Mesh::Renumber::index, "Get index list to apply renumbering")

  .def("__repr__", [](const GooseFEM::Mesh::Renumber &){ return "<GooseFEM.Mesh.Renumber>"; });

// -------------------------------------------------------------------------------------------------

py::class_<GooseFEM::Mesh::Reorder>(m, "Reorder")

  .def(py::init([](xt::xtensor<size_t,1>& a) { return new GooseFEM::Mesh::Reorder({a}); }))
  .def(py::init([](xt::xtensor<size_t,1>& a, xt::xtensor<size_t,1>& b) { return new GooseFEM::Mesh::Reorder({a,b}); }))
  .def(py::init([](xt::xtensor<size_t,1>& a, xt::xtensor<size_t,1>& b, xt::xtensor<size_t,1>& c) { return new GooseFEM::Mesh::Reorder({a,b,c}); }))
  .def(py::init([](xt::xtensor<size_t,1>& a, xt::xtensor<size_t,1>& b, xt::xtensor<size_t,1>& c, xt::xtensor<size_t,1>& d) { return new GooseFEM::Mesh::Reorder({a,b,c,d}); }))

  .def("get", &GooseFEM::Mesh::Reorder::get, "Reorder list")
  .def("index", &GooseFEM::Mesh::Reorder::index, "Get index list to apply renumbering")

  .def("__repr__", [](const GooseFEM::Mesh::Reorder &){ return "<GooseFEM.Mesh.Reorder>"; });

// -------------------------------------------------------------------------------------------------

m.def("dofs", &GooseFEM::Mesh::dofs, "List with DOF-numbers (in sequential order)", py::arg("nnode"), py::arg("ndim"));

m.def("coordination", &GooseFEM::Mesh::coordination, "Coordination number of each node", py::arg("conn"));

m.def("elem2node", &GooseFEM::Mesh::elem2node, "Elements connect to each node", py::arg("conn"));

}

// =================================================================================================

