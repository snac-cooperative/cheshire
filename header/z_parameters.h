/*
 *  Copyright (c) 1990-1999 [see Other Notes, below]. The Regents of the
 *  University of California (Regents). All Rights Reserved.
 *  
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for educational, research, and not-for-profit purposes,
 *  without fee and without a signed licensing agreement, is hereby
 *  granted, provided that the above copyright notice, this paragraph and
 *  the following two paragraphs appear in all copies, modifications, and
 *  distributions. Contact The Office of Technology Licensing, UC
 *  Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620,
 *  (510) 643-7201, for commercial licensing opportunities. 
 *  
 *  Created by Ray R. Larson, Aitao Chen, and Jerome McDonough, 
 *             School of Information Management and Systems, 
 *             University of California, Berkeley.
 *  
 *    
 *       IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT,
 *       INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 *       INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
 *       AND ITS DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE
 *       POSSIBILITY OF SUCH DAMAGE. 
 *    
 *       REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
 *       NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *       FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND
 *       ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
 *       PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 *       MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
 */
#ifndef	Z_PARAMETERS_H
#define Z_PARAMETERS_H

/* Object Identifiers from the Z39.50 V.3 (1995) standard */
/* Context */
#define	OID_AC_BASIC_Z3950_AC	"1.2.840.10003.1.1"

/* APDUs */
#define	OID_AS_Z3950_APDUS	"1.2.840.10003.2.1"

/* Attribute Set OIDs */
#define OID_DEFAULT		"1.2.840.10003.3.1"
/* BIB-1 -- also default */
#define OID_BIB1		"1.2.840.10003.3.1"
/* explain */
#define OID_EXP1		"1.2.840.10003.3.2"
/* extended services */
#define OID_EXT1		"1.2.840.10003.3.3"
/* common command language */
#define OID_CCL1		"1.2.840.10003.3.4"
/* GILS Govt. Information Locator Service attr set */
#define OID_GILS		"1.2.840.10003.3.5"
/* Scientific and Technical Attr Set */
#define OID_STAS		"1.2.840.10003.3.6"
/* collections Attr Set */
#define OID_COLLECTIONS1	"1.2.840.10003.3.7"
/* Museum collections Attr Set */
#define OID_CIMI1		"1.2.840.10003.3.8"
/* geospatial metadata */
#define OID_GEO                 "1.2.840.10003.3.9"
/* ZBID biological Attr Set */
#define OID_ZBIG		"1.2.840.10003.3.10"
/* Utility Attr Set */
#define OID_UTIL		"1.2.840.10003.3.11"
/* Cross Domain Attr Set */
#define OID_XD1 		"1.2.840.10003.3.12"
/* Thesuarus Navigation Attr Set */
#define OID_ZTHES		"1.2.840.10003.3.13"
/* Finnish Attr Set */
#define OID_FIN1		"1.2.840.10003.3.14"
/* Danish Attr Set */
#define OID_DAN1		"1.2.840.10003.3.15"
/* Holdings Attr Set */
#define OID_HOLD		"1.2.840.10003.3.16"

/* diagnostic syntax OIDs */
#define	OID_DS_BIB1		"1.2.840.10003.4.1"
#define	OID_DS_DIAG1		"1.2.840.10003.4.2"

/* Record Syntax OIDs */
#define MARCRECSYNTAX 		"1.2.840.10003.5.10"
#define	OID_USMARC		"1.2.840.10003.5.10"
#define EXPLAINRECSYNTAX 	"1.2.840.10003.5.100"
#define SUTRECSYNTAX		"1.2.840.10003.5.101"
#define OPACRECSYNTAX		"1.2.840.10003.5.102"
#define SUMMARYRECSYNTAX 	"1.2.840.10003.5.103"
#define GRS0RECSYNTAX    	"1.2.840.10003.5.104"
#define GRS1RECSYNTAX    	"1.2.840.10003.5.105"
#define ESRECSYNTAX      	"1.2.840.10003.5.106"

#define UNIMARC_RECSYNTAX "1.2.840.10003.5.1"
#define INTERMARC_RECSYNTAX "1.2.840.10003.5.2"
#define CCF_RECSYNTAX "1.2.840.10003.5.3"
#define USMARC_RECSYNTAX "1.2.840.10003.5.10"
#define UKMARC_RECSYNTAX "1.2.840.10003.5.11"
#define NORMARC_RECSYNTAX "1.2.840.10003.5.12"
#define LIBRISMARC_RECSYNTAX "1.2.840.10003.5.13"
#define DANMARC_RECSYNTAX "1.2.840.10003.5.14"
#define FINMARC_RECSYNTAX "1.2.840.10003.5.15"
#define MAB_RECSYNTAX "1.2.840.10003.5.16"
#define CANMARC_RECSYNTAX "1.2.840.10003.5.17"
#define SBN_RECSYNTAX "1.2.840.10003.5.18"
#define PICAMARC_RECSYNTAX "1.2.840.10003.5.19"
#define AUSMARC_RECSYNTAX "1.2.840.10003.5.20"
#define IBERMARC_RECSYNTAX "1.2.840.10003.5.21"
#define CATMARC_RECSYNTAX "1.2.840.10003.5.22"
#define MALMARC_RECSYNTAX "1.2.840.10003.5.23"
#define EXPLAIN_RECSYNTAX "1.2.840.10003.5.100"
#define SUTRS_RECSYNTAX "1.2.840.10003.5.101"
#define OPAC_RECSYNTAX "1.2.840.10003.5.102"
#define SUMMARY_RECSYNTAX "1.2.840.10003.5.103"
#define GRS0_RECSYNTAX "1.2.840.10003.5.104"
#define GRS1_RECSYNTAX "1.2.840.10003.5.105"
#define EXTENDEDSERVICES_RECSYNTAX "1.2.840.10003.5.106"
#define FRAGMENTSYNTAX_RECSYNTAX "1.2.840.10003.5.107"
#define SQL_RS_RECSYNTAX "1.2.840.10003.5.111"
#define PDF_RECSYNTAX "1.2.840.10003.5.109.1"
#define POSTSCRIPT_RECSYNTAX "1.2.840.10003.5.109.2"
#define HTML_RECSYNTAX "1.2.840.10003.5.109.3"
#define TIFF_RECSYNTAX "1.2.840.10003.5.109.4"
#define GIF_RECSYNTAX "1.2.840.10003.5.109.5"
#define JPEG_RECSYNTAX "1.2.840.10003.5.109.6"
#define PNG_RECSYNTAX "1.2.840.10003.5.109.7"
#define MPEG_RECSYNTAX "1.2.840.10003.5.109.8"
#define SGML_RECSYNTAX "1.2.840.10003.5.109.9"
#define XML_RECSYNTAX "1.2.840.10003.5.109.10"
#define XML_B_RECSYNTAX "1.2.840.10003.5.109.112"
#define TIFF_B_RECSYNTAX "1.2.840.10003.5.110.1"
#define WAV_RECSYNTAX "1.2.840.10003.5.110.2"


/* Transfer syntax OID (not used) */
#define TRANSFERSYNTAX          "1.2.840.10003.6.1"

/* Resource Report Formats */
#define	OID_RRF_RESOURCE1	"1.2.840.10003.7.1"
#define	OID_RRF_RESOURCE2	"1.2.840.10003.7.2"

/* Access Control */
#define	OID_AC_PROMPT1 		"1.2.840.10003.8.1"
#define	OID_AC_DES1 		"1.2.840.10003.8.2"
#define	OID_AC_KRB1 		"1.2.840.10003.8.3"

/* Extended Service */
#define	OID_ES_PERSIST_RS	"1.2.840.10003.9.1"
#define	OID_ES_PERSIST_QRY	"1.2.840.10003.9.2"
#define	OID_ES_PERIOD_QRY	"1.2.840.10003.9.3"
#define	OID_ES_ITEM_ORDER	"1.2.840.10003.9.4"
#define	OID_ES_DB_UPDATE	"1.2.840.10003.9.5"
#define	OID_ES_EXPORT_SPEC	"1.2.840.10003.9.6"
#define	OID_ES_EXPORT_INV	"1.2.840.10003.9.7"

/* user Info Format */
#define	OID_USR_SEARCH_RESULT1	"1.2.840.10003.10.1"

/* element spec format */
#define	OID_SPEC_ESPEC1		"1.2.840.10003.11.1"

/* variant Set format */
#define	OID_VAR_VARIANT1	"1.2.840.10003.12.1"

/* schema format */
#define	OID_SCHEMA_WAIS		"1.2.840.10003.13.1"
#define	OID_SCHEMA_GILS		"1.2.840.10003.13.2"
#define	OID_SCHEMA_GEO		"1.2.840.10003.13.4"

/* tagset format */
#define	OID_TAG_TAGSET_M	"1.2.840.10003.14.1"
#define	OID_TAG_TAGSET_G	"1.2.840.10003.14.2"
#define	OID_TAG_TAGSET_STAS	"1.2.840.10003.14.3"


/* some default constants */
#define	SMALL_SET_UPPER_BOUND		0
#define LARGE_SET_LOWER_BOUND		2
#define	MEDIUM_SET_PRESENT_NUMBER	0

#endif






