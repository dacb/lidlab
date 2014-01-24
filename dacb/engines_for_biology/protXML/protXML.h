/*  Copyright (C) 2009 David A. C. Beck

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef _PROTXML_H
#define _PROTXML_H

#include "util.h"

/* attribute and element names */
#define PROTXML_PROTEIN_GROUP "protein_group"
#define PROTXML_GROUP_NUMBER "group_number"
#define PROTXML_PSEUDO_NAME "pseudo_name"
#define PROTXML_PROBABILITY "probability"
#define PROTXML_PROTEIN "protein"
#define PROTXML_PROTEIN_NAME "protein_name"
#define PROTXML_PERCENT_COVERAGE "percent_coverage"
#define PROTXML_UNIQUE_STRIPPED_PEPTIDES "unique_stripped_peptides"
#define PROTXML_GROUP_SIBLING_ID "group_sibling_id"
#define PROTXML_TOTAL_NUMBER_PEPTIDES "total_number_peptides"
#define PROTXML_SUBSUMING_PROTEIN_ENTRY "subsuming_protein_entry"
#define PROTXML_PCT_SPECTRUM_IDS "pct_spectrum_ids"
#define PROTXML_CONFIDENCE "confidence"
#define PROTXML_N_INDISTINGUISHABLE_PROTEINS "n_indistinguishable_proteins"
#define PROTXML_INDISTINGUISHABLE_PROTEIN "indistinguishable_protein"
#define PROTXML_ANNOTATION "annotation"
#define PROTXML_PEPTIDE "peptide"
#define PROTXML_CHARGE "charge"
#define PROTXML_INITIAL_PROBABILITY "initial_probability"
#define PROTXML_NSP_ADJUSTED_PROBABILITY "nsp_adjusted_probability"
#define PROTXML_NI_ADJUSTED_PROBABILITY "ni_adjusted_probability"
#define PROTXML_EXP_SIBLING_ION_INSTANCES "exp_sibling_ion_instances"
#define PROTXML_EXP_SIBLING_ION_BIN "exp_sibling_ion_bin"
#define PROTXML_EXP_TOT_INSTANCES "exp_tot_instances"
#define PROTXML_PEPTIDE_GROUP_DESIGNATOR "peptide_group_designator"
#define PROTXML_WEIGHT "weight"
#define PROTXML_IS_NONDEGENERATE_EVIDENCE "is_nondegenerate_evidence"
#define PROTXML_N_ENZYMATIC_TERMINI "n_enzymatic_termini"
#define PROTXML_N_SIBLING_PEPTIDES "n_sibling_peptides"
#define PROTXML_N_SIBLING_PEPTIDES_BIN "n_sibling_peptides_bin"
#define PROTXML_N_INSTANCES "n_instances"
#define PROTXML_IS_CONTRIBUTING_EVIDENCE "is_contributing_evidence"
#define PROTXML_PROTEIN_DESCRIPTION "protein_description"
#define PROTXML_IPI_NAME "ipi_name"
#define PROTXML_REFSEQ_NAME "refseq_name"
#define PROTXML_SWISSPROT_NAME "swissprot_name"
#define PROTXML_ENSEMBL_NAME "ensembl_name"
#define PROTXML_TREMBL_NAME "trembl_name"
#define PROTXML_LOCUS_LINK_NAME "locus_link_name"
#define PROTXML_FLYBASE "flybase"
#define PROTXML_INDISTINGUISHABLE_PEPTIDE "indistinguishable_peptide"
#define PROTXML_PEPTIDE_SEQUENCE "peptide_sequence"
#define PROTXML_PEPTIDE_PARENT_PROTEIN "peptide_parent_protein"
#define PROTXML_MODIFICATION_INFO "modification_info"
#define PROTXML_MODIFIED_PEPTIDE "modified_peptide"
#define PROTXML_MOD_NTERM_MASS "mod_nterm_mass"
#define PROTXML_MOD_CTERM_MASS "mod_cterm_mass"
#define PROTXML_CALC_NEUTRAL_PEP_MASS "calc_neutral_pep_mass"
#define PROTXML_MOD_AMINOACID_MASS "mod_aminoacid_mass"
#define PROTXML_POSITION "position"
#define PROTXML_MASS "mass"
#define PROTXML_PARAMETER "parameter"
#define PROTXML_NAME "name"
#define PROTXML_VALUE "value"

/* parser states */
#define ROOT 1
#define PROTEIN_GROUP 2
#define PROTEIN 3 
#define INDISTINGUISHABLE_PROTEIN 4
#define PEPTIDE 5
#define INDISTINGUISHABLE_PEPTIDE 6
#define MODIFICATION_INFO 7

struct protXML_parser_data {
	struct util_stack *state;
	struct util_ptr_list *protein_group;
	struct protein_group *current_protein_group;
	struct protein *current_protein;
	struct indistinguishable_protein *current_indistinguishable_protein;
	struct peptide *current_peptide;
	struct indistinguishable_peptide *current_indistinguishable_peptide;
	struct modification_info *current_modification_info;
};

struct mod_aminoacid_mass {
	char *position;
	char *mass;
	bool position_found;
	bool mass_found;
};

struct modification_info {
	char *modified_peptide;
	char *mod_nterm_mass;
	char *mod_cterm_mass;
	char *calc_neutral_pep_mass;
	bool modified_peptide_found;
	bool mod_nterm_mass_found;
	bool mod_cterm_mass_found;
	bool calc_neutral_pep_mass_found;
	struct util_ptr_list *mod_aminoacid_mass;
};

struct peptide_parent_protein {
	char *protein_name;
	bool protein_name_found;
};

struct indistinguishable_peptide {
	char *peptide_sequence;
	char *charge;
	char *calc_neutral_pep_mass;
	bool peptide_sequence_found;
	bool charge_found;
	bool calc_neutral_pep_mass_found;
	struct util_ptr_list *modification_info;
};

struct peptide {
	char *peptide_sequence;
	char *charge;
	char *initial_probability;
	char *nsp_adjusted_probability;		/* optional */
	char *ni_adjusted_probability;		/* optional */
	char *exp_sibling_ion_instances;	/* optional */
	char *exp_sibling_ion_bin;		/* optional */
	char *exp_tot_instances;		/* optional */
	char *peptide_group_designator;
	char *weight;				/* default 1 */
	char *is_nondegenerate_evidence;
	char *n_enzymatic_termini;
	char *n_sibling_peptides;		/* should this really be double? */
	char *n_sibling_peptides_bin;		/* discretized version of previous, default 0 */
	char *n_instances;			/* should this be unsigned */
	char *calc_neutral_pep_mass;
	char *is_contributing_evidence;
	bool peptide_sequence_found;
	bool charge_found;
	bool initial_probability_found;
	bool nsp_adjusted_probability_found;
	bool ni_adjusted_probability_found;
	bool exp_sibling_ion_instances_found;
	bool exp_sibling_ion_bin_found;
	bool exp_tot_instances_found;
	bool peptide_group_designator_found;
	bool weight_found;
	bool is_nondegenerate_evidence_found;
	bool n_enzymatic_termini_found;
	bool n_sibling_peptides_found;
	bool n_sibling_peptides_bin_found;
	bool n_instances_found;
	bool calc_neutral_pep_mass_found;
	bool is_contributing_evidence_found;
	struct util_ptr_list *peptide_parent_protein;
	struct util_ptr_list *indistinguishable_peptide;
	struct util_ptr_list *parameter;	/* not implemented */
	struct util_ptr_list *modification_info;
};

struct parameter {
	char *name;
	char *value;
	bool name_found;
	bool value_found;
};

struct annotation {
	char *protein_description;
	char *ipi_name;
	char *refseq_name;
	char *swissprot_name;
	char *ensembl_name;
	char *trembl_name;
	char *locus_link_name;
	char *flybase;
	bool protein_description_found;
	bool ipi_name_found;
	bool refseq_name_found;
	bool swissprot_name_found;
	bool ensembl_name_found;
	bool trembl_name_found;
	bool locus_link_name_found;
	bool flybase_found;
};

struct indistinguishable_protein {
	char *protein_name;
	bool protein_name_found;
	struct util_ptr_list *parameter;	/* not implemented */
	struct annotation annotation;
};

struct protein {
	char *protein_name;
	char *n_indistinguishable_proteins;
	char *probability;
	char *percent_coverage;
	char *unique_stripped_peptides;
	char *group_sibling_id;
	char *total_number_peptides;
	char *subsuming_protein_entry;
	char *pct_spectrum_ids;
	char *confidence;
	bool protein_name_found;
	bool n_indistinguishable_proteins_found;
	bool probability_found;
	bool percent_coverage_found;
	bool unique_stripped_peptides_found;
	bool group_sibling_id_found;
	bool total_number_peptides_found;
	bool subsuming_protein_entry_found;
	bool pct_spectrum_ids_found;
	bool confidence_found;
	/* parameter */
	struct util_ptr_list *parameter;	/* not implemented */
	/* analysis result */
	struct util_ptr_list *analysis_result;	/* not implemented */
	/* annotation */
	struct annotation annotation;
	/* indistinguishable protein list */
	struct util_ptr_list *indistinguishable_protein;
	/* peptides */
	struct util_ptr_list *peptide;
};

struct protein_group {
	char *group_number;
	char *pseudo_name;
	char *probability;
	bool group_number_found;
	bool pseudo_name_found;
	bool probability_found;
	struct util_ptr_list *protein;
};

void protXML_start_handler(void *data, const char *el, const char **attr);
void protXML_end_handler(void *data, const char *el);

struct protXML_parser_data *protXML_parse_file(char *filename);

struct protXML_parser_data *protXML_parser_data_malloc(void);
void protXML_parser_data_free(protXML_parser_data *pxpd);

struct protein_group *protein_group_malloc(void);
void protein_group_free(struct protein_group *pg);

struct protein *protein_malloc(void);
void protein_free(struct protein *p);

struct parameter *parameter_malloc(void);
void parameter_free(struct parameter *p);

void annotation_free(struct annotation *a);

struct indistinguishable_protein *indistinguishable_protein_malloc(void);
void indistinguishable_protein_free(struct indistinguishable_protein *ip);

struct peptide *peptide_malloc(void);
void peptide_free(struct peptide *p);

struct peptide_parent_protein *peptide_parent_protein_malloc(void);
void peptide_parent_protein_free(struct peptide_parent_protein *ppp);

struct indistinguishable_peptide *indistinguishable_peptide_malloc(void);
void indistinguishable_peptide_free(struct indistinguishable_peptide *ip);

struct modification_info *modification_info_malloc(void);
void modification_info_free(struct modification_info *mi);

struct mod_aminoacid_mass *mod_aminoacid_mass_malloc(void);
void mod_aminoacid_mass_free(struct mod_aminoacid_mass *maam);

#endif /* _PROTXML_H */
