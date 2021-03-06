// topology_parser.h -- Topology parser for the CHARMM36/EEF1-SB module
// Copyright (C) 2014 Anders S. Christensen
//
// This file is part of PHAISTOS
//
// PHAISTOS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PHAISTOS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Phaistos.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef TERM_TOPOLOGY_PARSER_H
#define TERM_TOPOLOGY_PARSER_H

#include <string>
#include <math.h>

#include "protein/iterators/pair_iterator_chaintree.h"

#include "eef1_sb_parser.h"
#include "topology_items.h"
#include "../constants.h"


namespace topology {


//! Generates an vector, over which all CMAP interactions in the chain can be iterated.
//! \param chain The protein chain object.
//! \returns A vector of CMAP interaction term objects
std::vector<CmapInteraction> generate_cmap_interactions(phaistos::ChainFB *chain) {

    using namespace phaistos;
    using namespace definitions;

    std::vector<CmapInteraction> cmap_interactions;

    int i = -1;

    for (ResidueIterator<ChainFB> it(*(chain)); !(it).end(); ++it) {

          i += 1;
          Residue *res = &*it;

          if (res->terminal_status == NTERM) continue;
          if (res->terminal_status == CTERM) continue;

          std::string type1 = eef1_sb_parser::get_atom_type((*(res->get_neighbour(-1)))[C]);
          std::string type2 = eef1_sb_parser::get_atom_type((*res)[N]);
          std::string type3 = eef1_sb_parser::get_atom_type((*res)[CA]);
          std::string type4 = eef1_sb_parser::get_atom_type((*res)[C]);
          std::string type5 = eef1_sb_parser::get_atom_type((*(res->get_neighbour(+1)))[N]);

          CmapInteraction cmap_interaction;

          cmap_interaction.residue = res;
          cmap_interaction.residue_index = i;

          if ((type1 == "C") &&
              (type2 == "NH1") &&
              (type3 == "CT1") &&
              (type4 == "C") &&
              (type5 == "NH1")) {
                cmap_interaction.cmap_type_index = 0;
          } else if ((type1 == "C") &&
                     (type2 == "NH1") &&
                     (type3 == "CT1") &&
                     (type4 == "C") &&
                     (type5 == "N")) {
                cmap_interaction.cmap_type_index = 1;
          } else if ((type1 == "C") &&
                     (type2 == "N") &&
                     (type3 == "CP1") &&
                     (type4 == "C") &&
                     (type5 == "NH1")) {
                cmap_interaction.cmap_type_index = 2;
          } else if ((type1 == "C") &&
                     (type2 == "N") &&
                     (type3 == "CP1") &&
                     (type4 == "C") &&
                     (type5 == "N")) {
                cmap_interaction.cmap_type_index = 3;
          } else if ((type1 == "C") &&
                     (type2 == "NH1") &&
                     (type3 == "CT2") &&
                     (type4 == "C") &&
                     (type5 == "NH1")) {
                cmap_interaction.cmap_type_index = 4;
          } else if ((type1 == "C") &&
                     (type2 == "NH1") &&
                     (type3 == "CT2") &&
                     (type4 == "C") &&
                     (type5 == "N")) {
                cmap_interaction.cmap_type_index = 5;
          } else {
                std::cerr << "# Error: Unknown CMAP parameters for residue" << *res << " .\n";
                exit(EXIT_FAILURE);
          }

          cmap_interactions.push_back(cmap_interaction);
    }

    return cmap_interactions;
}


//! Reads a GROMACS van der Waal parameter .itp file (converted to one long string) and returns a vector of parameter objects 
//! \param itp A parameter file in converted to string
//! \returns A vector of non-bonded parameters
std::vector<NonBondedParameter> read_nonbonded_parameters(const std::string &itp) {

    std::istringstream input_stream(itp);

    std::vector<NonBondedParameter> parameters;

    while (input_stream.good()) {

        std::string line;
        std::getline(input_stream, line);

        boost::trim(line);

        if (line.size() == 0 || line[0] == ';') {
            continue;
        }

        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" \t"), boost::token_compress_on);

        NonBondedParameter parameter;

        parameter.atom_type   = boost::lexical_cast<std::string >(split_line[0]);
        parameter.atom_number = boost::lexical_cast<unsigned int>(split_line[1]);
        parameter.atom_mass   = boost::lexical_cast<double      >(split_line[2]);
        parameter.atom_charge = 0.0;
        parameter.atom_ptype  = boost::lexical_cast<std::string >(split_line[4]);
        parameter.sigma       = boost::lexical_cast<double      >(split_line[5]);
        parameter.epsilon     = boost::lexical_cast<double      >(split_line[6]);

        parameters.push_back(parameter);

      }

    return parameters;
}


//! Reads a GROMACS van der Waal "1-4" parameter .itp file (converted to one long string) and returns a vector of parameter objects 
//! \param itp A parameter file in converted to string
//! \returns A vector of non-bonded parameters
std::vector<NonBonded14Parameter> read_nonbonded_14_parameters(const std::string &itp) {

    std::istringstream input_stream(itp);

    std::vector<NonBonded14Parameter> parameters;

    while (input_stream.good()) {

        std::string line;
        std::getline(input_stream, line);

        boost::trim(line);

        if (line.size() == 0 || line[0] == ';') {
            continue;
        }

        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" \t"), boost::token_compress_on);

        NonBonded14Parameter parameter;

        parameter.atom_type1    = boost::lexical_cast<std::string >(split_line[0]);
        parameter.atom_type2    = boost::lexical_cast<std::string >(split_line[1]);
        parameter.pair_function = boost::lexical_cast<unsigned int>(split_line[2]);
        parameter.sigma         = boost::lexical_cast<double      >(split_line[3]);
        parameter.epsilon       = boost::lexical_cast<double      >(split_line[4]);

        parameters.push_back(parameter);

      }

    return parameters;
}


NonBondedParameter get_non_bonded_parameter(const std::string &atom_type,
        const std::vector<NonBondedParameter> &non_bonded_parameters) {

    NonBondedParameter parameter;

    for (unsigned int i = 0; i < non_bonded_parameters.size(); i++) {

        if (non_bonded_parameters[i].atom_type == atom_type)
            return non_bonded_parameters[i];

    }

    return parameter;

}

NonBonded14Parameter get_non_bonded14_parameter(const std::string &atom_type1, 
        const std::string &atom_type2,
        const std::vector<NonBonded14Parameter> &non_bonded14_parameters,
        const std::vector<NonBondedParameter> &non_bonded_parameters) {

    NonBonded14Parameter parameter;
    bool found_parameter = false;

    for (unsigned int i = 0; i < non_bonded14_parameters.size(); i++) {

        if (((non_bonded14_parameters[i].atom_type1 == atom_type1) &&
             (non_bonded14_parameters[i].atom_type2 == atom_type2)) ||
            ((non_bonded14_parameters[i].atom_type1 == atom_type2) &&
             (non_bonded14_parameters[i].atom_type2 == atom_type1))) {

            parameter = non_bonded14_parameters[i];
            found_parameter = true;
            break;

        }
    }

    if (!found_parameter) {

        NonBondedParameter parameter1 = get_non_bonded_parameter(atom_type1, non_bonded_parameters);
        NonBondedParameter parameter2 = get_non_bonded_parameter(atom_type2, non_bonded_parameters);

        parameter.atom_type1 = atom_type1;
        parameter.atom_type2 = atom_type1;

        const double epsilon_effective = sqrt(parameter1.epsilon * parameter2.epsilon);
        const double sigma_effective   = 0.5 * (parameter1.sigma + parameter2.sigma);

        parameter.sigma = sigma_effective;
        parameter.epsilon = epsilon_effective;

    }

    return parameter;

}

struct AtomTypeInfo {
    phaistos::Atom  *atom;
    NonBondedParameter non_bonded_parameter;
    std::string atom_type;
    double charge;
};

std::vector<NonBondedInteraction> generate_non_bonded_interactions(phaistos::ChainFB *chain,
    const std::vector<NonBondedParameter> &non_bonded_parameters,
    const std::vector<NonBonded14Parameter> &non_bonded_14_parameters) {

    using namespace phaistos;

    std::vector<NonBondedInteraction> non_bonded_interactions;

    std::vector<AtomTypeInfo> atom_type_infos;

    for (AtomIterator<ChainFB, definitions::ALL> it1(*chain); !it1.end(); ++it1) {

        AtomTypeInfo atom_type_info;

        Atom *atom1 = &*it1;

        atom_type_info.atom = atom1;
        atom_type_info.atom_type = eef1_sb_parser::get_atom_type(atom1);
        atom_type_info.charge = eef1_sb_parser::get_atom_charge(atom1);
        atom_type_info.non_bonded_parameter = get_non_bonded_parameter(atom_type_info.atom_type,
                                                                       non_bonded_parameters);

        atom_type_infos.push_back(atom_type_info);
    }
    

    for (unsigned int i = 0; i < atom_type_infos.size(); i++) {


        Atom *atom1 = atom_type_infos[i].atom;
        const std::string atom_type1 = atom_type_infos[i].atom_type;
        const double atom_charge1 = atom_type_infos[i].charge;
        const NonBondedParameter parameter1 = atom_type_infos[i].non_bonded_parameter;

        for (unsigned int j = 0; j < atom_type_infos.size(); j++) {

            if (i > j) continue;

            Atom *atom2 = atom_type_infos[j].atom;
            const std::string atom_type2 = atom_type_infos[j].atom_type;
            const double atom_charge2 = atom_type_infos[j].charge;
            const NonBondedParameter parameter2 = atom_type_infos[j].non_bonded_parameter;

            const int d = chain_distance<ChainFB>(atom1,atom2);

            if (d > 3) {
               NonBondedInteraction non_bonded_interaction;

               double epsilon_effective = std::sqrt(parameter1.epsilon * parameter2.epsilon);
               double sigma_effective   = 0.5 * (parameter1.sigma + parameter2.sigma);

               non_bonded_interaction.atom1 = atom1;
               non_bonded_interaction.atom2 = atom2;
               //non_bonded_interaction.q1 = atom_charge1;
               //non_bonded_interaction.q2 = atom_charge2;
               //non_bonded_interaction.sigma1 = parameter1.sigma;
               //non_bonded_interaction.sigma2 = parameter2.sigma;
               //non_bonded_interaction.epsilon1 = parameter1.epsilon;
               //non_bonded_interaction.epsilon2 = parameter2.epsilon;
               //non_bonded_interaction.sigma_effective = sigma_effective;
               //non_bonded_interaction.epsilon_effective = epsilon_effective;
               // non_bonded_interaction.qq  = non_bonded_interaction.q1 * non_bonded_interaction.q2 * charmm_constants::FELEC;
               non_bonded_interaction.qq  = atom_charge1 * atom_charge2 * charmm_constants::FELEC;
               non_bonded_interaction.c6  = 4 * epsilon_effective * std::pow(sigma_effective, 6.0);
               non_bonded_interaction.c12 = 4 * epsilon_effective * std::pow(sigma_effective, 12.0);

               non_bonded_interaction.is_14_interaction = false;

               //non_bonded_interaction.i1 = i;
               //non_bonded_interaction.i2 = j;


               non_bonded_interactions.push_back(non_bonded_interaction);

             } else if (d == 3) {


                NonBonded14Parameter parameter14 = get_non_bonded14_parameter(atom_type1, atom_type2,
                                                                              non_bonded_14_parameters,
                                                                              non_bonded_parameters);

                NonBondedParameter parameter2 = get_non_bonded_parameter(atom_type2, non_bonded_parameters);
                NonBondedInteraction non_bonded_interaction;

                non_bonded_interaction.atom1 = atom1;
                non_bonded_interaction.atom2 = atom2;
                // non_bonded_interaction.q1 = atom_charge1;
                // non_bonded_interaction.q2 = atom_charge2;
                // non_bonded_interaction.sigma1 = parameter1.sigma;
                // non_bonded_interaction.sigma2 = parameter2.sigma;
                // non_bonded_interaction.epsilon1 = parameter1.epsilon;
                // non_bonded_interaction.epsilon2 = parameter2.epsilon;
                // non_bonded_interaction.sigma_effective = parameter14.sigma;
                // non_bonded_interaction.epsilon_effective = parameter14.epsilon;
                // non_bonded_interaction.qq  = non_bonded_interaction.q1 * non_bonded_interaction.q2 * charmm_constants::FELEC;
                non_bonded_interaction.qq  = atom_charge1 * atom_charge2 * charmm_constants::FELEC;
                non_bonded_interaction.c6  = 4 * parameter14.epsilon * std::pow(parameter14.sigma, 6.0);
                non_bonded_interaction.c12 = 4 * parameter14.epsilon * std::pow(parameter14.sigma, 12.0);

                non_bonded_interaction.is_14_interaction = true;

                non_bonded_interactions.push_back(non_bonded_interaction);
            }
        }
    }

    return non_bonded_interactions;
}



std::vector<NonBondedInteraction> generate_non_bonded_interactions_cached(phaistos::ChainFB *chain,
                    const std::vector<NonBondedParameter> &non_bonded_parameters,
                    const std::vector<NonBonded14Parameter> &non_bonded_14_parameters,
                    const std::vector<double> &dGref,
                    const std::vector< std::vector<double> > &factors,
                    const std::vector<double> &vdw_radii,
                    const std::vector<double> &lambda,
                    // &eef1_atom_type_index_map cannot be const for some reason?
                    std::map<std::string, unsigned int> &eef1_atom_type_index_map) {

    using namespace phaistos;

    std::vector<NonBondedInteraction> non_bonded_interactions;

    std::vector<AtomTypeInfo> atom_type_infos;

    for (AtomIterator<ChainFB, definitions::ALL> it1(*chain); !it1.end(); ++it1) {

        AtomTypeInfo atom_type_info;

        Atom *atom1 = &*it1;

        atom_type_info.atom = atom1;
        atom_type_info.atom_type = eef1_sb_parser::get_atom_type(atom1);
        atom_type_info.charge = eef1_sb_parser::get_atom_charge(atom1);
        atom_type_info.non_bonded_parameter = get_non_bonded_parameter(atom_type_info.atom_type,
                                                                       non_bonded_parameters);

        atom_type_infos.push_back(atom_type_info);
    }
    

    for (unsigned int i = 0; i < atom_type_infos.size(); i++) {


        Atom *atom1 = atom_type_infos[i].atom;
        const std::string atom_type1 = atom_type_infos[i].atom_type;
        const double atom_charge1 = atom_type_infos[i].charge;
        const NonBondedParameter parameter1 = atom_type_infos[i].non_bonded_parameter;

        for (unsigned int j = 0; j < atom_type_infos.size(); j++) {

            if (i > j) continue;

            Atom *atom2 = atom_type_infos[j].atom;
            const std::string atom_type2 = atom_type_infos[j].atom_type;
            const double atom_charge2 = atom_type_infos[j].charge;
            const NonBondedParameter parameter2 = atom_type_infos[j].non_bonded_parameter;

            const int d = chain_distance<ChainFB>(atom1,atom2);

            if (d > 3) {
               NonBondedInteraction non_bonded_interaction;

               double epsilon_effective = sqrt(parameter1.epsilon * parameter2.epsilon);
               double sigma_effective   = 0.5 * (parameter1.sigma + parameter2.sigma);

               non_bonded_interaction.atom1 = atom1;
               non_bonded_interaction.atom2 = atom2;
               // non_bonded_interaction.q1 = atom_charge1;
               // non_bonded_interaction.q2 = atom_charge2;
               // non_bonded_interaction.sigma1 = parameter1.sigma;
               // non_bonded_interaction.sigma2 = parameter2.sigma;
               // non_bonded_interaction.epsilon1 = parameter1.epsilon;
               // non_bonded_interaction.epsilon2 = parameter2.epsilon;
               // non_bonded_interaction.sigma_effective = sigma_effective;
               // non_bonded_interaction.epsilon_effective = epsilon_effective;
               // non_bonded_interaction.qq  = non_bonded_interaction.q1 * non_bonded_interaction.q2 * charmm_constants::FELEC;
               non_bonded_interaction.qq  = atom_charge1 * atom_charge2 * charmm_constants::FELEC;
               non_bonded_interaction.c6  = 4 * epsilon_effective * std::pow(sigma_effective, 6.0);
               non_bonded_interaction.c12 = 4 * epsilon_effective * std::pow(sigma_effective, 12.0);

               non_bonded_interaction.is_14_interaction = false;

               // non_bonded_interaction.i1 = i;
               // non_bonded_interaction.i2 = j;

               if (!(atom1->mass == definitions::atom_h_weight) &&
                   !(chain_distance<ChainFB>(atom1,atom2) < 3) &&
                   !(atom2->mass == definitions::atom_h_weight)) {

                   non_bonded_interaction.do_eef1 = true;

                   std::string atom_type36_1 = atom_type_infos[i].atom_type;
                   std::string atom_type36_2 = atom_type_infos[j].atom_type;

                   unsigned int index1 = eef1_atom_type_index_map[atom_type36_1];
                   unsigned int index2 = eef1_atom_type_index_map[atom_type36_2];

                   non_bonded_interaction.fac_12 = factors[index1][index2];
                   non_bonded_interaction.fac_21 = factors[index2][index1];
                   non_bonded_interaction.R_vdw_1 = vdw_radii[index1];
                   non_bonded_interaction.R_vdw_2 = vdw_radii[index2];
                   non_bonded_interaction.lambda1 = lambda[index1];
                   non_bonded_interaction.lambda2 = lambda[index2];

               } else { 
                   non_bonded_interaction.do_eef1 = false;
               }
               non_bonded_interactions.push_back(non_bonded_interaction);

             } else if (d == 3) {


                NonBonded14Parameter parameter14 = get_non_bonded14_parameter(atom_type1, atom_type2,
                                                                              non_bonded_14_parameters,
                                                                              non_bonded_parameters);

                NonBondedParameter parameter2 = get_non_bonded_parameter(atom_type2, non_bonded_parameters);
                NonBondedInteraction non_bonded_interaction;

                non_bonded_interaction.atom1 = atom1;
                non_bonded_interaction.atom2 = atom2;
                // non_bonded_interaction.q1 = atom_charge1;
                // non_bonded_interaction.q2 = atom_charge2;
                // non_bonded_interaction.sigma1 = parameter1.sigma;
                // non_bonded_interaction.sigma2 = parameter2.sigma;
                // non_bonded_interaction.epsilon1 = parameter1.epsilon;
                // non_bonded_interaction.epsilon2 = parameter2.epsilon;
                // non_bonded_interaction.sigma_effective = parameter14.sigma;
                // non_bonded_interaction.epsilon_effective = parameter14.epsilon;
                non_bonded_interaction.qq  = atom_charge1 * atom_charge2 * charmm_constants::FELEC;
                non_bonded_interaction.c6  = 4 * parameter14.epsilon * std::pow(parameter14.sigma, 6.0);
                non_bonded_interaction.c12 = 4 * parameter14.epsilon * std::pow(parameter14.sigma, 12.0);

                non_bonded_interaction.is_14_interaction = true;

               if (!(atom1->mass == definitions::atom_h_weight) &&
                   !(chain_distance<ChainFB>(atom1,atom2) < 3) &&
                   !(atom2->mass == definitions::atom_h_weight)) {

                   non_bonded_interaction.do_eef1 = true;

                   std::string atom_type36_1 = atom_type_infos[i].atom_type;
                   std::string atom_type36_2 = atom_type_infos[j].atom_type;

                   unsigned int index1 = eef1_atom_type_index_map[atom_type36_1];
                   unsigned int index2 = eef1_atom_type_index_map[atom_type36_2];

                   non_bonded_interaction.fac_12 = factors[index1][index2];
                   non_bonded_interaction.fac_21 = factors[index2][index1];
                   non_bonded_interaction.R_vdw_1 = vdw_radii[index1];
                   non_bonded_interaction.R_vdw_2 = vdw_radii[index2];
                   non_bonded_interaction.lambda1 = lambda[index1];
                   non_bonded_interaction.lambda2 = lambda[index2];

               } else { 
                   non_bonded_interaction.do_eef1 = false;
               }

                non_bonded_interactions.push_back(non_bonded_interaction);
            }
        }
    }

    return non_bonded_interactions;
}


std::vector<TorsionParameter> read_torsion_parameters(const std::string &itp) {

    std::istringstream input_stream(itp);

    std::vector<TorsionParameter> parameters;

    while (input_stream.good()) {

        std::string line;
        std::getline(input_stream, line);

        boost::trim(line);

        // std::cout << line << std::endl;
        if (line.size() == 0 || line[0] == ';') {
            continue;
        }

        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" \t"), boost::token_compress_on);

        TorsionParameter parameter;

        parameter.type1 = boost::lexical_cast<std::string >(split_line[0]);
        parameter.type2 = boost::lexical_cast<std::string >(split_line[1]);
        parameter.type3 = boost::lexical_cast<std::string >(split_line[2]);
        parameter.type4 = boost::lexical_cast<std::string >(split_line[3]);
        parameter.phi0  = boost::lexical_cast<double>(split_line[5]);
        parameter.cp    = boost::lexical_cast<double>(split_line[6]);
        parameter.mult  = boost::lexical_cast<unsigned int >(split_line[7]);

        parameters.push_back(parameter);

      }

    return parameters;
}


std::vector<TorsionInteraction> generate_torsion_interactions(phaistos::ChainFB *chain,
    const std::vector<TorsionParameter> &torsion_parameters) {

    using namespace phaistos;

    std::vector<TorsionInteraction> torsion_interactions;

    // all torsions
    for (AtomIterator<ChainFB, definitions::ALL> it2(*chain); !it2.end(); ++it2) {
        Atom *atom2 = &*it2;
        for (AtomIterator<ChainFB, definitions::ALL> it3(*chain); !it3.end(); ++it3) {
            Atom *atom3 = &*it3;

            if ((atom2->residue->index >
                 atom3->residue->index ) ||
                ((atom2->residue->index == atom3->residue->index) &&
                 (atom2->index > atom3->index)))
                continue;

            if(chain_distance<ChainFB>(atom2,atom3) != 1)
                continue;
            for (CovalentBondIterator<ChainFB> it1(atom2, CovalentBondIterator<ChainFB>::DEPTH_1_ONLY);
                !it1.end(); ++it1) {
                Atom *atom1 = &*it1;
                for (CovalentBondIterator<ChainFB> it4(atom3, CovalentBondIterator<ChainFB>::DEPTH_1_ONLY);
                    !it4.end(); ++it4) {
                    Atom *atom4 = &*it4;

                    if ((atom2 != atom4) && (atom1 != atom3)) {

                        bool found_this_parameter = false;

                        std::string type1 = eef1_sb_parser::get_atom_type(atom1);
                        std::string type2 = eef1_sb_parser::get_atom_type(atom2);
                        std::string type3 = eef1_sb_parser::get_atom_type(atom3);
                        std::string type4 = eef1_sb_parser::get_atom_type(atom4);

                        for (unsigned int i = 0; i < torsion_parameters.size(); i++) {

                            TorsionParameter p = torsion_parameters[i];

                            if ((p.type1 == type1) && (p.type2 == type2) && (p.type3 == type3)&& (p.type4 == type4)) {
                                TorsionInteraction torsion_interaction;
                                torsion_interaction.atom1 = atom1;
                                torsion_interaction.atom2 = atom2;
                                torsion_interaction.atom3 = atom3;
                                torsion_interaction.atom4 = atom4;
                                torsion_interaction.phi0  = p.phi0;
                                torsion_interaction.cp    = p.cp;
                                torsion_interaction.mult  = p.mult;
                                found_this_parameter = true;
                                torsion_interactions.push_back(torsion_interaction);

                            } else if ((p.type1 == type4) && (p.type2 == type3) && (p.type3 == type2)&& (p.type4 == type1)) {
                                TorsionInteraction torsion_interaction;
                                torsion_interaction.atom1 = atom4;
                                torsion_interaction.atom2 = atom3;
                                torsion_interaction.atom3 = atom2;
                                torsion_interaction.atom4 = atom1;
                                torsion_interaction.phi0  = p.phi0;
                                torsion_interaction.cp    = p.cp;
                                torsion_interaction.mult  = p.mult;
                                found_this_parameter = true;
                                torsion_interactions.push_back(torsion_interaction);
                            }
                        }

                        if (!found_this_parameter) {
                            for (unsigned int i = 0; i < torsion_parameters.size(); i++) {

                                TorsionParameter p = torsion_parameters[i];

                                if ((p.type1 == "X") && (p.type2 == type2) && (p.type3 == type3)&& (p.type4 == "X")) {
                                    TorsionInteraction torsion_interaction;
                                    torsion_interaction.atom1 = atom1;
                                    torsion_interaction.atom2 = atom2;
                                    torsion_interaction.atom3 = atom3;
                                    torsion_interaction.atom4 = atom4;
                                    torsion_interaction.phi0  = p.phi0;
                                    torsion_interaction.cp    = p.cp;
                                    torsion_interaction.mult  = p.mult;
                                    found_this_parameter = true;
                                    torsion_interactions.push_back(torsion_interaction);
                                    break;
                                } else if ((p.type1 == "X") && (p.type2 == type3) && (p.type3 == type2)&& (p.type4 == "X")) {
                                    TorsionInteraction torsion_interaction;
                                    torsion_interaction.atom1 = atom4;
                                    torsion_interaction.atom2 = atom3;
                                    torsion_interaction.atom3 = atom2;
                                    torsion_interaction.atom4 = atom1;
                                    torsion_interaction.phi0  = p.phi0;
                                    torsion_interaction.cp    = p.cp;
                                    torsion_interaction.mult  = p.mult;
                                    found_this_parameter = true;
                                    torsion_interactions.push_back(torsion_interaction);
                                    break;
                                }
                            }
                            if (!found_this_parameter) {
                                std::cout << "ASC: TORERR COULD NOT FIND DIHEDRAL PARAM" << atom1 << atom2 << atom3 << atom4 << std::endl;
                                std::cout << "ASC: TORERR COULD NOT FIND DIHEDRAL PARAM   " << type1 << "  " << type2 << "  " << type3 << "  " << type4 << "  " << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
    return torsion_interactions;
}


std::vector<BondedPairParameter> read_bonded_pair_parameters(const std::string &itp) {

    std::istringstream input_stream(itp);

    std::vector<BondedPairParameter> parameters;

    while (input_stream.good()) {

        std::string line;
        std::getline(input_stream, line);

        boost::trim(line);

        if (line.size() == 0 || line[0] == ';') {
            continue;
        }

        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" \t"), boost::token_compress_on);

        BondedPairParameter parameter;

        parameter.type1 = boost::lexical_cast<std::string >(split_line[0]);
        parameter.type2 = boost::lexical_cast<std::string >(split_line[1]);
        parameter.r0    = boost::lexical_cast<double>(split_line[3]);
        parameter.kb    = boost::lexical_cast<double>(split_line[4]);

        parameters.push_back(parameter);

      }

    return parameters;
}


std::vector<BondedPairInteraction> generate_bonded_pair_interactions(phaistos::ChainFB *chain,
                    std::vector<BondedPairParameter> bonded_pair_parameters) {

    using namespace phaistos;

    std::vector<BondedPairInteraction> bonded_pairs;


    for (AtomIterator<ChainFB,definitions::ALL> it1(*(chain)); !it1.end(); ++it1) {
        Atom *atom1 = &*it1;
        std::string type1 = eef1_sb_parser::get_atom_type(atom1);

        for (CovalentBondIterator<ChainFB> it2(atom1, CovalentBondIterator<ChainFB>::DEPTH_1_ONLY);
            !it2.end(); ++it2) {

            Atom *atom2 = &*it2;
            if (atom1->residue->index < atom2->residue->index ||
                (atom1->residue->index == atom2->residue->index && atom1->index < atom2->index)) {

                std::string type2 = eef1_sb_parser::get_atom_type(atom2);

                for (unsigned int i = 0; i < bonded_pair_parameters.size(); i++) {

                    BondedPairParameter parameter = bonded_pair_parameters[i];

                    if (((parameter.type1 == type1) && (parameter.type2 == type2)) ||
                        ((parameter.type1 == type2) && (parameter.type2 == type1))) {

                        BondedPairInteraction pair;
                        pair.atom1 = atom1;
                        pair.atom2 = atom2;
                        pair.kb = parameter.kb;
                        pair.r0 = parameter.r0;

                        bonded_pairs.push_back(pair);

                        break;
                    }
                }
            }
        }
    }

    return bonded_pairs;

}

std::vector<AngleBendParameter> read_angle_bend_parameters(const std::string &itp) {

    std::istringstream input_stream(itp);

    std::vector<AngleBendParameter> parameters;

    while (input_stream.good()) {

        std::string line;
        std::getline(input_stream, line);

        boost::trim(line);

        // std::cout << line << std::endl;
        if (line.size() == 0 || line[0] == ';') {
            continue;
        }

        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" \t"), boost::token_compress_on);

        AngleBendParameter parameter;

        parameter.type1  = boost::lexical_cast<std::string >(split_line[0]);
        parameter.type2  = boost::lexical_cast<std::string >(split_line[1]);
        parameter.type3  = boost::lexical_cast<std::string >(split_line[2]);
        parameter.theta0 = boost::lexical_cast<double>(split_line[4]);
        parameter.k0     = boost::lexical_cast<double>(split_line[5]);
        parameter.r13    = boost::lexical_cast<double>(split_line[6]);
        parameter.kub    = boost::lexical_cast<double>(split_line[7]);

        parameters.push_back(parameter);

      }

    return parameters;
}


std::vector<AngleBendInteraction> generate_angle_bend_interactions(phaistos::ChainFB *chain,
    const std::vector<AngleBendParameter> &angle_bend_parameters) {

    using namespace phaistos;

    std::vector<AngleBendInteraction> angle_bend_interactions;

    for (AtomIterator<ChainFB,definitions::ALL> it1(*(chain)); !it1.end(); ++it1) {
        Atom *atom2 = &*it1;

        std::string type2 = eef1_sb_parser::get_atom_type(atom2);
        for (CovalentBondIterator<ChainFB> it2(atom2, CovalentBondIterator<ChainFB>::DEPTH_1_ONLY);
            !it2.end(); ++it2) {

            Atom *atom1 = &*it2;
            std::string type1 = eef1_sb_parser::get_atom_type(atom1);

            CovalentBondIterator<ChainFB> it3(it2);
            // Fancy way to discard it3 = it1
            ++it3;
            for (; !it3.end(); ++it3) {
                Atom *atom3 = &*it3;

                std::string type3 = eef1_sb_parser::get_atom_type(atom3);

                bool found_this_parameter = false;

                for (unsigned int i = 0; i < angle_bend_parameters.size(); i++) {

                    AngleBendParameter parameter = angle_bend_parameters[i];

                    if (((parameter.type1 == type1) && (parameter.type2 == type2) && (parameter.type3 == type3)) ||
                        ((parameter.type1 == type3) && (parameter.type2 == type2) && (parameter.type3 == type1))) {

                        AngleBendInteraction interaction;
                        interaction.atom1 = atom1;
                        interaction.atom2 = atom2;
                        interaction.atom3 = atom3;
                        interaction.theta0 = parameter.theta0;
                        interaction.k0     = parameter.k0;
                        interaction.r13    = parameter.r13;
                        interaction.kub    = parameter.kub;

                        found_this_parameter = true;
                        angle_bend_interactions.push_back(interaction);

                        break;
                    }
                }
                if (!found_this_parameter) {
                    std::cout << "ASC: COULD NOT FIND angle bend PARAM" << atom1 << atom2 << atom3 << std::endl;
                    std::cout << "ASC: COULD NOT FIND angle-bend PARAM   " << type1 << "  " << type2 << "  " << type3 << std::endl;
                }
            }
        }
    }

    return angle_bend_interactions;

}


std::vector<ImproperTorsionParameter> read_improper_torsion_parameters(const std::string &itp) {

    std::istringstream input_stream(itp);

    std::vector<ImproperTorsionParameter> parameters;

    while (input_stream.good()) {

        std::string line;
        std::getline(input_stream, line);

        boost::trim(line);

        // std::cout << line << std::endl;
        if (line.size() == 0 || line[0] == ';') {
            continue;
        }

        std::vector<std::string> split_line;
        boost::split(split_line, line, boost::is_any_of(" \t"), boost::token_compress_on);

        ImproperTorsionParameter parameter;

        parameter.type1 = boost::lexical_cast<std::string >(split_line[0]);
        parameter.type2 = boost::lexical_cast<std::string >(split_line[1]);
        parameter.type3 = boost::lexical_cast<std::string >(split_line[2]);
        parameter.type4 = boost::lexical_cast<std::string >(split_line[3]);
        parameter.phi0  = boost::lexical_cast<double>(split_line[5]);
        parameter.cp    = boost::lexical_cast<double>(split_line[6]);

        parameters.push_back(parameter);

      }

    return parameters;
}


ImproperTorsionInteraction atoms_to_improper_torsion(const std::vector<phaistos::Atom*> &atoms,
    const std::vector<ImproperTorsionParameter> &improper_torsion_parameters) {

    ImproperTorsionInteraction improper_torsion;

    improper_torsion.atom1 = atoms[0];
    improper_torsion.atom2 = atoms[1];
    improper_torsion.atom3 = atoms[2];
    improper_torsion.atom4 = atoms[3];

    std::string type1 = eef1_sb_parser::get_atom_type(atoms[0]);
    std::string type2 = eef1_sb_parser::get_atom_type(atoms[1]);
    std::string type3 = eef1_sb_parser::get_atom_type(atoms[2]);
    std::string type4 = eef1_sb_parser::get_atom_type(atoms[3]);

    bool found_parameter = false;

    for (unsigned int i = 0; i < improper_torsion_parameters.size(); i++) {

        ImproperTorsionParameter p = improper_torsion_parameters[i];

        if (((p.type1 == type1) && (p.type2 == type2) && (p.type3 == type3) && (p.type4 == type4)) ||
            ((p.type1 == type4) && (p.type2 == type3) && (p.type3 == type2) && (p.type4 == type1)) ||
            ((p.type1 == type1) && (p.type2 == "X"  ) && (p.type3 == "X"  ) && (p.type4 == type4)) ||
            ((p.type1 == type4) && (p.type2 == "X"  ) && (p.type3 == "X"  ) && (p.type4 == type1))) {

            found_parameter = true;

            improper_torsion.phi0 = p.phi0;
            improper_torsion.cp = p.cp;
        }
    }

    if (!found_parameter)
        std::cout << "ERROR: DIDN'T FIND PARAMETER" << std::endl;

    return improper_torsion;
}


std::vector<ImproperTorsionInteraction> generate_improper_torsion_interactions(phaistos::ChainFB *chain,
    const std::vector<ImproperTorsionParameter> &improper_torsion_parameters) {

    using namespace phaistos;
    using namespace definitions;
    using namespace vector_utils;

    std::vector<ImproperTorsionInteraction> improper_torsions;

    for (ResidueIterator<ChainFB> res(*(chain)); !(res).end(); ++res) {


        std::vector<std::vector<AtomEnum> > enum_pairs;


        switch (res->residue_type) {
        case ALA:
            break;

        case ARG:
            enum_pairs.push_back(make_vector(CZ,      NH1,     NH2,     NE));
            break;

        case ASN:
            enum_pairs.push_back(make_vector(CG,      ND2,     CB,      OD1));
            enum_pairs.push_back(make_vector(CG,      CB,      ND2,     OD1));
            enum_pairs.push_back(make_vector(ND2,     CG,      HD21,    HD22));
            enum_pairs.push_back(make_vector(ND2,    CG,      HD22,    HD21));
            break;

        case ASP:
            enum_pairs.push_back(make_vector(CG,      CB,      OD2,     OD1));
            break;

        case CYS:
            break;

        case GLN:
	        enum_pairs.push_back(make_vector(CD,	NE2,	CG,	OE1));
        	enum_pairs.push_back(make_vector(CD,	CG,	NE2,	OE1));
        	enum_pairs.push_back(make_vector(NE2,	CD,	HE21,	HE22));
        	enum_pairs.push_back(make_vector(NE2,	CD,	HE22,	HE21));
            break;

        case GLU:
         	enum_pairs.push_back(make_vector(CD,	CG,	OE2,	OE1));
            break;

        case GLY:
            break;

        case HIS:
            if (res->has_atom(HD1) && res->has_atom(HE2)) {
                enum_pairs.push_back(make_vector(ND1, 	CG	, CE1,	HD1));
            	enum_pairs.push_back(make_vector(ND1, 	CE1	, CG , HD1));
            	enum_pairs.push_back(make_vector(NE2, 	CD2	, CE1,	HE2));
            	enum_pairs.push_back(make_vector(NE2, 	CE1	, CD2,	HE2));
            } else if (!(res->has_atom(HD1)) && res->has_atom(HE2)) {
            	enum_pairs.push_back(make_vector(NE2, 	CD2	, CE1,	HE2));
            	enum_pairs.push_back(make_vector(CD2, 	CG	, NE2,	HD2));
            	enum_pairs.push_back(make_vector(CE1, 	ND1	, NE2,	HE1));
            	enum_pairs.push_back(make_vector(NE2, 	CE1	, CD2,	HE2));
            	enum_pairs.push_back(make_vector(CD2, 	NE2	, CG , HD2));
        	    enum_pairs.push_back(make_vector(CE1, 	NE2	, ND1,	HE1));
            } else if (res->has_atom(HD1) && !(res->has_atom(HE2))) {
            	enum_pairs.push_back(make_vector(ND1, 	CG	, CE1,	HD1));
            	enum_pairs.push_back(make_vector(CD2, 	CG	, NE2,	HD2));
            	enum_pairs.push_back(make_vector(CE1, 	ND1	, NE2,	HE1));
            	enum_pairs.push_back(make_vector(ND1, 	CE1	, CG , HD1));
        	    enum_pairs.push_back(make_vector(CD2, 	NE2	, CG , HD2));
            	enum_pairs.push_back(make_vector(CE1, 	NE2	, ND1,	HE1));
            } else {
                std::cout << "Unknown protonations state on " << res << std::endl;
            }
            break;

        case ILE:
            break;

        case LEU:
            break;

        case LYS:
            break;

        case MET:
            break;

        case PHE:
            break;

        case PRO:
            break;

        case SER:
            break;

        case THR:
            break;

        case TRP:
            break;

        case TYR:
            break;

        case VAL:
            break;

        default:
            std::cout << "ASC: Unknown residue type: " << res << std::endl;
            break;
        };


        for (unsigned int i = 0; i < enum_pairs.size(); i++) {

            ImproperTorsionInteraction sc_improper_torsion 
                = atoms_to_improper_torsion(make_vector((*res)[enum_pairs[i][0]],
                                                        (*res)[enum_pairs[i][1]],
                                                        (*res)[enum_pairs[i][2]],
                                                        (*res)[enum_pairs[i][3]]),
                                            improper_torsion_parameters);

            improper_torsions.push_back(sc_improper_torsion);


        }

        Residue *previous_residue = res->get_neighbour(-1);
        Residue *next_residue     = res->get_neighbour(+1);

         //N   -C  CA  HN
         if (!(res->terminal_status == NTERM)) {

             AtomEnum amide_atom = H;

             if (res->residue_type == PRO)
                amide_atom = CD;

             ImproperTorsionInteraction bb_improper_torsion 
                 = atoms_to_improper_torsion(make_vector((*res)[N],
                                                         (*previous_residue)[C],
                                                         (*res)[CA],
                                                         (*res)[amide_atom]),
                                                improper_torsion_parameters);

             improper_torsions.push_back(bb_improper_torsion);
         }

          //C   CA  +N  O
         if (!(res->terminal_status == CTERM)) {

             ImproperTorsionInteraction bb_improper_torsion 
                 = atoms_to_improper_torsion(make_vector((*res)[C],
                                                         (*res)[CA],
                                                         (*next_residue)[N],
                                                         (*res)[O]),
                                                improper_torsion_parameters);

             improper_torsions.push_back(bb_improper_torsion);
         } else if (res->terminal_status == CTERM) {

             ImproperTorsionInteraction bb_improper_torsion 
                 = atoms_to_improper_torsion(make_vector((*res)[C],
                                                         (*res)[CA],
                                                         (*res)[OXT],
                                                         (*res)[O]),
                                                improper_torsion_parameters);

             improper_torsions.push_back(bb_improper_torsion);
         }

    }

    return improper_torsions;

}

} // End namespace topology
#endif
