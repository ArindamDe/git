#ifndef MERGE_ORT_WRAPPERS_H
#define MERGE_ORT_WRAPPERS_H

#include "merge-recursive.h"

/*
 * rename-detecting three-way merge, no recursion.
 * Wrapper mimicking the old merge_trees() function.
 */
int merge_ort_nonrecursive(struct merge_options *opt,
			   struct tree *head,
			   struct tree *merge,
			   struct tree *common);

/*
 * rename-detecting three-way merge with recursive ancestor consolidation.
 * Wrapper mimicking the old merge_recursive() function.
 */
int merge_ort_recursive(struct merge_options *opt,
			struct cummit *h1,
			struct cummit *h2,
			struct cummit_list *ancestors,
			struct cummit **result);

#endif
