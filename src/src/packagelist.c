/*
 * Copyright (C) 2012-2013 Red Hat, Inc.
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "assert.h"

// libsolv
#include <solv/pool.h>
#include <solv/repo.h>
#include <solv/queue.h>
#include <solv/util.h>

// hawkey
#include "packagelist.h"
#include "package_internal.h"
#include "sack_internal.h"

struct _HyPackageList {
    HyPackage *elements;
    int count;
};

#define BLOCK_SIZE 31

HyPackageList
hy_packagelist_create(void)
{
    HyPackageList plist = solv_calloc(1, sizeof(*plist));
    return plist;
}

void
hy_packagelist_free(HyPackageList plist)
{
    int i;

    for (i = 0; i < plist->count; ++i)
	hy_package_free(plist->elements[i]);
    solv_free(plist->elements);
    solv_free(plist);
}

int
hy_packagelist_count(HyPackageList plist)
{
    return plist->count;
}

/**
 * Returns the package at position 'index'.
 *
 * Returns NULL if the packagelist doesn't have enough elements for index.
 *
 * Borrows the caller packagelist's reference, caller shouldn't call
 * hy_package_free().
 */
HyPackage
hy_packagelist_get(HyPackageList plist, int index)
{
    if (index < plist->count)
	return plist->elements[index];
    return NULL;
}

/**
 * Returns the package at position 'index'.
 *
 * Gives caller a new reference to the package that can survive the
 * packagelist. The returned package has to be freed via hy_package_free().
 */
HyPackage
hy_packagelist_get_clone(HyPackageList plist, int index)
{
    return hy_package_link(hy_packagelist_get(plist, index));
}

int
hy_packagelist_has(HyPackageList plist, HyPackage pkg)
{
    for (int i = 0; i < plist->count; ++i)
	if (hy_package_identical(pkg, plist->elements[i]))
	    return 1;
    return 0;
}

/**
 * Adds pkg at the end of plist.
 *
 * Assumes ownership of pkg and will free it during hy_packagelist_free().
 */
void hy_packagelist_push(HyPackageList plist, HyPackage pkg)
{
    plist->elements = solv_extend(plist->elements, plist->count, 1,
				  sizeof(pkg), BLOCK_SIZE);
    plist->elements[plist->count++] = pkg;
}
