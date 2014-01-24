#ifndef _UTIL_H
#define _UTIL_H

struct util_ptr_list {
	int	ps;
	void	**p;
};
struct util_ptr_list *util_ptr_list_malloc(void);
void util_ptr_list_add_ptr(struct util_ptr_list *pl, void *a);
void util_ptr_list_cat_lists(struct util_ptr_list *dest, struct util_ptr_list *source);
void util_ptr_list_free(struct util_ptr_list *pl);
void util_ptr_list_add_ptr_ec(struct util_ptr_list *pl, void *a);
void util_ptr_list_cat_lists_ec(struct util_ptr_list *dest, struct util_ptr_list *source);
void util_ptr_list_remove_index(struct util_ptr_list *pl, int index);

struct util_stack {
	void		**data;
	unsigned int	empty;
	unsigned int	top;
	unsigned int	size;
};

struct util_stack *util_stack_malloc(void);
void *util_stack_pop(struct util_stack *stack);
void *util_stack_peek(struct util_stack *stack);
unsigned int util_stack_push(struct util_stack *stack, void *data);
void util_stack_free(struct util_stack *stack);

#endif /* _UTIL_H */
