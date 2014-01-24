/*  Copyright (C) 2010 David A. C. Beck

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <expat.h>

#include "protXML.h"

#define BUFF_SIZE	8192

void protXML_start_handler(void *data, const char *el, const char **attr) {
	int i;
	struct protXML_parser_data *pxpd = (struct protXML_parser_data *)data;
	struct protein_group *pg;

	if (strcmp(el, PROTXML_PROTEIN_GROUP) == 0) {
		struct protein_group *pg;

		/* the start of a new protein group has been detected */
		util_stack_push(pxpd->state, (void *)PROTEIN_GROUP);
		/* allocate new protein group */
		pg = protein_group_malloc();
		util_ptr_list_add_ptr(pxpd->protein_group, pg);
		pxpd->current_protein_group = pg;
		/* parse attributes */
		for (i = 0; attr[i]; i += 2) {
			if (!pg->group_number_found && strcmp(attr[i], PROTXML_GROUP_NUMBER) == 0) {
				pg->group_number_found = true;
				pg->group_number = strdup(attr[i+1]);
			} else if (!pg->pseudo_name_found && strcmp(attr[i], PROTXML_PSEUDO_NAME) == 0) {
				pg->pseudo_name_found = true;
				pg->pseudo_name = strdup(attr[i+1]);
			} else if (!pg->probability_found && strcmp(attr[i], PROTXML_PROBABILITY) == 0) {
				pg->probability_found = true;
				pg->probability = strdup(attr[i+1]);
			} else {
				fprintf(stderr, "on protein group element, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
			}
		}
	} else if (strcmp(el, PROTXML_PROTEIN) == 0) {
		struct protein *p;
		/* start of new protein */
		util_stack_push(pxpd->state, (void *)PROTEIN);
		/* allocate new protein */
		p = protein_malloc();
		/* add protein to protein group */
		pg = pxpd->current_protein_group;
		util_ptr_list_add_ptr(pg->protein, p);
		pxpd->current_protein = p;

		/* parse attributes */
		for (i = 0; attr[i]; i += 2) {
			if (!p->protein_name_found && strcmp(attr[i], PROTXML_PROTEIN_NAME) == 0) {
				p->protein_name_found = true;
				p->protein_name = strdup(attr[i+1]);
			} else if (!p->n_indistinguishable_proteins_found && strcmp(attr[i], PROTXML_N_INDISTINGUISHABLE_PROTEINS) == 0) {
				p->n_indistinguishable_proteins_found = true;
				p->n_indistinguishable_proteins = strdup(attr[i+1]);
			} else if (!p->probability_found && strcmp(attr[i], PROTXML_PROBABILITY) == 0) {
				p->probability_found = true;
				p->probability = strdup(attr[i+1]);
			} else if (!p->percent_coverage_found && strcmp(attr[i], PROTXML_PERCENT_COVERAGE) == 0) {
				p->percent_coverage_found = true;
				p->percent_coverage = strdup(attr[i+1]);
			} else if (!p->unique_stripped_peptides_found && strcmp(attr[i], PROTXML_UNIQUE_STRIPPED_PEPTIDES) == 0) {
				p->unique_stripped_peptides_found = true;
				p->unique_stripped_peptides = strdup(attr[i+1]);
			} else if (!p->group_sibling_id_found && strcmp(attr[i], PROTXML_GROUP_SIBLING_ID) == 0) {
				p->group_sibling_id_found = true;
				p->group_sibling_id = strdup(attr[i+1]);
			} else if (!p->total_number_peptides_found && strcmp(attr[i], PROTXML_TOTAL_NUMBER_PEPTIDES) == 0) {
				p->total_number_peptides_found = true;
				p->total_number_peptides = strdup(attr[i+1]);
			} else if (!p->subsuming_protein_entry_found && strcmp(attr[i], PROTXML_SUBSUMING_PROTEIN_ENTRY) == 0) {
				p->subsuming_protein_entry_found = true;
				p->subsuming_protein_entry = strdup(attr[i+1]);
			} else if (!p->pct_spectrum_ids_found && strcmp(attr[i], PROTXML_PCT_SPECTRUM_IDS) == 0) {
				p->pct_spectrum_ids_found = true;
				p->pct_spectrum_ids = strdup(attr[i+1]);
			} else if (!p->confidence_found && strcmp(attr[i], PROTXML_CONFIDENCE) == 0) {
				p->confidence_found = true;
				p->confidence = strdup(attr[i+1]);
			} else {
				fprintf(stderr, "on protein element, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
			}
		}
	} else if (strcmp(el, PROTXML_PARAMETER) == 0) {
		intptr_t state;
		struct parameter *p;
		/* allocate new parameter */
		p = parameter_malloc();

		/* parse attributes */
		for (i = 0; attr[i]; i += 2) {
			if (!p->name_found && strcmp(attr[i], PROTXML_NAME) == 0) {
				p->name_found = 1;
				p->name = strdup(attr[i+1]);
			} else if (!p->value_found && strcmp(attr[i], PROTXML_VALUE) == 0) {
				p->value_found = 1;
				p->value = strdup(attr[i+1]);
			} else {
				fprintf(stderr, "on parameter element, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
			}
		}

		/* attach to correct object */
		state = (intptr_t)util_stack_peek(pxpd->state);
		switch (state) {
			case PROTEIN:
				if (!pxpd->current_protein) {
					fprintf(stderr, "parameter element found while in state PROTEIN without valid protein on stack\n");
				} else {
					util_ptr_list_add_ptr(pxpd->current_protein->parameter, p);
				}
				break;
			case PEPTIDE:
				if (!pxpd->current_peptide) {
					fprintf(stderr, "parameter element found while in state PROTEIN without valid protein on stack\n");
				} else {
					util_ptr_list_add_ptr(pxpd->current_peptide->parameter, p);
				}
				break;
			case INDISTINGUISHABLE_PROTEIN:
				if (!pxpd->current_indistinguishable_protein) {
					fprintf(stderr, "parameter element found while in state PROTEIN without valid protein on stack\n");
				} else {
					util_ptr_list_add_ptr(pxpd->current_indistinguishable_protein->parameter, p);
				}
				break;
			default:
				fprintf(stderr, "parameter element found on an unexpected element (state = %d)\n", (int)state);
				parameter_free(p);
				break;
		};
	} else if (strcmp(el, PROTXML_MODIFICATION_INFO) == 0) {
		intptr_t state;
		struct modification_info *mi;
		/* allocate new structure */
		mi = modification_info_malloc();

		pxpd->current_modification_info = mi;
		/* parse attributes */
		for (i = 0; attr[i]; i += 2) {
			if (!mi->modified_peptide && strcmp(attr[i], PROTXML_MODIFIED_PEPTIDE) == 0) {
				mi->modified_peptide_found = 1;
				mi->modified_peptide = strdup(attr[i+1]);
			} else if (!mi->mod_nterm_mass_found && strcmp(attr[i], PROTXML_MOD_NTERM_MASS) == 0) {
				mi->mod_nterm_mass_found = 1;
				mi->mod_nterm_mass = strdup(attr[i+1]);
			} else if (!mi->mod_cterm_mass_found && strcmp(attr[i], PROTXML_MOD_CTERM_MASS) == 0) {
				mi->mod_cterm_mass_found = 1;
				mi->mod_cterm_mass = strdup(attr[i+1]);
			} else if (!mi->calc_neutral_pep_mass_found && strcmp(attr[i], PROTXML_CALC_NEUTRAL_PEP_MASS) == 0) {
				mi->calc_neutral_pep_mass_found = 1;
				mi->calc_neutral_pep_mass = strdup(attr[i+1]);
			} else {
				fprintf(stderr, "on modification_info, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
			}
		}

		/* attach to correct object */
		state = (intptr_t)util_stack_peek(pxpd->state);
		switch (state) {
			case PEPTIDE:
				if (!pxpd->current_peptide) {
					fprintf(stderr, "in state PEPTIDE, modification_info element found without valid peptide on stack\n");
				} else {
					util_ptr_list_add_ptr(pxpd->current_peptide->modification_info, mi);
				}
				break;
			case INDISTINGUISHABLE_PEPTIDE:
				if (!pxpd->current_indistinguishable_peptide) {
					fprintf(stderr, "in state INDISTINGUISHABLE_PEPTIDE, modification_info element found without valid indistinguishable_peptide on stack\n");
				} else {
					util_ptr_list_add_ptr(pxpd->current_indistinguishable_peptide->modification_info, mi);
				}
				break;
			default:
				fprintf(stderr, "modification_info element found on an unexpected element (state = %d)\n", (int)state);
				modification_info_free(mi);
				pxpd->current_modification_info = NULL;
				break;
		};

		/* make sure we push after we peek */
		util_stack_push(pxpd->state, (void *)MODIFICATION_INFO);

	} else if (strcmp(el, PROTXML_MOD_AMINOACID_MASS) == 0) {
		struct mod_aminoacid_mass *maam;
		if (!pxpd->current_modification_info) {
			fprintf(stderr, "mod_aminoacid_mass element found without valid modification_info on stack\n");
		} else {
			int i;
			maam = mod_aminoacid_mass_malloc();
			/* parse attributes */
			for (i = 0; attr[i]; i+=2) {
				if (!maam->position_found && strcmp(attr[i], PROTXML_POSITION) == 0) {
					maam->position_found = true;
					maam->position = strdup(attr[i+1]);
				} else if (!maam->mass_found && strcmp(attr[i], PROTXML_MASS) == 0) {
					maam->mass_found = true;
					maam->mass = strdup(attr[i+1]);
				} else {
					fprintf(stderr, "on mod_aminoacid_mass element, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
				}
			}
			util_ptr_list_add_ptr(pxpd->current_modification_info->mod_aminoacid_mass, maam);
		}
	} else if (strcmp(el, PROTXML_PEPTIDE) == 0) {
		struct peptide *p;
		/* start of new peptide */
		util_stack_push(pxpd->state, (void *)PEPTIDE);
		if (!pxpd->current_protein) {
			fprintf(stderr, "peptide element found without valid protein on stack\n");
		} else {
			/* allocate new protein */
			p = peptide_malloc();
			/* add peptide to protein */
			util_ptr_list_add_ptr(pxpd->current_protein->peptide, p);
			pxpd->current_peptide = p;

			/* parse attributes */
			for (i = 0; attr[i]; i += 2) {
				if (!p->peptide_sequence_found && strcmp(attr[i], PROTXML_PEPTIDE_SEQUENCE) == 0) {
					p->peptide_sequence_found = true;
					p->peptide_sequence = strdup(attr[i+1]);
				} else if (!p->charge_found && strcmp(attr[i], PROTXML_CHARGE) == 0) {
					p->charge_found = true;
					p->charge = strdup(attr[i+1]);
				} else if (!p->initial_probability_found && strcmp(attr[i], PROTXML_INITIAL_PROBABILITY) == 0) {
					p->initial_probability_found = true;
					p->initial_probability = strdup(attr[i+1]);
				} else if (!p->nsp_adjusted_probability_found && strcmp(attr[i], PROTXML_NSP_ADJUSTED_PROBABILITY) == 0) {
					p->nsp_adjusted_probability_found = true;
					p->nsp_adjusted_probability = strdup(attr[i+1]);
				} else if (!p->ni_adjusted_probability_found && strcmp(attr[i], PROTXML_NI_ADJUSTED_PROBABILITY) == 0) {
					p->ni_adjusted_probability_found = true;
					p->ni_adjusted_probability = strdup(attr[i+1]);
				} else if (!p->exp_sibling_ion_instances_found && strcmp(attr[i], PROTXML_EXP_SIBLING_ION_INSTANCES) == 0) {
					p->exp_sibling_ion_instances_found = true;
					p->exp_sibling_ion_instances = strdup(attr[i+1]);
				} else if (!p->exp_sibling_ion_bin_found && strcmp(attr[i], PROTXML_EXP_SIBLING_ION_BIN) == 0) {
					p->exp_sibling_ion_bin_found = true;
					p->exp_sibling_ion_bin = strdup(attr[i+1]);
				} else if (!p->exp_tot_instances_found && strcmp(attr[i], PROTXML_EXP_TOT_INSTANCES) == 0) {
					p->exp_tot_instances_found = true;
					p->exp_tot_instances = strdup(attr[i+1]);
				} else if (!p->peptide_group_designator_found && strcmp(attr[i], PROTXML_PEPTIDE_GROUP_DESIGNATOR) == 0) {
					p->peptide_group_designator_found = true;
					p->peptide_group_designator = strdup(attr[i+1]);
				} else if (!p->weight_found && strcmp(attr[i], PROTXML_WEIGHT) == 0) {
					p->weight_found = true;
					p->weight = strdup(attr[i+1]);
				} else if (!p->is_nondegenerate_evidence_found && strcmp(attr[i], PROTXML_IS_NONDEGENERATE_EVIDENCE) == 0) {
					p->is_nondegenerate_evidence_found = true;
					p->is_nondegenerate_evidence = strdup(attr[i+1]);
				} else if (!p->n_enzymatic_termini_found && strcmp(attr[i], PROTXML_N_ENZYMATIC_TERMINI) == 0) {
					p->n_enzymatic_termini_found = true;
					p->n_enzymatic_termini = strdup(attr[i+1]);
				} else if (!p->n_sibling_peptides_found && strcmp(attr[i], PROTXML_N_SIBLING_PEPTIDES) == 0) {
					p->n_sibling_peptides_found = true;
					p->n_sibling_peptides = strdup(attr[i+1]);
				} else if (!p->n_sibling_peptides_bin_found && strcmp(attr[i], PROTXML_N_SIBLING_PEPTIDES_BIN) == 0) {
					p->n_sibling_peptides_bin_found = true;
					p->n_sibling_peptides_bin = strdup(attr[i+1]);
				} else if (!p->n_instances_found && strcmp(attr[i], PROTXML_N_INSTANCES) == 0) {
					p->n_instances_found = true;
					p->n_instances = strdup(attr[i+1]);
				} else if (!p->calc_neutral_pep_mass_found && strcmp(attr[i], PROTXML_CALC_NEUTRAL_PEP_MASS) == 0) {
					p->calc_neutral_pep_mass_found = true;
					p->calc_neutral_pep_mass = strdup(attr[i+1]);
				} else if (!p->is_contributing_evidence_found && strcmp(attr[i], PROTXML_IS_CONTRIBUTING_EVIDENCE) == 0) {
					p->is_contributing_evidence_found = true;
					p->is_contributing_evidence = strdup(attr[i+1]);
				} else {
					fprintf(stderr, "on peptide element, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
				}
			}
			/* set defaults */
			if (!p->weight_found)
				p->weight = strdup("1.0");
			if (!p->n_sibling_peptides_bin_found)
				p->n_sibling_peptides_bin = strdup("0");
		}
	} else if (strcmp(el, PROTXML_INDISTINGUISHABLE_PROTEIN) == 0) {
		int i;
		struct indistinguishable_protein *ip;

		util_stack_push(pxpd->state, (void *)INDISTINGUISHABLE_PROTEIN);

		if (!pxpd->current_protein) {
			fprintf(stderr, "indistinguishable_protein element found without valid protein on stack\n");
		} else {
			ip = indistinguishable_protein_malloc();
			util_ptr_list_add_ptr(pxpd->current_protein->indistinguishable_protein, ip);

			for (i = 0; attr[i]; i+=2) {
				if (!ip->protein_name_found && strcmp(attr[i], PROTXML_PROTEIN_NAME) == 0) {
					ip->protein_name_found = true;
					ip->protein_name = strdup(attr[i+1]);
				} else {
					fprintf(stderr, "on indistinguishable_protein, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
				}
			}
			pxpd->current_indistinguishable_protein = ip;
		}
	} else if (strcmp(el, PROTXML_INDISTINGUISHABLE_PEPTIDE) == 0) {
		int i;
		struct indistinguishable_peptide *ip;

		util_stack_push(pxpd->state, (void *)INDISTINGUISHABLE_PEPTIDE);

		if (!pxpd->current_peptide) {
			fprintf(stderr, "indistinguishable_peptide element found without valid peptide on stack\n");
		} else {
			ip = indistinguishable_peptide_malloc();
			util_ptr_list_add_ptr(pxpd->current_peptide->indistinguishable_peptide, ip);

			for (i = 0; attr[i]; i+=2) {
				if (!ip->peptide_sequence_found && strcmp(attr[i], PROTXML_PEPTIDE_SEQUENCE) == 0) {
					ip->peptide_sequence_found = true;
					ip->peptide_sequence = strdup(attr[i+1]);
				} else if (!ip->charge_found && strcmp(attr[i], PROTXML_CHARGE) == 0) {
					ip->charge_found = true;
					ip->charge = strdup(attr[i+1]);
				} else if (!ip->calc_neutral_pep_mass_found && strcmp(attr[i], PROTXML_CALC_NEUTRAL_PEP_MASS) == 0) {
					ip->calc_neutral_pep_mass_found = true;
					ip->calc_neutral_pep_mass = strdup(attr[i+1]);
				} else {
					fprintf(stderr, "on indistinguishable_peptide, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
				}
			}
			pxpd->current_indistinguishable_peptide = ip;
		}
	} else if (strcmp(el, PROTXML_PEPTIDE_PARENT_PROTEIN) == 0) {
		int i;
		struct peptide_parent_protein *ppp;

		if (!pxpd->current_peptide) {
			fprintf(stderr, "peptide_parent_protein element found without valid peptide on stack\n");
		} else {
			ppp = peptide_parent_protein_malloc();
			util_ptr_list_add_ptr(pxpd->current_peptide->peptide_parent_protein, ppp);
			for (i = 0; attr[i]; i+=2) {
				if (!ppp->protein_name_found && strcmp(attr[i], PROTXML_PROTEIN_NAME) == 0) {
					ppp->protein_name_found = true;
					ppp->protein_name = strdup(attr[i+1]);
				} else {
					fprintf(stderr, "on peptide_parent_protein element, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
				}
			}
		}
	} else if (strcmp(el, PROTXML_ANNOTATION) == 0) {
		int i;
		struct annotation *a = NULL;
		intptr_t state;

		/* attach to correct object */
		state = (intptr_t)util_stack_peek(pxpd->state);
		switch (state) {
			case PROTEIN:
				if (!pxpd->current_protein) {
					fprintf(stderr, "annotation element found while in state PROTEIN without valid protein on stack\n");
				} else {
					a = &pxpd->current_protein->annotation;
				}
				break;
			case INDISTINGUISHABLE_PROTEIN:
				if (!pxpd->current_indistinguishable_protein) {
					fprintf(stderr, "annotation element found while in state INDISTINGUISHABLE_PROTEIN without valid indistinguishable protein on stack\n");
				} else {
					a = &pxpd->current_indistinguishable_protein->annotation;
				}
				break;
			default:
				fprintf(stderr, "annotation element found in an unknown state (%d)\n", (int)state);
				break;
		};

		if (a) {
			for (i = 0; attr[i]; i+=2) {
				if (!a->protein_description_found && strcmp(attr[i], PROTXML_PROTEIN_DESCRIPTION) == 0) {
					a->protein_description_found = true;
					a->protein_description = strdup(attr[i+1]);
				} else if (!a->ipi_name_found && strcmp(attr[i], PROTXML_IPI_NAME) == 0) {
					a->ipi_name_found = true;
					a->ipi_name = strdup(attr[i+1]);
				} else if (!a->refseq_name_found && strcmp(attr[i], PROTXML_REFSEQ_NAME) == 0) {
					a->refseq_name_found = true;
					a->refseq_name = strdup(attr[i+1]);
				} else if (!a->swissprot_name_found && strcmp(attr[i], PROTXML_SWISSPROT_NAME) == 0) {
					a->swissprot_name_found = true;
					a->swissprot_name = strdup(attr[i+1]);
				} else if (!a->ensembl_name_found && strcmp(attr[i], PROTXML_ENSEMBL_NAME) == 0) {
					a->ensembl_name_found = true;
					a->ensembl_name = strdup(attr[i+1]);
				} else if (!a->trembl_name_found && strcmp(attr[i], PROTXML_TREMBL_NAME) == 0) {
					a->trembl_name_found = true;
					a->trembl_name = strdup(attr[i+1]);
				} else if (!a->locus_link_name_found && strcmp(attr[i], PROTXML_LOCUS_LINK_NAME) == 0) {
					a->locus_link_name_found = true;
					a->locus_link_name = strdup(attr[i+1]);
				} else if (!a->flybase_found && strcmp(attr[i], PROTXML_FLYBASE) == 0) {
					a->flybase_found = true;
					a->flybase = strdup(attr[i+1]);
				} else {
					fprintf(stderr, "on annotation element, unknown attribute found: %s w/ value %s\n", attr[i], attr[i+1]);
				}
			}
		}
	}
}

void protXML_end_handler(void *data, const char *el) {
	struct protXML_parser_data *pxpd = (struct protXML_parser_data *)data;
	bool pop = false;

	if (strcmp(el, PROTXML_PROTEIN_GROUP) == 0) {
		/* a complete protein group has been detected */
		/* do something with the record */
		/* increment counters, if any */
		pxpd->current_protein_group = NULL;
		pop = true;
	} else if (strcmp(el, PROTXML_PROTEIN) == 0) {
		pxpd->current_protein = NULL;
		pop = true;
	} else if (strcmp(el, PROTXML_PEPTIDE) == 0) {
		pxpd->current_peptide = NULL;
		pop = true;
	} else if (strcmp(el, PROTXML_MODIFICATION_INFO) == 0) {
		pxpd->current_modification_info = NULL;
		pop = true;
	} else if (strcmp(el, PROTXML_INDISTINGUISHABLE_PROTEIN) == 0) {
		pxpd->current_indistinguishable_protein = NULL;
		pop = true;
	} else if (strcmp(el, PROTXML_INDISTINGUISHABLE_PEPTIDE) == 0) {
		pxpd->current_indistinguishable_peptide = NULL;
		pop = true;
	}
	if (pop)
		util_stack_pop(pxpd->state);
}

/* core function */
struct protXML_parser_data *protXML_parse_file(char *filename) {
	void *parse_buffer;
	struct protXML_parser_data *pxpd;
	FILE *fp;

	pxpd = protXML_parser_data_malloc();

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "unable to open file '%s'\n", filename);
		return NULL;
	}

	XML_Parser p = XML_ParserCreate(NULL);
	if (p == NULL) {
		fprintf(stderr, "unable to allocate memory for protXML parser\n");
		protXML_parser_data_free(pxpd);
		fclose(fp);
		return NULL;
	}

	XML_SetUserData(p, (void *)pxpd);
	XML_SetElementHandler(p, protXML_start_handler, protXML_end_handler);

	while (1) {
		int len, end_of_file, parse_result;

		parse_buffer = XML_GetBuffer(p, BUFF_SIZE);

		len = fread(parse_buffer, 1, BUFF_SIZE, fp);
		if (ferror(fp)) {
			fprintf(stderr, "read error on file '%s'\n", filename);
			protXML_parser_data_free(pxpd);
			pxpd = NULL;
			break;
		}
		end_of_file = feof(fp);

		parse_result = XML_ParseBuffer(p, len, end_of_file);
		if (!parse_result) {
			fprintf(stderr, "parse error in file '%s' at line %d:\n%s\n", filename, (int)XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)));
			protXML_parser_data_free(pxpd);
			pxpd = NULL;
			break;
		}

		if (end_of_file)
			break;
	}

	XML_ParserFree(p);
	fclose(fp);

	return pxpd;
}

/* allocation and dealloc of structures */

struct protXML_parser_data *protXML_parser_data_malloc(void) {
	struct protXML_parser_data *pxpd;

	pxpd = (struct protXML_parser_data *)malloc(sizeof(struct protXML_parser_data));
	memset(pxpd, 0, sizeof(struct protXML_parser_data));

	pxpd->state = util_stack_malloc();
	util_stack_push(pxpd->state, (void *)ROOT);

	pxpd->protein_group = util_ptr_list_malloc();

	return pxpd;
}

void protXML_parser_data_free(protXML_parser_data *pxpd) {
	intptr_t state;
	int i;

	state = (intptr_t)util_stack_pop(pxpd->state);
	if (state != ROOT) {
		if (state == 0) {
			fprintf(stderr, "parser exited with empty state stack, an internal error may have occured\n");
		} else {
			fprintf(stderr, "parser exited with non-empty state stack (state = %d at top), the protXML document may be malformed\n", (int)state);
		}
	}
	util_stack_free(pxpd->state);

	for (i = 0; i < pxpd->protein_group->ps; ++i)
		protein_group_free((struct protein_group *)pxpd->protein_group->p[i]);
	util_ptr_list_free(pxpd->protein_group);

	free(pxpd);
}

struct protein_group *protein_group_malloc(void) {
	struct protein_group *pg;

	pg = (struct protein_group *)malloc(sizeof(struct protein_group));
	memset(pg, 0, sizeof(struct protein_group));

	pg->protein = util_ptr_list_malloc();

	return pg;
}

void protein_group_free(struct protein_group *pg) {
	int i;

	if (pg->group_number_found)
		free(pg->group_number);
	if (pg->pseudo_name_found)
		free(pg->pseudo_name);
	if (pg->probability_found)
		free(pg->probability);

	for (i = 0; i < pg->protein->ps; ++i)
		protein_free((struct protein *)pg->protein->p[i]);
	util_ptr_list_free(pg->protein);
	
	free(pg);
}

struct protein *protein_malloc(void) {
	struct protein *p;

	p = (struct protein *)malloc(sizeof(struct protein));
	memset(p, 0, sizeof(struct protein));

	p->parameter = util_ptr_list_malloc();
#if 0
	/* not implemented */
	p->analysis_result = util_ptr_list_malloc();
#endif
	p->indistinguishable_protein = util_ptr_list_malloc();

	p->peptide = util_ptr_list_malloc();

	return p;
}

void protein_free(struct protein *p) {
	int i;

	if (p->protein_name_found)
		free(p->protein_name);
	if (p->n_indistinguishable_proteins_found)
		free(p->n_indistinguishable_proteins);
	if (p->probability_found)
		free(p->probability);
	if (p->percent_coverage_found)
		free(p->percent_coverage);
	if (p->unique_stripped_peptides_found)
		free(p->unique_stripped_peptides);
	if (p->group_sibling_id_found)
		free(p->group_sibling_id);
	if (p->total_number_peptides_found)
		free(p->total_number_peptides);
	if (p->subsuming_protein_entry_found)
		free(p->subsuming_protein_entry);
	if (p->pct_spectrum_ids_found)
		free(p->pct_spectrum_ids);
	if (p->confidence_found)
		free(p->confidence);

	for (i = 0; i < p->parameter->ps; ++i)
		parameter_free((struct parameter *)p->parameter->p[i]);
	util_ptr_list_free(p->parameter);

#if 0
	/* not implemented */
	for (i = 0; i < p->analysis_result->ps; ++i)
		analysis_result_free((struct analysis_result *)p->analysis_result->p[i]);
	util_ptr_list_free(p->analysis_result);
#endif

	annotation_free(&p->annotation);

	for (i = 0; i < p->indistinguishable_protein->ps; ++i)
		indistinguishable_protein_free((struct indistinguishable_protein *)p->indistinguishable_protein->p[i]);
	util_ptr_list_free(p->indistinguishable_protein);

	for (i = 0; i < p->peptide->ps; ++i)
		peptide_free((struct peptide *)p->peptide->p[i]);
	util_ptr_list_free(p->peptide);

	free(p);
}

struct parameter *parameter_malloc(void) {
	struct parameter *p;

	p = (struct parameter *)malloc(sizeof(struct parameter));
	memset((void *)p, 0, sizeof(struct parameter));

	return p;
}

void parameter_free(struct parameter *p) {
	if (p->name_found)
		free(p->name);
	if (p->value_found)
		free(p->value);

	free(p);
}

void annotation_free(struct annotation *a) {	/* this does not free the struct */
	if (a->protein_description_found)
		free(a->protein_description);
	if (a->ipi_name_found)
		free(a->ipi_name);
	if (a->refseq_name_found)
		free(a->refseq_name);
	if (a->swissprot_name_found)
		free(a->swissprot_name);
	if (a->ensembl_name_found)
		free(a->ensembl_name);
	if (a->trembl_name_found)
		free(a->trembl_name);
	if (a->locus_link_name_found)
		free(a->locus_link_name);
	if (a->flybase_found)
		free(a->flybase);
}

struct indistinguishable_protein *indistinguishable_protein_malloc(void) {
	struct indistinguishable_protein *ip;

	ip = (struct indistinguishable_protein *)malloc(sizeof(struct indistinguishable_protein));
	memset((void *)ip, 0, sizeof(struct indistinguishable_protein));

	ip->parameter = util_ptr_list_malloc();

	return ip;
}

void indistinguishable_protein_free(struct indistinguishable_protein *ip) {
	int i;

	if (ip->protein_name_found)
		free(ip->protein_name);

	for (i = 0; i < ip->parameter->ps; ++i)
		parameter_free((struct parameter *)ip->parameter->p[i]);
	util_ptr_list_free(ip->parameter);

	annotation_free(&ip->annotation);

	free(ip);
}

struct peptide *peptide_malloc(void) {
	struct peptide *p;

	p = (struct peptide *)malloc(sizeof(struct peptide));
	memset((void *)p, 0, sizeof(struct peptide));

	p->peptide_parent_protein = util_ptr_list_malloc();
	p->indistinguishable_peptide = util_ptr_list_malloc();
	p->parameter = util_ptr_list_malloc();
	p->modification_info = util_ptr_list_malloc();

	return p;
}

void peptide_free(struct peptide *p) {
	int i;

	if (p->peptide_sequence_found)
		free(p->peptide_sequence);
	if (p->charge_found)
		free(p->charge);
	if (p->initial_probability_found)
		free(p->initial_probability);
	if (p->nsp_adjusted_probability_found)
		free(p->nsp_adjusted_probability);
	if (p->ni_adjusted_probability_found)
		free(p->ni_adjusted_probability);
	if (p->exp_sibling_ion_instances_found)
		free(p->exp_sibling_ion_instances);
	if (p->exp_sibling_ion_bin_found)
		free(p->exp_sibling_ion_bin);
	if (p->exp_tot_instances_found)
		free(p->exp_tot_instances);
	if (p->peptide_group_designator_found)
		free(p->peptide_group_designator);
	if (p->weight_found)
		free(p->weight);
	if (p->is_nondegenerate_evidence_found)
		free(p->is_nondegenerate_evidence);
	if (p->n_enzymatic_termini_found)
		free(p->n_enzymatic_termini);
	if (p->n_sibling_peptides_found)
		free(p->n_sibling_peptides);
	if (p->n_sibling_peptides_bin_found)
		free(p->n_sibling_peptides_bin);
	if (p->n_instances_found)
		free(p->n_instances);
	if (p->calc_neutral_pep_mass_found)
		free(p->calc_neutral_pep_mass);
	if (p->is_contributing_evidence_found)
		free(p->is_contributing_evidence);

	for (i = 0; i < p->peptide_parent_protein->ps; ++i)
		peptide_parent_protein_free((struct peptide_parent_protein *)p->peptide_parent_protein->p[i]);
	util_ptr_list_free(p->peptide_parent_protein);

	for (i = 0; i < p->indistinguishable_peptide->ps; ++i)
		indistinguishable_peptide_free((struct indistinguishable_peptide *)p->indistinguishable_peptide->p[i]);
	util_ptr_list_free(p->indistinguishable_peptide);

	for (i = 0; i < p->parameter->ps; ++i)
		parameter_free((struct parameter *)p->parameter->p[i]);
	util_ptr_list_free(p->parameter);

	for (i = 0; i < p->modification_info->ps; ++i)
		modification_info_free((struct modification_info *)p->modification_info->p[i]);
	util_ptr_list_free(p->modification_info);

	free(p);
}

struct peptide_parent_protein *peptide_parent_protein_malloc(void) {
	struct peptide_parent_protein *ppp;

	ppp = (struct peptide_parent_protein *)malloc(sizeof(struct peptide_parent_protein));
	memset(ppp, 0, sizeof(struct peptide_parent_protein));

	return ppp;
}

void peptide_parent_protein_free(struct peptide_parent_protein *ppp) {
	if (ppp->protein_name_found)
		free(ppp->protein_name);
	free(ppp);
}

struct indistinguishable_peptide *indistinguishable_peptide_malloc(void) {
	struct indistinguishable_peptide *ip;

	ip = (struct indistinguishable_peptide *)malloc(sizeof(struct indistinguishable_peptide));
	memset(ip, 0, sizeof(struct indistinguishable_peptide));

	ip->modification_info = util_ptr_list_malloc();

	return ip;
}

void indistinguishable_peptide_free(struct indistinguishable_peptide *ip) {
	int i;

	if (ip->peptide_sequence_found)
		free(ip->peptide_sequence);
	if (ip->charge_found)
		free(ip->charge);

	for (i = 0; i < ip->modification_info->ps; ++i)
		modification_info_free((struct modification_info *)ip->modification_info->p[i]);
	util_ptr_list_free(ip->modification_info);

	free(ip);
}

struct modification_info *modification_info_malloc(void) {
	struct modification_info *mi;

	mi = (struct modification_info *)malloc(sizeof(struct modification_info));
	memset((void *)mi, 0, sizeof(struct modification_info));

	mi->mod_aminoacid_mass = util_ptr_list_malloc();

	return mi;
}

void modification_info_free(struct modification_info *mi) {
	int i;

	if (mi->modified_peptide_found)
		free(mi->modified_peptide);

	for (i = 0; i < mi->mod_aminoacid_mass->ps; ++i)
		mod_aminoacid_mass_free((struct mod_aminoacid_mass *)mi->mod_aminoacid_mass->p[i]);
	util_ptr_list_free(mi->mod_aminoacid_mass);

	free(mi);
}

struct mod_aminoacid_mass *mod_aminoacid_mass_malloc(void) {
	struct mod_aminoacid_mass *maam;

	maam = (struct mod_aminoacid_mass *)malloc(sizeof(struct mod_aminoacid_mass));
	memset((void *)maam, 0, sizeof(struct mod_aminoacid_mass));

	return maam;
}

void mod_aminoacid_mass_free(struct mod_aminoacid_mass *maam) {
	if (maam->position_found)
		free(maam->position);
	if (maam->mass_found)
		free(maam->mass);

	free(maam);
}
