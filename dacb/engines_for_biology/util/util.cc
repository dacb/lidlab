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

#include <stdlib.h>
#include <string.h>
#include "util.h"

struct util_ptr_list *util_ptr_list_malloc(void) {
	struct util_ptr_list *pl = (struct util_ptr_list *)malloc(sizeof(struct util_ptr_list));
	pl->ps = 0;
	pl->p = NULL;
	return pl;
}

void util_ptr_list_add_ptr(struct util_ptr_list *pl, void *p) {
	int a = pl->ps++;
	pl->p = (void **)realloc(pl->p, pl->ps * sizeof(void *));
	pl->p[a] = p;
}

void util_ptr_list_cat_lists(struct util_ptr_list *dest, struct util_ptr_list *source) {
	int a, b, c = dest->ps + source->ps;
	dest->p = (void **)realloc(dest->p, c * sizeof(void *));
	for (a = dest->ps, b = 0; b < source->ps; ++a, ++b)
		dest->p[a] = source->p[b];
	dest->ps = c;
}

void util_ptr_list_free(struct util_ptr_list *pl) {
	if (pl == NULL) return;
	if (pl->p != NULL) free(pl->p);
	free(pl);
}

void util_ptr_list_remove_index(struct util_ptr_list *pl, int index) {
	if (index < 0 || index >= pl->ps) return;
	/* if necessary move memory */
	if (index != pl->ps - 1)
		memmove(&pl->p[index], pl->p[index + 1], (pl->ps - (index + 1)) * sizeof(void *));
	/* resize */
	pl->p = (void **)realloc(pl->p, (--pl->ps) * sizeof(void *));
}

struct util_stack *util_stack_malloc(void) {
	struct util_stack	*tmp;
	tmp = (struct util_stack *)malloc(sizeof(struct util_stack));
	memset(tmp, 0, sizeof(struct util_stack));
	tmp->empty = 1;
	return tmp;
}

void *util_stack_pop(struct util_stack *stack) {
	int	pop;
	if (stack->empty)
		return NULL;
	if (stack->top > 0)
		pop = stack->top--;
	else {
		pop = 0;
		stack->empty = 1;
	}
	return stack->data[pop];
}

void *util_stack_peek(struct util_stack *stack) {
	if (stack->empty)
		return NULL;
	return stack->data[stack->top];
}

unsigned int util_stack_push(struct util_stack *stack, void *data) {
	if (stack->empty) {
		if (!stack->size) {
			stack->top = 0;
			stack->size = 1;
			stack->data = (void **)malloc(sizeof(void *));
		}
	} else if (stack->top + 1 == stack->size) {
			stack->top = stack->size++;
			stack->data = (void **)realloc(stack->data, stack->size * sizeof(void *));
	} else {
		stack->top++;
	}

	stack->data[stack->top] = data;
	stack->empty = 0;
	return stack->top + 1;
}

void util_stack_free(struct util_stack *tmp) {
	free(tmp->data);
	free(tmp);
}
