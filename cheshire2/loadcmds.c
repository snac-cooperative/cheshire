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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifndef WIN32
#include <sys/param.h>
#endif
#include "tcl.h"
#include "z3950_3.h"
#include "session.h"

/* maintains a list of active z-sessions */
ZASSOCIATION zAssociations = {NULL, NULL, 0, 0, 0, 0, 0, 0};   
 
/* current or defaul user session pointer */
ZSESSION         *TKZ_User_Session = NULL;

/* Z39.50 interface */
extern int TKZ_SelectCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_CloseCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_SearchCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_SQLSearchCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_DisplayCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_SetCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_ShowCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_FormatCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_MakeFormatCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_RemoveFormatCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_ShowFormatCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_DeleteCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_ScanCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_SortCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_HighlightCmd(ClientData, Tcl_Interp *, int, char **);

/* Additional Z39.50 V.3 interface */
extern int ConnectCmd(ClientData, Tcl_Interp *, int, char **);
extern int MConnectCmd(ClientData, Tcl_Interp *, int, char **);
extern int DisconnectCmd(ClientData, Tcl_Interp *, int, char **);
extern int SearchCmd(ClientData, Tcl_Interp *, int, char **);
extern int MSearchCmd(ClientData, Tcl_Interp *, int, char **);
extern int PresentCmd(ClientData, Tcl_Interp *, int, char **);
extern int PresentCmd2(ClientData, Tcl_Interp *, int, char **);
extern int ResourceReportCmd(ClientData, Tcl_Interp *, int, char **);
extern int TriggerResourceControlCmd(ClientData, Tcl_Interp *, int, char **);
extern int DisplayCmd(ClientData, Tcl_Interp *, int, char **);
extern int DisplayCmd2(ClientData, Tcl_Interp *, int, char **);
extern int QuitCmd(ClientData, Tcl_Interp *, int, char **);

/* Additional Non-Z39.50 commands */
extern int pTmpNam();
extern int pTranLog();
extern int sResults();
extern int pSFnorm();
#ifndef WIN32
/* there may be a windows version, but until it is demanded, forget it */
extern Tcl_ObjCmdProc *TKZ_EncryptCmd(ClientData, Tcl_Interp *, int, Tcl_Obj *);
#endif
/* direct search backend (when linked correctly) */
extern int Cheshire_CloseUp(ClientData, Tcl_Interp *, int, char **);
extern int Cheshire_Search(ClientData, Tcl_Interp *, int, char **);
extern int Cheshire_Scan(ClientData, Tcl_Interp *, int, char **);
extern int Cheshire_Fetch_Results(ClientData, Tcl_Interp *, int, char **);
extern int TileBar_Search(ClientData, Tcl_Interp *, int, char **);
extern int Cheshire_Sort(ClientData, Tcl_Interp *, int, char **);
extern int TermFreq_Search(ClientData, Tcl_Interp *, int, char **);
extern int Cheshire_Delete_Results(ClientData, Tcl_Interp *, int, char **);
extern int Cheshire_Get_Result_Names(ClientData, Tcl_Interp *, int, char **);

/* for dealing with LCC class_var trees */
extern int TKZ_LCCGetCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_LCCBuildCmd(ClientData, Tcl_Interp *, int, char **);
extern int TKZ_LCCDestroyCmd(ClientData, Tcl_Interp *, int, char **);

/* this procedure loads the user defined commands into the interpreter */
LoadCheshireCmds (interp)
Tcl_Interp *interp;
{
    Tcl_CreateCommand(interp,"Zselect", TKZ_SelectCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zfind", TKZ_SearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zretry", TKZ_SearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zql", TKZ_SQLSearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zdisplay", TKZ_DisplayCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zshow", TKZ_ShowCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zclose", TKZ_CloseCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zset", TKZ_SetCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zformat", TKZ_FormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zmakeformat", TKZ_MakeFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zremoveformat", TKZ_RemoveFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Zshowformat", TKZ_ShowFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "Zdelete", TKZ_DeleteCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "Zscan", TKZ_ScanCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "Zsort", TKZ_SortCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"ZSELECT", TKZ_SelectCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZFIND", TKZ_SearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZRETRY", TKZ_SearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZQL", TKZ_SQLSearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZDISPLAY", TKZ_DisplayCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZSHOW", TKZ_ShowCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZCLOSE", TKZ_CloseCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZSET", TKZ_SetCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZFORMAT", TKZ_FormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZMAKEFORMAT", TKZ_MakeFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZREMOVEFORMAT", TKZ_RemoveFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"ZSHOWFORMAT", TKZ_ShowFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "ZDELETE", TKZ_DeleteCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "ZSCAN", TKZ_ScanCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "ZSORT", TKZ_SortCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"zselect", TKZ_SelectCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zfind", TKZ_SearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zretry", TKZ_SearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zql", TKZ_SQLSearchCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zdisplay", TKZ_DisplayCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zshow", TKZ_ShowCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zclose", TKZ_CloseCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zset", TKZ_SetCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zformat", TKZ_FormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zmakeformat", TKZ_MakeFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zremoveformat", TKZ_RemoveFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"zshowformat", TKZ_ShowFormatCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"pTmpNam", pTmpNam, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "pTranLog", pTranLog, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "sResults", sResults, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL); 
    Tcl_CreateCommand(interp, "pSFnorm", pSFnorm,
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zdelete", TKZ_DeleteCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zDelete", TKZ_DeleteCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zscan", TKZ_ScanCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zsort", TKZ_SortCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zhighlight", TKZ_HighlightCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "cheshirehighlight", TKZ_HighlightCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "highlight", TKZ_HighlightCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);

#ifndef WIN32
    /* Try the new object versions of cmd creation */
    Tcl_CreateObjCommand (interp,"crypt", TKZ_EncryptCmd, 
			  NULL, NULL);
    Tcl_CreateObjCommand (interp,"encrypt", TKZ_EncryptCmd, 
			  NULL, NULL);
#endif
    /*
     *  New Z39.50 V.3 commands from Aitao
     */
    Tcl_CreateCommand(interp, "zConnect", ConnectCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "mConnect", MConnectCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zDisconnect", DisconnectCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zSearch", SearchCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "pSearch", MSearchCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zPresent", PresentCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zPresent2", PresentCmd2, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zResourceReport", ResourceReportCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zTRC", TriggerResourceControlCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zDisplay_new", DisplayCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zDisplay2", DisplayCmd2, 
		      (ClientData)&zAssociations, (void (*)()) NULL);
    Tcl_CreateCommand(interp, "zQuit", QuitCmd, 
		      (ClientData)&zAssociations, (void (*)()) NULL);

    /* local backend find command aliases */
    Tcl_CreateCommand(interp,"lfind", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LFIND", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"lfind", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"search", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"cheshire_search", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"sql", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"lsql", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"SQL", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LSQL", Cheshire_Search, 
		      (ClientData)NULL, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"lscan", Cheshire_Scan, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LSCAN", Cheshire_Scan, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"cheshire_scan", Cheshire_Scan, 
		      (ClientData)NULL, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"local_sort", Cheshire_Sort, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LS", Cheshire_Sort, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"cheshire_sort", Cheshire_Sort, 
		      (ClientData)NULL, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"fetch", Cheshire_Fetch_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"FETCH", Cheshire_Fetch_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"fetch_result", Cheshire_Fetch_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"fetch_results", Cheshire_Fetch_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"cheshire_fetch", Cheshire_Fetch_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Cheshire_Fetch", Cheshire_Fetch_Results, 
		      (ClientData)NULL, (void (*)()) NULL);


    Tcl_CreateCommand(interp,"delete_sets", Cheshire_Delete_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"DELETE_SETS", Cheshire_Delete_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"DEL_SETS", Cheshire_Delete_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"local_delete_sets", Cheshire_Delete_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"cheshire_delete_sets", Cheshire_Delete_Results, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Cheshire_Delete_Sets", Cheshire_Delete_Results, 
		      (ClientData)NULL, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"get_sets", Cheshire_Get_Result_Names, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"GET_SETS", Cheshire_Get_Result_Names, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"local_get_sets", Cheshire_Get_Result_Names, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"cheshire_get_sets", Cheshire_Get_Result_Names, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"Cheshire_Get_Sets", Cheshire_Get_Result_Names, 
		      (ClientData)NULL, (void (*)()) NULL);



    Tcl_CreateCommand(interp,"TileBar_Search", TileBar_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"tilebar_search", TileBar_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"tbfind", TileBar_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"tbsearch", TileBar_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"tb_search", TileBar_Search, 
		      (ClientData)NULL, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"TermFreq_Search", TermFreq_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"termfreq_search", TermFreq_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"tffind", TermFreq_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"tfsearch", TermFreq_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"tf_search", TermFreq_Search, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"page_search", TermFreq_Search, 
		      (ClientData)NULL, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"cheshire_close", Cheshire_CloseUp, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"cheshire_exit", Cheshire_CloseUp, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"close_cheshire", Cheshire_CloseUp, 
		      (ClientData)NULL, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"exit_cheshire", Cheshire_CloseUp, 
		      (ClientData)NULL, (void (*)()) NULL);

    Tcl_CreateCommand(interp,"LCCBuild", TKZ_LCCBuildCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LCCGet", TKZ_LCCGetCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LCCDestroy", TKZ_LCCDestroyCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LCCBUILD", TKZ_LCCBuildCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LCCGET", TKZ_LCCGetCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"LCCDESTROY", TKZ_LCCDestroyCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"lccbuild", TKZ_LCCBuildCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"lccget", TKZ_LCCGetCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);
    Tcl_CreateCommand(interp,"lccdestroy", TKZ_LCCDestroyCmd, 
		      (ClientData)TKZ_User_Session, (void (*)()) NULL);

}








