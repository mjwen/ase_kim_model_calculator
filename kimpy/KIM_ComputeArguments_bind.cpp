#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "KIM_SimulatorHeaders.hpp"
//@ TODO replace the header with the following. (need to solve forward declaration)
//#include "KIM_ComputeArguments.hpp"

namespace py = pybind11;
using namespace KIM;


PYBIND11_MODULE(compute_arguments, module) {
  module.doc() = "Python binding to ... ";

  // C++ class constructor and destructor are private, and as a result, simple ways
  // to construct py::init() does not exist.
  // We use factory function to create the instance.
  // std::unique_ptr<ComputeArguments, py::nodelete> avoids calling the destructor
  // of the C++ class. In this case, it is crucial that instances are deallocated
  // on the C++ side to avoid memory leaks. ComputeArguments_Destroy should do the
  // work.
  py::class_<ComputeArguments, std::unique_ptr<ComputeArguments, py::nodelete>>
      cl (module, "ComputeArguments");

  // python constructor needs to return a pointer to the C++ instance
  cl.def(py::init(
    [](Model& mo, py::array_t<int> error) {
      ComputeArguments * ca;
      auto e = error.mutable_data(0);
      e[0] = mo.ComputeArgumentsCreate(&ca);
      return ca;
    }
  ))

  .def("get_argument_support_status",
    [](ComputeArguments& inst, ComputeArgumentName const computeArgumentName) {
      SupportStatus supportStatus;
      int error = inst.GetArgumentSupportStatus(computeArgumentName, &supportStatus);

      py::tuple re(2);
      re[0] = supportStatus;
      re[1] = error;
      return re;
    },
    py::arg("ComputeArgumentName"),
    "Return(SupportStatus, error)"
  )

  .def("get_callback_support_status",
    [](ComputeArguments& inst, ComputeCallbackName const computeCallbackName) {
      SupportStatus supportStatus;
      int error = inst.GetCallbackSupportStatus(computeCallbackName, &supportStatus);

      py::tuple re(2);
      re[0] = supportStatus;
      re[1] = error;
      return re;
    },
    py::arg("ComputeCallbackName"),
    "Return(SupportStatus, error)"
  )

  .def("set_argument_pointer",
    [](ComputeArguments& inst,
       ComputeArgumentName const computeArgumentName,
       py::array_t<int> ptr
    ) {
      int* data = ptr.mutable_data(0);
      int error = inst.SetArgumentPointer(computeArgumentName, data);
      return error;
    },
    py::arg("ComputeArgumentName"),
    py::arg("ptr").noconvert()
  )

  .def("set_argument_pointer",
    [](ComputeArguments& inst,
      ComputeArgumentName const computeArgumentName,
      py::array_t<double> ptr
    ) {
      double* data = ptr.mutable_data(0);
      int error = inst.SetArgumentPointer(computeArgumentName, data);
      return error;
    },
    py::arg("ComputeArgumentName"),
    py::arg("ptr").noconvert()
  )

  .def("set_callback_pointer",
    [](ComputeArguments& inst,
      ComputeCallbackName const computeCallbackName,
      LanguageName const languageName,
      void const * const fptr,  // cannpt use: KIM::func * const fptr
                                // the argument passed in is of type: void const *
                                // we cast type explicitly in the funciton body
      void const * const dataObject
    ) {
      KIM::func * new_fptr = (KIM::func*) fptr;
      int error = inst.SetCallbackPointer(computeCallbackName,
        languageName, new_fptr, dataObject);
      return error;
    },
    py::arg("ComputeCallbackName"),
    py::arg("LanguageName"),
    py::arg("ptr"),
    py::arg("dataObject")
  )

  .def("are_all_required_arguments_and_callbacks_present",
    [](ComputeArguments& inst) {
      int result;
      inst.AreAllRequiredArgumentsAndCallbacksPresent(&result);
      return result;
    }
  )

  .def("__repr__", &ComputeArguments::String);

}

