#!/bin/sh 
# start with WISH \
	exec wish8.3 "$0" ${1+"$@"} 
# VisualGIPSY 2.5
####VG-START####
set ::sc(dir) "[file dirname [info script]]"; 
proc srcStartUp {file type script} {
    variable ::sc;
    set src [file join $::sc(dir) $file];
    if {[file exists $src]} {
	if {[catch {uplevel "#0" [list $script $src]} result]} {
	    puts stderr  "Error loading file $src:\n$result"; 
	}
    }
}
####VG-CODE:normal####

# ---------------------------------------------------------------------------
# Template    : staffcheshire
# 
# Attributes  :

# ---------------------------------------------------------------------------
namespace eval staffcheshire {

    # Define the built-in arrays
    variable w;
    variable var;

    # Widget array for template staffcheshire
    array set w [list 286 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactdesclbl \
	35 .fdbcfg.dbcfgfr.gridwin.data.datafr.nicknamefr.dataNickEntry 287 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactdescentry \
	36 .fdbcfg.dbcfgfr.gridwin.data.datafr.dtdfr 288 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactaddrlbl \
	37 .fdbcfg.dbcfgfr.gridwin.data.datafr.dtdfr.dtdlbl 300 \
	.fdbcfg.dbtabset 290 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactemail \
	289 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactaddentry \
	38 .fdbcfg.dbcfgfr.gridwin.data.datafr.dtdfr.dataDtdEntry 291 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactemailentry \
	301 .fdbcfg.dbstatusbar 40 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.sgmlcatfr 39 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dtdfr.dataDtdbb 302 \
	.fdbcfg.menuframe.dbswitch.switchcascade 292 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactphonelbl \
	41 .fdbcfg.dbcfgfr.gridwin.data.datafr.sgmlcatfr.sgmlcatlbl 303 \
	.fdbcfg.menuframe.dbenv 293 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactphoneentry \
	42 .fdbcfg.dbcfgfr.gridwin.data.datafr.sgmlcatfr.dataSgmlCatEntry 304 \
	.fdbcfg.menuframe.dbenv.envdef 294 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactinfolbl \
	43 .fdbcfg.dbcfgfr.gridwin.data.datafr.sgmlcatfr.dataSgmlCatbb 305 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dpathfr 295 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactinfolbl \
	44 .fdbcfg.dbcfgfr.gridwin.data.datafr.sgmlsubdocfr 306 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dpathfr.dpathchck 296 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactinfolbl \
	45 .fdbcfg.dbcfgfr.gridwin.data.datafr.sgmlsubdocfr.sgmlsubdoclbl 307 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dpathfr.dpathentry 297 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.spacerfr 46 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.sgmlsubdocfr.dataSubDocEntry 298 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.dummyfr4 47 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.assocpathfr 299 \
	.fdbcfg.dbcfgfr.gridwin.explain.canvas.frame 48 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.assocpathfr.assocpathlbl 49 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.assocpathfr.dataAssocPathEntry 50 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.assocpathfr.dataAssocPathbb 51 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.historypathfr 52 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.historypathfr.hispathlbl 53 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.historypathfr.dataHisPathEntry 54 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.historypathfr.dataHisPathbb 55 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dispoptionfr 56 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dispoptionfr.dispoptlbl 57 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dispoptionfr.dispOptList 58 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.dummyfr2 60 \
	.fdbcfg.dbcfgfr.gridwin.indexes 59 \
	.fdbcfg.dbcfgfr.gridwin.data.canvas.frame 61 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr 62 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexbuttonfr 63 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexbuttonfr.indexNextButton \
	64 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexbuttonfr.indexdefheaderlbl \
	65 .fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr 66 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexNameEntry 67 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexPathEntry 68 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexStoplistEntry \
	69 .fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexAccessLbl \
	70\
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexExtractLbl 71 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexNormalLbl 72 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexPrimaryKeyLbl \
	73 .fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexNameLbl\
	74 .fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexPathLbl 75 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexPathBrowseButton \
	76 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexStoplistLbl \
	77 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexStoplistBrowseButton \
	78 .fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexMapLbl 79 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexmapCreateButton \
	80 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexPRIMARYKEYCombo \
	81 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexACCESSCombo \
	82 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexEXTRACTCombo \
	83 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexNORMALCombo \
	84 .fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexKeyspecFr \
	85 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexKeyspecFr.indexKeyspecLbl \
	86 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexKeyspecFr.indexKeyMoreButton \
	87 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexKeyspecFr.indexKeyspecEntry1 \
	0 {} 88 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexKeyspecFr.indexkeyspecdummyfr \
	1 .fdbcfg 89 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexExcludeFr 90 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexExcludeFr.indexExcLbl \
	2 .fdbcfg.menuframe 91 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexExcludeFr.indexExcMoreButton \
	3 .fdbcfg.menuframe.dbfile 92 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexExcludeFr.indexexcspecdummyfr \
	4 .fdbcfg.menuframe.dbfile.filecascade 93 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.indexdeffr.indexExcludeFr.indexExcspecEntry1 \
	5 .fdbcfg.menuframe.dbview 94 \
	.fdbcfg.dbcfgfr.gridwin.indexes.indexesfr.dummyfr 6 \
	.fdbcfg.menuframe.dbview.viewcascade 95 \
	.fdbcfg.dbcfgfr.gridwin.indexes.canvas.frame 7\
	.fdbcfg.menuframe.dbhelp 96 .fdbcfg.dbcfgfr.gridwin.clusters 8 \
	.fdbcfg.menuframe.dbhelp.helpcascade 97 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr 9 \
	.fdbcfg.menuframe.dbfiledef 98 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr 99 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterbuttonfr \
	100 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterbuttonfr.clusterNextButton \
	101 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterbuttonfr.clusterdefheaderlbl \
	102 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr \
	103 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterNameLbl \
	104 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterNameEntry \
	105 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterStoplistEntry \
	106 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterStoplistBrowseButton \
	107 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterMapLbl \
	108 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterkeyfr \
	109 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterkeyfr.clusterKeyNormalCombo \
	110 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterkeyfr.clusterKeyNormalLbl \
	111 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterkeyfr.cluskeyTagspecFr \
	112 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterkeyfr.cluskeyTagspecFr.cluskeyTagspecLbl \
	113 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterkeyfr.cluskeyTagspecFr.cluskeyTagspecEntry1 \
	114 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterKeyLbl \
	115 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterMapCreateButton \
	116 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusternamefr.clusterdeffr.clusterStoplistLbl \
	117 .fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusbasefr 118 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusbasefr.clusbaselbl 119 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusbasefr.clusbaseent 120 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusbasefr.clusbasebbut\
	121 \
	.fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusbasefr.clusbasedummyfr \
	122 .fdbcfg.dbcfgfr.gridwin.clusters.clustersfr.clusterfrdumfr 123 \
	.fdbcfg.dbcfgfr.gridwin.clusters.canvas.frame 124 \
	.fdbcfg.dbcfgfr.gridwin.display 125 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr 126 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaybuttonfr 127 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaybuttonfr.displaydefheaderlbl \
	128 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaybuttonfr.displayNextButton \
	129 .fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr 130 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatnameLbl \
	131 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatnameEntry \
	132\
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatOIDLbl 133 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr \
	134 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertallCheckbutton \
	135 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr \
	136 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncradio1 \
	137 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncradio2 \
	138 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncradio3 \
	139 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncradio4 \
	140 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncradio5 \
	141 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncextfr \
	142 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncextfr.formatfuncextLbl \
	143 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncextfr.formatfuncextEntry \
	144 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncextfr.formatfuncextButton \
	145 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatconvertfunclbl \
	146 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.formatfuncextdum \
	147 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertfunctionfr.dummy \
	148 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertclusmapButton \
	149 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertclusmapLbl \
	150 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatconvertfr.formatconvertLbl \
	151 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatdefaultCheckbutton \
	152 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatexcludeFr \
	153 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatexcludeFr.formatexcludeLbl \
	154 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatexcludeFr.formatexcludecompressCheckbutton \
	155 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatexcludeFr.formatexclTagspecEntry1 \
	156 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatexcludeFr.formatexclmoreButton \
	157 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatexcludeFr.frame2 \
	158 \
	.fdbcfg.dbcfgfr.gridwin.display.displayfr.displaydeffr.formatOIDCombo \
	159 .fdbcfg.dbcfgfr.gridwin.display.displayfr.dummyfr 160 \
	.fdbcfg.dbcfgfr.gridwin.display.canvas.frame 161 \
	.fdbcfg.dbcfgfr.gridwin.component 162 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr 163 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbuttonfr 164 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbuttonfr.compNextButton \
	165 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbuttonfr.compdefheaderlbl \
	166 .fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr 167 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr\
	168 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compnamelbl \
	169 .fdbcfg.menuframe.dbswitch 170 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compnormlbl \
	171 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compnameentry \
	172 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compindexlbl \
	173 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compstarttagEntry \
	174 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compendtagEntry \
	175 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compstarttaglbl \
	176 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compendtaglbl \
	177 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compNormCombo \
	178 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componenttopfr.componentfr.compNameBButton \
	179 .fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr 180 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexbuttonfr \
	181 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexbuttonfr.compindexdefheaderlbl \
	182 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexbuttonfr.compindexNextButton \
	183 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr \
	184 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexNameEntry \
	185 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexPathEntry \
	186 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexStoplistEntry \
	187 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexAccessLbl \
	188 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexExtractLbl \
	200 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexEXTRACTCombo \
	189 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexNormalLbl \
	190 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexPrimaryKeyLbl \
	201 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexNORMALCombo \
	191 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexNameLbl \
	202 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexKeyspecFr \
	192 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexPathLbl \
	203 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexKeyspecFr.compindexKeyspecLbl \
	193 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexPathBrowseButton \
	204 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexKeyspecFr.compindexKeyMoreButton \
	194 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexStoplistLbl \
	195 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexStoplistBrowseButton \
	205 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexKeyspecFr.compindexKeyspecEntry1 \
	206 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexKeyspecFr.compindexKeydummyfr \
	196 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexMapLbl \
	207 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexExcludeFr \
	197 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexmapCreateButton \
	208 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexExcludeFr.compindexExcLbl \
	198 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexPRIMARYKEYCombo \
	210 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexExcludeFr.compindexExcdummyfr \
	209 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexExcludeFr.compindexExcMoreButton \
	199 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexACCESSCombo \
	211 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.compindexdeffr.compindexExcludeFr.compindexExcspecEntry1 \
	212 \
	.fdbcfg.dbcfgfr.gridwin.component.compfr.componentbottomfr.bottomdummyfr \
	213 .fdbcfg.dbcfgfr.gridwin.component.canvas.frame 214 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr 215 \
	.fdbcfg.dbcfgfr.gridwin.hori 216 .fdbcfg.dbcfgfr.gridwin.vert 217 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explainchoicefr 218 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explainchoicefr.explainlbl \
	220 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explainchoicefr.explainskip \
	219 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explainchoicefr.explaindefine \
	221 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr 222 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.explainlbl 223 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.explainlclbl\
	224 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.tslbl 225 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.dslbl 226 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.disclalbl 227 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.newslbl 228 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.hourslbl 230 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.btentry 229 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.besttimelbl 231 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.btlcentry 232 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.tstringentry\
	233 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.tslcentry\
	234 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.descentry\
	235 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.desclcentry \
	236 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.disclentry \
	237 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.disclcentry \
	238 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.newsentry \
	240 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.hrsentry\
	239 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.newslcentry \
	241 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.hrslcentry \
	242 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.coverlbl\
	243 .fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.coverentry \
	244\
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.coverlcentry 245 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.cprightlbl 246 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.cprightentry\
	247 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.cprightlcentry \
	248 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.cprightnoticelbl \
	250 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.cprightnoticelcentry \
	249 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.cprightnoticeentry \
	251 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.lastupdatelbl \
	252 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.lastupdatentry \
	253 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.updateintervallbl \
	254 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.updateintervalfr \
	255 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.updateintervalfr.updateintervalnuentry \
	256 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.updateintervalfr.updateintervalunit \
	257 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.proprietarylbl \
	258 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.proprietaryfr \
	260 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.proprietaryfr.proprietarybb2 \
	259 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.proprietaryfr.proprietarybb1 \
	261 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr \
	10 .fdbcfg.menuframe.dbfiledef.filedefcas 262 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactnamelbl \
	11 .fdbcfg.menuframe.dbcontext 263 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactnameentry \
	12 .fdbcfg.menuframe.dbcontext.contextcascade 264 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactdesclbl \
	13 .fdbcfg.dbcfgfr 265 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactdescentry \
	14 .fdbcfg.dbcfgfr.gridwin 266 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactaddrlbl \
	15 .fdbcfg.dbcfgfr.gridwin.explain 267 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactaddentry \
	16 .fdbcfg.dbcfgfr.gridwin.data 268 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactemail \
	17 .fdbcfg.dbcfgfr.gridwin.data.datafr 270 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactphonelbl \
	269 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactemailentry \
	18 .fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr 271 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.producercontactfr.pcontactphoneentry \
	20 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.filepathfrup.filepathlbl \
	19 .fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.filepathfrup 272 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr \
	21 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.filepathfrup.singlebut \
	273 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactnamelbl \
	22\
	.fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.filepathfrup.multibut\
	274 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactnameentry \
	23 .fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.filepathfrlow 275 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactdesclbl \
	24 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.filepathfrlow.dataFilePathEntry \
	276 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactdescentry \
	25 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.filepathfrlow.dataFilePathbb \
	277 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactaddrlbl \
	26 .fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.contframe 278 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactaddentry \
	27 .fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.contframe.contlbl\
	280 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactemailentry \
	279 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactemail \
	28 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.contframe.dataContPathEntry \
	281 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactphonelbl \
	30 .fdbcfg.dbcfgfr.gridwin.data.datafr.filetypefr 29 \
	.fdbcfg.dbcfgfr.gridwin.data.datafr.filepathfr.contframe.dataContPathbb \
	282 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.suppliercontactfr.scontactphoneentry \
	31 .fdbcfg.dbcfgfr.gridwin.data.datafr.filetypefr.filetypelbl 283 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr \
	32 .fdbcfg.dbcfgfr.gridwin.data.datafr.filetypefr.dataFileTypeCombo\
	284 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactnamelbl \
	33 .fdbcfg.dbcfgfr.gridwin.data.datafr.nicknamefr 285 \
	.fdbcfg.dbcfgfr.gridwin.explain.explainfr.explaindeffr.submissioncontactfr.subcontactnameentry \
	34 .fdbcfg.dbcfgfr.gridwin.data.datafr.nicknamefr.nicknamelbl];

    # -----------------------------------------------------------------------
    # The template interface of staffcheshire
    array set var [list  original toplevel attributes {}];

}

# ---------------------------------------------------------------------------
# Function    : staffcheshire - template constructor
# 
# Returns     : 
# Parameters  : base - widget pathName
#               args - optional creation arguments, the option value
#                      pairs, stored in args, must corrospond to the
#                      attributes listed above the template
#                      definition.  In addition, the option -creator 
#                      is allways allowed. 
# 
# Description : This is the primary Constructor for widgets of class
#               staffcheshire. The Constructor may be used as each other
#               widget class.
# ---------------------------------------------------------------------------
proc staffcheshire {{base .staffcheshire} args} {
    variable ::staffcheshire::w;
    variable ::staffcheshire::var;

    if {![winfo exists $base]} {

	# -------------------------------------------------------------------
	# The widget hierarchy
	# -------------------------------------------------------------------
	# Look for creation class of template staffcheshire:
	[getCreator staffcheshire $base$w(0) $args] $base$w(0)  -class Staffcheshire\
		 -highlightbackground "lightgray";


	# Children of $base$w(0) := staffcheshire (level:2)
	frame $base$w(1) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(1) := fdbcfg (level:3)
	frame $base$w(2) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "raised" -width "800";

	# Children of $base$w(2) := menuframe (level:4)
	menubutton $base$w(3) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-menu "$base$w(4)" -padx "5" -pady "4";

	# Children of $base$w(3) := dbfile (level:5)
	menu $base$w(4) -activebackground "gray86"\
		-activeforeground "black" -selectcolor "yellow";

	# Items of $base$w(4) := filecascade 
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu1accelerator {Alt-o}]"\
		-command "OpenConfigFile"\
		-font "[opget $base$w(4) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu1label {Open File}]"\
		-underline "[opget $base$w(4) menu1underline {0}]";
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu2accelerator {Alt-n}]"\
		-command "NewConfigFile"\
		-font "[opget $base$w(4) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu2label {New}]"\
		-underline "[opget $base$w(4) menu2underline {0}]";
	$base$w(4) add separator;
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu4accelerator {Alt-s}]"\
		-command "SaveDbConfigFile"\
		-font "[opget $base$w(4) menu4font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu4label {Save}]"\
		-underline "[opget $base$w(4) menu4underline {0}]";
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu5accelerator {Alt-a}]"\
		-command "SaveDbConfigFileAs"\
		-font "[opget $base$w(4) menu5font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu5label {Save As}]"\
		-underline "[opget $base$w(4) menu5underline {5}]";
	$base$w(4) add separator;
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu7accelerator {Alt-x}]"\
		-command "FinishDbcfg"\
		-font "[opget $base$w(4) menu7font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu7label {Exit}]"\
		-underline "[opget $base$w(4) menu7underline {1}]";

	menubutton $base$w(5) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-menu "$base$w(6)" -padx "5" -pady "4";

	# Children of $base$w(5) := dbview (level:5)
	menu $base$w(6) -activebackground "gray86"\
		-activeforeground "black" -selectcolor "yellow";

	# Items of $base$w(6) := viewcascade 
	$base$w(6) add command\
		-accelerator "[opget $base$w(6) menu1accelerator {Alt-c}]"\
		-command "ViewDTD"\
		-font "[opget $base$w(6) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(6) menu1label {Config File DTD}]"\
		-underline "[opget $base$w(6) menu1underline {0}]";
	$base$w(6) add command\
		-accelerator "[opget $base$w(6) menu2accelerator {Alt-o}]"\
		-command "ViewFile"\
		-font "[opget $base$w(6) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(6) menu2label {Config File}]"\
		-underline "[opget $base$w(6) menu2underline {1}]";

	menubutton $base$w(9) -activebackground "gray86"\
		-highlightbackground "lightgray" -menu "$base$w(10)" -padx "5"\
		-pady "4";

	# Children of $base$w(9) := dbfiledef (level:5)
	menu $base$w(10);

	# Items of $base$w(10) := filedefcas 
	$base$w(10) add command\
		-accelerator "[opget $base$w(10) menu1accelerator {Alt-n}]"\
		-command "ChangeFileDef next"\
		-font "[opget $base$w(10) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(10) menu1label {Next}]"\
		-underline "[opget $base$w(10) menu1underline {0}]";
	$base$w(10) add command\
		-accelerator "[opget $base$w(10) menu2accelerator {Alt-p}]"\
		-command "ChangeFileDef prev"\
		-font "[opget $base$w(10) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(10) menu2label {Previous}]"\
		-underline "[opget $base$w(10) menu2underline {0}]";

	menubutton $base$w(11) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-menu "$base$w(12)" -padx "5" -pady "4";

	# Children of $base$w(11) := dbcontext (level:5)
	menu $base$w(12) -activebackground "gray86"\
		-activeforeground "black" -selectcolor "yellow";

	# Items of $base$w(12) := contextcascade 
	$base$w(12) add command\
		-accelerator "[opget $base$w(12) menu1accelerator {Alt-c}]"\
		-command "ShowContext"\
		-font "[opget $base$w(12) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(12) menu1label {Config File Structure Tree}]"\
		-underline "[opget $base$w(12) menu1underline {0}]";

	menubutton $base$w(169) -activebackground "gray86"\
		-menu "$base$w(302)" -padx "5" -pady "3";

	# Children of $base$w(169) := dbswitch (level:5)
	menu $base$w(302);

	# Items of $base$w(302) := switchcascade 
	$base$w(302) add command\
		-accelerator "[opget $base$w(302) menu1accelerator {}]"\
		-command "DbToServer"\
		-font "[opget $base$w(302) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(302) menu1label {Server Config}]"\
		-underline "[opget $base$w(302) menu1underline {-1}]";
	$base$w(302) add command\
		-accelerator "[opget $base$w(302) menu2accelerator {}]"\
		-command "DbToCmd build"\
		-font "[opget $base$w(302) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(302) menu2label {buildassoc}]"\
		-underline "[opget $base$w(302) menu2underline {-1}]";
	$base$w(302) add command\
		-accelerator "[opget $base$w(302) menu3accelerator {}]"\
		-command "DbToCmd indexche"\
		-font "[opget $base$w(302) menu3font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(302) menu3label {index_cheshire}]"\
		-underline "[opget $base$w(302) menu3underline {-1}]";
	$base$w(302) add command\
		-accelerator "[opget $base$w(302) menu4accelerator {}]"\
		-command "DbToCmd indexclu"\
		-font "[opget $base$w(302) menu4font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(302) menu4label {index_clusters}]"\
		-underline "[opget $base$w(302) menu4underline {-1}]";

	menubutton $base$w(7) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-menu "$base$w(8)" -padx "5" -pady "4";

	# Children of $base$w(7) := dbhelp (level:5)
	menu $base$w(8) -activebackground "gray86"\
		-activeforeground "black" -selectcolor "yellow";

	# Items of $base$w(8) := helpcascade 
	$base$w(8) add command\
		-accelerator "[opget $base$w(8) menu1accelerator {}]"\
		-command "showHref config.html"\
		-font "[opget $base$w(8) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(8) menu1label {About this tool}]"\
		-underline "[opget $base$w(8) menu1underline {-1}]";
	$base$w(8) add command\
		-accelerator "[opget $base$w(8) menu2accelerator {}]"\
		-font "[opget $base$w(8) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(8) menu2label {Readme}]"\
		-underline "[opget $base$w(8) menu2underline {-1}]";

	menubutton $base$w(303) -activebackground "gray86"\
		-menu "$base$w(304)" -padx "4" -pady "3";

	# Children of $base$w(303) := dbenv (level:5)
	menu $base$w(304) -activebackground "gray86";

	# Items of $base$w(304) := envdef 
	$base$w(304) add checkbutton -variable "dbcfg(Defineenv)"\
		-accelerator "[opget $base$w(304) menu1accelerator {}]"\
		-command "DefineDBEnv $base$w(304)"\
		-font "[opget $base$w(304) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(304) menu1label {Define DBEnv}]"\
		-underline "[opget $base$w(304) menu1underline {-1}]";


	pack $base$w(3) -ipadx 5 -side left;
	pack $base$w(5) -ipadx 5 -side left;
	pack $base$w(9) -ipadx 5 -side left;
	pack $base$w(11) -ipadx 5 -side left;
	pack $base$w(169) -ipadx 5 -side left;
	pack $base$w(7) -ipadx 5 -side right;
	pack $base$w(303) -side left;
	rtl_tabset $base$w(300) -autovisible "1" -borderspace "10"\
		-height "60" -highlightbackground "lightgray" -offset "0"\
		-orient "horizontal" -relief "raised"\
		-show "data explain indexes clusters component display"\
		-tabbackground "lightgray" -tabfont "Helvetica 11 bold"\
		-tabforeground "black" -taboffset "50" -tabwidth "100"\
		-type "rtl_tabItem";

	# Items of $base$w(300) := dbtabset 
	$base$w(300) add data \
		-foreground "[opget $base$w(300) dataforeground {cornsilk}]"\
		-width "105" -onvalue "1"\
		-text "[opget $base$w(300) datatext {DATA      }]"\
		-command "ShowDataFrame"\
		-font "[opget $base$w(300) datafont {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*}]"\
		-background "[opget $base$w(300) databackground {lightblue}]"\
		-state "normal";
	$base$w(300) add explain \
		-foreground "[opget $base$w(300) explainforeground {cornsilk}]"\
		-width "105" -onvalue "1"\
		-text "[opget $base$w(300) explaintext {EXPLAIN   }]"\
		-command "ShowExplainFrame"\
		-font "[opget $base$w(300) explainfont {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*}]"\
		-background "[opget $base$w(300) explainbackground {lightblue}]"\
		-state "normal";
	$base$w(300) add indexes \
		-foreground "[opget $base$w(300) indexesforeground {cornsilk}]"\
		-width "105" -onvalue "1"\
		-text "[opget $base$w(300) indexestext {INDEX     }]"\
		-command "ShowIndexFrame"\
		-font "[opget $base$w(300) indexesfont {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*}]"\
		-background "[opget $base$w(300) indexesbackground {lightblue}]"\
		-state "normal";
	$base$w(300) add clusters \
		-foreground "[opget $base$w(300) clustersforeground {cornsilk}]"\
		-width "105" -onvalue "1"\
		-text "[opget $base$w(300) clusterstext {CLUSTER   }]"\
		-command "ShowClusterFrame"\
		-font "[opget $base$w(300) clustersfont {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*}]"\
		-background "[opget $base$w(300) clustersbackground {lightblue}]"\
		-state "normal";
	$base$w(300) add component \
		-foreground "[opget $base$w(300) componentforeground {cornsilk}]"\
		-width "105" -onvalue "1"\
		-text "[opget $base$w(300) componenttext {COMPONENT}]"\
		-command "ShowComponentFrame"\
		-font "[opget $base$w(300) componentfont {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*}]"\
		-background "[opget $base$w(300) componentbackground {lightblue}]"\
		-state "normal";
	$base$w(300) add display \
		-foreground "[opget $base$w(300) displayforeground {cornsilk}]"\
		-width "105" -onvalue "1"\
		-text "[opget $base$w(300) displaytext {DISPLAY   }]"\
		-command "ShowDisplayFrame"\
		-font "[opget $base$w(300) displayfont {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*}]"\
		-background "[opget $base$w(300) displaybackground {lightblue}]"\
		-state "normal";
	frame $base$w(13) -borderwidth "1" -height "40"\
		-highlightbackground "lightgray" -relief "groove" -width "40";

	# Children of $base$w(13) := dbcfgfr (level:4)
	rtl_gridwin $base$w(14) -activebackground "lightgray" -auto "1"\
		-fill "both" -height "600" -highlightbackground "lightgray"\
		-sides "bottom right" -widget "$base$w(124)" -width "750";
	rtl_scrollwin $base$w(15) -fill "none" -height "500"\
		-widget "$base$w(214)" -width "700";
	frame $base$w(214) -height "1200"\
		-highlightbackground "lightGray" -width "800";

	# Events for $base$w(214) := explainfr
	bindtags $base$w(214) [list $base$w(214) _staffcheshire_fdbcfg_dbcfgfr_gridwin_explain _staffcheshire_tabset_fdbcfg_dbcfgfr_gridwin_explain _staffcheshire_tabset_fdbcfg_dbcfgfr_explain_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_expln_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_gridwin_scrollwin _toplevel1_mainframe_gridwin_scrollwin _toplevel1_fdemo_gridwin_scrollwin _rtlDemo_tabtour_fscrollwin_fdemo_gridwin_scrollwin Frame all];


	# Children of $base$w(214) := explainfr (level:7)
	frame $base$w(217) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(217) := explainchoicefr (level:8)
	label $base$w(218) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	radiobutton $base$w(219) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -value "define" -variable "";
	radiobutton $base$w(220) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -value "skip" -variable "";

	pack $base$w(218) -padx 20 -pady 20 -side left;
	pack $base$w(219) -side left;
	pack $base$w(220) -side left;
	frame $base$w(221) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(221) := explaindeffr (level:8)
	label $base$w(222) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightGray" -relief "ridge";
	label $base$w(223) -anchor "e" -borderwidth "0"\
		-highlightbackground "lightGray" -relief "ridge";
	label $base$w(224) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightGray";
	label $base$w(225) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightGray";
	label $base$w(226) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightGray";
	label $base$w(227) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(228) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(229) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(232) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "50";
	entry $base$w(233) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(234) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "50";
	entry $base$w(235) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(236) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "50";
	entry $base$w(237) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(238) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	entry $base$w(239) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(240) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	entry $base$w(241) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	label $base$w(242) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(245) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(248) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(251) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(253) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(257) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(294) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray" -justify "left";
	label $base$w(295) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray" -justify "left";
	label $base$w(296) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray" -justify "left";
	frame $base$w(297) -height "20"\
		-highlightbackground "lightgray" -width "40";
	entry $base$w(230) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "50";
	entry $base$w(231) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(243) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	entry $base$w(244) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(246) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	entry $base$w(247) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(249) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	entry $base$w(250) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "4";
	entry $base$w(252) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	frame $base$w(254) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(254) := updateintervalfr (level:9)
	entry $base$w(255) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "6";
	rtl_combobox $base$w(256) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(256)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "5" -histlength "-1" -histposition "below"\
		-histwidth "0" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(256)"\
		-selectforeground "black" -width "10";

	pack $base$w(255) -padx 20 -side left;
	pack $base$w(256) -padx 20 -side left;
	frame $base$w(258) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(258) := proprietaryfr (level:9)
	radiobutton $base$w(259) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -value "YES" -variable "";
	radiobutton $base$w(260) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -value "NO" -variable "";

	pack $base$w(259) -padx 15 -side left;
	pack $base$w(260) -padx 15 -side left;
	frame $base$w(261) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(261) := producercontactfr (level:9)
	label $base$w(262) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(263) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(264) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(265) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(266) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(267) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(268) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(269) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(270) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(271) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";

	grid $base$w(262) -column 0 -row 0 -padx 20 -sticky nesw;
	grid $base$w(263) -column 1 -row 0 -padx 20 -sticky nesw;
	grid $base$w(264) -column 0 -row 1 -padx 20 -sticky nesw;
	grid $base$w(265) -column 1 -row 1 -padx 20 -sticky nesw;
	grid $base$w(266) -column 0 -row 2 -padx 20 -sticky nesw;
	grid $base$w(267) -column 1 -row 2 -padx 20 -sticky nesw;
	grid $base$w(268) -column 0 -row 3 -padx 20 -sticky nesw;
	grid $base$w(269) -column 1 -row 3 -padx 20 -sticky nesw;
	grid $base$w(270) -column 0 -row 4 -padx 20 -sticky nesw;
	grid $base$w(271) -column 1 -row 4 -padx 20 -sticky nesw;

	# Grid (size:2 5) $base$w(261) = producercontactfr
	frame $base$w(272) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(272) := suppliercontactfr (level:9)
	label $base$w(273) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(274) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(275) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(276) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(277) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(278) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(279) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(280) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(281) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(282) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";

	grid $base$w(273) -column 0 -row 0 -padx 20 -sticky nesw;
	grid $base$w(274) -column 1 -row 0 -padx 20 -sticky nesw;
	grid $base$w(275) -column 0 -row 1 -padx 20 -sticky nesw;
	grid $base$w(276) -column 1 -row 1 -padx 20 -sticky nesw;
	grid $base$w(277) -column 0 -row 2 -padx 20 -sticky nesw;
	grid $base$w(278) -column 1 -row 2 -padx 20 -sticky nesw;
	grid $base$w(279) -column 0 -row 3 -padx 20 -sticky nesw;
	grid $base$w(280) -column 1 -row 3 -padx 20 -sticky nesw;
	grid $base$w(281) -column 0 -row 4 -padx 20 -sticky nesw;
	grid $base$w(282) -column 1 -row 4 -padx 20 -sticky nesw;

	# Grid (size:2 5) $base$w(272) = suppliercontactfr
	frame $base$w(283) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(283) := submissioncontactfr (level:9)
	label $base$w(284) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(285) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(286) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(287) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(288) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(289) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(290) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(291) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";
	label $base$w(292) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(293) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "45";

	grid $base$w(284) -column 0 -row 0 -padx 20 -sticky nesw;
	grid $base$w(285) -column 1 -row 0 -padx 20 -sticky nesw;
	grid $base$w(286) -column 0 -row 1 -padx 20 -sticky nesw;
	grid $base$w(287) -column 1 -row 1 -padx 20 -sticky nesw;
	grid $base$w(288) -column 0 -row 2 -padx 20 -sticky nesw;
	grid $base$w(289) -column 1 -row 2 -padx 20 -sticky nesw;
	grid $base$w(290) -column 0 -row 3 -padx 20 -sticky nesw;
	grid $base$w(291) -column 1 -row 3 -padx 20 -sticky nesw;
	grid $base$w(292) -column 0 -row 4 -padx 20 -sticky nesw;
	grid $base$w(293) -column 1 -row 4 -padx 20 -sticky nesw;

	# Grid (size:2 5) $base$w(283) = submissioncontactfr

	grid $base$w(222) -column 0 -row 0 -columnspan 2 -pady 20 -sticky w;
	grid $base$w(223) -column 2 -row 0 -pady 20 -sticky e;
	grid $base$w(224) -column 0 -row 1 -pady 2 -sticky nesw;
	grid $base$w(225) -column 0 -row 2 -pady 2 -sticky nesw;
	grid $base$w(226) -column 0 -row 3 -pady 2 -sticky nesw;
	grid $base$w(227) -column 0 -row 4 -pady 2 -sticky nesw;
	grid $base$w(228) -column 0 -row 5 -pady 2 -sticky nesw;
	grid $base$w(229) -column 0 -row 6 -pady 2 -sticky nesw;
	grid $base$w(232) -column 1 -row 1 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(233) -column 2 -row 1 -padx 20 -sticky w;
	grid $base$w(234) -column 1 -row 2 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(235) -column 2 -row 2 -padx 20 -sticky w;
	grid $base$w(236) -column 1 -row 3 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(237) -column 2 -row 3 -padx 20 -sticky w;
	grid $base$w(238) -column 1 -row 4 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(239) -column 2 -row 4 -padx 20 -sticky w;
	grid $base$w(240) -column 1 -row 5 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(241) -column 2 -row 5 -padx 20 -sticky w;
	grid $base$w(242) -column 0 -row 7 -pady 2 -sticky nesw;
	grid $base$w(245) -column 0 -row 8 -pady 2 -sticky nesw;
	grid $base$w(248) -column 0 -row 9 -pady 2 -sticky nesw;
	grid $base$w(251) -column 0 -row 10 -pady 2 -sticky nesw;
	grid $base$w(253) -column 0 -row 11 -pady 2 -sticky nesw;
	grid $base$w(257) -column 0 -row 12 -pady 2 -sticky nesw;
	grid $base$w(294) -column 0 -row 14 -columnspan 3 -sticky nesw;
	grid $base$w(295) -column 0 -row 16 -columnspan 3 -sticky nesw;
	grid $base$w(296) -column 0 -row 18 -columnspan 3 -sticky nesw;
	grid $base$w(297) -column 0 -row 13 -columnspan 3 -sticky nesw;
	grid $base$w(230) -column 1 -row 6 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(231) -column 2 -row 6 -padx 20 -sticky w;
	grid $base$w(243) -column 1 -row 7 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(244) -column 2 -row 7 -padx 20 -sticky w;
	grid $base$w(246) -column 1 -row 8 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(247) -column 2 -row 8 -padx 20 -sticky w;
	grid $base$w(249) -column 1 -row 9 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(250) -column 2 -row 9 -padx 20 -sticky w;
	grid $base$w(252) -column 1 -row 10 -padx 20 -pady 2 -sticky nesw;
	grid $base$w(254) -column 1 -row 11 -sticky nesw;
	grid $base$w(258) -column 1 -row 12 -sticky nesw;
	grid $base$w(261) -column 0 -row 15 -columnspan 3 -pady 10 -sticky nesw;
	grid $base$w(272) -column 0 -row 17 -columnspan 3 -pady 10 -sticky nesw;
	grid $base$w(283) -column 0 -row 19 -columnspan 3 -pady 10 -sticky nesw;

	# Grid (size:3 20) $base$w(221) = explaindeffr
	frame $base$w(298) -height "200" -width "40";

	pack $base$w(217) -fill x;
	pack $base$w(221) -anchor n -padx 30 -side left;
	pack $base$w(298) -expand 1 -fill x;
	pack $base$w(214) -in $base$w(299) -expand 1 -fill both -padx 20;
	rtl_scrollwin $base$w(60) -fill "none" -height "500"\
		-widget "$base$w(61)" -width "700";
	frame $base$w(61) -height "1000"\
		-highlightbackground "lightGray" -width "800";

	# Events for $base$w(61) := indexesfr
	bindtags $base$w(61) [list $base$w(61) _staffcheshire_fdbcfg_dbcfgfr_gridwin_indexes _staffcheshire_tabset_fdbcfg_dbcfgfr_gridwin_indexes _staffcheshire_tabset_fdbcfg_dbcfgfr_indexes_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_index_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_datainfo_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_gridwin_scrollwin _toplevel1_mainframe_gridwin_scrollwin _toplevel1_fdemo_gridwin_scrollwin _rtlDemo_tabtour_fscrollwin_fdemo_gridwin_scrollwin Frame all];


	# Children of $base$w(61) := indexesfr (level:7)
	frame $base$w(62) -borderwidth "2" -height "40"\
		-relief "ridge" -width "40";

	# Children of $base$w(62) := indexbuttonfr (level:8)
	button $base$w(63) -command "ChangeIndexDef";
	label $base$w(64)\
		-textvariable "appGlobal(indexdefheader)";

	grid $base$w(63) -column 2 -row 0 -padx 10 -sticky w;
	grid $base$w(64) -column 1 -row 0 -sticky nesw;

	# Grid (size:3 1) $base$w(62) = indexbuttonfr
	frame $base$w(65) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(65) := indexdeffr (level:8)
	label $base$w(69) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(70) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(71) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(72) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(73) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(74) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(76) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(78) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	rtl_combobox $base$w(81) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(81)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "4" -histlength "-1" -histposition "below"\
		-histwidth "20" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(81)"\
		-selectforeground "black";
	rtl_combobox $base$w(82) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(82)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "15" -histlength "-1" -histposition "below"\
		-histwidth "35" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(82)"\
		-selectforeground "black";
	rtl_combobox $base$w(83) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(83)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "5" -histlength "-1" -histposition "below"\
		-histwidth "0" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(83)"\
		-selectforeground "black";
	rtl_combobox $base$w(80) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(80)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "4" -histlength "-1" -histposition "below"\
		-histwidth "0" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(80)"\
		-selectforeground "black";
	entry $base$w(66) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	entry $base$w(67) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	button $base$w(75) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(75)"\
		-highlightbackground "lightgray" -padx "1";
	entry $base$w(68) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	button $base$w(77) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(77)"\
		-highlightbackground "lightgray" -padx "1";
	button $base$w(79) -activebackground "gray86"\
		-activeforeground "black" -command "IdxMapCreateEntries"\
		-highlightbackground "lightgray";
	frame $base$w(84) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(84) := indexKeyspecFr (level:9)
	label $base$w(85) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	button $base$w(86) -command "MoreIndexKeySpec" -padx "1";
	entry $base$w(87) -width "75";
	frame $base$w(88) -height "10" -width "25";

	grid $base$w(85) -column 0 -row 0 -sticky w;
	grid $base$w(86) -column 0 -row 0 -padx 10 -sticky e;
	grid $base$w(87) -column 0 -row 1 -padx 10 -pady 5 -sticky ew;
	grid $base$w(88) -column 1 -row 0 -rowspan 2 -sticky nesw;

	# Grid (size:2 2) $base$w(84) = indexKeyspecFr
	frame $base$w(89) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(89) := indexExcludeFr (level:9)
	label $base$w(90) -borderwidth "0";
	button $base$w(91) -command "MoreIndexExcSpec" -padx "1";
	frame $base$w(92) -height "10" -width "25";
	entry $base$w(93) -width "75";

	grid $base$w(90) -column 0 -row 0 -sticky w;
	grid $base$w(91) -column 0 -row 0 -padx 10 -sticky e;
	grid $base$w(92) -column 1 -row 0 -rowspan 2 -sticky nesw;
	grid $base$w(93) -column 0 -row 1 -padx 10 -pady 5 -sticky ew;

	# Grid (size:2 2) $base$w(89) = indexExcludeFr

	grid $base$w(69) -column 0 -row 0 -padx 25 -pady 7 -sticky w;
	grid $base$w(70) -column 0 -row 1 -padx 25 -pady 7 -sticky w;
	grid $base$w(71) -column 0 -row 2 -padx 25 -pady 7 -sticky w;
	grid $base$w(72) -column 0 -row 3 -padx 25 -pady 7 -sticky w;
	grid $base$w(73) -column 0 -row 4 -padx 25 -pady 7 -sticky w;
	grid $base$w(74) -column 0 -row 5 -padx 25 -pady 10 -sticky w;
	grid $base$w(76) -column 0 -row 7 -padx 25 -pady 10 -sticky w;
	grid $base$w(78) -column 0 -row 11 -padx 25 -pady 20 -sticky w;
	grid $base$w(81) -column 1 -row 0 -padx 20 -pady 7 -sticky ew;
	grid $base$w(82) -column 1 -row 1 -padx 20 -pady 7 -sticky ew;
	grid $base$w(83) -column 1 -row 2 -padx 20 -pady 7 -sticky ew;
	grid $base$w(80) -column 1 -row 3 -padx 20 -pady 7 -sticky ew;
	grid $base$w(66) -column 1 -row 4 -padx 20 -pady 7 -sticky ew;
	grid $base$w(67) -column 0 -row 6 -columnspan 2 -padx 25 -pady 7\
		 -sticky nesw;
	grid $base$w(75) -column 2 -row 6 -sticky w;
	grid $base$w(68) -column 0 -row 8 -columnspan 2 -padx 25 -pady 7\
		 -sticky nesw;
	grid $base$w(77) -column 2 -row 8 -sticky w;
	grid $base$w(79) -column 0 -row 11 -sticky e;
	grid $base$w(84) -column 0 -row 12 -columnspan 3 -padx 25 -pady 10\
		 -sticky w;
	grid $base$w(89) -column 0 -row 13 -columnspan 3 -padx 25 -pady 10\
		 -sticky w;

	# Grid (size:3 14) $base$w(65) = indexdeffr
	frame $base$w(94) -height "100" -width "40";

	pack $base$w(62) -expand 1 -fill x;
	pack $base$w(65) -expand 1 -fill both -pady 10;
	pack $base$w(94) -expand 1 -fill x;
	pack $base$w(61) -in $base$w(95) -expand 1 -fill both -padx 20;
	rtl_scrollwin $base$w(96) -fill "none" -height "500"\
		-widget "$base$w(97)" -width "700";
	frame $base$w(97) -height "40"\
		-highlightbackground "lightGray" -width "800";

	# Events for $base$w(97) := clustersfr
	bindtags $base$w(97) [list $base$w(97) _staffcheshire_fdbcfg_dbcfgfr_gridwin_clusters _staffcheshire_tabset_fdbcfg_dbcfgfr_gridwin_clusters _staffcheshire_tabset_fdbcfg_dbcfgfr_clusters_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_cluster_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_index_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_datainfo_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_gridwin_scrollwin _toplevel1_mainframe_gridwin_scrollwin _toplevel1_fdemo_gridwin_scrollwin _rtlDemo_tabtour_fscrollwin_fdemo_gridwin_scrollwin Frame all];


	# Children of $base$w(97) := clustersfr (level:7)
	frame $base$w(98) -height "40" -width "40";

	# Children of $base$w(98) := clusternamefr (level:8)
	frame $base$w(99) -borderwidth "2" -height "40"\
		-relief "ridge" -width "40";

	# Children of $base$w(99) := clusterbuttonfr (level:9)
	button $base$w(100) -command "ChangeClusterDef";
	label $base$w(101)\
		-textvariable "appGlobal(clusterdefheader)";

	grid $base$w(100) -column 2 -row 0 -padx 10 -sticky nesw;
	grid $base$w(101) -column 1 -row 0 -sticky nesw;

	# Grid (size:3 1) $base$w(99) = clusterbuttonfr
	frame $base$w(102) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(102) := clusterdeffr (level:9)
	label $base$w(103) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(107) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(114) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(116) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(104) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	frame $base$w(108) -borderwidth "1" -height "40"\
		-highlightbackground "lightgray" -relief "raised" -width "40";

	# Children of $base$w(108) := clusterkeyfr (level:10)
	rtl_combobox $base$w(109) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(109)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "5" -histlength "-1" -histposition "below"\
		-histwidth "0" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(109)"\
		-selectforeground "black";
	label $base$w(110) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	frame $base$w(111) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(111) := cluskeyTagspecFr (level:11)
	label $base$w(112);
	entry $base$w(113) -width "60";

	grid $base$w(112) -column 0 -row 0 -padx 30 -sticky w;
	grid $base$w(113) -column 0 -row 1 -columnspan 2 -padx 20 -pady 5\
		 -sticky nesw;

	# Grid (size:2 2) $base$w(111) = cluskeyTagspecFr

	grid $base$w(109) -column 1 -row 0 -padx 30 -pady 5 -sticky nesw;
	grid $base$w(110) -column 0 -row 0 -padx 30 -pady 5 -sticky w;
	grid $base$w(111) -column 0 -row 1 -columnspan 2 -pady 20 -sticky nesw;

	# Grid (size:2 2) $base$w(108) = clusterkeyfr
	entry $base$w(105) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "40";
	button $base$w(106) -activebackground "gray86"\
		-activeforeground "black"\
		-command "HandleBBSelect $base$w(106)"\
		-highlightbackground "lightgray" -padx "1";
	button $base$w(115) -activebackground "gray86"\
		-activeforeground "black"\
		-command "ClusMapCreateEntries cluster"\
		-highlightbackground "lightgray";

	grid $base$w(103) -column 0 -row 0 -padx 30 -pady 10 -sticky w;
	grid $base$w(107) -column 0 -row 5 -padx 30 -pady 20 -sticky w;
	grid $base$w(114) -column 0 -row 1 -padx 30 -pady 10 -sticky w;
	grid $base$w(116) -column 0 -row 3 -columnspan 2 -padx 30 -pady 10\
		 -sticky w;
	grid $base$w(104) -column 1 -row 0 -columnspan 2 -pady 10 -sticky w;
	grid $base$w(108) -column 0 -row 2 -columnspan 2 -padx 20 -pady 5\
		 -sticky nesw;
	grid $base$w(105) -column 0 -row 4 -columnspan 2 -padx 20 -sticky ew;
	grid $base$w(106) -column 2 -row 4 -sticky e;
	grid $base$w(115) -column 1 -row 5 -padx 40 -pady 20 -sticky w;

	# Grid (size:3 6) $base$w(102) = clusterdeffr

	pack $base$w(99) -expand 1 -fill x;
	pack $base$w(102) -expand 1 -fill both -padx 40 -pady 10;
	frame $base$w(117) -height "300"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(117) := clusbasefr (level:8)
	label $base$w(118) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(119) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "50";
	button $base$w(120) -command "HandleBBSelect $base$w(120)";
	frame $base$w(121) -height "40" -width "80";

	grid $base$w(118) -column 0 -row 0 -padx 30 -sticky w;
	grid $base$w(119) -column 0 -row 1 -padx 30 -pady 10 -sticky ew;
	grid $base$w(120) -column 1 -row 1 -pady 10 -sticky w;
	grid $base$w(121) -column 2 -row 0 -sticky nesw;

	# Grid (size:3 2) $base$w(117) = clusbasefr
	frame $base$w(122) -height "100" -width "40";

	grid $base$w(98) -column 0 -row 0;
	grid $base$w(117) -column 0 -row 0;
	grid $base$w(122) -column 0 -row 1 -sticky nesw;

	# Grid (size:1 2) $base$w(97) = clustersfr
	pack $base$w(97) -in $base$w(123) -expand 1 -fill both -padx 20;
	rtl_scrollwin $base$w(16) -fill "none" -height "500"\
		-widget "$base$w(17)" -width "700";
	frame $base$w(17) -height "1000"\
		-highlightbackground "lightGray" -width "800";

	# Events for $base$w(17) := datafr
	bindtags $base$w(17) [list $base$w(17) _staffcheshire_fdbcfg_dbcfgfr_gridwin_data _staffcheshire_tabset_fdbcfg_dbcfgfr_gridwin_data _staffcheshire_tabset_fdbcfg_dbcfgfr_data_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_datainfo_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_gridwin_scrollwin _toplevel1_mainframe_gridwin_scrollwin _toplevel1_fdemo_gridwin_scrollwin _rtlDemo_tabtour_fscrollwin_fdemo_gridwin_scrollwin Frame all];


	# Children of $base$w(17) := datafr (level:7)
	frame $base$w(30) -height "40"\
		-highlightbackground "lightgray" -width "800";

	# Children of $base$w(30) := filetypefr (level:8)
	label $base$w(31) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	rtl_combobox $base$w(32) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(32)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "8" -histlength "-1" -histposition "below"\
		-histwidth "25" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(32)"\
		-selectforeground "black" -width "25";

	pack $base$w(31) -anchor w -side left;
	pack $base$w(32) -anchor w -padx 30 -side left;
	frame $base$w(18) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(18) := filepathfr (level:8)
	frame $base$w(19) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(19) := filepathfrup (level:9)
	label $base$w(20) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	radiobutton $base$w(21) -activebackground "gray86"\
		-activeforeground "black" -command "HideContFrame"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "single" -variable "";
	radiobutton $base$w(22) -activebackground "gray86"\
		-activeforeground "black" -command "ShowContFrame"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "multiple" -variable "";

	pack $base$w(20) -anchor w -side left;
	pack $base$w(21) -side left;
	pack $base$w(22) -side left;
	frame $base$w(23) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(23) := filepathfrlow (level:9)
	entry $base$w(24) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "60";
	button $base$w(25) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(25)"\
		-highlightbackground "lightgray" -padx "1";

	pack $base$w(24) -padx 15 -pady 5 -side left;
	pack $base$w(25) -anchor w;
	frame $base$w(26) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(26) := contframe (level:9)
	label $base$w(27) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(28) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "60";
	button $base$w(29) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(29)"\
		-highlightbackground "lightgray" -padx "1";

	pack $base$w(27) -anchor w -padx 15;
	pack $base$w(28) -padx 15 -pady 5 -side left;
	pack $base$w(29) -anchor w;

	pack $base$w(19) -fill x;
	pack $base$w(23) -fill x;
	pack $base$w(26) -anchor n -fill x -pady 5;
	frame $base$w(33) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(33) := nicknamefr (level:8)
	label $base$w(34) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(35) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "15";

	pack $base$w(34) -side left;
	pack $base$w(35) -padx 30 -side left;
	frame $base$w(36) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(36) := dtdfr (level:8)
	label $base$w(37) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(38) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "60";
	button $base$w(39) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(39)"\
		-highlightbackground "lightgray" -padx "1";

	pack $base$w(37) -anchor w;
	pack $base$w(38) -padx 15 -pady 5 -side left;
	pack $base$w(39) -anchor w;
	frame $base$w(40) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(40) := sgmlcatfr (level:8)
	label $base$w(41) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(42) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "60";
	button $base$w(43) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(43)"\
		-highlightbackground "lightgray" -padx "1";

	pack $base$w(41) -anchor w;
	pack $base$w(42) -padx 15 -pady 5 -side left;
	pack $base$w(43) -anchor w;
	frame $base$w(44) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(44) := sgmlsubdocfr (level:8)
	label $base$w(45) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray" -width "100";
	entry $base$w(46) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "40";

	pack $base$w(45) -anchor w;
	pack $base$w(46) -padx 15 -pady 5 -side left;
	frame $base$w(47) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(47) := assocpathfr (level:8)
	label $base$w(48) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(49) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "60";
	button $base$w(50) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(50)"\
		-highlightbackground "lightgray" -padx "1";

	pack $base$w(48) -anchor w;
	pack $base$w(49) -padx 15 -pady 5 -side left;
	pack $base$w(50) -anchor w;
	frame $base$w(51) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(51) := historypathfr (level:8)
	label $base$w(52) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(53) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "60";
	button $base$w(54) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(54)"\
		-highlightbackground "lightgray" -padx "1";

	pack $base$w(52) -anchor w;
	pack $base$w(53) -padx 15 -pady 5 -side left;
	pack $base$w(54) -anchor w -pady 5;
	frame $base$w(55) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(55) := dispoptionfr (level:8)
	label $base$w(56) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	listbox $base$w(57) -height "4"\
		-highlightbackground "lightgray" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-selectmode "multiple" -setgrid "1";

	pack $base$w(56) -anchor n -padx 10 -side left;
	pack $base$w(57) -padx 10 -side left;
	frame $base$w(58) -height "100" -width "40";
	frame $base$w(305) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(305) := dpathfr (level:8)
	checkbutton $base$w(306) -activebackground "gray86"\
		-command "CheckDefaultPath $base$w(306)"\
		-highlightthickness "0" -variable "dbcfg(dpathchck_var)";
	entry $base$w(307) -highlightbackground "lightgray"\
		-width "50";

	pack $base$w(306) -anchor w -side left;
	pack $base$w(307) -anchor w -padx 30;

	grid $base$w(30) -column 0 -row 1 -padx 30 -pady 15 -sticky ew;
	grid $base$w(18) -column 0 -row 2 -padx 30 -pady 15 -sticky ew;
	grid $base$w(33) -column 0 -row 3 -padx 30 -pady 15 -sticky nesw;
	grid $base$w(36) -column 0 -row 4 -padx 30 -pady 15 -sticky ew;
	grid $base$w(40) -column 0 -row 5 -padx 30 -pady 15 -sticky ew;
	grid $base$w(44) -column 0 -row 6 -padx 30 -pady 15 -sticky ew;
	grid $base$w(47) -column 0 -row 7 -padx 30 -pady 15 -sticky ew;
	grid $base$w(51) -column 0 -row 8 -padx 30 -pady 15 -sticky ew;
	grid $base$w(55) -column 0 -row 9 -padx 30 -pady 15 -sticky ew;
	grid $base$w(58) -column 0 -row 10 -sticky nesw;
	grid $base$w(305) -column 0 -row 0 -padx 30 -pady 15 -sticky ew;

	# Grid (size:1 11) $base$w(17) = datafr
	pack $base$w(17) -in $base$w(59) -expand 1 -fill both -padx 20;
	rtl_scrollwin $base$w(161) -fill "none" -height "360"\
		-widget "$base$w(162)" -width "700";
	frame $base$w(162) -height "40" -width "40";

	# Events for $base$w(162) := compfr
	bindtags $base$w(162) [list $base$w(162) _staffcheshire_fdbcfg_dbcfgfr_gridwin_component _staffcheshire_tabset_fdbcfg_dbcfgfr_gridwin_component Frame . all];


	# Children of $base$w(162) := compfr (level:7)
	frame $base$w(163) -borderwidth "2" -height "40"\
		-relief "ridge" -width "40";

	# Children of $base$w(163) := componentbuttonfr (level:8)
	button $base$w(164) -command "ChangeComponentDef";
	label $base$w(165) -textvariable "appGlobal(compdefheader)";

	grid $base$w(164) -column 2 -row 0 -padx 10 -sticky w;
	grid $base$w(165) -column 1 -row 0 -sticky nesw;

	# Grid (size:3 1) $base$w(163) = componentbuttonfr
	frame $base$w(166) -height "40" -width "40";

	# Children of $base$w(166) := componenttopfr (level:8)
	frame $base$w(167) -height "40" -width "40";

	# Children of $base$w(167) := componentfr (level:9)
	label $base$w(168) -highlightbackground "lightgray";
	label $base$w(170);
	label $base$w(172);
	label $base$w(175);
	label $base$w(176);
	button $base$w(178) -command "HandleBBSelect $base$w(178)"\
		-padx "1";
	entry $base$w(171) -width "40";
	rtl_combobox $base$w(177) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(177)"\
		-highlightthickness "1" -histheight "5" -histlength "-1"\
		-histposition "below" -histwidth "0" -listtype "listbox"\
		-relief "sunken"\
		-selectcommand "HandleComboSelect $base$w(177)";
	entry $base$w(173) -width "40";
	entry $base$w(174) -width "40";

	grid $base$w(168) -column 0 -row 0 -padx 15 -pady 10 -sticky w;
	grid $base$w(170) -column 0 -row 1 -padx 15 -pady 10 -sticky w;
	grid $base$w(172) -column 0 -row 4 -padx 15 -pady 10 -sticky w;
	grid $base$w(175) -column 0 -row 2 -padx 15 -pady 10 -sticky w;
	grid $base$w(176) -column 0 -row 3 -padx 15 -pady 10 -sticky w;
	grid $base$w(178) -column 3 -row 0 -sticky w;
	grid $base$w(171) -column 1 -row 0 -columnspan 2 -padx 10 -pady 10\
		 -sticky w;
	grid $base$w(177) -column 1 -row 1 -padx 10 -pady 10 -sticky w;
	grid $base$w(173) -column 1 -row 2 -columnspan 2 -padx 10 -pady 10\
		 -sticky ew;
	grid $base$w(174) -column 1 -row 3 -columnspan 2 -padx 10 -pady 10\
		 -sticky nesw;

	# Grid (size:4 5) $base$w(167) = componentfr

	pack $base$w(167) -expand 1 -fill x -pady 10;
	frame $base$w(179) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(179) := componentbottomfr (level:8)
	frame $base$w(180) -height "40" -width "40";

	# Children of $base$w(180) := compindexbuttonfr (level:9)
	label $base$w(181)\
		-textvariable "appGlobal(compindexdefheader)";
	button $base$w(182)\
		-command "ChangeIndexDef next . InsideComponent";

	grid $base$w(181) -column 1 -row 0 -pady 5;
	grid $base$w(182) -column 2 -row 0 -padx 10 -pady 5;

	# Grid (size:3 1) $base$w(180) = compindexbuttonfr
	frame $base$w(183) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(183) := compindexdeffr (level:9)
	label $base$w(187) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(188) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(189) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(190) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(191) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(192) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(194) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(196) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	rtl_combobox $base$w(199) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(199)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "4" -histlength "-1" -histposition "below"\
		-histwidth "20" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(199)"\
		-selectforeground "black";
	rtl_combobox $base$w(200) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(200)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "15" -histlength "-1" -histposition "below"\
		-histwidth "35" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(200)"\
		-selectforeground "black";
	rtl_combobox $base$w(201) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(201)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "5" -histlength "-1" -histposition "below"\
		-histwidth "0" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(201)"\
		-selectforeground "black";
	rtl_combobox $base$w(198) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(198)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "4" -histlength "-1" -histposition "below"\
		-histwidth "0" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(198)"\
		-selectforeground "black";
	entry $base$w(184) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	entry $base$w(185) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	button $base$w(193) -activebackground "gray86"\
		-activeforeground "black"\
		-command "HandleBBSelect $base$w(193)"\
		-highlightbackground "lightgray" -padx "1";
	entry $base$w(186) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	button $base$w(195) -activebackground "gray86"\
		-activeforeground "black"\
		-command "HandleBBSelect $base$w(195)"\
		-highlightbackground "lightgray" -padx "1";
	button $base$w(197) -activebackground "gray86"\
		-activeforeground "black"\
		-command "IdxMapCreateEntries InsideComponent"\
		-highlightbackground "lightgray" -padx "1";
	frame $base$w(202) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(202) := compindexKeyspecFr (level:10)
	label $base$w(203) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	button $base$w(204)\
		-command "MoreIndexKeySpec InsideComponent" -padx "1";
	entry $base$w(205) -width "75";
	frame $base$w(206) -height "20" -width "20";

	grid $base$w(203) -column 0 -row 0 -sticky w;
	grid $base$w(204) -column 0 -row 0 -padx 10 -sticky e;
	grid $base$w(205) -column 0 -row 1 -padx 10 -pady 5 -sticky ew;
	grid $base$w(206) -column 1 -row 0 -rowspan 2 -sticky nesw;

	# Grid (size:2 2) $base$w(202) = compindexKeyspecFr
	frame $base$w(207) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(207) := compindexExcludeFr (level:10)
	label $base$w(208) -borderwidth "0";
	button $base$w(209)\
		-command "MoreIndexExcSpec InsideComponent" -padx "1";
	frame $base$w(210) -height "20" -width "20";
	entry $base$w(211) -width "75";

	grid $base$w(208) -column 0 -row 0 -sticky w;
	grid $base$w(209) -column 0 -row 0 -padx 10 -sticky e;
	grid $base$w(210) -column 1 -row 0 -rowspan 2 -sticky nesw;
	grid $base$w(211) -column 0 -row 1 -padx 10 -pady 5 -sticky ew;

	# Grid (size:2 2) $base$w(207) = compindexExcludeFr

	grid $base$w(187) -column 0 -row 0 -padx 25 -pady 7 -sticky w;
	grid $base$w(188) -column 0 -row 1 -padx 25 -pady 7 -sticky w;
	grid $base$w(189) -column 0 -row 2 -padx 25 -pady 7 -sticky w;
	grid $base$w(190) -column 0 -row 3 -padx 25 -pady 7 -sticky w;
	grid $base$w(191) -column 0 -row 4 -padx 25 -pady 7 -sticky w;
	grid $base$w(192) -column 0 -row 5 -padx 25 -pady 10 -sticky w;
	grid $base$w(194) -column 0 -row 7 -padx 25 -pady 10 -sticky w;
	grid $base$w(196) -column 0 -row 11 -padx 25 -pady 20 -sticky w;
	grid $base$w(199) -column 1 -row 0 -padx 20 -pady 7 -sticky ew;
	grid $base$w(200) -column 1 -row 1 -padx 20 -pady 7 -sticky ew;
	grid $base$w(201) -column 1 -row 2 -padx 20 -pady 7 -sticky ew;
	grid $base$w(198) -column 1 -row 3 -padx 20 -pady 7 -sticky nesw;
	grid $base$w(184) -column 1 -row 4 -padx 20 -pady 7 -sticky ew;
	grid $base$w(185) -column 0 -row 6 -columnspan 2 -padx 25 -sticky ew;
	grid $base$w(193) -column 2 -row 6 -sticky w;
	grid $base$w(186) -column 0 -row 8 -columnspan 2 -padx 30 -sticky ew;
	grid $base$w(195) -column 2 -row 8 -sticky w;
	grid $base$w(197) -column 0 -row 11 -pady 20 -sticky e;
	grid $base$w(202) -column 0 -row 12 -columnspan 3 -padx 25 -pady 10\
		 -sticky w;
	grid $base$w(207) -column 0 -row 13 -columnspan 3 -padx 25 -pady 10\
		 -sticky w;

	# Grid (size:3 14) $base$w(183) = compindexdeffr
	frame $base$w(212) -height "40" -width "40";

	pack $base$w(180) -expand 1 -fill x;
	pack $base$w(183);
	pack $base$w(212) -expand 1 -fill x;

	pack $base$w(163) -expand 1 -fill x;
	pack $base$w(166) -anchor n -expand 1 -fill both;
	pack $base$w(179) -anchor n -expand 1 -fill both;
	pack $base$w(162) -in $base$w(213) -expand 1 -fill both -padx 20;
	rtl_scrollwin $base$w(124) -fill "none" -height "500"\
		-widget "$base$w(125)" -width "700"\
		-xscrollcommand "rslider $base$w(124) $base$w(215) x 1; $base$w(215) set"\
		-yscrollcommand "rslider $base$w(124) $base$w(216) y 1; $base$w(216) set";
	frame $base$w(125) -height "800"\
		-highlightbackground "lightGray" -width "800";

	# Events for $base$w(125) := displayfr
	bindtags $base$w(125) [list $base$w(125) _staffcheshire_fdbcfg_dbcfgfr_gridwin_display _staffcheshire_tabset_fdbcfg_dbcfgfr_gridwin_display _staffcheshire_tabset_fdbcfg_dbcfgfr_display_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_display_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_cluster_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_index_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_datainfo_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_gridwin_scrollwin _toplevel1_mainframe_gridwin_scrollwin _toplevel1_fdemo_gridwin_scrollwin _rtlDemo_tabtour_fscrollwin_fdemo_gridwin_scrollwin Frame all];


	# Children of $base$w(125) := displayfr (level:7)
	frame $base$w(126) -borderwidth "2" -height "40"\
		-relief "ridge" -width "40";

	# Children of $base$w(126) := displaybuttonfr (level:8)
	label $base$w(127)\
		-textvariable "appGlobal(displaydefheader)";
	button $base$w(128) -command "ChangeDisplayDef";

	grid $base$w(127) -column 1 -row 0 -sticky nesw;
	grid $base$w(128) -column 2 -row 0 -padx 10 -sticky nesw;

	# Grid (size:3 1) $base$w(126) = displaybuttonfr
	frame $base$w(129) -height "40"\
		-highlightbackground "lightgray" -width "800";

	# Children of $base$w(129) := displaydeffr (level:8)
	label $base$w(130) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(132) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	checkbutton $base$w(151) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "formatdefaultCheckbutton";
	entry $base$w(131) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black";
	rtl_combobox $base$w(158) -borderwidth "1"\
		-clientbackground "gray95"\
		-command "HandleComboSelect $base$w(158)"\
		-highlightbackground "lightgray" -highlightthickness "1"\
		-histheight "9" -histlength "-1" -histposition "below"\
		-histwidth "0" -listtype "listbox" -relief "sunken"\
		-selectbackground "lightblue"\
		-selectcommand "HandleComboSelect $base$w(158)";
	frame $base$w(133) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "groove" -width "40";

	# Children of $base$w(133) := formatconvertfr (level:9)
	checkbutton $base$w(134) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "checkbutton3";
	frame $base$w(135) -borderwidth "1" -height "40"\
		-highlightbackground "lightgray" -relief "raised" -width "40";

	# Children of $base$w(135) := formatconvertfunctionfr (level:10)
	radiobutton $base$w(136) -activebackground "gray86"\
		-activeforeground "black"\
		-command "ShowHideExtFuncFr $base$w(136)"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "PAGE_PATH" -variable "";
	radiobutton $base$w(137) -activebackground "gray86"\
		-activeforeground "black"\
		-command "ShowHideExtFuncFr $base$w(137)"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "RECMAP" -variable "";
	radiobutton $base$w(138) -activebackground "gray86"\
		-activeforeground "black"\
		-command "ShowHideExtFuncFr $base$w(138)"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "TAGSET-M" -variable "";
	radiobutton $base$w(139) -activebackground "gray86"\
		-activeforeground "black"\
		-command "ShowHideExtFuncFr $base$w(139)"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "TAGSET-G" -variable "";
	radiobutton $base$w(140) -activebackground "gray86"\
		-activeforeground "black"\
		-command "ShowHideExtFuncFr $base$w(140)"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-variable "";
	frame $base$w(141) -height "28"\
		-highlightbackground "lightgray" -width "538";

	# Children of $base$w(141) := formatfuncextfr (level:11)
	label $base$w(142) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(143) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "30";
	button $base$w(144) -activebackground "gray86"\
		-activeforeground "black"\
		-command "HandleBBSelect $base$w(144)"\
		-highlightbackground "lightgray";

	pack $base$w(142) -padx 30 -side left;
	pack $base$w(143) -expand 1 -fill x -padx 30 -side left;
	pack $base$w(144) -side left;
	label $base$w(145) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	frame $base$w(146) -height "28"\
		-highlightbackground "lightgray" -width "538";
	frame $base$w(147) -height "25" -width "40";

	grid $base$w(136) -column 0 -row 1 -padx 30 -sticky w;
	grid $base$w(137) -column 1 -row 1 -padx 30 -sticky w;
	grid $base$w(138) -column 0 -row 2 -padx 30 -sticky w;
	grid $base$w(139) -column 1 -row 2 -padx 30 -sticky w;
	grid $base$w(140) -column 0 -row 3 -padx 30 -sticky w;
	grid $base$w(141) -column 0 -row 4 -columnspan 3 -sticky nesw;
	grid propagate $base$w(141) 0;
	grid $base$w(145) -column 0 -row 0 -padx 20 -sticky w;
	grid $base$w(146) -column 0 -row 4 -columnspan 3 -sticky nesw;
	grid propagate $base$w(146) 0;
	grid $base$w(147) -column 2 -row 2 -sticky nesw;

	# Grid (size:3 5) $base$w(135) = formatconvertfunctionfr
	foreach {id opt param} [list  0 minsize 22] {
	    grid rowconfigure $base$w(135) $id -$opt $param;
	}
	button $base$w(148) -activebackground "gray86"\
		-activeforeground "black"\
		-command "ClusMapCreateEntries display"\
		-highlightbackground "lightgray";
	label $base$w(149) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(150) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray" -justify "left";

	grid $base$w(134) -column 1 -row 0 -padx 50 -pady 5 -sticky w;
	grid $base$w(135) -column 0 -row 1 -columnspan 3 -padx 20 -sticky nesw;
	grid $base$w(148) -column 1 -row 2 -padx 20 -pady 10 -sticky w;
	grid $base$w(149) -column 0 -row 2 -padx 40 -pady 10 -sticky w;
	grid $base$w(150) -column 0 -row 0 -padx 15 -pady 5 -sticky w;

	# Grid (size:3 3) $base$w(133) = formatconvertfr
	frame $base$w(152) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(152) := formatexcludeFr (level:9)
	label $base$w(153) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	checkbutton $base$w(154) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "checkbutton2";
	button $base$w(156) -command "MoreFormatExcludeTagSpec"\
		-padx "3";
	entry $base$w(155) -highlightbackground "lightgray"\
		-width "60";
	frame $base$w(157) -height "20" -width "20";

	grid $base$w(153) -column 0 -row 0 -pady 10 -sticky w;
	grid $base$w(154) -column 0 -row 0 -padx 200 -pady 10 -sticky w;
	grid $base$w(156) -column 0 -row 0 -padx 20 -sticky e;
	grid $base$w(155) -column 0 -row 1 -padx 20 -pady 5 -sticky nesw;
	grid $base$w(157) -column 1 -row 0 -rowspan 2 -sticky nesw;

	# Grid (size:2 2) $base$w(152) = formatexcludeFr

	grid $base$w(130) -column 0 -row 1 -padx 40 -pady 10 -sticky w;
	grid $base$w(132) -column 0 -row 2 -padx 40 -pady 10 -sticky w;
	grid $base$w(151) -column 0 -row 0 -padx 40 -pady 10 -sticky w;
	grid $base$w(131) -column 1 -row 1 -padx 40 -pady 10 -sticky ew;
	grid $base$w(158) -column 1 -row 2 -padx 40 -pady 10 -sticky ew;
	grid $base$w(133) -column 0 -row 3 -columnspan 3 -padx 40 -pady 20\
		 -sticky nesw;
	grid $base$w(152) -column 0 -row 4 -columnspan 3 -padx 40 -pady 10\
		 -sticky nesw;

	# Grid (size:3 5) $base$w(129) = displaydeffr
	frame $base$w(159) -height "100" -width "40";

	pack $base$w(126) -expand 1 -fill x;
	pack $base$w(129) -expand 1 -fill both -padx 20;
	pack $base$w(159) -expand 1 -fill x;
	pack $base$w(125) -in $base$w(160) -expand 1 -fill both -padx 20;
	grid $base$w(124) -column 1 -row 1 -sticky nesw;

	pack $base$w(14) -expand 1 -fill both;
	pack propagate $base$w(14) 0;
	rtl_statusbar $base$w(301) -height "30" -labelwidth "1";

	pack $base$w(2) -expand 1 -fill x;
	pack $base$w(300) -anchor n -expand 1 -fill x;
	pack propagate $base$w(300) 0;
	pack $base$w(13) -anchor n -expand 1 -fill both;
	pack $base$w(301) -expand 1 -fill x;

	# Grid (size:2 3) $base$w(1) = fdbcfg
	foreach {id opt param} [list  0 minsize 80 1 weight 1 1 minsize 690] {
	    grid columnconfigure $base$w(1) $id -$opt $param;
	}
	foreach {id opt param} [list  1 weight 1 1 minsize 550 2 minsize 1] {
	    grid rowconfigure $base$w(1) $id -$opt $param;
	}

	pack $base$w(1) -anchor n -expand 1 -fill both;

	# -------------------------------------------------------------------
	# Call the init method
	if {[catch {
	    ::staffcheshire::init $base;
	} msg]} {
	    # init does not exist.
	}

	# Additional Code
	
	$base$w(15) update;
	$base$w(60) update;
	$base$w(96) update;
	$base$w(16) update;
	$base$w(161) update;
	$base$w(124) update;
	$base$w(14) update;

	# -------------------------------------------------------------------
	# Use the template interface for this instance
	set base [mkProc $base staffcheshire $args];
    }

    return $base;
}


# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet!
# ---------------------------------------------------------------------------


namespace eval staffcheshire {
 
    # option database items to template staffcheshire

    option add *Staffcheshire$w(0).background {lightgray};
    option add *Staffcheshire$w(1).background {lightgray};
    option add *Staffcheshire$w(2).background {lightgray};
    option add *Staffcheshire$w(3).background {lightgray};
    option add *Staffcheshire$w(3).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(3).foreground {black};
    option add *Staffcheshire$w(3).text {File};
    option add *Staffcheshire$w(3).underline {0};
    option add *Staffcheshire$w(4).background {lightgray};
    option add *Staffcheshire$w(4).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(4).foreground {black};
    foreach {opt param} [list  menu1accelerator {Alt-o} menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {Open File} menu1underline {0} menu2accelerator {Alt-n}\
	    menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}\
	    menu2label {New} menu2underline {0} menu4accelerator {Alt-s}\
	    menu4font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}\
	    menu4label {Save} menu4underline {0} menu5accelerator\
	    {Alt-a} menu5font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu5label\
	    {Save As} menu5underline {5} menu7accelerator {Alt-x}\
	    menu7font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}\
	    menu7label {Exit} menu7underline {1}] {
 
	option add *Staffcheshire$w(4).$opt $param; 
    }

    option add *Staffcheshire$w(5).background {lightgray};
    option add *Staffcheshire$w(5).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(5).foreground {black};
    option add *Staffcheshire$w(5).text {View};
    option add *Staffcheshire$w(5).underline {0};
    option add *Staffcheshire$w(6).background {lightgray};
    option add *Staffcheshire$w(6).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(6).foreground {black};
    foreach {opt param} [list  menu1accelerator {Alt-c} menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {Config File DTD} menu1underline {0} menu2accelerator\
	    {Alt-o} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {Config File} menu2underline {1}] {
 
	option add *Staffcheshire$w(6).$opt $param; 
    }

    option add *Staffcheshire$w(9).background {lightgray};
    option add *Staffcheshire$w(9).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(9).foreground {#000000000000};
    option add *Staffcheshire$w(9).text {Filedef};
    option add *Staffcheshire$w(9).underline {4};
    option add *Staffcheshire$w(10).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    foreach {opt param} [list  menu1accelerator {Alt-n} menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {Next} menu1underline {0} menu2accelerator {Alt-p} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {Previous} menu2underline {0}] {
 
	option add *Staffcheshire$w(10).$opt $param; 
    }

    option add *Staffcheshire$w(11).background {lightgray};
    option add *Staffcheshire$w(11).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(11).foreground {black};
    option add *Staffcheshire$w(11).text {TreeView};
    option add *Staffcheshire$w(11).underline {0};
    option add *Staffcheshire$w(12).background {lightgray};
    option add *Staffcheshire$w(12).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(12).foreground {black};
    foreach {opt param} [list  menu1accelerator {Alt-c} menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {Config File Structure Tree} menu1underline {0}] {
 
	option add *Staffcheshire$w(12).$opt $param; 
    }

    option add *Staffcheshire$w(169).background {lightgray};
    option add *Staffcheshire$w(169).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(169).text {SwitchTask};
    option add *Staffcheshire$w(169).underline {0};
    foreach {opt param} [list  menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {Server Config} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {buildassoc} menu3font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu3label\
	    {index_cheshire} menu4font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu4label\
	    {index_clusters}] {
 
	option add *Staffcheshire$w(302).$opt $param; 
    }

    option add *Staffcheshire$w(7).background {lightgray};
    option add *Staffcheshire$w(7).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(7).foreground {black};
    option add *Staffcheshire$w(7).text {Help};
    option add *Staffcheshire$w(7).underline {0};
    option add *Staffcheshire$w(8).background {lightgray};
    option add *Staffcheshire$w(8).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(8).foreground {black};
    foreach {opt param} [list  menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {About this tool} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {Readme}] {
 
	option add *Staffcheshire$w(8).$opt $param; 
    }

    option add *Staffcheshire$w(303).background {lightgray};
    option add *Staffcheshire$w(303).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(303).text {DBenv};
    option add *Staffcheshire$w(303).underline {3};
    option add *Staffcheshire$w(304).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    foreach {opt param} [list  menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {Define DBEnv}] {
 
	option add *Staffcheshire$w(304).$opt $param; 
    }

    option add *Staffcheshire$w(300).background {lightgray};
    option add *Staffcheshire$w(13).background {lightgray};
    option add *Staffcheshire$w(14).background {lightgray};
    option add *Staffcheshire$w(15).background {lightgray};
    option add *Staffcheshire$w(214).background {lightgray};
    option add *Staffcheshire$w(217).background {lightgray};
    option add *Staffcheshire$w(218).background {lightgray};
    option add *Staffcheshire$w(218).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(218).foreground {black};
    option add *Staffcheshire$w(218).text {Explain Definitions (optional):};
    option add *Staffcheshire$w(219).background {lightgray};
    option add *Staffcheshire$w(219).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(219).foreground {black};
    option add *Staffcheshire$w(219).text {Define};
    option add *Staffcheshire$w(220).background {lightgray};
    option add *Staffcheshire$w(220).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(220).foreground {black};
    option add *Staffcheshire$w(220).text {Skip};
    option add *Staffcheshire$w(221).background {lightgray};
    option add *Staffcheshire$w(222).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(222).foreground {black};
    option add *Staffcheshire$w(222).text {Explain Information};
    option add *Staffcheshire$w(223).background {lightgray};
    option add *Staffcheshire$w(223).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(223).foreground {black};
    option add *Staffcheshire$w(223).text {Language Code};
    option add *Staffcheshire$w(224).background {lightgray};
    option add *Staffcheshire$w(224).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(224).foreground {black};
    option add *Staffcheshire$w(224).text {Title String};
    option add *Staffcheshire$w(225).background {lightgray};
    option add *Staffcheshire$w(225).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(225).foreground {black};
    option add *Staffcheshire$w(225).text {Description};
    option add *Staffcheshire$w(226).background {lightgray};
    option add *Staffcheshire$w(226).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(226).foreground {black};
    option add *Staffcheshire$w(226).text {Disclaimer};
    option add *Staffcheshire$w(227).background {lightgray};
    option add *Staffcheshire$w(227).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(227).foreground {black};
    option add *Staffcheshire$w(227).text {News};
    option add *Staffcheshire$w(228).background {lightgray};
    option add *Staffcheshire$w(228).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(228).foreground {black};
    option add *Staffcheshire$w(228).text {Hours};
    option add *Staffcheshire$w(229).background {lightgray};
    option add *Staffcheshire$w(229).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(229).foreground {black};
    option add *Staffcheshire$w(229).text {Best Time};
    option add *Staffcheshire$w(232).background {gray95};
    option add *Staffcheshire$w(232).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(232).foreground {black};
    option add *Staffcheshire$w(233).background {gray95};
    option add *Staffcheshire$w(233).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(233).foreground {black};
    option add *Staffcheshire$w(234).background {gray95};
    option add *Staffcheshire$w(234).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(234).foreground {black};
    option add *Staffcheshire$w(235).background {gray95};
    option add *Staffcheshire$w(235).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(235).foreground {black};
    option add *Staffcheshire$w(236).background {gray95};
    option add *Staffcheshire$w(236).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(236).foreground {black};
    option add *Staffcheshire$w(237).background {gray95};
    option add *Staffcheshire$w(237).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(237).foreground {black};
    option add *Staffcheshire$w(238).background {gray95};
    option add *Staffcheshire$w(238).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(238).foreground {black};
    option add *Staffcheshire$w(239).background {gray95};
    option add *Staffcheshire$w(239).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(239).foreground {black};
    option add *Staffcheshire$w(240).background {gray95};
    option add *Staffcheshire$w(240).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(240).foreground {black};
    option add *Staffcheshire$w(241).background {gray95};
    option add *Staffcheshire$w(241).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(241).foreground {black};
    option add *Staffcheshire$w(242).background {lightgray};
    option add *Staffcheshire$w(242).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(242).foreground {black};
    option add *Staffcheshire$w(242).text {Coverage};
    option add *Staffcheshire$w(245).background {lightgray};
    option add *Staffcheshire$w(245).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(245).foreground {black};
    option add *Staffcheshire$w(245).text {Copyright Text};
    option add *Staffcheshire$w(248).background {lightgray};
    option add *Staffcheshire$w(248).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(248).foreground {black};
    option add *Staffcheshire$w(248).text {Copyright Notice};
    option add *Staffcheshire$w(251).background {lightgray};
    option add *Staffcheshire$w(251).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(251).foreground {black};
    option add *Staffcheshire$w(251).text {Last Update};
    option add *Staffcheshire$w(253).background {lightgray};
    option add *Staffcheshire$w(253).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(253).foreground {black};
    option add *Staffcheshire$w(253).text {Update Interval};
    option add *Staffcheshire$w(257).background {lightgray};
    option add *Staffcheshire$w(257).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(257).foreground {black};
    option add *Staffcheshire$w(257).text {Proprietary?};
    option add *Staffcheshire$w(294).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(294).foreground {black};
    option add *Staffcheshire$w(294).text {Producer Contact Information};
    option add *Staffcheshire$w(295).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(295).foreground {black};
    option add *Staffcheshire$w(295).text {Supplier Contact Information};
    option add *Staffcheshire$w(296).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(296).foreground {black};
    option add *Staffcheshire$w(296).text {Submission Contact Information};
    option add *Staffcheshire$w(297).background {lightgray};
    option add *Staffcheshire$w(230).background {gray95};
    option add *Staffcheshire$w(230).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(230).foreground {black};
    option add *Staffcheshire$w(231).background {gray95};
    option add *Staffcheshire$w(231).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(231).foreground {black};
    option add *Staffcheshire$w(243).background {gray95};
    option add *Staffcheshire$w(243).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(243).foreground {black};
    option add *Staffcheshire$w(244).background {gray95};
    option add *Staffcheshire$w(244).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(244).foreground {black};
    option add *Staffcheshire$w(246).background {gray95};
    option add *Staffcheshire$w(246).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(246).foreground {black};
    option add *Staffcheshire$w(247).background {gray95};
    option add *Staffcheshire$w(247).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(247).foreground {black};
    option add *Staffcheshire$w(249).background {gray95};
    option add *Staffcheshire$w(249).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(249).foreground {black};
    option add *Staffcheshire$w(250).background {gray95};
    option add *Staffcheshire$w(250).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(250).foreground {black};
    option add *Staffcheshire$w(252).background {gray95};
    option add *Staffcheshire$w(252).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(252).foreground {black};
    option add *Staffcheshire$w(254).background {lightgray};
    option add *Staffcheshire$w(255).background {gray95};
    option add *Staffcheshire$w(255).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(255).foreground {black};
    option add *Staffcheshire$w(256).background {lightgray};
    option add *Staffcheshire$w(256).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(256).foreground {black};
    option add *Staffcheshire$w(258).background {lightgray};
    option add *Staffcheshire$w(259).background {lightgray};
    option add *Staffcheshire$w(259).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(259).foreground {black};
    option add *Staffcheshire$w(259).text {Yes};
    option add *Staffcheshire$w(260).background {lightgray};
    option add *Staffcheshire$w(260).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(260).foreground {black};
    option add *Staffcheshire$w(260).text {No};
    option add *Staffcheshire$w(261).background {lightgray};
    option add *Staffcheshire$w(262).background {lightgray};
    option add *Staffcheshire$w(262).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(262).foreground {black};
    option add *Staffcheshire$w(262).text {Contact Name};
    option add *Staffcheshire$w(263).background {gray95};
    option add *Staffcheshire$w(263).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(263).foreground {black};
    option add *Staffcheshire$w(264).background {lightgray};
    option add *Staffcheshire$w(264).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(264).foreground {black};
    option add *Staffcheshire$w(264).text {Contact Description};
    option add *Staffcheshire$w(265).background {gray95};
    option add *Staffcheshire$w(265).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(265).foreground {black};
    option add *Staffcheshire$w(266).background {lightgray};
    option add *Staffcheshire$w(266).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(266).foreground {black};
    option add *Staffcheshire$w(266).text {Contact Address};
    option add *Staffcheshire$w(267).background {gray95};
    option add *Staffcheshire$w(267).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(267).foreground {black};
    option add *Staffcheshire$w(268).background {lightgray};
    option add *Staffcheshire$w(268).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(268).foreground {black};
    option add *Staffcheshire$w(268).text {Contact Email};
    option add *Staffcheshire$w(269).background {gray95};
    option add *Staffcheshire$w(269).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(269).foreground {black};
    option add *Staffcheshire$w(270).background {lightgray};
    option add *Staffcheshire$w(270).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(270).foreground {black};
    option add *Staffcheshire$w(270).text {Contact Phone};
    option add *Staffcheshire$w(271).background {gray95};
    option add *Staffcheshire$w(271).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(271).foreground {black};
    option add *Staffcheshire$w(272).background {lightgray};
    option add *Staffcheshire$w(273).background {lightgray};
    option add *Staffcheshire$w(273).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(273).foreground {black};
    option add *Staffcheshire$w(273).text {Contact Name};
    option add *Staffcheshire$w(274).background {gray95};
    option add *Staffcheshire$w(274).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(274).foreground {black};
    option add *Staffcheshire$w(275).background {lightgray};
    option add *Staffcheshire$w(275).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(275).foreground {black};
    option add *Staffcheshire$w(275).text {Contact Description};
    option add *Staffcheshire$w(276).background {gray95};
    option add *Staffcheshire$w(276).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(276).foreground {black};
    option add *Staffcheshire$w(277).background {lightgray};
    option add *Staffcheshire$w(277).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(277).foreground {black};
    option add *Staffcheshire$w(277).text {Contact Address};
    option add *Staffcheshire$w(278).background {gray95};
    option add *Staffcheshire$w(278).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(278).foreground {black};
    option add *Staffcheshire$w(279).background {lightgray};
    option add *Staffcheshire$w(279).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(279).foreground {black};
    option add *Staffcheshire$w(279).text {Contact Email};
    option add *Staffcheshire$w(280).background {gray95};
    option add *Staffcheshire$w(280).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(280).foreground {black};
    option add *Staffcheshire$w(281).background {lightgray};
    option add *Staffcheshire$w(281).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(281).foreground {black};
    option add *Staffcheshire$w(281).text {Contact Phone};
    option add *Staffcheshire$w(282).background {gray95};
    option add *Staffcheshire$w(282).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(282).foreground {black};
    option add *Staffcheshire$w(283).background {lightgray};
    option add *Staffcheshire$w(284).background {lightgray};
    option add *Staffcheshire$w(284).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(284).foreground {black};
    option add *Staffcheshire$w(284).text {Contact Name};
    option add *Staffcheshire$w(285).background {gray95};
    option add *Staffcheshire$w(285).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(285).foreground {black};
    option add *Staffcheshire$w(286).background {lightgray};
    option add *Staffcheshire$w(286).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(286).foreground {black};
    option add *Staffcheshire$w(286).text {Contact Description};
    option add *Staffcheshire$w(287).background {gray95};
    option add *Staffcheshire$w(287).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(287).foreground {black};
    option add *Staffcheshire$w(288).background {lightgray};
    option add *Staffcheshire$w(288).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(288).foreground {black};
    option add *Staffcheshire$w(288).text {Contact Address};
    option add *Staffcheshire$w(289).background {gray95};
    option add *Staffcheshire$w(289).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(289).foreground {black};
    option add *Staffcheshire$w(290).background {lightgray};
    option add *Staffcheshire$w(290).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(290).foreground {black};
    option add *Staffcheshire$w(290).text {Contact Email};
    option add *Staffcheshire$w(291).background {gray95};
    option add *Staffcheshire$w(291).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(291).foreground {black};
    option add *Staffcheshire$w(292).background {lightgray};
    option add *Staffcheshire$w(292).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(292).foreground {black};
    option add *Staffcheshire$w(292).text {Contact Phone};
    option add *Staffcheshire$w(293).background {gray95};
    option add *Staffcheshire$w(293).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(293).foreground {black};
    option add *Staffcheshire$w(298).background {lightgray};
    option add *Staffcheshire$w(60).background {lightgray};
    option add *Staffcheshire$w(61).background {lightgray};
    option add *Staffcheshire$w(62).background {lightgray};
    option add *Staffcheshire$w(63).background {lightgray};
    option add *Staffcheshire$w(63).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(63).foreground {blue};
    option add *Staffcheshire$w(63).text {Next};
    option add *Staffcheshire$w(64).background {cornsilk};
    option add *Staffcheshire$w(64).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(64).foreground {blue};
    option add *Staffcheshire$w(64).text {Index Definition 1};
    option add *Staffcheshire$w(65).background {lightgray};
    option add *Staffcheshire$w(69).background {lightgray};
    option add *Staffcheshire$w(69).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(69).foreground {black};
    option add *Staffcheshire$w(69).text {1. Access type:};
    option add *Staffcheshire$w(70).background {lightgray};
    option add *Staffcheshire$w(70).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(70).foreground {black};
    option add *Staffcheshire$w(70).text {2. Extract key type:};
    option add *Staffcheshire$w(71).background {lightgray};
    option add *Staffcheshire$w(71).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(71).foreground {black};
    option add *Staffcheshire$w(71).text {3. Normalization type:};
    option add *Staffcheshire$w(72).background {lightgray};
    option add *Staffcheshire$w(72).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(72).foreground {black};
    option add *Staffcheshire$w(72).text {4. Primary key option:};
    option add *Staffcheshire$w(73).background {lightgray};
    option add *Staffcheshire$w(73).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(73).foreground {black};
    option add *Staffcheshire$w(73).text {5. Name for this Index:};
    option add *Staffcheshire$w(74).background {lightgray};
    option add *Staffcheshire$w(74).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(74).foreground {black};
    option add *Staffcheshire$w(74).text {6. Full path for the actual index file created:};
    option add *Staffcheshire$w(76).background {lightgray};
    option add *Staffcheshire$w(76).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(76).foreground {black};
    option add *Staffcheshire$w(76).text {7. Full path name for the stop list to use:};
    option add *Staffcheshire$w(78).background {lightgray};
    option add *Staffcheshire$w(78).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(78).foreground {black};
    option add *Staffcheshire$w(78).text {8. Indexmap Entries:};
    option add *Staffcheshire$w(81).background {lightgray};
    option add *Staffcheshire$w(81).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(81).foreground {black};
    option add *Staffcheshire$w(82).background {lightgray};
    option add *Staffcheshire$w(82).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(82).foreground {black};
    option add *Staffcheshire$w(83).background {lightgray};
    option add *Staffcheshire$w(83).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(83).foreground {black};
    option add *Staffcheshire$w(80).background {lightgray};
    option add *Staffcheshire$w(80).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(80).foreground {black};
    option add *Staffcheshire$w(66).background {gray95};
    option add *Staffcheshire$w(66).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(66).foreground {black};
    option add *Staffcheshire$w(67).background {gray95};
    option add *Staffcheshire$w(67).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(67).foreground {black};
    option add *Staffcheshire$w(75).background {lightgray};
    option add *Staffcheshire$w(75).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(75).foreground {black};
    option add *Staffcheshire$w(75).text {Browse};
    option add *Staffcheshire$w(68).background {gray95};
    option add *Staffcheshire$w(68).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(68).foreground {black};
    option add *Staffcheshire$w(77).background {lightgray};
    option add *Staffcheshire$w(77).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(77).foreground {black};
    option add *Staffcheshire$w(77).text {Browse};
    option add *Staffcheshire$w(79).background {lightgray};
    option add *Staffcheshire$w(79).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(79).foreground {black};
    option add *Staffcheshire$w(79).text {Create Entries};
    option add *Staffcheshire$w(84).background {lightgray};
    option add *Staffcheshire$w(85).background {lightgray};
    option add *Staffcheshire$w(85).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(85).foreground {black};
    option add *Staffcheshire$w(85).text {9. Index key tag specification:};
    option add *Staffcheshire$w(86).background {lightgray};
    option add *Staffcheshire$w(86).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(86).text {More};
    option add *Staffcheshire$w(87).background {gray95};
    option add *Staffcheshire$w(87).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(87).foreground {#000000000000};
    option add *Staffcheshire$w(88).background {lightgray};
    option add *Staffcheshire$w(89).background {lightgray};
    option add *Staffcheshire$w(90).background {lightgray};
    option add *Staffcheshire$w(90).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(90).text {10. Elements to be excluded from indexing:};
    option add *Staffcheshire$w(91).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(91).text {More};
    option add *Staffcheshire$w(92).background {lightgray};
    option add *Staffcheshire$w(93).background {gray95};
    option add *Staffcheshire$w(93).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(93).foreground {#000000000000};
    option add *Staffcheshire$w(94).background {lightgray};
    option add *Staffcheshire$w(96).background {lightgray};
    option add *Staffcheshire$w(97).background {lightgray};
    option add *Staffcheshire$w(98).background {lightgray};
    option add *Staffcheshire$w(99).background {lightgray};
    option add *Staffcheshire$w(100).background {lightgray};
    option add *Staffcheshire$w(100).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(100).foreground {blue};
    option add *Staffcheshire$w(100).text {Next};
    option add *Staffcheshire$w(101).background {cornsilk};
    option add *Staffcheshire$w(101).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(101).foreground {blue};
    option add *Staffcheshire$w(101).text {Cluster Definition 1};
    option add *Staffcheshire$w(102).background {lightgray};
    option add *Staffcheshire$w(103).background {lightgray};
    option add *Staffcheshire$w(103).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(103).foreground {black};
    option add *Staffcheshire$w(103).text {1. Cluster File Name:};
    option add *Staffcheshire$w(107).background {lightgray};
    option add *Staffcheshire$w(107).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(107).foreground {black};
    option add *Staffcheshire$w(107).text {4. Clusmap:};
    option add *Staffcheshire$w(114).background {lightgray};
    option add *Staffcheshire$w(114).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(114).foreground {black};
    option add *Staffcheshire$w(114).text {2. Cluskey:};
    option add *Staffcheshire$w(116).background {lightgray};
    option add *Staffcheshire$w(116).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(116).foreground {black};
    option add *Staffcheshire$w(116).text {3. Full path name for the stop list:};
    option add *Staffcheshire$w(104).background {gray95};
    option add *Staffcheshire$w(104).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(104).foreground {black};
    option add *Staffcheshire$w(108).background {lightgray};
    option add *Staffcheshire$w(109).background {lightgray};
    option add *Staffcheshire$w(109).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(109).foreground {black};
    option add *Staffcheshire$w(110).background {lightgray};
    option add *Staffcheshire$w(110).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(110).foreground {black};
    option add *Staffcheshire$w(110).text {Normalization Type:};
    option add *Staffcheshire$w(111).background {lightgray};
    option add *Staffcheshire$w(112).background {lightgray};
    option add *Staffcheshire$w(112).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(112).text {tagspec:};
    option add *Staffcheshire$w(113).background {gray95};
    option add *Staffcheshire$w(113).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(113).foreground {#000000000000};
    option add *Staffcheshire$w(105).background {gray95};
    option add *Staffcheshire$w(105).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(105).foreground {black};
    option add *Staffcheshire$w(106).background {lightgray};
    option add *Staffcheshire$w(106).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(106).foreground {black};
    option add *Staffcheshire$w(106).text {Browse};
    option add *Staffcheshire$w(115).background {lightgray};
    option add *Staffcheshire$w(115).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(115).foreground {black};
    option add *Staffcheshire$w(115).text {Create Clusmap Entries};
    option add *Staffcheshire$w(117).background {lightgray};
    option add *Staffcheshire$w(118).background {lightgray};
    option add *Staffcheshire$w(118).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(118).foreground {black};
    option add *Staffcheshire$w(118).text {Clusbase:};
    option add *Staffcheshire$w(119).background {gray95};
    option add *Staffcheshire$w(119).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(119).foreground {black};
    option add *Staffcheshire$w(120).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(120).text {Browse};
    option add *Staffcheshire$w(121).background {lightgray};
    option add *Staffcheshire$w(122).background {lightgray};
    option add *Staffcheshire$w(16).background {lightgray};
    option add *Staffcheshire$w(17).background {lightgray};
    option add *Staffcheshire$w(30).background {lightgray};
    option add *Staffcheshire$w(31).background {lightgray};
    option add *Staffcheshire$w(31).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(31).foreground {black};
    option add *Staffcheshire$w(31).text {1. File Type};
    option add *Staffcheshire$w(32).background {lightgray};
    option add *Staffcheshire$w(32).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(32).foreground {black};
    option add *Staffcheshire$w(18).background {lightgray};
    option add *Staffcheshire$w(19).background {lightgray};
    option add *Staffcheshire$w(20).background {lightgray};
    option add *Staffcheshire$w(20).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(20).foreground {black};
    option add *Staffcheshire$w(20).text {2. Full path name of the};
    option add *Staffcheshire$w(21).background {lightgray};
    option add *Staffcheshire$w(21).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(21).foreground {black};
    option add *Staffcheshire$w(21).text {data file};
    option add *Staffcheshire$w(22).background {lightgray};
    option add *Staffcheshire$w(22).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(22).foreground {black};
    option add *Staffcheshire$w(22).text {top-level directory if multiple continuation files are used};
    option add *Staffcheshire$w(23).background {lightgray};
    option add *Staffcheshire$w(24).background {gray95};
    option add *Staffcheshire$w(24).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(24).foreground {black};
    option add *Staffcheshire$w(25).background {lightgray};
    option add *Staffcheshire$w(25).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(25).foreground {black};
    option add *Staffcheshire$w(25).text {Browse};
    option add *Staffcheshire$w(26).background {lightgray};
    option add *Staffcheshire$w(27).background {lightgray};
    option add *Staffcheshire$w(27).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(27).foreground {black};
    option add *Staffcheshire$w(27).text {Full path name of the buildassoc-generated *.cont file containing <FILECONT> tags};
    option add *Staffcheshire$w(28).background {gray95};
    option add *Staffcheshire$w(28).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(28).foreground {black};
    option add *Staffcheshire$w(29).background {lightgray};
    option add *Staffcheshire$w(29).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(29).foreground {black};
    option add *Staffcheshire$w(29).text {Browse};
    option add *Staffcheshire$w(33).background {lightgray};
    option add *Staffcheshire$w(34).background {lightgray};
    option add *Staffcheshire$w(34).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(34).foreground {black};
    option add *Staffcheshire$w(34).text {3. Nickname for the file:};
    option add *Staffcheshire$w(35).background {gray95};
    option add *Staffcheshire$w(35).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(35).foreground {black};
    option add *Staffcheshire$w(36).background {lightgray};
    option add *Staffcheshire$w(37).background {lightgray};
    option add *Staffcheshire$w(37).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(37).foreground {black};
    option add *Staffcheshire$w(37).text {4. Full path of the SGML DTD file for this file:};
    option add *Staffcheshire$w(38).background {gray95};
    option add *Staffcheshire$w(38).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(38).foreground {black};
    option add *Staffcheshire$w(39).background {lightgray};
    option add *Staffcheshire$w(39).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(39).foreground {black};
    option add *Staffcheshire$w(39).text {Browse};
    option add *Staffcheshire$w(40).background {lightgray};
    option add *Staffcheshire$w(41).background {lightgray};
    option add *Staffcheshire$w(41).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(41).foreground {black};
    option add *Staffcheshire$w(41).text {5. Full path name of the SGML catalog file used to resolve PUBLIC name references for this DTD:};
    option add *Staffcheshire$w(42).background {gray95};
    option add *Staffcheshire$w(42).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(42).foreground {black};
    option add *Staffcheshire$w(43).background {lightgray};
    option add *Staffcheshire$w(43).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(43).foreground {black};
    option add *Staffcheshire$w(43).text {Browse};
    option add *Staffcheshire$w(44).background {lightgray};
    option add *Staffcheshire$w(45).background {lightgray};
    option add *Staffcheshire$w(45).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(45).foreground {black};
    option add *Staffcheshire$w(45).text {6. Full tag path of the SGML tag within each main document that is to be treated as if it were an entire document:};
    option add *Staffcheshire$w(46).background {gray95};
    option add *Staffcheshire$w(46).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(46).foreground {black};
    option add *Staffcheshire$w(47).background {lightgray};
    option add *Staffcheshire$w(48).background {lightgray};
    option add *Staffcheshire$w(48).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(48).foreground {black};
    option add *Staffcheshire$w(48).text {7. Full path name of the associator file for the data:};
    option add *Staffcheshire$w(49).background {gray95};
    option add *Staffcheshire$w(49).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(49).foreground {black};
    option add *Staffcheshire$w(50).background {lightgray};
    option add *Staffcheshire$w(50).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(50).foreground {black};
    option add *Staffcheshire$w(50).text {Browse};
    option add *Staffcheshire$w(51).background {lightgray};
    option add *Staffcheshire$w(52).background {lightgray};
    option add *Staffcheshire$w(52).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(52).foreground {black};
    option add *Staffcheshire$w(52).text {8. Full path name of the history file for the data:};
    option add *Staffcheshire$w(53).background {gray95};
    option add *Staffcheshire$w(53).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(53).foreground {black};
    option add *Staffcheshire$w(54).background {lightgray};
    option add *Staffcheshire$w(54).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(54).foreground {black};
    option add *Staffcheshire$w(54).text {Browse};
    option add *Staffcheshire$w(55).background {lightgray};
    option add *Staffcheshire$w(56).background {lightgray};
    option add *Staffcheshire$w(56).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(56).foreground {black};
    option add *Staffcheshire$w(56).text {9. Display Options};
    option add *Staffcheshire$w(57).background {gray92};
    option add *Staffcheshire$w(57).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(57).foreground {black};
    option add *Staffcheshire$w(58).background {lightgray};
    option add *Staffcheshire$w(305).background {lightgray};
    option add *Staffcheshire$w(306).background {lightgray};
    option add *Staffcheshire$w(306).text {Default Path};
    option add *Staffcheshire$w(307).background {gray95};
    option add *Staffcheshire$w(307).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(161).background {lightgray};
    option add *Staffcheshire$w(162).background {lightgray};
    option add *Staffcheshire$w(163).background {lightgray};
    option add *Staffcheshire$w(164).background {lightgray};
    option add *Staffcheshire$w(164).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(164).foreground {blue};
    option add *Staffcheshire$w(164).text {Next};
    option add *Staffcheshire$w(165).background {cornsilk};
    option add *Staffcheshire$w(165).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(165).foreground {blue};
    option add *Staffcheshire$w(165).text {Component Definition  1};
    option add *Staffcheshire$w(166).background {lightgray};
    option add *Staffcheshire$w(167).background {lightgray};
    option add *Staffcheshire$w(168).background {lightgray};
    option add *Staffcheshire$w(168).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(168).text {1. Component Name};
    option add *Staffcheshire$w(170).background {lightgray};
    option add *Staffcheshire$w(170).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(170).text {2. Normalization:};
    option add *Staffcheshire$w(172).background {lightgray};
    option add *Staffcheshire$w(172).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(172).text {5. Component Indexes:};
    option add *Staffcheshire$w(175).background {lightgray};
    option add *Staffcheshire$w(175).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(175).text {3. Start tagspec:};
    option add *Staffcheshire$w(176).background {lightgray};
    option add *Staffcheshire$w(176).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(176).text {4. End tagspec (optional):};
    option add *Staffcheshire$w(178).background {lightgray};
    option add *Staffcheshire$w(178).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(178).text {Browse};
    option add *Staffcheshire$w(171).background {gray95};
    option add *Staffcheshire$w(171).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(171).foreground {#000000000000};
    option add *Staffcheshire$w(177).background {lightgray};
    option add *Staffcheshire$w(177).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(177).foreground {#000000000000};
    option add *Staffcheshire$w(173).background {gray95};
    option add *Staffcheshire$w(173).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(173).foreground {#000000000000};
    option add *Staffcheshire$w(174).background {gray95};
    option add *Staffcheshire$w(174).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(174).foreground {#000000000000};
    option add *Staffcheshire$w(179).background {lightgray};
    option add *Staffcheshire$w(181).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(181).foreground {blue};
    option add *Staffcheshire$w(181).text {Component Indexdef 1};
    option add *Staffcheshire$w(182).background {lightgray};
    option add *Staffcheshire$w(182).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(182).foreground {blue};
    option add *Staffcheshire$w(182).text {Next};
    option add *Staffcheshire$w(183).background {cornsilk};
    option add *Staffcheshire$w(187).background {cornsilk};
    option add *Staffcheshire$w(187).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(187).foreground {black};
    option add *Staffcheshire$w(187).text {a. Access type:};
    option add *Staffcheshire$w(188).background {cornsilk};
    option add *Staffcheshire$w(188).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(188).foreground {black};
    option add *Staffcheshire$w(188).text {b. Extract key type:};
    option add *Staffcheshire$w(189).background {cornsilk};
    option add *Staffcheshire$w(189).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(189).foreground {black};
    option add *Staffcheshire$w(189).text {c. Normalization type:};
    option add *Staffcheshire$w(190).background {cornsilk};
    option add *Staffcheshire$w(190).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(190).foreground {black};
    option add *Staffcheshire$w(190).text {d. Primary key option:};
    option add *Staffcheshire$w(191).background {cornsilk};
    option add *Staffcheshire$w(191).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(191).foreground {black};
    option add *Staffcheshire$w(191).text {e. Name for this Index:};
    option add *Staffcheshire$w(192).background {cornsilk};
    option add *Staffcheshire$w(192).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(192).foreground {black};
    option add *Staffcheshire$w(192).text {f. Full path for the actual index file created:};
    option add *Staffcheshire$w(194).background {cornsilk};
    option add *Staffcheshire$w(194).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(194).foreground {black};
    option add *Staffcheshire$w(194).text {g. Full path name for the stop list to use:};
    option add *Staffcheshire$w(196).background {cornsilk};
    option add *Staffcheshire$w(196).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(196).foreground {black};
    option add *Staffcheshire$w(196).text {h. Indexmap Entries:};
    option add *Staffcheshire$w(199).background {lightgray};
    option add *Staffcheshire$w(199).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(199).foreground {black};
    option add *Staffcheshire$w(200).background {lightgray};
    option add *Staffcheshire$w(200).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(200).foreground {black};
    option add *Staffcheshire$w(201).background {lightgray};
    option add *Staffcheshire$w(201).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(201).foreground {black};
    option add *Staffcheshire$w(198).background {lightgray};
    option add *Staffcheshire$w(198).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(198).foreground {black};
    option add *Staffcheshire$w(184).background {gray95};
    option add *Staffcheshire$w(184).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(184).foreground {black};
    option add *Staffcheshire$w(185).background {gray95};
    option add *Staffcheshire$w(185).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(185).foreground {black};
    option add *Staffcheshire$w(193).background {lightgray};
    option add *Staffcheshire$w(193).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(193).foreground {black};
    option add *Staffcheshire$w(193).text {Browse};
    option add *Staffcheshire$w(186).background {gray95};
    option add *Staffcheshire$w(186).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(186).foreground {black};
    option add *Staffcheshire$w(195).background {lightgray};
    option add *Staffcheshire$w(195).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(195).foreground {black};
    option add *Staffcheshire$w(195).text {Browse};
    option add *Staffcheshire$w(197).background {lightgray};
    option add *Staffcheshire$w(197).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(197).foreground {black};
    option add *Staffcheshire$w(197).text {Create Entries};
    option add *Staffcheshire$w(202).background {cornsilk};
    option add *Staffcheshire$w(203).background {cornsilk};
    option add *Staffcheshire$w(203).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(203).foreground {black};
    option add *Staffcheshire$w(203).text {i. Index key tag specification:};
    option add *Staffcheshire$w(204).background {lightgray};
    option add *Staffcheshire$w(204).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(204).text {More};
    option add *Staffcheshire$w(205).background {gray95};
    option add *Staffcheshire$w(205).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(205).foreground {#000000000000};
    option add *Staffcheshire$w(206).background {cornsilk};
    option add *Staffcheshire$w(207).background {cornsilk};
    option add *Staffcheshire$w(208).background {cornsilk};
    option add *Staffcheshire$w(208).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(208).text {j. Elements to be excluded from indexing:};
    option add *Staffcheshire$w(209).background {lightgray};
    option add *Staffcheshire$w(209).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(209).text {More};
    option add *Staffcheshire$w(210).background {cornsilk};
    option add *Staffcheshire$w(211).background {gray95};
    option add *Staffcheshire$w(211).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(211).foreground {#000000000000};
    option add *Staffcheshire$w(212).background {lightgray};
    option add *Staffcheshire$w(124).background {lightgray};
    option add *Staffcheshire$w(125).background {lightgray};
    option add *Staffcheshire$w(126).background {lightgray};
    option add *Staffcheshire$w(127).background {cornsilk};
    option add *Staffcheshire$w(127).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(127).foreground {blue};
    option add *Staffcheshire$w(127).text {Display Definition 1};
    option add *Staffcheshire$w(128).background {lightgray};
    option add *Staffcheshire$w(128).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(128).foreground {blue};
    option add *Staffcheshire$w(128).text {Next};
    option add *Staffcheshire$w(129).background {lightgray};
    option add *Staffcheshire$w(130).background {lightgray};
    option add *Staffcheshire$w(130).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(130).foreground {black};
    option add *Staffcheshire$w(130).text {Format Name:};
    option add *Staffcheshire$w(132).background {lightgray};
    option add *Staffcheshire$w(132).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(132).foreground {black};
    option add *Staffcheshire$w(132).text {OID of the record syntax:};
    option add *Staffcheshire$w(151).background {lightgray};
    option add *Staffcheshire$w(151).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(151).foreground {black};
    option add *Staffcheshire$w(151).text {Default};
    option add *Staffcheshire$w(131).background {gray95};
    option add *Staffcheshire$w(131).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(131).foreground {black};
    option add *Staffcheshire$w(158).background {lightgray};
    option add *Staffcheshire$w(158).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(158).foreground {black};
    option add *Staffcheshire$w(133).background {lightgray};
    option add *Staffcheshire$w(134).background {lightgray};
    option add *Staffcheshire$w(134).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(134).foreground {black};
    option add *Staffcheshire$w(134).text {ALL};
    option add *Staffcheshire$w(135).background {lightgray};
    option add *Staffcheshire$w(136).background {lightgray};
    option add *Staffcheshire$w(136).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(136).foreground {black};
    option add *Staffcheshire$w(136).text {PAGE_PATH};
    option add *Staffcheshire$w(137).background {lightgray};
    option add *Staffcheshire$w(137).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(137).foreground {black};
    option add *Staffcheshire$w(137).text {RECMAP};
    option add *Staffcheshire$w(138).background {lightgray};
    option add *Staffcheshire$w(138).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(138).foreground {black};
    option add *Staffcheshire$w(138).text {TAGSET-M};
    option add *Staffcheshire$w(139).background {lightgray};
    option add *Staffcheshire$w(139).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(139).foreground {black};
    option add *Staffcheshire$w(139).text {TAGSET-G};
    option add *Staffcheshire$w(140).background {lightgray};
    option add *Staffcheshire$w(140).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(140).foreground {black};
    option add *Staffcheshire$w(140).text {External};
    option add *Staffcheshire$w(141).background {lightgray};
    option add *Staffcheshire$w(142).background {lightgray};
    option add *Staffcheshire$w(142).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(142).foreground {black};
    option add *Staffcheshire$w(142).text {Path to function:};
    option add *Staffcheshire$w(143).background {gray95};
    option add *Staffcheshire$w(143).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(143).foreground {black};
    option add *Staffcheshire$w(144).background {lightgray};
    option add *Staffcheshire$w(144).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(144).foreground {black};
    option add *Staffcheshire$w(144).text {Browse};
    option add *Staffcheshire$w(145).background {lightgray};
    option add *Staffcheshire$w(145).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(145).foreground {black};
    option add *Staffcheshire$w(145).text {Function Name:};
    option add *Staffcheshire$w(146).background {lightgray};
    option add *Staffcheshire$w(147).background {lightgray};
    option add *Staffcheshire$w(148).background {lightgray};
    option add *Staffcheshire$w(148).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(148).foreground {black};
    option add *Staffcheshire$w(148).text {Create Entries};
    option add *Staffcheshire$w(149).background {lightgray};
    option add *Staffcheshire$w(149).font {-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(149).foreground {black};
    option add *Staffcheshire$w(149).text {Clusmap:};
    option add *Staffcheshire$w(150).background {lightgray};
    option add *Staffcheshire$w(150).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(150).foreground {black};
    option add *Staffcheshire$w(150).text {Convert:};
    option add *Staffcheshire$w(152).background {lightgray};
    option add *Staffcheshire$w(153).background {lightgray};
    option add *Staffcheshire$w(153).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(153).foreground {black};
    option add *Staffcheshire$w(153).text {Exclude tagspec:};
    option add *Staffcheshire$w(154).background {lightgray};
    option add *Staffcheshire$w(154).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(154).foreground {black};
    option add *Staffcheshire$w(154).text {Compress};
    option add *Staffcheshire$w(156).background {lightgray};
    option add *Staffcheshire$w(156).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(156).text {More};
    option add *Staffcheshire$w(155).background {gray95};
    option add *Staffcheshire$w(155).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Staffcheshire$w(155).foreground {#000000000000};
    option add *Staffcheshire$w(157).background {lightgray};
    option add *Staffcheshire$w(159).background {lightgray};
    option add *Staffcheshire$w(301).background {lightgray};
    option add *Staffcheshire$w(301).label {};
}


# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,staffcheshire) staffcheshire;
# Local used Packages:
set ::rat(PACKAGES,LOADED) [list ];
# ---------------------------------------------------------------------------

srcStartUp scifc.tcl CODE source;
####VG-STOP####


