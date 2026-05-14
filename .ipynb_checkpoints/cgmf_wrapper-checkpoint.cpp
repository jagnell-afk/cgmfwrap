

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <cstdlib>

#include "cgmfEvents.h"
#include "config.h"
#include "physics.h"
#include "kcksyst.h"
#include "ripl2levels.h"
#include "rngcgm.h"
#include "iostream"
#include "config-ff.h"

using namespace std;
struct EventOutput {
    std::vector<double> neutron_energies;
    std::vector<double> gamma_energies;
    std::vector<double> neutron_dir_cosu;
    std::vector<double> neutron_dir_cosv;
    std::vector<double> neutron_dir_cosw;
    std::vector<double> gamma_dir_cosu;
    std::vector<double> gamma_dir_cosv;
    std::vector<double> gamma_dir_cosw;
    int nu_n;
    int nu_g;
};
// ZAID = 1000*Z+A of fissioning nucleus
EventOutput run_single_event(int ZAIDt, double Ein) {

    setdatapath("/nfs/stak/users/agnellj/project/CGMF/data");
    // static bool initialized = false;
    //if (!initialized) {
     //   static cgmfEvent init;   // calls initialization() and allocates memory
     //   initialized = true;
    //}
     static bool rng_set = false;
    if (!rng_set) {
        UniformRNG rng(1);      // seed = 1 or whatever you want
        set_rng(rng);           // REQUIRED
        rng_set = true;
    }
    static bool initialized = false;
    if (!initialized) {
        static cgmfEvent init;   // calls initialization() and allocates memory
        initialized = true;
    }

    double time = 0.0;
    double timeWindow = 0.0;
    cgmfEvent*  event = 0;
    event = new cgmfEvent(ZAIDt, Ein, time, timeWindow);
    

    EventOutput out;

    // Neutrons
    int nuTot = event->getNeutronNu();

    for (int j = 0; j < nuTot; j++) {
        out.neutron_energies.push_back(event->getNeutronEnergy(j));
        out.neutron_dir_cosu.push_back(event->getNeutronDircosu(j));
        out.neutron_dir_cosv.push_back(event->getNeutronDircosv(j));
        out.neutron_dir_cosw.push_back(event->getNeutronDircosw(j));
    }

    out.nu_n = nuTot;

    // Gammas
    int nugLF = event->getLightFragmentPhotonNu();
    int nugHF = event->getHeavyFragmentPhotonNu();
    int nugTot = nugLF + nugHF;

    for (int j = 0; j < nugTot; j++) {
        out.gamma_energies.push_back(event->getPhotonEnergy(j));
    }

    out.nu_g = nugTot;

    return out;
}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(cgmfwrap, m) {
    py::class_<EventOutput>(m, "EventOutput")
        .def(py::init<>())
        .def_readwrite("neutron_energies", &EventOutput::neutron_energies)
        .def_readwrite("gamma_energies", &EventOutput::gamma_energies)
        .def_readwrite("nu_n", &EventOutput::nu_n)
        .def_readwrite("nu_g", &EventOutput::nu_g)
        .def_readwrite("neutron_dir_cosu", &EventOutput::neutron_dir_cosu)
        .def_readwrite("neutron_dir_cosv", &EventOutput::neutron_dir_cosv)
        .def_readwrite("neutron_dir_cosw", &EventOutput::neutron_dir_cosw)
        ;m.doc() = "CGMF single-event wrapper";

    m.def("run_event", &run_single_event,
          "Generate a single CGMF fission event");
        py::arg("ZAIDt"), py::arg("Ein");
   

}
