// term_angle_bend.h --- angle-bend energy term
// Copyright (C) 2009-2011 Kristoffer Enøe Johansson, Wouter Boomsma
//
// This file is part of Phaistos
//
// Phaistos is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Phaistos is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Phaistos.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef TERM_GROMACS_ANGLEBEND_H
#define TERM_GROMACS_ANGLEBEND_H

#include <boost/type_traits/is_base_of.hpp>
#include "energy/energy_term.h"

namespace phaistos {


//! OPLS anglebend energy term - base class containing all functionality
template<typename DERIVED_CLASS>
class TermGromacsAngleBendBase: public EnergyTermCommon<DERIVED_CLASS,ChainFB> {

          //! For convenience, define local EnergyTermCommon
          typedef phaistos::EnergyTermCommon<DERIVED_CLASS, ChainFB> EnergyTermCommon;

          //! Number of interactions in lastevaluation
          int counter;

     public:

          //! Local settings class.
          const class Settings: public EnergyTerm<ChainFB>::SettingsClassicEnergy {
          public:

          //! Whether to exclude sidechain interactions
          bool omit_sidechains;

          //! Constructor. Defines default values for settings object.
          Settings(bool omit_sidechains = false)
               : omit_sidechains(omit_sidechains) {}

          //! Output operator
          friend std::ostream &operator<<(std::ostream &o, const Settings &settings) {
               o << "omit-sidechains:" << settings.omit_sidechains << "\n";
               o << static_cast<const typename EnergyTerm<ChainFB>::Settings>(settings);
               return o;
          }
     } settings;  //!< Local settings object

     //! Constructor
     //! \param chain Molecule chain
     //! \param name Energy term name
     //! \param settings Local Settings object
     //! \param random_number_engine Object from which random number generators can be created.
     TermGromacsAngleBendBase(ChainFB *chain, std::string name, const Settings &settings = Settings(),
                              RandomNumberEngine *random_number_engine = &random_global) :
          EnergyTermCommon(chain, name, settings, random_number_engine),settings(settings) {
     }

     //! Copy constructor
     //! \param other Source object from which copy is made
     //! \param random_number_engine Object from which random number generators can be created.
     //! \param thread_index Index indicating in which thread|rank the copy exists
     //! \param chain Molecule chain
     TermGromacsAngleBendBase(const TermGromacsAngleBendBase &other,
                              RandomNumberEngine *random_number_engine,
                              int thread_index, ChainFB *chain) :
          EnergyTermCommon(other, random_number_engine, thread_index, chain),
          counter(other.counter),settings(other.settings) {

     }


     //! Evaluate anglebend energy for an atom
     //! \param atom2 Central atom
     //! \return Angle bend energy between all neighboring atoms
     inline double calc_anglebend_energy(Atom *atom2) {

          // Import protein definitions (such as residue names)
          using namespace definitions;

          double angle, energy = 0.0;
          CovalentBondIterator<ChainFB> it1(atom2, CovalentBondIterator<ChainFB>::DEPTH_1_ONLY);
          for (; !it1.end(); ++it1) {
               Atom *atom1 = &*it1;
               CovalentBondIterator<ChainFB> it3(it1);
               //discard it3 = it1
               ++it3;
               for (; !it3.end(); ++it3) {
                    Atom *atom3 = &*it3;
                    if (atom1->atom_type == PS || atom2->atom_type == PS || atom3->atom_type == PS )
                         continue;
                    angle = calc_angle(atom1->position, atom2->position, atom3->position);
                    // Some values
                    double eq = 1.0;
                    double k = 1.0;
                    ///////
                    energy += calc_spring_energy(angle,eq,k);
               }
          }
          return energy;
     }

     //! Evaluate the potetial energy in a spring
     //! \param x Angle
     //! \param x_eq Equilibrium angle
     //! \param k Spring constant
     //! \return Spring energy
     inline double calc_spring_energy(double x, double x_eq, double k) {
          counter++;
          double dx = x - x_eq;
          /* if (dx > M_PI || dx < -M_PI) */
          /*      std::cout<<"dx = "<<dx<<"\n"; */
          return (k * dx * dx);
     }

     //! Evaluate chain energy
     //! \param move_info object containing information about last move
     //! \return angle bend potential energy of the chain in the object
     double evaluate(MoveInfo *move_info = NULL) {

          double energy_sum = 0.0;
          counter = 0;

          // all angles
          int size = (this->chain)->size();
          for (int r = 0; r < size; r++) {
               Residue *res = &(*(this->chain))[r];
               int res_size = res->size();
               for (int a = 0; a < res_size; a++) {
                    Atom *atom = res->atoms[a];

                    // Atom must have more than one neighbour to contain angles
                    //if (parameters.get_bond_n(atom) < 2)
                    //     continue;

                    if (settings.omit_sidechains && atom->is_sidechain_atom)
                         continue;

                    energy_sum += calc_anglebend_energy(atom);
               }
          }

          /* std::cout<<"Angle Bending  "<<energy_sum<<" kcal/mol  "<<counter<<" interactions\n"; */
          return energy_sum;
     }

};


//! anglebend energy term
class TermGromacsAngleBend: public TermGromacsAngleBendBase<TermGromacsAngleBend> {

public:

     //! For convenience, define local TermAngleBendBase
     typedef phaistos::TermGromacsAngleBendBase<TermGromacsAngleBend> TermGromacsAngleBendBase;

     //! Reuse base-class Settings object
     typedef TermGromacsAngleBendBase::Settings Settings;

     //! Constructor
     TermGromacsAngleBend(ChainFB *chain, const Settings &settings = Settings(),
                          RandomNumberEngine *random_number_engine = &random_global)
          : TermGromacsAngleBendBase(chain, "gromacs-angle-bend", settings, random_number_engine) {}

     //! Copy constructor
     TermGromacsAngleBend(const TermGromacsAngleBend &other,
                          RandomNumberEngine *random_number_engine,
                          int thread_index, ChainFB *chain)
          : TermGromacsAngleBendBase(other, random_number_engine, thread_index, chain) {
     }
};

}

#endif
