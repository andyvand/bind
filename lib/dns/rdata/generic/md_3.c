/*
 * Copyright (C) 1998  Internet Software Consortium.
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

 /* $Id: md_3.c,v 1.4 1999/01/20 05:20:20 marka Exp $ */

#ifndef RDATA_GENERIC_MD_3_H
#define RDATA_GENERIC_MD_3_H

static dns_result_t
fromtext_md(dns_rdataclass_t class, dns_rdatatype_t type,
	    isc_lex_t *lexer, dns_name_t *origin,
	    isc_boolean_t downcase, isc_buffer_t *target) {
	isc_token_t token;
	isc_result_t result;
	dns_name_t name;
	isc_buffer_t buffer;
	unsigned int options = ISC_LEXOPT_EOL | ISC_LEXOPT_EOF;

	REQUIRE(type == 3);

	class = class;	/*unused*/

	result = isc_lex_gettoken(lexer, options, &token); 
	if (result != ISC_R_SUCCESS)   
		return (DNS_R_UNEXPECTED);
	if (token.type != isc_tokentype_string) {
		isc_lex_ungettoken(lexer, &token);
		if (token.type == isc_tokentype_eol ||
		    token.type == isc_tokentype_eof)
			return(DNS_R_UNEXPECTEDEND);
		return (DNS_R_UNEXPECTED);
	}

	dns_name_init(&name, NULL);
	buffer_fromregion(&buffer, &token.value.as_region,
			  ISC_BUFFERTYPE_TEXT);
	origin = (origin != NULL) ? origin : dns_rootname;
	return (dns_name_fromtext(&name, &buffer, origin, downcase, target));
}

static dns_result_t
totext_md(dns_rdata_t *rdata, dns_name_t *origin, isc_buffer_t *target) {
	isc_region_t region;
	dns_name_t name;
	dns_name_t prefix;
	isc_boolean_t sub;

	REQUIRE(rdata->type == 3);

	dns_name_init(&name, NULL);
	dns_rdata_toregion(rdata, &region);
	dns_name_fromregion(&name, &region);

	sub = name_prefix(&name, origin, &prefix);
		
	return(dns_name_totext(&prefix, sub, target));
}

static dns_result_t
fromwire_md(dns_rdataclass_t class, dns_rdatatype_t type,
	    isc_buffer_t *source, dns_decompress_t *dctx,
	    isc_boolean_t downcase, isc_buffer_t *target) {
        dns_name_t name;

	REQUIRE(type == 3);

	class = class;	/*unused*/
        
        dns_name_init(&name, NULL);
        return (dns_name_fromwire(&name, source, dctx, downcase, target));
}

static dns_result_t
towire_md(dns_rdata_t *rdata, dns_compress_t *cctx, isc_buffer_t *target) {
	dns_name_t name;
	isc_region_t region;

	REQUIRE(rdata->type == 3);

	dns_name_init(&name, NULL);
	dns_rdata_toregion(rdata, &region);
	dns_name_fromregion(&name, &region);

	return (dns_name_towire(&name, cctx, target));
}

static int
compare_md(dns_rdata_t *rdata1, dns_rdata_t *rdata2) {
	dns_name_t name1;
	dns_name_t name2;
	isc_region_t region1;
	isc_region_t region2;

	REQUIRE(rdata1->type == rdata2->type);
	REQUIRE(rdata1->class == rdata2->class);
	REQUIRE(rdata1->type == 3);

	dns_name_init(&name1, NULL);
	dns_name_init(&name2, NULL);

	dns_rdata_toregion(rdata1, &region1);
	dns_rdata_toregion(rdata2, &region2);

	dns_name_fromregion(&name1, &region1);
	dns_name_fromregion(&name2, &region2);

	return (dns_name_compare(&name1, &name2));
}

static dns_result_t
fromstruct_md(dns_rdataclass_t class, dns_rdatatype_t type, void *source,
	     isc_buffer_t *target) {

	REQUIRE(type == 3);

	class = class;	/*unused*/

	source = source;
	target = target;

	return (DNS_R_NOTIMPLEMENTED);
}

static dns_result_t
tostruct_md(dns_rdata_t *rdata, void *target) {

	REQUIRE(rdata->type == 3);

	target = target;

	return (DNS_R_NOTIMPLEMENTED);
}
#endif	/* RDATA_GENERIC_MD_3_H */
