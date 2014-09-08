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
/*
 * Copyright (c) 1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:      Ray Larson, ray@sherlock.berkeley.edu
 *              School of Library and Information Studies, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**************************************************************************/
/* DispExplain - print explain format records                             */
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <tcl.h>

#include "z3950_3.h"
#include "zprimitive.h"
#include "z_parameters.h"



int
GetResultGSTR (GSTR *gstr, Tcl_Interp *interp)
{
  char *p, *c, *dup;
  char *lastptr, *strtok_r();
  int bracecnt;

  if (gstr == NULL)
    return 1;
  if (gstr->data != NULL) {
    if (strchr(gstr->data,'{') == NULL && strchr(gstr->data,'}') == NULL) {
      Tcl_AppendResult(interp, gstr->data, (char *) NULL);
    } 
    else { /* string contains braces that will mess up the list structure */
      dup = strdup(gstr->data);
      p = strtok_r(dup, "{}", &lastptr);
      if (p == NULL) {	
	if (*dup == '{')
	  Tcl_AppendResult(interp, "\\{", (char *) NULL);
	else if (*dup == '}')
	  Tcl_AppendResult(interp, "\\}", (char *) NULL);	
      } else {
	c = gstr->data;
	do {
	  Tcl_AppendResult(interp, p, (char *) NULL);
	  c += strlen(p);
	  if (*c == '{')
	    Tcl_AppendResult(interp, "\\{", (char *) NULL);
	  else if (*c == '}')
	    Tcl_AppendResult(interp, "\\}", (char *) NULL);	
	  /* get the next chunk */
	  p = strtok_r (NULL, "{}", &lastptr);
	} while (p != NULL);
      }
      FREE (dup);
    }
  }
  return 0;
}



int
GetLanguageCode(LanguageCode *in, Tcl_Interp *interp)
{
 Tcl_AppendResult(interp, "(", (char *) NULL);
  GetResultGSTR((GSTR *)in, interp);
  Tcl_AppendResult(interp, ")", (char *) NULL);
  return 0;
}

int
GetInternationalString2(InternationalString in, Tcl_Interp *interp)
{
  GetResultGSTR((GSTR *)in, interp);
  return 0;
}

int
GetOctetString2(InternationalString in, Tcl_Interp *interp)
{
  GetResultGSTR((GSTR *)in, interp);
  return 0;
}

int 
GetInt(int in, Tcl_Interp *interp)
{
  char temp[20];

  sprintf(temp,"%d",in);
  Tcl_AppendResult(interp, temp, (char *) NULL);

  return 0;
}

int
GetBoolean(Boolean in, Tcl_Interp *interp)
{
  if (in == 0)    
    Tcl_AppendResult(interp, "FALSE", (char *) NULL);
  else
    Tcl_AppendResult(interp, "TRUE", (char *) NULL);
  return 0;
}

int
GetStringOrNumeric(StringOrNumeric *in, Tcl_Interp *interp)
{
  char temp[20];

  if (in == NULL)
    return 1;


  if(in->which ==  e25_string)
    GetInternationalString2(in->u.string, interp);
  else {
    sprintf(temp,"%d",in->u.numeric);
    Tcl_AppendResult(interp, temp, (char *) NULL);
  }

  return 0;
}

int
GetOid(ObjectIdentifier in, Tcl_Interp *interp)
{
  GetResultGSTR((GSTR *)in, interp);
  return 0;
}

int
GetUnit(Unit *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;


  Tcl_AppendResult(interp, " {{unitSystem:: ", (char *) NULL);
  GetInternationalString2(in->unitSystem,interp);
  Tcl_AppendResult(interp, "} {unitType:: ", (char *) NULL);
  GetStringOrNumeric(in->unitType, interp);
  Tcl_AppendResult(interp, "} {unit:: ", (char *) NULL);
  GetStringOrNumeric(in->unit, interp);
  Tcl_AppendResult(interp, "} {scaleFactor:: ", (char *) NULL);
  GetInt(in->scaleFactor, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}
 
int
GetIntUnit(IntUnit *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {{value:: ", (char *) NULL);
  GetInt(in->value, interp);
  Tcl_AppendResult(interp, "} {unitUsed:: ", (char *) NULL);
  GetUnit(in->unitUsed, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}
 

int
GetGeneralizedTime(GeneralizedTime in, Tcl_Interp *interp)
{
  GetResultGSTR((GSTR *)in, interp);
  return 0;
}

int
GetHumanString(HumanString *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{language:: ", (char *) NULL);
  GetLanguageCode(in->item.language, interp); 
  Tcl_AppendResult(interp, "} {text:: ", (char *) NULL);
  GetInternationalString2(in->item.text, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  GetHumanString(in->next, interp);
  
  return 0;
}

int
GetIconObject(IconObject *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {bodyType:: ", (char *) NULL);

  switch (in->item.bodyType.which) {
  case e40_ianaType:
  Tcl_AppendResult(interp, "{ianaType:: ", (char *) NULL);
    GetInternationalString2(in->item.bodyType.u.ianaType, interp);
    break;
  case e40_z3950type:
  Tcl_AppendResult(interp, "{z3950type:: ", (char *) NULL);
    GetInternationalString2(in->item.bodyType.u.z3950type, interp);
    break;
  case e40_otherType:
  Tcl_AppendResult(interp, "{otherType:: ", (char *) NULL);
    GetInternationalString2(in->item.bodyType.u.otherType, interp);
    break;
  }

  Tcl_AppendResult(interp, "} {content:: ", (char *) NULL);
  GetOctetString2(in->item.content, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);


  GetIconObject(in->next, interp);
  
  return 0;
}
int
GetContactInfo(ContactInfo *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;


  Tcl_AppendResult(interp, " {ContactInfo:: {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {address:: ", (char *) NULL);
  GetHumanString(in->address, interp);
  Tcl_AppendResult(interp, "} {email:: ", (char *) NULL);
  GetInternationalString2(in->email, interp);
  Tcl_AppendResult(interp, "} {phone:: ", (char *) NULL);
  GetInternationalString2(in->phone, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  

  return 0;
}

int
GetDatabaseList(DatabaseList *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " ", (char *) NULL);
  GetResultGSTR((GSTR *)in->item, interp);

  GetDatabaseList(in->next, interp);
  
  return 0;
}

int
GetNetworkAddress(NetworkAddress *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;


  Tcl_AppendResult(interp, " {NetworkAddress:: ", (char *) NULL);

  switch(in->which) {
  case e41_internetAddress:

    Tcl_AppendResult(interp, " {internetAddress:: {hostAddress:: ", (char *) NULL);
    GetInternationalString2(in->u.internetAddress.hostAddress, interp);
    Tcl_AppendResult(interp, "} {port:: ", (char *) NULL);
    GetInt(in->u.internetAddress.port, interp);
    Tcl_AppendResult(interp, "}}}", (char *) NULL);
    break;
  case e41_osiPresentationAddress:
    Tcl_AppendResult(interp, " {OSIPresentationAddress:: ", (char *) NULL);
    Tcl_AppendResult(interp, " {pSel:: ", (char *) NULL);
    GetInternationalString2(in->u.osiPresentationAddress.pSel, interp);
    Tcl_AppendResult(interp, "} {sSel:: ", (char *) NULL);
    GetInternationalString2(in->u.osiPresentationAddress.sSel, interp);
    Tcl_AppendResult(interp, "} {tSel:: ", (char *) NULL);
    GetInternationalString2(in->u.osiPresentationAddress.tSel, interp);
    Tcl_AppendResult(interp, "} {nSap:: ", (char *) NULL);
    GetInternationalString2(in->u.osiPresentationAddress.nSap, interp);
    Tcl_AppendResult(interp, "}}}", (char *) NULL);
    break;
  case e41_other:

    Tcl_AppendResult(interp, " {otherAddress:: {type:: ", (char *) NULL);
    GetInternationalString2(in->u.other.type, interp);
    Tcl_AppendResult(interp, "} {address:: ", (char *) NULL);
    GetInternationalString2(in->u.other.address, interp);
    Tcl_AppendResult(interp, "}}}", (char *) NULL);
    break;
  }
  
  return 0;
}

int
GetAccessRestrictions(AccessRestrictions *in, Tcl_Interp *interp)
{
  struct accessChallenges_List83 *nextac, *iac;

  if (in == NULL)
    return 1;
 
  Tcl_AppendResult(interp, " {AccessRestrictions:: {accessType:: ", 
		   (char *) NULL);
  GetInt(in->item.accessType, interp);

  Tcl_AppendResult(interp, "} {accessType:: ", (char *) NULL);
  GetInt(in->item.accessType, interp);
  Tcl_AppendResult(interp, "} {accessText:: ", (char *) NULL);
  GetHumanString(in->item.accessText, interp);
  Tcl_AppendResult(interp, "} {accessChallenges::", (char *) NULL);

  for (iac = in->item.accessChallenges; iac != NULL; iac = nextac) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid(iac->item, interp);
    nextac = iac->next;    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
 
  GetAccessRestrictions(in->next, interp);
 
  return 0;
}
int
GetSearchKey(SearchKey *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;


  Tcl_AppendResult(interp, " {{searchKey:: ", (char *) NULL);
  GetInternationalString2(in->searchKey, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}
int
GetPrivateCapabilities(PrivateCapabilities *in, Tcl_Interp *interp)
{
  struct operators_List77 *nextop, *iop;
  struct searchKeys_List78 *nextsk, *isk;
  struct description_List79 *nextds, *ids;

  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {PrivateCapabilities:: {operators:: "
		   , (char *) NULL);

  for (iop = in->operators; iop != NULL; iop = nextop) {

    Tcl_AppendResult(interp, " {operator:: ", (char *) NULL);
    GetInternationalString2(iop->item.operator_var, interp);
    Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
    GetHumanString(iop->item.description, interp);
    Tcl_AppendResult(interp, "}", (char *) NULL);
    nextop = iop->next;
  }
  Tcl_AppendResult(interp, "} {searchKeys:: ", (char *) NULL);

  for (isk = in->searchKeys; isk != NULL; isk = nextsk) {
    GetSearchKey(isk->item, interp);
    nextsk = isk->next;
  }
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);

  for (ids = in->description; ids != NULL; ids = nextds) {
    GetHumanString(ids->item, interp);
    nextds = ids->next;
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}




int
GetCommonInfo(CommonInfo *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, "{DateAdded:: ", (char *) NULL);
  GetGeneralizedTime(in->dateAdded, interp);
  Tcl_AppendResult(interp, "} {DateChanged:: ", (char *) NULL);
  GetGeneralizedTime(in->dateChanged, interp);
  Tcl_AppendResult(interp, "} {Expiry:: ", (char *) NULL);
  GetGeneralizedTime(in->expiry, interp);
  Tcl_AppendResult(interp, "} {Language:: ", (char *) NULL);
  GetLanguageCode(in->humanString_Language, interp);
  Tcl_AppendResult(interp, "} {OtherInfo:: ", (char *) NULL);
  /* GetOtherInformation(in->otherInfo, interp);*/
  Tcl_AppendResult(interp, "}", (char *) NULL);
  
  return 0;
}



int
GetProximitySupport(ProximitySupport *in, Tcl_Interp *interp)
{
  struct unitsSupported_List82 *nextus, *ius;

  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {ProximitySupport:: {anySupport:: ", 
		   (char *) NULL);
  GetBoolean(in->anySupport, interp);
  Tcl_AppendResult(interp, "} {unitsSupported:: ", (char *) NULL);

  for (ius = in->unitsSupported; ius != NULL; ius = nextus) {
    if (ius->item != NULL) {      
      Tcl_AppendResult(interp, " {unitstypes:: ", (char *) NULL);
      if (ius->item->which == e43_known) {
	Tcl_AppendResult(interp, " {known:: ", (char *) NULL);
	GetInt(ius->item->u.known, interp);
	Tcl_AppendResult(interp, "}", (char *) NULL);
      }
      else { 
	Tcl_AppendResult(interp, " {unit:: ", (char *) NULL);
	GetInt(ius->item->u.private_var.unit, interp);
	Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
	GetHumanString(ius->item->u.private_var.description, interp);
	Tcl_AppendResult(interp, "}", (char *) NULL);
      }
      Tcl_AppendResult(interp, "}", (char *) NULL);
    }
    nextus = ius->next;
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
    
  
  return 0;
}
int
GetRpnCapabilities(RpnCapabilities *in, Tcl_Interp *interp)
{
  struct operators_List80 *nextop, *iop;;

  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {RpnCapabilities:: {operators:: {", (char *) NULL);
  for (iop = in->operators; iop != NULL; iop = nextop) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetInt(iop->item, interp);
    nextop = iop->next;    
  }
  Tcl_AppendResult(interp, "}} {resultSetAsOperandSupported:: ", (char *) NULL);
  GetBoolean(in->resultSetAsOperandSupported, interp);
  Tcl_AppendResult(interp, "} {restrictionOperandSupported:: ", (char *) NULL);
  GetBoolean(in->restrictionOperandSupported, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);
  GetProximitySupport(in->proximity, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  
  return 0;
}

int
GetIso8777Capabilities(Iso8777Capabilities *in, Tcl_Interp *interp)
{
  struct searchKeys_List81 *nextsk, *isk;;

  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {ISO8777Capabilities:: {searchKeys:: ", (char *) NULL);
  
  for (isk = in->searchKeys; isk != NULL; isk = nextsk) {
    GetSearchKey(isk->item, interp);
    nextsk = isk->next;
    
  }
  Tcl_AppendResult(interp, "} {restrictions:: ", (char *) NULL);

  GetHumanString(in->restrictions, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  return 0;
}

int
GetQueryTypeDetails(QueryTypeDetails *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;

  switch (in->which) {
  case e42_private:
    Tcl_AppendResult(interp, " {Type0:: ", (char *) NULL);
    GetPrivateCapabilities(in->u.private_var, interp);
    break;
  case e42_rpn:
    Tcl_AppendResult(interp, " {Type1:: ", (char *) NULL);
    GetRpnCapabilities(in->u.rpn, interp);
    break;
  case e42_iso8777:
    Tcl_AppendResult(interp, " {Type2:: ", (char *) NULL);
    GetIso8777Capabilities(in->u.iso8777, interp);
    break;
  case e42_z39_58:
    Tcl_AppendResult(interp, " {Type100:: ", (char *) NULL);
    GetHumanString(in->u.z39_58, interp);
    break;
  case e42_erpn:
    Tcl_AppendResult(interp, " {Type101:: ", (char *) NULL);
    GetRpnCapabilities(in->u.erpn, interp);
    break;
  case e42_rankedList:
    Tcl_AppendResult(interp, " {Type102:: ", (char *) NULL);
    GetHumanString(in->u.rankedList, interp);
    break;
  }
  Tcl_AppendResult(interp, "}", (char *) NULL);
  
  return 0;
}

int
GetCharge(Charge *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  Tcl_AppendResult(interp, " {Charge:: {cost:: ", (char *) NULL); 
  GetIntUnit(in->cost, interp);
  Tcl_AppendResult(interp, "} {perWhat:: ", (char *) NULL); 
  GetUnit(in->perWhat, interp);
  Tcl_AppendResult(interp, "} {text:: ", (char *) NULL); 
  GetHumanString(in->text, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL); 

  
  return 0;
}
int
GetCosts(Costs *in, Tcl_Interp *interp)
{
  struct otherCharges_List84 *nextoc, *ioc;

  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {Costs:: {connectCharge:: ", (char *) NULL); 
  GetCharge(in->connectCharge, interp);
  Tcl_AppendResult(interp, "} {connectTime:: ", (char *) NULL); 
  GetCharge(in->connectTime, interp);
  Tcl_AppendResult(interp, "} {displayCharge:: ", (char *) NULL); 
  GetCharge(in->displayCharge, interp);
  Tcl_AppendResult(interp, "} {searchCharge:: ", (char *) NULL); 
  GetCharge(in->searchCharge, interp);
  Tcl_AppendResult(interp, "} {subscriptCharge:: ", (char *) NULL); 
  GetCharge(in->subscriptCharge, interp);
  Tcl_AppendResult(interp, "} {OtherCharges:: ", (char *) NULL); 

  for (ioc = in->otherCharges; ioc != NULL; ioc = nextoc) {
  Tcl_AppendResult(interp, " {{forWhat:: ", (char *) NULL); 
    GetHumanString(ioc->item.forWhat, interp);
  Tcl_AppendResult(interp, "} {charge:: ", (char *) NULL); 
    GetCharge(ioc->item.charge, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL); 
    nextoc = ioc->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL); 
  
  return 0;
}
int
GetAccessInfo(AccessInfo *in, Tcl_Interp *interp)
{
  struct queryTypesSupported_List68 *nextqt, *iqt;
  struct diagnosticsSets_List69 *nextds, *ids;
  struct attributeSetIds_List70 *nextas, *ias;
  struct schemas_List71 *nextsc, *isc;
  struct recordSyntaxes_List72 *nextrs, *irs;
  struct resourceChallenges_List73 *nextrc, *irc;
  struct variantSets_List74 *nextvs, *ivs;
  struct elementSetNames_List75 *nextes, *ies;
  struct unitSystems_List76 *nextus, *ius;

  if (in == NULL)
    return 1;
 
  Tcl_AppendResult(interp, " {{QueryTypesSupported:: ", (char *) NULL);

  for (iqt = in->queryTypesSupported; iqt != NULL; iqt = nextqt) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetQueryTypeDetails(iqt->item, interp);
    nextqt = iqt->next;
    
  }

  Tcl_AppendResult(interp, "} {diagnosticsSets:: ", (char *) NULL);

  for (ids = in->diagnosticsSets; ids != NULL; ids = nextds) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid(ids->item, interp);
    nextds = ids->next;
    
  }

  Tcl_AppendResult(interp, "} {AttributeSetIds:: ", (char *) NULL);
  for (ias = in->attributeSetIds; ias != NULL; ias = nextas) {
  Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid((ObjectIdentifier)ias->item, interp);
    nextas = ias->next;
    
  }

  Tcl_AppendResult(interp, "} {Schemas:: ", (char *) NULL);
  for (isc = in->schemas; isc != NULL; isc = nextsc) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid(isc->item, interp);
    nextsc = isc->next;
    
  }

  Tcl_AppendResult(interp, "} {RecordSyntaxes:: ", (char *) NULL);
  for (irs = in->recordSyntaxes; irs != NULL; irs = nextrs) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid(irs->item, interp);
    nextrs = irs->next;
    
  }

  Tcl_AppendResult(interp, "} {ResourceChallenges:: ", (char *) NULL);
  for (irc = in->resourceChallenges; irc != NULL; irc = nextrc) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid(irc->item, interp);
    nextrc= irc->next;
    
  }

  Tcl_AppendResult(interp, "} {AccessRestrictions:: ", (char *) NULL);
  GetAccessRestrictions(in->restrictedAccess, interp);

  Tcl_AppendResult(interp, "} {Costs:: ", (char *) NULL);
  GetCosts(in->costInfo, interp);

  Tcl_AppendResult(interp, "} {VariantSets:: ", (char *) NULL);
  for (ivs = in->variantSets; ivs != NULL; ivs = nextvs) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid(ivs->item, interp);
    nextvs = ivs->next;
    
  }

  Tcl_AppendResult(interp, "} {ElementSetNames:: ", (char *) NULL);
  for (ies = in->elementSetNames; ies != NULL; ies = nextes) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetInternationalString2((InternationalString)ies->item, interp);
    nextes = ies->next;
    
  }
  Tcl_AppendResult(interp, "} {UnitSystems:: ", (char *) NULL);

  for (ius = in->unitSystems; ius != NULL; ius = nextus) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetInternationalString2(ius->item, interp);
    nextus = ius->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  return 0;
}


int
GetTargetInfo(TargetInfo *in, Tcl_Interp *interp)
{
  struct nicknames_List30 *nextnn, *inn;
  struct dbCombinations_List31 *nextdb, *idb;
  struct addresses_List32 *nextad, *iad;
  struct languages_List33 *nextln, *iln;
  
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {TargetInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  /* start  with a space for list formatting */
  Tcl_AppendResult(interp, " {Target_Name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {Recent_news:: ", (char *) NULL);
  GetHumanString(in->recent_news, interp);
  Tcl_AppendResult(interp, "} {Icon:: ", (char *) NULL);
  GetIconObject(in->icon, interp);
  Tcl_AppendResult(interp, "} {namedResultSets:: ", (char *) NULL);
  GetBoolean(in->namedResultSets, interp);
  Tcl_AppendResult(interp, "} {multipleDBsearch:: ", (char *) NULL);
  GetBoolean(in->multipleDBsearch, interp);

  Tcl_AppendResult(interp, "} {maxResultSets:: ", (char *) NULL);
  GetInt(in->maxResultSets, interp);
  Tcl_AppendResult(interp, "} {maxResultSize:: ", (char *) NULL);
  GetInt(in->maxResultSize, interp);
  Tcl_AppendResult(interp, "} {maxTerms:: ", (char *) NULL);
  GetInt(in->maxTerms, interp);

  Tcl_AppendResult(interp, "} {TimeoutInterval:: ", (char *) NULL);
  GetIntUnit(in->timeoutInterval, interp);
  Tcl_AppendResult(interp, "} {Welcome_Message:: ", (char *) NULL);
  GetHumanString(in->welcomeMessage, interp);
  Tcl_AppendResult(interp, "} {Contact_Info:: ", (char *) NULL);
  GetContactInfo(in->contactInfo, interp);
  Tcl_AppendResult(interp, "} {Description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  
  for (inn = in->nicknames; inn != NULL; inn = nextnn) {
    Tcl_AppendResult(interp, "} {Nickname:: ", (char *) NULL);
    GetInternationalString2(inn->item, interp);
    nextnn = inn->next;
    
  }
  
  Tcl_AppendResult(interp, "} {Usage_restrictions:: ", (char *) NULL);
  GetHumanString(in->usage_restrictions, interp);
  Tcl_AppendResult(interp, "} {Payment_address:: ", (char *) NULL);
  GetHumanString(in->paymentAddr, interp);
  Tcl_AppendResult(interp, "} {Hours:: ", (char *) NULL);
  GetHumanString(in->hours, interp);
  
  for (idb = in->dbCombinations; idb != NULL; idb = nextdb) {
    Tcl_AppendResult(interp, "} {DB_combinations:: ", (char *) NULL);
    GetDatabaseList(idb->item, interp);
    nextdb = idb->next;
    
  }
  
  for (iad = in->addresses; iad != NULL; iad = nextad) {
    Tcl_AppendResult(interp, "} {NetworkAddress:: ", (char *) NULL);
    GetNetworkAddress(iad->item, interp);
    nextad = iad->next;
    
  }
  
  for (iln = in->languages; iln != NULL; iln = nextln) {
    Tcl_AppendResult(interp, "} {Language:: ", (char *) NULL);
    GetInternationalString2(iln->item, interp);
    nextln = iln->next;
    
  }
  Tcl_AppendResult(interp, "} {CommonAccessInfo:: ", (char *) NULL);
  GetAccessInfo(in->commonAccessInfo, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}

int
GetDatabaseInfo(DatabaseInfo *in, Tcl_Interp *interp)
{
  struct nicknames_List34 *nextnn, *inn;
  struct keywords_List35 *nextkw, *ikw;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {DatabaseInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {DatabaseName:: ", (char *) NULL);
  GetResultGSTR((GSTR *)in->name, interp);

  if (in->explainDatabase == 0) 
    Tcl_AppendResult(interp, "} {explainDatabase:: NO", (char *) NULL);
  else
    Tcl_AppendResult(interp, "} {explainDatabase:: YES", (char *) NULL);

  
  for (inn = in->nicknames; inn != NULL; inn = nextnn) {
    Tcl_AppendResult(interp, "} {Nickname:: ", (char *) NULL);
    GetResultGSTR((GSTR *)inn->item, interp);
    nextnn = inn->next;
    
  }
  Tcl_AppendResult(interp, "} {Icon:: ", (char *) NULL);
  GetIconObject(in->icon, interp);

  Tcl_AppendResult(interp, "} {user_fee:: ", (char *) NULL);
  GetBoolean(in->user_fee, interp);
  Tcl_AppendResult(interp, "} {available:: ", (char *) NULL);
  GetBoolean(in->available, interp);


  Tcl_AppendResult(interp, "} {titleString:: ", (char *) NULL);
  GetHumanString(in->titleString, interp);
  
  for (ikw = in->keywords; ikw != NULL; ikw = nextkw) {
    Tcl_AppendResult(interp, "} {Keywords:: ", (char *) NULL);
    GetHumanString(ikw->item, interp);
    nextkw = ikw->next;
  }
  
  Tcl_AppendResult(interp, "} {Description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {AssociatedDBs:: ", (char *) NULL);
  GetDatabaseList(in->associatedDbs, interp);
  Tcl_AppendResult(interp, "} {SubDBs:: ", (char *) NULL);
  GetDatabaseList(in->subDbs, interp);
  Tcl_AppendResult(interp, "} {Disclaimers:: ", (char *) NULL);
  GetHumanString(in->disclaimers, interp);
  Tcl_AppendResult(interp, "} {News:: ", (char *) NULL);
  GetHumanString(in->news, interp);

  Tcl_AppendResult(interp, "} {RecordCount:: ", (char *) NULL);
  if (in->recordCount) {
    if (in->recordCount->which == e35_actualNumber) {
      Tcl_AppendResult(interp, " {actualNumber:: ", (char *) NULL);
      GetInt(in->recordCount->u.actualNumber, interp);
      Tcl_AppendResult(interp, "}", (char *) NULL);
    }
    else {
      Tcl_AppendResult(interp, " {approxNumber:: ", (char *) NULL);
      GetInt(in->recordCount->u.approxNumber, interp);
      Tcl_AppendResult(interp, "}", (char *) NULL);
    }
  }
  
  Tcl_AppendResult(interp, "} {DefaultOrder:: ", (char *) NULL);
  GetHumanString(in->defaultOrder, interp);

  Tcl_AppendResult(interp, "} {avRecordSize:: ", (char *) NULL);
  GetInt(in->avRecordSize, interp);
  Tcl_AppendResult(interp, "} {maxRecordSize:: ", (char *) NULL);
  GetInt(in->maxRecordSize, interp);

  Tcl_AppendResult(interp, "} {Hours:: ", (char *) NULL);
  GetHumanString(in->hours, interp);
  Tcl_AppendResult(interp, "} {BestTime:: ", (char *) NULL);
  GetHumanString(in->bestTime, interp);
  Tcl_AppendResult(interp, "} {LastUpdate:: ", (char *) NULL);
  GetGeneralizedTime(in->lastUpdate, interp);
  Tcl_AppendResult(interp, "} {UpdateInterval:: ", (char *) NULL);
  GetIntUnit(in->updateInterval, interp);
  
  Tcl_AppendResult(interp, "} {coverage:: ", (char *) NULL);
  GetHumanString(in->coverage, interp);
  Tcl_AppendResult(interp, "} {proprietary:: ", (char *) NULL);
  GetBoolean(in->proprietary, interp);
  Tcl_AppendResult(interp, "} {CopyrightText:: ", (char *) NULL);
  GetHumanString(in->copyrightText, interp);
  Tcl_AppendResult(interp, "} {CopyrightNotice:: ", (char *) NULL);
  GetHumanString(in->copyrightNotice, interp);
  Tcl_AppendResult(interp, "} {ProducerContactInfo:: ", (char *) NULL);
  GetContactInfo(in->producerContactInfo, interp);
  Tcl_AppendResult(interp, "} {SupplierContactInfo:: ", (char *) NULL);
  GetContactInfo(in->supplierContactInfo, interp);
  Tcl_AppendResult(interp, "} {SubmissionContactInfo:: ", (char *) NULL);
  GetContactInfo(in->submissionContactInfo, interp);
  Tcl_AppendResult(interp, "} {AccessInfo:: ", (char *) NULL);
  GetAccessInfo(in->accessInfo, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}

int
ex_GetPath(Path *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{tagType:: ", (char *) NULL);
  GetInt(in->item.tagType, interp);
  Tcl_AppendResult(interp, "} {tagValue:: ", (char *) NULL);
  GetStringOrNumeric(in->item.tagValue, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  ex_GetPath(in->next, interp);
  
  
  return 0;
}

int
GetElementDataType(ElementDataType *in, Tcl_Interp *interp)
{
  struct structured_List38 *nextrs, *irs;;
  
  if (in == NULL)
    return 1;
  
  if (in->which != e36_primitive) {
    Tcl_AppendResult(interp, " {structured:: ", (char *) NULL); 
    for (irs = in->u.structured; irs != NULL; irs = nextrs) {
      GetElementInfo(irs->item, interp);
      nextrs = irs->next;
    }
    Tcl_AppendResult(interp, "}", (char *) NULL); 
  }
  else {
    Tcl_AppendResult(interp, " {primitive:: ", (char *) NULL); 
    GetInt(in->u.primitive, interp);
    Tcl_AppendResult(interp, "}", (char *) NULL); 
  }
    
  
  return 0;
}
int
GetElementInfo(ElementInfo *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{elementName:: ", (char *) NULL);
  GetInternationalString2(in->elementName, interp);
  Tcl_AppendResult(interp, "} {elementTagPath:: ", (char *) NULL);
  ex_GetPath(in->elementTagPath, interp);
  Tcl_AppendResult(interp, "} {dataType:: ", (char *) NULL);
  GetElementDataType(in->dataType, interp);

  Tcl_AppendResult(interp, "} {required:: ", (char *) NULL);
  GetBoolean(in->required, interp);
  Tcl_AppendResult(interp, "} {repeatable:: ", (char *) NULL);
  GetBoolean(in->repeatable, interp);


  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);  
  
  return 0;
}
int
GetSchemaInfo(SchemaInfo *in, Tcl_Interp *interp)
{
  struct tagTypeMapping_List36 *nexttt, *itt;
  struct recordStructure_List37 *nextrs, *irs;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {SchemaInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {schema:: ", (char *) NULL);
  GetOid(in->schema, interp);
  Tcl_AppendResult(interp, "} {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  
  Tcl_AppendResult(interp, "} {tagTypeMapping:: ", (char *) NULL);
  for (itt = in->tagTypeMapping; itt != NULL; itt = nexttt) {    
    Tcl_AppendResult(interp, " {{tagtype:: ", (char *) NULL);
    GetInt(itt->item.tagType, interp);
    Tcl_AppendResult(interp, "} {tagSet:: ", (char *) NULL);
    GetOid(itt->item.tagSet, interp);
    if (itt->item.defaultTagType == 0)
      Tcl_AppendResult(interp, "} {defaultTagType:: NO", (char *) NULL);
    else
      Tcl_AppendResult(interp, "} {defaultTagType:: YES", (char *) NULL);
      Tcl_AppendResult(interp, "}}", (char *) NULL);
    nexttt = itt->next;
    
  }
  Tcl_AppendResult(interp, "} {recordStructure:: ", (char *) NULL);
  
  for (irs = in->recordStructure; irs != NULL; irs = nextrs) {
    GetElementInfo(irs->item, interp);
    nextrs = irs->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetTagSetInfo(TagSetInfo *in, Tcl_Interp *interp)
{
  struct nicknames_List40 *nextnn, *inn;
  struct elements_List39 *nextel, *iel;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {TagSetInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {tagSet:: ", (char *) NULL);
  GetOid(in->tagSet, interp);
  Tcl_AppendResult(interp, "} {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  
  Tcl_AppendResult(interp, "} {elements:: ", (char *) NULL);

  for (iel = in->elements; iel != NULL; iel = nextel) {
    Tcl_AppendResult(interp, " {{name:: ", (char *) NULL);
    GetInternationalString2(iel->item.elementname, interp);
    
    Tcl_AppendResult(interp, "} {nicknames:: ", (char *) NULL);
    for (inn = iel->item.nicknames; inn != NULL; inn = nextnn) {
      Tcl_AppendResult(interp, " ", (char *) NULL);
      GetInternationalString2(inn->item, interp);
      nextnn = inn->next;
      
    }
    Tcl_AppendResult(interp, "} {elementTag:: ", (char *) NULL);
    GetStringOrNumeric(iel->item.elementTag, interp);
    Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
    GetHumanString(iel->item.description, interp);
    Tcl_AppendResult(interp, "} {dataType:: ", (char *) NULL);
    GetInt(iel->item.dataType, interp);
    Tcl_AppendResult(interp, "} {otherTagInfo:: ", (char *) NULL);
    /* GetOtherInformation(iel->item.otherTagInfo, interp);*/
    Tcl_AppendResult(interp, "}}", (char *) NULL);
    
    nextel = iel->next;
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}

int
GetRecordSyntaxInfo(RecordSyntaxInfo *in, Tcl_Interp *interp)
{
  struct transferSyntaxes_List41 *nextts, *its;
  struct abstractStructure_List42 *nextas, *ias;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {RecordSyntaxInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {recordSyntax:: ", (char *) NULL);
  GetOid(in->recordSyntax, interp);
  Tcl_AppendResult(interp, "} {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  
  Tcl_AppendResult(interp, "} {transfersyntaxes:: ", (char *) NULL);
  for (its = in->transferSyntaxes; its != NULL; its = nextts) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetOid(its->item, interp);
    nextts = its->next;
    
  }
  
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {asn1Module:: ", (char *) NULL);
  GetInternationalString2(in->asn1Module, interp);
  
  Tcl_AppendResult(interp, "} {abstractStructure:: ", (char *) NULL);

  for (ias = in->abstractStructure; ias != NULL; ias = nextas) {
    GetElementInfo(ias->item, interp);
    nextas = ias->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}

int
GetAttributeDescription(AttributeDescription *in, Tcl_Interp *interp)
{
  struct equivalentAttributes_List45 *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {attributeValue:: ", (char *) NULL);
  GetStringOrNumeric(in->attributeValue, interp);
  
  Tcl_AppendResult(interp, "} {equivalentAttributes:: ", (char *) NULL);
  for (iat = in->equivalentAttributes; iat != NULL; iat = nextat) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetStringOrNumeric(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}

int
GetAttributeType(AttributeType *in, Tcl_Interp *interp)
{
  struct attributeValues_List44	*nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {attributeType:: ", (char *) NULL);
  GetInt(in->attributeType, interp);
  Tcl_AppendResult(interp, "} {attributeValues:: ", (char *) NULL);
  for (iat = in->attributeValues; iat != NULL; iat = nextat) {
    GetAttributeDescription(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetAttributeSetInfo(AttributeSetInfo *in, Tcl_Interp *interp)
{
  struct attributes_List43 *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {AttributeSetInfo:: {Commoninfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "} {AttributeSet:: ", (char *) NULL);
  GetOid(in->attributeSet, interp);
  Tcl_AppendResult(interp, "} {Name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  
  Tcl_AppendResult(interp, "} {Attributes:: ", (char *) NULL);
  for (iat = in->attributes; iat != NULL; iat = nextat) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetAttributeType(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "} {Description:: ", (char *) NULL);  
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}

int
GetTermListInfo(TermListInfo *in, Tcl_Interp *interp)
{
  struct termLists_List46 *nexttl, *itl;
  struct broader_List47 *nextbl, *ibl;
  struct narrower_List48 *nextnl, *inl;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {TermListInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {databaseName:: ", (char *) NULL);
  GetResultGSTR((GSTR *)in->databaseName, interp);
  
  Tcl_AppendResult(interp, "} {termLists:: ", (char *) NULL);
  for (itl = in->termLists; itl != NULL; itl = nexttl) {
    Tcl_AppendResult(interp, " {{name:: ", (char *) NULL);
    GetInternationalString2(itl->item.name, interp);
    Tcl_AppendResult(interp, "} {title:: ", (char *) NULL);
    GetHumanString(itl->item.title, interp);
    Tcl_AppendResult(interp, "} {searchCost:: ", (char *) NULL);
    GetInt(itl->item.searchCost, interp);
    Tcl_AppendResult(interp, "} {scanable:: ", (char *) NULL);
    GetBoolean(itl->item.scanable, interp);    
    Tcl_AppendResult(interp, "} {broader:: ", (char *) NULL);
    for (ibl = itl->item.broader; ibl != NULL; ibl = nextbl) {
      Tcl_AppendResult(interp, " ", (char *) NULL);
      GetInternationalString2(ibl->item, interp);
      nextbl = ibl->next;
      
    }
    Tcl_AppendResult(interp, "} {narrower:: ", (char *) NULL);
    for (inl = itl->item.narrower; inl != NULL; inl = nextnl) {
      Tcl_AppendResult(interp, " ", (char *) NULL);
      GetInternationalString2(inl->item, interp);
      nextnl = inl->next;
      
    }

    Tcl_AppendResult(interp, "}}", (char *) NULL);
    nexttl = itl->next;
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}

int
GetExtendedServicesInfo(ExtendedServicesInfo *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {ExtendedServicesInfo:: {CommonInfo:: ", 
		   (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {type:: ", (char *) NULL);
  GetOid(in->type, interp);
  Tcl_AppendResult(interp, "} {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);

  Tcl_AppendResult(interp, "} {privateType:: ", (char *) NULL);
  GetBoolean(in->privateType, interp);
  Tcl_AppendResult(interp, "} {restrictionsApply:: ", (char *) NULL);
  GetBoolean(in->restrictionsApply, interp);
  Tcl_AppendResult(interp, "} {feeApply:: ", (char *) NULL);
  GetBoolean(in->feeApply, interp);
  Tcl_AppendResult(interp, "} {available:: ", (char *) NULL);
  GetBoolean(in->available, interp);
  Tcl_AppendResult(interp, "} {retentionSupported:: ", (char *) NULL);
  GetBoolean(in->retentionSupported, interp);

  Tcl_AppendResult(interp, "} {waitAction:: ", (char *) NULL);
  GetInt(in->waitAction, interp);

  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {EXTERNAL_OBJECT:: --not printed", (char *) NULL);
  /* GetExternal(in->specificExplain, interp); */
  Tcl_AppendResult(interp, "} {esASN:: ", (char *) NULL);
  GetInternationalString2(in->esASN, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}

int
GetOmittedAttributeInterpretation(OmittedAttributeInterpretation *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{defaultValue:: ", (char *) NULL);
  GetStringOrNumeric(in->defaultValue, interp);
  Tcl_AppendResult(interp, "} {defaultDescription:: ", (char *) NULL);
  GetHumanString(in->defaultDescription, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}

int
GetAttributeValue(AttributeValue *in, Tcl_Interp *interp)
{
  struct subAttributes_List52  *nextat1, *iat1;
  struct superAttributes_List53 *nextat2, *iat2;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{value:: ", (char *) NULL);
  GetStringOrNumeric(in->value, interp);

  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  

  Tcl_AppendResult(interp, "} {subAttributes:: ", (char *) NULL);
  for (iat1 = in->subAttributes; iat1 != NULL; iat1 = nextat1) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetStringOrNumeric(iat1->item, interp);
    nextat1 = iat1->next;
    
  }
  
  Tcl_AppendResult(interp, "} {superAttributes:: ", (char *) NULL);
  for (iat2 = in->superAttributes; iat2 != NULL; iat2 = nextat2) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetStringOrNumeric(iat2->item, interp);
    nextat2 = iat2->next;
    
  }

  Tcl_AppendResult(interp, "} {partialSupport:: ", (char *) NULL);
  if (in->partialSupport == 0)
    Tcl_AppendResult(interp, "NO", (char *) NULL);
  else
    Tcl_AppendResult(interp, "YES", (char *) NULL);

  Tcl_AppendResult(interp, "}}", (char *) NULL);

  
  return 0;
}

int
GetAttributeTypeDetails(AttributeTypeDetails *in, Tcl_Interp *interp)
{
  struct attributeValues_List51 *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{attributeType:: ", (char *) NULL);
  GetInt(in->attributeType, interp);

  Tcl_AppendResult(interp, "} {defaultIfOmitted:: ", (char *) NULL);
  GetOmittedAttributeInterpretation(in->defaultIfOmitted, interp);
  
  Tcl_AppendResult(interp, "} {attributeValues:: ", (char *) NULL);
  for (iat = in->attributeValues; iat != NULL; iat = nextat) {
    GetAttributeValue(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}
int
GetAttributeSetDetails(AttributeSetDetails *in, Tcl_Interp *interp)
{
  struct attributesByType_List50 *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{attributeSet:: ", (char *) NULL);
  GetOid(in->attributeSet, interp);
  
  Tcl_AppendResult(interp, "} {attributesByType:: ", (char *) NULL);
  for (iat = in->attributesByType; iat != NULL; iat = nextat) {
    GetAttributeTypeDetails(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}
int
GetAttributeOccurrence(AttributeOccurrence *in, Tcl_Interp *interp)
{
  struct specific_List86 *nextat, *iat;
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{attributeSet:: ", (char *) NULL);
  GetOid(in->attributeSet, interp);

  Tcl_AppendResult(interp, "} {attributeType:: ", (char *) NULL);
  GetInt(in->attributeType, interp);

  Tcl_AppendResult(interp, "} {mustBeSupplied:: ", (char *) NULL);
  if (in->mustBeSupplied == 0)
    Tcl_AppendResult(interp, "NO", (char *) NULL);
  else
    Tcl_AppendResult(interp, "YES", (char *) NULL);

  if (in->attributeValues.which == e44_any_or_none) {
    if (in->attributeValues.u.any_or_none == 0)
      Tcl_AppendResult(interp, "} {any_or_none:: NO", (char *) NULL);
    else
      Tcl_AppendResult(interp, "} {any_or_none:: YES", (char *) NULL);

  }
  else {
    Tcl_AppendResult(interp, "} {specific::", (char *) NULL);
  
    for (iat = in->attributeValues.u.specific; iat != NULL; iat = nextat) {
      Tcl_AppendResult(interp, " ", (char *) NULL);
      GetStringOrNumeric(iat->item, interp);
      nextat = iat->next;
      
    }
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  
  return 0;
}

int
GetAttributeCombination(AttributeCombination *in, Tcl_Interp *interp)
{
  AttributeCombination *comb;
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {AttributeCombination:: ", (char *) NULL);
  for (comb = in; comb; comb=comb->next) {
    GetAttributeOccurrence(comb->item, interp);
  }
  Tcl_AppendResult(interp, "}", (char *) NULL);  
  
  return 0;
}
int
GetAttributeCombinations(AttributeCombinations *in, Tcl_Interp *interp)
{
  struct legalCombinations_List85 *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{defaultAttributeSet:: ", (char *) NULL);
  GetOid(in->defaultAttributeSet, interp);
  
  Tcl_AppendResult(interp, "} {legalCombinations:: ", (char *) NULL);
  for (iat = in->legalCombinations; iat != NULL; iat = nextat) {
    GetAttributeCombination(iat->item, interp);
    nextat = iat->next;    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}
int
GetAttributeDetails(AttributeDetails *in, Tcl_Interp *interp)
{
  struct attributesBySet_List49 *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {AttributeDetails:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {databaseName:: ", (char *) NULL);
  GetResultGSTR((GSTR *)in->databaseName, interp);
  
  Tcl_AppendResult(interp, "} {attributesBySet:: ", (char *) NULL);
  for (iat = in->attributesBySet; iat != NULL; iat = nextat) {
    GetAttributeSetDetails(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "} {attributeCombinations:: ", (char *) NULL);
  GetAttributeCombinations(in->attributeCombinations, interp);

  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetTermListDetails(TermListDetails *in, Tcl_Interp *interp)
{
  struct sampleTerms_List54  *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {TermListDetails:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {termListName:: ", (char *) NULL);
  GetInternationalString2(in->termListName, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {attributes:: ", (char *) NULL);
  GetAttributeCombinations(in->attributes, interp);
  
  Tcl_AppendResult(interp, "} {scanInfo:: ", (char *) NULL);
  if (in->scanInfo) {
    Tcl_AppendResult(interp, " {maxStepSize:: ", (char *) NULL);
    GetInt(in->scanInfo->maxStepSize, interp);
    Tcl_AppendResult(interp, "} {collatingSequence:: ", (char *) NULL);
    GetHumanString(in->scanInfo->collatingSequence, interp);
    Tcl_AppendResult(interp, "} {increasing:: ", (char *) NULL);
    GetBoolean(in->scanInfo->increasing, interp);
    Tcl_AppendResult(interp, "}", (char *) NULL);
    
  }
  Tcl_AppendResult(interp, "} {estNumberTerms:: ", (char *) NULL);
  GetInt(in->estNumberTerms, interp);

  Tcl_AppendResult(interp, "} {sampleTerms:: ", (char *) NULL);
  for (iat = in->sampleTerms; iat != NULL; iat = nextat) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetInternationalString2((InternationalString)iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetRecordTag(RecordTag *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  Tcl_AppendResult(interp, " ", (char *) NULL);
  GetStringOrNumeric(in->qualifier, interp);
  Tcl_AppendResult(interp, " ", (char *) NULL);
  GetStringOrNumeric(in->tagValue, interp);
  
  
  return 0;
}
int
GetPerElementDetails(PerElementDetails *in, Tcl_Interp *interp)
{
  struct schemaTags_List57 *nextst, *ist;
  struct alternateNames_List58 *nextalt, *ialt;
  struct genericNames_List59 *nextgen, *igen;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{name:: ", (char *) NULL);
  GetInternationalString2 (in->name, interp);
  Tcl_AppendResult(interp, "} {recordTag:: ", (char *) NULL);
  GetRecordTag (in->recordTag, interp);
  Tcl_AppendResult(interp, "} {schemaTags:: ", (char *) NULL);
  
  for (ist = in->schemaTags; ist != NULL; ist = nextst) {
    Tcl_AppendResult(interp, " {Path:: ", (char *) NULL);
    ex_GetPath(ist->item, interp);
    Tcl_AppendResult(interp, "}", (char *) NULL);
    nextst = ist->next;
    
  }

  Tcl_AppendResult(interp, "} {maxSize:: ", (char *) NULL);
  GetInt (in->maxSize, interp);
  Tcl_AppendResult(interp, "} {minSize:: ", (char *) NULL);
  GetInt (in->minSize, interp);
  Tcl_AppendResult(interp, "} {avgSize:: ", (char *) NULL);
  GetInt (in->avgSize, interp);
  Tcl_AppendResult(interp, "} {fixedSize:: ", (char *) NULL);
  GetInt (in->fixedSize, interp);

  Tcl_AppendResult(interp, "} {repeatable:: ", (char *) NULL);
  GetBoolean (in->repeatable, interp);
  Tcl_AppendResult(interp, "} {required:: ", (char *) NULL);
  GetBoolean (in->required, interp);

  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString (in->description, interp);
  Tcl_AppendResult(interp, "} {contents:: ", (char *) NULL);
  GetHumanString (in->contents, interp);
  Tcl_AppendResult(interp, "} {billingInfo:: ", (char *) NULL);
  GetHumanString (in->billingInfo, interp);
  Tcl_AppendResult(interp, "} {restrictions:: ", (char *) NULL);
  GetHumanString (in->restrictions, interp);

  Tcl_AppendResult(interp, "} {alternatNames:: ", (char *) NULL);
  
  for (ialt = in->alternateNames; ialt != NULL; ialt = nextalt) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetInternationalString2(ialt->item, interp);
    nextalt = ialt->next;
    
  }
  
  Tcl_AppendResult(interp, "} {genericNames:: ", (char *) NULL);
  
  for (igen = in->genericNames; igen != NULL; igen = nextgen) {
    Tcl_AppendResult(interp, " ", (char *) NULL);
    GetInternationalString2(igen->item, interp);
    nextgen = igen->next;
    
  }
  Tcl_AppendResult(interp, "} {searchAccess:: ", (char *) NULL);

  GetAttributeCombinations(in->searchAccess, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}

int
GetElementSetDetails(ElementSetDetails *in, Tcl_Interp *interp)
{
  struct detailsPerElement_List55   *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {ElementSetDetails:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {databaseName:: ", (char *) NULL);
  GetResultGSTR((GSTR *)in->databaseName, interp);
  Tcl_AppendResult(interp, "} {elementSetName:: ", (char *) NULL);
  GetInternationalString2((InternationalString)in->elementSetName, interp);
  Tcl_AppendResult(interp, "} {recordSyntax:: ", (char *) NULL);
  GetOid(in->recordSyntax, interp);
  Tcl_AppendResult(interp, "} {schema:: ", (char *) NULL);
  GetOid(in->schema, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  
  Tcl_AppendResult(interp, "} {detailsPerElement:: ", (char *) NULL);
  for (iat = in->detailsPerElement; iat != NULL; iat = nextat) {
    GetPerElementDetails(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetRetrievalRecordDetails(RetrievalRecordDetails *in, Tcl_Interp *interp)
{
  struct detailsPerElement_List56  *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {RetrievalRecordDetails:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {DatabaseName:: ", (char *) NULL);
  GetResultGSTR((GSTR *)in->databaseName, interp);
  Tcl_AppendResult(interp, "} {RecordSyntax:: ", (char *) NULL);
  GetOid(in->recordSyntax, interp);
  Tcl_AppendResult(interp, "} {Schema:: ", (char *) NULL);
  GetOid(in->schema, interp);
  Tcl_AppendResult(interp, "} {Description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {DetailsPerElement:: ", (char *) NULL);
  
  for (iat = in->detailsPerElement; iat != NULL; iat = nextat) {
    GetPerElementDetails(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}

int
GetSpecification(Specification *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " ", (char *) NULL);
  GetOid(in->schema, interp);
  
  if (in->elementSpec != NULL) {
    if (in->elementSpec->which == e16_elementSetName) {
      Tcl_AppendResult(interp, " ", (char *) NULL);
      GetInternationalString2(in->elementSpec->u.elementSetName, interp);
    }
    else {
      Tcl_AppendResult(interp, " EXTERNAL_OBJECT", (char *) NULL);
      /* GetExternal(in->elementSpec->u.externalEspec, interp); */
    }
  }
  
  return 0;
}

int
GetSortKeyDetails(SortKeyDetails *in, Tcl_Interp *interp)
{
  struct elementSpecifications_List61  *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  
  Tcl_AppendResult(interp, "} {elementSpecifications:: ", (char *) NULL);
  for (iat = in->elementSpecifications; iat != NULL; iat = nextat) {
    GetSpecification(iat->item, interp);
    nextat = iat->next;
    
  }
  Tcl_AppendResult(interp, "} {attributeSpecifications:: ", (char *) NULL);
  GetAttributeCombinations(in->attributeSpecifications, interp);
  Tcl_AppendResult(interp, "} {SortType:: ", (char *) NULL);

  if (in->sortType != NULL) {
    switch (in->sortType->which) {
    case e37_character: 
      Tcl_AppendResult(interp, "character", (char *) NULL);
      break;
    case e37_numeric:
      Tcl_AppendResult(interp, "numeric", (char *) NULL);
      break;
    case e37_structured:
      Tcl_AppendResult(interp, " {structured:: ", (char *) NULL);
      GetHumanString(in->sortType->u.structured, interp);
      Tcl_AppendResult(interp, "}", (char *) NULL);
      break;
    }
  }
  Tcl_AppendResult(interp, "} {caseSensitivity:: ", (char *) NULL);
  GetInt(in->caseSensitivity, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  
  return 0;
}
int
GetSortDetails(SortDetails *in, Tcl_Interp *interp)
{
  struct sortKeys_List60    *nextat, *iat;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {SortDetails:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {databaseName:: ", (char *) NULL);
  GetResultGSTR((GSTR *)in->databaseName, interp);
  
  Tcl_AppendResult(interp, "} {sortKeys:: ", (char *) NULL);
  for (iat = in->sortKeys; iat != NULL; iat = nextat) {
    GetSortKeyDetails(iat->item, interp);
    nextat = iat->next;    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetProcessingInformation(ProcessingInformation *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {ProcessingInformation:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {databaseName:: ", (char *) NULL);
  GetResultGSTR((GSTR *)in->databaseName, interp);
  Tcl_AppendResult(interp, "} {processingContext:: ", (char *) NULL);
  GetInt(in->processingContext, interp);
  Tcl_AppendResult(interp, "} {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {oid:: ", (char *) NULL);
  GetOid(in->oid, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {EXTERNAL_OBJECT:: --not printed", (char *) NULL);
  /* GetExternal(in->instructions, interp); */
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}
int
GetValueDescription(ValueDescription *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {", (char *) NULL);

  switch (in->which) {
  case e39_integer:
    Tcl_AppendResult(interp, "integer:: ", (char *) NULL);
    GetInt(in->u.integer, interp);
    break;
  case e39_string:
    Tcl_AppendResult(interp, "string:: ", (char *) NULL);
    GetInternationalString2(in->u.string, interp);
    break;
  case e39_octets:
    Tcl_AppendResult(interp, "octets:: ", (char *) NULL);
    GetOctetString2(in->u.octets, interp);
    break;
  case e39_oid:
    Tcl_AppendResult(interp, "oid:: ", (char *) NULL);
    GetOid(in->u.oid, interp);
    break;
  case e39_unit:
    Tcl_AppendResult(interp, "unit:: ", (char *) NULL);
    GetUnit(in->u.unit, interp);
    break;
  case e39_valueAndUnit:
    Tcl_AppendResult(interp, "valueAndUnit:: ", (char *) NULL);
    GetIntUnit(in->u.valueAndUnit, interp);
    break;
  }
  Tcl_AppendResult(interp, "}", (char *) NULL);

  return 0;
}

int
GetValueRange(ValueRange *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {lower:: ", (char *) NULL);
  GetValueDescription(in->lower, interp);
  Tcl_AppendResult(interp, "} {upper:: ", (char *) NULL);
  GetValueDescription(in->upper, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);
  
  return 0;
}

int
GetValueSet(ValueSet *in, Tcl_Interp *interp)
{
  struct enumerated_List64 *nextvr, *ivr;
  
  if (in == NULL)
    return 1;


  if (in->which == e38_range) {
    Tcl_AppendResult(interp, " {range:: ", (char *) NULL);
    GetValueRange(in->u.range, interp);
    Tcl_AppendResult(interp, "}", (char *) NULL);

  }
  else {
    Tcl_AppendResult(interp, " {enumerated:: ", (char *) NULL);
    for (ivr = in->u.enumerated; ivr != NULL; ivr = nextvr) {
      GetValueDescription(ivr->item, interp);
      nextvr = ivr->next;
    }
    Tcl_AppendResult(interp, "}", (char *) NULL);
  }
  
  return 0;
}

int
GetVariantValue(VariantValue *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{dataType:: ", (char *) NULL);
  GetInt(in->dataType, interp);
  Tcl_AppendResult(interp, "} {values:: ", (char *) NULL);
  GetValueSet(in->values, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetVariantType(VariantType *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {variantType:: ", (char *) NULL);
  GetInt(in->variantType, interp);
  Tcl_AppendResult(interp, "} {variantValue:: ", (char *) NULL);
  GetVariantValue(in->variantValue, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}

int
GetVariantClass(VariantClass *in, Tcl_Interp *interp)
{
  struct variantTypes_List63 *nextvr, *ivr;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {VarianClass:: {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {variantClass:: ", (char *) NULL);
  GetInt(in->variantClass, interp);
  Tcl_AppendResult(interp, "} {variantTypes:: ", (char *) NULL);
  for (ivr = in->variantTypes; ivr != NULL; ivr = nextvr) {
    GetVariantType(ivr->item, interp);
    nextvr = ivr->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}
int
GetVariantSetInfo(VariantSetInfo *in, Tcl_Interp *interp)
{
  struct variants_List62 *nextvr, *ivr;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {VariantSetInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {variantSet:: ", (char *) NULL);
  GetOid(in->variantSet, interp);

  Tcl_AppendResult(interp, "} {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  
  Tcl_AppendResult(interp, "} {variants:: ", (char *) NULL);
  for (ivr = in->variants; ivr != NULL; ivr = nextvr) {
    GetVariantClass(ivr->item, interp);
    nextvr = ivr->next;
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}

int
GetUnits(Units *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {{name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {unit:: ", (char *) NULL);
  GetStringOrNumeric(in->unit, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}


int
GetUnitType(UnitType *in, Tcl_Interp *interp)
{
  struct units_List66 *nextvr, *ivr;;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {UnitType:: {name:: ", (char *) NULL);
  GetInternationalString2(in->name, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {unittype:: ", (char *) NULL);
  GetStringOrNumeric(in->unitType, interp);
  Tcl_AppendResult(interp, "} {units:: ", (char *) NULL);
  
  for (ivr = in->units; ivr != NULL; ivr = nextvr) {
    GetUnits(ivr->item, interp);
    nextvr = ivr->next;    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  return 0;
}
int
GetUnitInfo(UnitInfo *in, Tcl_Interp *interp)
{
  struct units_List65 *nextvr, *ivr;;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {UnitInfo:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);

  Tcl_AppendResult(interp, " {unitSystem:: ", (char *) NULL);
  GetInternationalString2(in->unitSystem, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {units:: ", (char *) NULL);  
  for (ivr = in->units; ivr != NULL; ivr = nextvr) {
    GetUnitType(ivr->item, interp);
    nextvr = ivr->next;    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  return 0;
}
int
GetCategoryInfo(CategoryInfo *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {CategoryInfo:: {category:: ", (char *) NULL);
  GetInternationalString2(in->category, interp);
  Tcl_AppendResult(interp, "} {originalCategory:: ", (char *) NULL);
  GetInternationalString2(in->originalCategory, interp);
  Tcl_AppendResult(interp, "} {description:: ", (char *) NULL);
  GetHumanString(in->description, interp);
  Tcl_AppendResult(interp, "} {asn1Module:: ", (char *) NULL);
  GetInternationalString2(in->asn1Module, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}
int
GetCategoryList(CategoryList *in, Tcl_Interp *interp)
{
  struct categories_List67 *nextvr, *ivr;
  
  if (in == NULL)
    return 1;
  
  Tcl_AppendResult(interp, " {CategoryList:: {CommonInfo:: ", (char *) NULL);
  GetCommonInfo(in->commonInfo, interp);
  Tcl_AppendResult(interp, "} {Categories:: ", (char *) NULL);  
  for (ivr = in->categories; ivr != NULL; ivr = nextvr) {
    GetCategoryInfo(ivr->item, interp);
    nextvr = ivr->next;
    
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  
  
  return 0;
}

int
GetExplain_Record(Explain_Record *in, Tcl_Interp *interp)
{
  if (in == NULL)
    return 1;
  
  switch (in->which) {
  case e34_targetInfo:
    GetTargetInfo(in->u.targetInfo, interp);
    break;
  case e34_databaseInfo:
    GetDatabaseInfo(in->u.databaseInfo, interp);
    break;
  case e34_schemaInfo:
    GetSchemaInfo(in->u.schemaInfo, interp);
    break;
  case e34_tagSetInfo:
    GetTagSetInfo(in->u.tagSetInfo, interp);
    break;
  case e34_recordSyntaxInfo:
    GetRecordSyntaxInfo(in->u.recordSyntaxInfo, interp);
    break;
  case e34_attributeSetInfo:
    GetAttributeSetInfo(in->u.attributeSetInfo, interp);
    break;
  case e34_termListInfo:
    GetTermListInfo(in->u.termListInfo, interp);
    break;
  case e34_extendedServicesInfo:
    GetExtendedServicesInfo(in->u.extendedServicesInfo, interp);
    break;
  case e34_attributeDetails:
    GetAttributeDetails(in->u.attributeDetails, interp);
    break;
  case e34_termListDetails:
    GetTermListDetails(in->u.termListDetails, interp);
    break;
  case e34_elementSetDetails:
    GetElementSetDetails(in->u.elementSetDetails, interp);
    break;
  case e34_retrievalRecordDetails:
    GetRetrievalRecordDetails(in->u.retrievalRecordDetails, interp);
    break;
  case e34_sortDetails:
    GetSortDetails(in->u.sortDetails, interp);
    break;
  case e34_processing:
    GetProcessingInformation(in->u.processing, interp);
    break;
  case e34_variants:
    GetVariantSetInfo(in->u.variants, interp);
    break;
  case e34_units:
    GetUnitInfo(in->u.units, interp);
    break;
  case e34_categoryList:
    GetCategoryList(in->u.categoryList, interp);
    break;
    
  }
  
  return 0;
}


int
MakeExplainListElement(Explain_Record *explainrec, Tcl_Interp *interp)
{
  
  if (explainrec == NULL)
    return 1;
  
  GetExplain_Record(explainrec, interp);
  
  return 0;
}


