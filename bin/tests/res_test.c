/*
 * Copyright (C) 1999  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * Principal Author: Bob Halley
 */

#include <config.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	/* XXX Naughty. */

#include <isc/assertions.h>
#include <isc/error.h>
#include <isc/task.h>
#include <isc/timer.h>
#include <isc/app.h>
#include <isc/mutex.h>
#include <isc/boolean.h>
#include <isc/net.h>

#include "../../isc/util.h"		/* XXX Naughty. */

#include <dns/types.h>
#include <dns/result.h>
#include <dns/name.h>
#include <dns/fixedname.h>
#include <dns/resolver.h>
#include <dns/events.h>

isc_mutex_t lock;

ISC_LIST(dns_fetch_t) fetches;

static void
cancel(dns_fetch_t *fetch, isc_task_t *task) {
	isc_boolean_t need_shutdown = ISC_FALSE;

	LOCK(&lock);

	printf("fetch %p canceled\n", fetch);
	ISC_LIST_UNLINK(fetches, fetch, link);
	dns_resolver_destroyfetch(&fetch, task);
	if (ISC_LIST_EMPTY(fetches))
		need_shutdown = ISC_TRUE;

	UNLOCK(&lock);

	if (need_shutdown)
		isc_app_shutdown();
}

static void
done(isc_task_t *task, isc_event_t *event) {
	dns_fetchdoneevent_t *devent = (dns_fetchdoneevent_t *)event;
	isc_boolean_t need_shutdown = ISC_FALSE;
	dns_fetch_t *fetch, *next_fetch;

	REQUIRE(devent->type == DNS_EVENT_FETCHDONE);

	(void)task;

	LOCK(&lock);

	for (fetch = ISC_LIST_HEAD(fetches);
	     fetch != NULL;
	     fetch = next_fetch) {
		next_fetch = ISC_LIST_NEXT(fetch, link);
		if (fetch->private == event->sender &&
		    fetch == event->tag)
			break;
	}
	INSIST(fetch != NULL);
	printf("fetch %p done: %s\n", fetch,
	       dns_result_totext(devent->result));
	ISC_LIST_UNLINK(fetches, fetch, link);
	if (ISC_LIST_EMPTY(fetches))
		need_shutdown = ISC_TRUE;

	UNLOCK(&lock);

	isc_event_free(&event);
	dns_resolver_destroyfetch(&fetch, NULL);

	if (need_shutdown)
		isc_app_shutdown();
}

static dns_fetch_t *
launch(dns_resolver_t *res, dns_name_t *name, dns_rdatatype_t type,
       isc_boolean_t shared_ok, isc_boolean_t recursive, isc_task_t *task)
{
	dns_fetch_t *fetch;
	unsigned int options = 0;

	if (!shared_ok)
		options |= DNS_FETCHOPT_UNSHARED;
	if (recursive)
		options |= DNS_FETCHOPT_RECURSIVE;

	LOCK(&lock);

	fetch = NULL;
	RUNTIME_CHECK(dns_resolver_createfetch(res, name, type, NULL, NULL,
					       options, task, done,
					       NULL,
					       &fetch) ==
		      DNS_R_SUCCESS);
	ISC_LIST_APPEND(fetches, fetch, link);

	UNLOCK(&lock);

	return (fetch);
}

int
main(int argc, char *argv[]) {
	isc_mem_t *mctx;
	isc_boolean_t verbose = ISC_FALSE;
	unsigned int workers = 2;
	isc_taskmgr_t *taskmgr;
	isc_task_t *task1, *task2;
	isc_timermgr_t *timermgr;
	dns_resolver_t *res;
	int ch;
	dns_fetch_t *fetch;

	RUNTIME_CHECK(isc_app_start() == ISC_R_SUCCESS);

	RUNTIME_CHECK(isc_mutex_init(&lock) == ISC_R_SUCCESS);

	mctx = NULL;
	RUNTIME_CHECK(isc_mem_create(0, 0, &mctx) == ISC_R_SUCCESS);

	while ((ch = getopt(argc, argv, "vw:")) != -1) {
		switch (ch) {
		case 'v':
			verbose = ISC_TRUE;
			break;
		case 'w':
			workers = (unsigned int)atoi(optarg);
			break;
		}
	}

	if (verbose) {
		printf("%u workers\n", workers);
		printf("IPv4: %s\n", isc_result_totext(isc_net_haveipv4()));
		printf("IPv6: %s\n", isc_result_totext(isc_net_haveipv6()));
	}

	taskmgr = NULL;
	RUNTIME_CHECK(isc_taskmgr_create(mctx, workers, 0, &taskmgr) ==
		      ISC_R_SUCCESS);
	task1 = NULL;
	RUNTIME_CHECK(isc_task_create(taskmgr, mctx, 0, &task1) ==
		      ISC_R_SUCCESS);
	task2 = NULL;
	RUNTIME_CHECK(isc_task_create(taskmgr, mctx, 0, &task2) ==
		      ISC_R_SUCCESS);
	timermgr = NULL;
	RUNTIME_CHECK(isc_timermgr_create(mctx, &timermgr) == ISC_R_SUCCESS);

	argc -= optind;
	argv += optind;

	if (argc != 0)
		printf("ignoring trailing arguments\n");

	res = NULL;
	RUNTIME_CHECK(dns_resolver_create(mctx, taskmgr, 10, timermgr,
					  dns_rdataclass_in, NULL, &res) ==
		      DNS_R_SUCCESS);

	ISC_LIST_INIT(fetches);
	fetch = launch(res, dns_rootname, dns_rdatatype_a,
		       ISC_TRUE, ISC_FALSE, task1);
	fetch = launch(res, dns_rootname, dns_rdatatype_a,
		       ISC_TRUE, ISC_FALSE, task2);
	fetch = launch(res, dns_rootname, dns_rdatatype_a,
		       ISC_TRUE, ISC_FALSE, task2);
	fetch = launch(res, dns_rootname, dns_rdatatype_mx,
		       ISC_TRUE, ISC_FALSE, task1);
	fetch = launch(res, dns_rootname, dns_rdatatype_ns,
		       ISC_FALSE, ISC_TRUE, task2);
	fetch = launch(res, dns_rootname, dns_rdatatype_ns,
		       ISC_TRUE, ISC_FALSE, task1);
	cancel(fetch, task1);

	(void)isc_app_run();

	dns_resolver_detach(&res);

	isc_task_shutdown(task1);
	isc_task_detach(&task1);
	isc_task_shutdown(task2);
	isc_task_detach(&task2);
	isc_taskmgr_destroy(&taskmgr);

	isc_timermgr_destroy(&timermgr);

	if (verbose)
		isc_mem_stats(mctx, stdout);
	isc_mem_destroy(&mctx);

	isc_mutex_destroy(&lock);

	isc_app_finish();

	return (0);
}
