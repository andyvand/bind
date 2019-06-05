# Hey Emacs, this is -*- makefile-automake -*- file!

if HAVE_CMOCKA
SUBDIRS += %reldir%/tests
endif

ISC_INCLUDES = \
	-I%canon_reldir%/unix/include		\
	-I%canon_reldir%/pthreads/include	\
	-I%canon_reldir%/include		\
	$(OPENSSL_CFLAGS)			\
	$(OPENSSL_INCLUDES)			\
	$(JSON_C_CFLAGS)

lib_LTLIBRARIES += libisc.la

libisc_ladir = $(includedir)/isc
libisc_la_HEADERS = 			\
	%reldir%/include/isc/aes.h	\
	%reldir%/include/isc/app.h	\
	%reldir%/include/isc/assertions.h	\
	%reldir%/include/isc/backtrace.h	\
	%reldir%/include/isc/base32.h	\
	%reldir%/include/isc/base64.h	\
	%reldir%/include/isc/bind9.h	\
	%reldir%/include/isc/buffer.h	\
	%reldir%/include/isc/bufferlist.h	\
	%reldir%/include/isc/commandline.h	\
	%reldir%/include/isc/counter.h	\
	%reldir%/include/isc/crc64.h	\
	%reldir%/include/isc/deprecated.h	\
	%reldir%/include/isc/errno.h	\
	%reldir%/include/isc/error.h	\
	%reldir%/include/isc/event.h	\
	%reldir%/include/isc/eventclass.h	\
	%reldir%/include/isc/file.h	\
	%reldir%/include/isc/formatcheck.h	\
	%reldir%/include/isc/fsaccess.h	\
	%reldir%/include/isc/fuzz.h	\
	%reldir%/include/isc/hash.h	\
	%reldir%/include/isc/heap.h	\
	%reldir%/include/isc/hex.h	\
	%reldir%/include/isc/hmac.h	\
	%reldir%/include/isc/ht.h	\
	%reldir%/include/isc/httpd.h	\
	%reldir%/include/isc/interfaceiter.h	\
	%reldir%/include/isc/iterated_hash.h	\
	%reldir%/include/isc/json.h	\
	%reldir%/include/isc/lang.h	\
	%reldir%/include/isc/lex.h	\
	%reldir%/include/isc/lfsr.h	\
	%reldir%/include/isc/lib.h	\
	%reldir%/include/isc/likely.h	\
	%reldir%/include/isc/list.h	\
	%reldir%/include/isc/log.h	\
	%reldir%/include/isc/magic.h	\
	%reldir%/include/isc/md.h	\
	%reldir%/include/isc/mem.h	\
	%reldir%/include/isc/meminfo.h	\
	%reldir%/include/isc/mutexblock.h	\
	%reldir%/include/isc/netaddr.h	\
	%reldir%/include/isc/netscope.h	\
	%reldir%/include/isc/nonce.h	\
	%reldir%/include/isc/os.h	\
	%reldir%/include/isc/parseint.h	\
	%reldir%/include/isc/pool.h	\
	%reldir%/include/isc/portset.h	\
	%reldir%/include/isc/print.h	\
	%reldir%/include/isc/queue.h	\
	%reldir%/include/isc/quota.h	\
	%reldir%/include/isc/radix.h	\
	%reldir%/include/isc/random.h	\
	%reldir%/include/isc/ratelimiter.h	\
	%reldir%/include/isc/refcount.h	\
	%reldir%/include/isc/regex.h	\
	%reldir%/include/isc/region.h	\
	%reldir%/include/isc/resource.h	\
	%reldir%/include/isc/result.h	\
	%reldir%/include/isc/resultclass.h	\
	%reldir%/include/isc/rwlock.h	\
	%reldir%/include/isc/safe.h	\
	%reldir%/include/isc/serial.h	\
	%reldir%/include/isc/sockaddr.h	\
	%reldir%/include/isc/socket.h	\
	%reldir%/include/isc/stats.h	\
	%reldir%/include/isc/stdio.h	\
	%reldir%/include/isc/string.h	\
	%reldir%/include/isc/symtab.h	\
	%reldir%/include/isc/task.h	\
	%reldir%/include/isc/taskpool.h	\
	%reldir%/include/isc/timer.h	\
	%reldir%/include/isc/tm.h	\
	%reldir%/include/isc/types.h	\
	%reldir%/include/isc/util.h	\
	%reldir%/include/isc/xml.h

libisc_la_SOURCES =			\
	$(libisc_la_HEADERS)		\
	%reldir%/unix/pk11_api.c	\
	%reldir%/unix/app.c		\
	%reldir%/unix/dir.c		\
	%reldir%/unix/errno.c		\
	%reldir%/unix/errno2result.c	\
	%reldir%/unix/file.c		\
	%reldir%/unix/fsaccess.c	\
	%reldir%/unix/interfaceiter.c	\
	%reldir%/unix/meminfo.c		\
	%reldir%/unix/net.c		\
	%reldir%/unix/os.c		\
	%reldir%/unix/resource.c	\
	%reldir%/unix/socket.c		\
	%reldir%/unix/stdio.c		\
	%reldir%/unix/stdtime.c		\
	%reldir%/unix/syslog.c		\
	%reldir%/unix/time.c		\
	%reldir%/pk11.c			\
	%reldir%/pk11_result.c		\
	%reldir%/aes.c			\
	%reldir%/assertions.c		\
	%reldir%/backtrace.c		\
	%reldir%/backtrace-emptytbl.c	\
	%reldir%/base32.c		\
	%reldir%/base64.c		\
	%reldir%/bind9.c		\
	%reldir%/buffer.c		\
	%reldir%/bufferlist.c		\
	%reldir%/commandline.c		\
	%reldir%/counter.c		\
	%reldir%/crc64.c		\
	%reldir%/entropy.c		\
	%reldir%/error.c		\
	%reldir%/event.c		\
	%reldir%/hash.c			\
	%reldir%/ht.c			\
	%reldir%/heap.c			\
	%reldir%/hex.c			\
	%reldir%/hmac.c			\
	%reldir%/httpd.c		\
	%reldir%/iterated_hash.c	\
	%reldir%/lex.c			\
	%reldir%/lfsr.c			\
	%reldir%/lib.c			\
	%reldir%/log.c			\
	%reldir%/md.c			\
	%reldir%/mem.c			\
	%reldir%/mutexblock.c		\
	%reldir%/netaddr.c		\
	%reldir%/netscope.c		\
	%reldir%/nonce.c		\
	%reldir%/openssl_shim.c		\
	%reldir%/include/openssl_shim.h	\
	%reldir%/pool.c			\
	%reldir%/parseint.c		\
	%reldir%/portset.c		\
	%reldir%/quota.c		\
	%reldir%/radix.c		\
	%reldir%/random.c		\
	%reldir%/ratelimiter.c		\
	%reldir%/region.c		\
	%reldir%/regex.c		\
	%reldir%/result.c		\
	%reldir%/rwlock.c		\
	%reldir%/serial.c		\
	%reldir%/sockaddr.c		\
	%reldir%/stats.c		\
	%reldir%/string.c		\
	%reldir%/symtab.c		\
	%reldir%/task.c			\
	%reldir%/taskpool.c		\
	%reldir%/timer.c		\
	%reldir%/tm.c			\
	%reldir%/pthreads/condition.c	\
	%reldir%/pthreads/mutex.c	\
	%reldir%/pthreads/thread.c

libisc_la_CPPFLAGS =			\
	$(AM_CPPFLAGS)			\
	$(ISC_INCLUDES)			\
	$(DNS_INCLUDES)

libisc_la_LDFLAGS =			\
	-release $(PACKAGE_VERSION)	\
	$(OPENSSL_LDFLAGS)

libisc_la_LIBADD =			\
	$(OPENSSL_LIBS)		\
	$(JSONC_C_LIBS)