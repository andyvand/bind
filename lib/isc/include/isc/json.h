/*
 * Copyright (C) Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * See the COPYRIGHT file distributed with this work for additional
 * information regarding copyright ownership.
 */

#ifndef ISC_JSON_H
#define ISC_JSON_H 1

#ifdef HAVE_JSON_C
/*
 * This file is here mostly to make it easy to add additional json-c header
 * files as needed across all the users of this file.  Rather than place
 * these json-c includes in each file, one include makes it easy to handle
 * the ifdef as well as adding the ability to add additional functions
 * which may be useful.
 */
#include <json_c_version.h>
#include <json_object.h>
#include <json_object_iterator.h>
#include <json_tokener.h>
#include <json_util.h>
#include <linkhash.h>
#endif

#define ISC_JSON_RENDERCONFIG		0x00000001 /* render config data */
#define ISC_JSON_RENDERSTATS		0x00000002 /* render stats */
#define ISC_JSON_RENDERALL		0x000000ff /* render everything */

#endif /* ISC_JSON_H */
