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


/*! \file
 * \brief
 * The built-in "version", "hostname", "id", "authors" and "empty" databases.
 */

#include <string.h>
#include <stdio.h>

#include <isc/mem.h>
#include <isc/print.h>
#include <isc/result.h>
#include <isc/util.h>

#include <dns/db.h>
#include <dns/result.h>
#include <dns/sdb.h>

#include <named/builtin.h>
#include <named/globals.h>
#include <named/server.h>
#include <named/os.h>

#define CHECK(op)						\
	do {							\
		result = (op);					\
		if (result != ISC_R_SUCCESS) {			\
			goto cleanup;				\
		}						\
	} while (0)

typedef struct builtin builtin_t;

static isc_result_t do_version_lookup(dns_sdblookup_t *lookup);
static isc_result_t do_hostname_lookup(dns_sdblookup_t *lookup);
static isc_result_t do_authors_lookup(dns_sdblookup_t *lookup);
static isc_result_t do_id_lookup(dns_sdblookup_t *lookup);
static isc_result_t do_empty_lookup(dns_sdblookup_t *lookup);

/*
 * We can't use function pointers as the db_data directly
 * because ANSI C does not guarantee that function pointers
 * can safely be cast to void pointers and back.
 */

struct builtin {
	isc_result_t (*do_lookup)(dns_sdblookup_t *lookup);
	isc_mem_t *mctx;
	char *server;
	char *contact;
};

static builtin_t version_builtin = { do_version_lookup, NULL, NULL, NULL };
static builtin_t hostname_builtin = { do_hostname_lookup, NULL, NULL, NULL };
static builtin_t authors_builtin = { do_authors_lookup, NULL, NULL, NULL };
static builtin_t id_builtin = { do_id_lookup, NULL, NULL, NULL };
static builtin_t empty_builtin = { do_empty_lookup, NULL, NULL, NULL };

static dns_sdbimplementation_t *builtin_impl;

static isc_result_t
builtin_lookup(const char *zone, const char *name,
	       void *dbdata, dns_sdblookup_t *lookup,
	       dns_clientinfomethods_t *methods,
	       dns_clientinfo_t *clientinfo)
{
	builtin_t *b = (builtin_t *) dbdata;

	UNUSED(zone);
	UNUSED(methods);
	UNUSED(clientinfo);

	if (strcmp(name, "@") == 0) {
		return (b->do_lookup(lookup));
	} else {
		return (ISC_R_NOTFOUND);
	}
}

static isc_result_t
put_txt(dns_sdblookup_t *lookup, const char *text) {
	unsigned char buf[256];
	unsigned int len = strlen(text);
	if (len > 255) {
		len = 255; /* Silently truncate */
	}
	buf[0] = len;
	memmove(&buf[1], text, len);
	return (dns_sdb_putrdata(lookup, dns_rdatatype_txt, 0, buf, len + 1));
}

static isc_result_t
do_version_lookup(dns_sdblookup_t *lookup) {
	if (named_g_server->version_set) {
		if (named_g_server->version == NULL) {
			return (ISC_R_SUCCESS);
		} else {
			return (put_txt(lookup, named_g_server->version));
		}
	} else {
		return (put_txt(lookup, named_g_version));
	}
}

static isc_result_t
do_hostname_lookup(dns_sdblookup_t *lookup) {
	if (named_g_server->hostname_set) {
		if (named_g_server->hostname == NULL) {
			return (ISC_R_SUCCESS);
		} else {
			return (put_txt(lookup, named_g_server->hostname));
		}
	} else {
		char buf[256];
		isc_result_t result = named_os_gethostname(buf, sizeof(buf));
		if (result != ISC_R_SUCCESS) {
			return (result);
		}
		return (put_txt(lookup, buf));
	}
}

static isc_result_t
do_authors_lookup(dns_sdblookup_t *lookup) {
	isc_result_t result;
	const char **p;
	static const char *authors[] = {
		"Mark Andrews",
		"Curtis Blackburn",
		"James Brister",
		"Ben Cottrell",
		"John H. DuBois III",
		"Francis Dupont",
		"Michael Graff",
		"Andreas Gustafsson",
		"Bob Halley",
		"Evan Hunt",
		"JINMEI Tatuya",
		"Witold Krecicki",
		"David Lawrence",
		"Scott Mann",
		"Danny Mayer",
		"Damien Neil",
		"Matt Nelson",
		"Jeremy C. Reed",
		"Michael Sawyer",
		"Brian Wellington",
		NULL
	};

	/*
	 * If a version string is specified, disable the authors.bind zone.
	 */
	if (named_g_server->version_set) {
		return (ISC_R_SUCCESS);
	}

	for (p = authors; *p != NULL; p++) {
		result = put_txt(lookup, *p);
		if (result != ISC_R_SUCCESS) {
			return (result);
		}
	}
	return (ISC_R_SUCCESS);
}

static isc_result_t
do_id_lookup(dns_sdblookup_t *lookup) {
	if (named_g_server->sctx->gethostname != NULL) {
		char buf[256];
		isc_result_t result;

		result = named_g_server->sctx->gethostname(buf, sizeof(buf));
		if (result != ISC_R_SUCCESS) {
			return (result);
		}
		return (put_txt(lookup, buf));
	} else if (named_g_server->sctx->server_id != NULL) {
		return (put_txt(lookup, named_g_server->sctx->server_id));
	} else {
		return (ISC_R_SUCCESS);
	}
}

static isc_result_t
do_empty_lookup(dns_sdblookup_t *lookup) {
	UNUSED(lookup);
	return (ISC_R_SUCCESS);
}

static isc_result_t
builtin_authority(const char *zone, void *dbdata, dns_sdblookup_t *lookup) {
	isc_result_t result;
	const char *contact = "hostmaster";
	const char *server = "@";
	builtin_t *b = (builtin_t *) dbdata;

	UNUSED(zone);
	UNUSED(dbdata);

	if (b == &empty_builtin) {
		server = ".";
		contact = ".";
	} else {
		if (b->server != NULL) {
			server = b->server;
		}
		if (b->contact != NULL) {
			contact = b->contact;
		}
	}

	result = dns_sdb_putsoa(lookup, server, contact, 0);
	if (result != ISC_R_SUCCESS) {
		return (ISC_R_FAILURE);
	}

	result = dns_sdb_putrr(lookup, "ns", 0, server);
	if (result != ISC_R_SUCCESS) {
		return (ISC_R_FAILURE);
	}

	return (ISC_R_SUCCESS);
}

static isc_result_t
builtin_create(const char *zone, isc_mem_t *mctx, int argc, char **argv,
	       void *driverdata, void **dbdata)
{
	REQUIRE(argc >= 1);

	UNUSED(zone);
	UNUSED(mctx);
	UNUSED(driverdata);

	if (strcmp(argv[0], "empty") == 0) {
		if (argc != 3) {
			return (DNS_R_SYNTAX);
		}
	} else if (argc != 1) {
		return (DNS_R_SYNTAX);
	}

	if (strcmp(argv[0], "version") == 0) {
		*dbdata = &version_builtin;
	} else if (strcmp(argv[0], "hostname") == 0) {
		*dbdata = &hostname_builtin;
	} else if (strcmp(argv[0], "authors") == 0) {
		*dbdata = &authors_builtin;
	} else if (strcmp(argv[0], "id") == 0) {
		*dbdata = &id_builtin;
	} else if (strcmp(argv[0], "empty") == 0) {
		builtin_t *empty;
		char *server;
		char *contact;
		/*
		 * We don't want built-in zones to fail.  Fallback to
		 * the static configuration if memory allocation fails.
		 */
		empty = isc_mem_get(named_g_mctx, sizeof(*empty));
		server = isc_mem_strdup(named_g_mctx, argv[1]);
		contact = isc_mem_strdup(named_g_mctx, argv[2]);
		if (empty == NULL || server == NULL || contact == NULL) {
			*dbdata = &empty_builtin;
			if (server != NULL) {
				isc_mem_free(named_g_mctx, server);
			}
			if (contact != NULL) {
				isc_mem_free(named_g_mctx, contact);
			}
			if (empty != NULL) {
				isc_mem_put(named_g_mctx, empty,
					    sizeof (*empty));
			}
		} else {
			memmove(empty, &empty_builtin, sizeof (empty_builtin));
			empty->server = server;
			empty->contact = contact;
			*dbdata = empty;
		}
	} else {
		return (ISC_R_NOTIMPLEMENTED);
	}
	return (ISC_R_SUCCESS);
}

static void
builtin_destroy(const char *zone, void *driverdata, void **dbdata) {
	builtin_t *b = (builtin_t *) *dbdata;

	UNUSED(zone);
	UNUSED(driverdata);

	/*
	 * Don't free the static versions.
	 */
	if (*dbdata == &version_builtin || *dbdata == &hostname_builtin ||
	    *dbdata == &authors_builtin || *dbdata == &id_builtin ||
	    *dbdata == &empty_builtin)
	{
		return;
	}

	isc_mem_free(named_g_mctx, b->server);
	isc_mem_free(named_g_mctx, b->contact);
	isc_mem_put(named_g_mctx, b, sizeof (*b));
}

static dns_sdbmethods_t builtin_methods = {
	builtin_lookup,
	builtin_authority,
	NULL,           /* allnodes */
	builtin_create,
	builtin_destroy,
	NULL
};

isc_result_t
named_builtin_init(void) {
	RUNTIME_CHECK(dns_sdb_register("_builtin", &builtin_methods, NULL,
				       DNS_SDBFLAG_RELATIVEOWNER |
				       DNS_SDBFLAG_RELATIVERDATA,
				       named_g_mctx, &builtin_impl)
		      == ISC_R_SUCCESS);
	return (ISC_R_SUCCESS);
}

void
named_builtin_deinit(void) {
	dns_sdb_unregister(&builtin_impl);
}
