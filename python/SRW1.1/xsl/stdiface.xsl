<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:srw="http://www.loc.gov/zing/srw/" xmlns:dc="http://www.loc.gov/zing/srw/dcschema/v1.0/" xmlns:card="http://srw.o-r-g.org/schemas/ccg/1.0/" xmlns:diag="http://www.loc.gov/zing/srw/diagnostic/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xcql="http://www.loc.gov/zing/cql/xcql/" version="1.0"> 

<xsl:template match="/" name="stdiface">

<html><head><title><xsl:value-of select="$databaseTitle"/></title>
</head><body bgcolor="white"><center><h2 style="font-family: sans-serif; color: #FF5500; background-color: #eeeeff; padding-top: 10px; padding-bottom: 10px; border: 1px solid #3333FF"><xsl:value-of select="$databaseTitle"/></h2></center>
<p><xsl:apply-templates/></p>

<p>
<h3 style="font-family: sans-serif; color: #F65500; text-indent: 20px; border-left: solid 1px #3333FF; border-top: solid 1px #3333FF; padding-top: 5px">New Search</h3>
<p>
<a>
<xsl:attribute name="href">
  <xsl:value-of select="$databaseURL"/>
</xsl:attribute>
Home
</a>
</p>
<form>
<xsl:attribute name="action"><xsl:value-of select="$databaseURL"/></xsl:attribute>
<input type="hidden" name="version" value="1.1"/>
<input type="hidden" name="operation" value="searchRetrieve"/>
<table>
<tr><td><b>CQL Query</b></td><td><input type="text" size="80" name="query">
<xsl:attribute name="value">
  <xsl:choose>
  <xsl:when test="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:query">
    <xsl:value-of select="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:query"/>
  </xsl:when>
  <xsl:when test="/srw:scanResponse/srw:echoedScanRequest/srw:scanClause">
    <xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:scanClause"/>
  </xsl:when>
  </xsl:choose>
</xsl:attribute>
</input>
</td></tr>

<tr><td><b>Result Set TTL</b></td><td>
<input type="text" size = "10" name="resultSetTTL">
<xsl:attribute name="value">
  <xsl:choose>
  <xsl:when test="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:resultSetTTL">
    <xsl:value-of select="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:resultSetTTL"/>
  </xsl:when>
  <xsl:otherwise>0</xsl:otherwise>
  </xsl:choose>
</xsl:attribute>
</input>
</td></tr>
<tr><td><b>Schema</b></td><td><input type="text" size="80" name="recordSchema">
<!-- Check echoed, then check records -->
<xsl:attribute name="value">
  <xsl:choose>
    <xsl:when test="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:recordSchema">	
      <xsl:value-of select="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:recordSchema"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="/srw:searchRetrieveResponse/srw:records/srw:record/srw:recordSchema"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:attribute>
</input>
</td></tr>

<tr><td><b>Start Record</b></td><td>
<input name="startRecord" type="text" size="10">
<xsl:attribute name="value">
  <xsl:choose>
    <xsl:when test="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:startRecord">	
      <xsl:value-of select="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:startRecord"/>
    </xsl:when>
    <xsl:otherwise>1</xsl:otherwise>
  </xsl:choose>
</xsl:attribute>
</input>
</td></tr>

<tr><td><b>Number of Records</b></td><td>
<input name="maximumRecords" type="text" size="10">
<xsl:attribute name="value">
  <xsl:choose>
    <xsl:when test="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:maximumRecords">	
      <xsl:value-of select="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:maximumRecords"/>
    </xsl:when>
    <xsl:otherwise>10</xsl:otherwise>
  </xsl:choose>
</xsl:attribute>

</input>
</td></tr>

<tr><td><b>Record Packing</b></td><td>
<select name="recordPacking">
  <option value="xml">XML</option>
  <option value="string">String</option>
</select>
</td>
</tr>
<tr><td><b>Record XPath</b></td><td><input name="recordXPath" type="text" size="80">
<xsl:attribute name="value">
  <xsl:value-of select="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:recordXPath"/>
</xsl:attribute>
</input>
</td></tr>
<tr><td><b>Sort Keys</b></td><td><input name="sortKeys" type="text" size="80">
<xsl:attribute name="value">
  <xsl:value-of select="/srw:searchRetrieveResponse/srw:echoedSearchRetrieveRequest/srw:sortKeys"/>
</xsl:attribute>
</input>
</td></tr>

<tr><td></td><td><input type="submit" value="search"/></td></tr>
</table>
</form>
</p>
</body>
</html>

</xsl:template>

<xsl:template match="scanResponse">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="srw:terms">
  <h3 style="font-family: sans-serif; color: #F65500; text-indent: 20px; border-left: solid 1px #3333FF; border-top: solid 1px #3333FF; padding-top: 5px">Terms</h3>
  <table width="50%" border="1" cellpadding="3" cellspacing="0">
  <tr bgcolor="#eeeeff"><th>Term</th><th>Frequency</th></tr>
  <xsl:apply-templates/>
  </table>

<p>
&lt;--
<a>
<xsl:attribute name="href">
<xsl:value-of select="$databaseURL"/>?operation=scan&amp;scanClause=
<xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:xScanClause/xcql:index"/>%20<xsl:value-of select="//srw:scanResponse/srw:echoedScanRequest/srw:xScanClause/xcql:relation/xcql:value"/>%20"<xsl:value-of select ="./srw:term[1]/srw:value"/>"
&amp;responsePosition=<xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:maximumTerms"/>
&amp;version=1.1
&amp;maximumTerms=<xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:maximumTerms"/>
</xsl:attribute>
Previous
</a>

|

<a>
<xsl:attribute name="href">
<xsl:value-of select="$databaseURL"/>?operation=scan&amp;scanClause=
<xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:xScanClause/xcql:index"/>%20<xsl:value-of select="//srw:scanResponse/srw:echoedScanRequest/srw:xScanClause/xcql:relation/xcql:value"/>%20"<xsl:value-of select ="./srw:term[count(//srw:scanResponse/srw:terms/srw:term)]/srw:value"/>"
&amp;responsePosition=1&amp;version=1.1
&amp;maximumTerms=<xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:maximumTerms"/>
</xsl:attribute>
Next 
</a>
--&gt;
</p>


</xsl:template>

<xsl:template match="srw:term">
<tr>
  <xsl:apply-templates/>
</tr>
</xsl:template>

<xsl:template match="srw:value">
<td>
  <a>
    <xsl:attribute name="href"><xsl:value-of select="$databaseURL"/>?recordPacking=xml&amp;operation=searchRetrieve&amp;version=1.1&amp;query=<xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:xScanClause/xcql:index"/>%20<xsl:value-of select="/srw:scanResponse/srw:echoedScanRequest/srw:xScanClause/xcql:relation/xcql:value"/>%20"<xsl:value-of select="."/>"</xsl:attribute>
    <xsl:value-of select="."/>
  </a> 
</td>
</xsl:template>

<xsl:template match="srw:term/srw:numberOfRecords">
<td><xsl:value-of select="."/></td>
</xsl:template>

<xsl:template match="searchRetrieveResponse">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="srw:numberOfRecords">
  <p><b>Matches:</b><xsl:text> </xsl:text><xsl:value-of select="."/></p>
</xsl:template>

<xsl:template match="srw:resultSetId">
  Result Set Identifier:<xsl:text> </xsl:text><xsl:value-of select="."/>
</xsl:template>

<xsl:template match="srw:resultSetIdleTime">
  <xsl:text> </xsl:text>(Will last for<xsl:text> </xsl:text><xsl:value-of select="."/><xsl:text> </xsl:text>seconds)
</xsl:template>

<xsl:template match="srw:records">
  <h3 style="font-family: sans-serif; color: #F65500; text-indent: 20px; border-left: solid 1px #3333FF; border-top: solid 1px #3333FF; padding-top: 5px">Records</h3>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="srw:record">
  <p>
    <xsl:apply-templates select="child::srw:recordPosition"/>
    <xsl:apply-templates select="child::srw:recordSchema"/>
    <xsl:apply-templates select="child::srw:recordData"/>
  </p>
</xsl:template>

<xsl:template match="srw:record/srw:recordSchema">
  <b>Schema: </b>
  <xsl:variable name="schema" select="."/> 
  <xsl:choose>
      <xsl:when test="$schema = 'info:srw/schema/1/dc-v1.1'">
	      Dublin Core
      </xsl:when>
      <xsl:when test="$schema = 'info:srw/schema/1/marcxml-v1.1'">
	      MARC XML
      </xsl:when>
      <xsl:when test="$schema = 'info:srw/schema/1/mods-v3.0'">
	      MODS
      </xsl:when>
      <xsl:when test="$schema = 'http://srw.o-r-g.org/schemas/ccg/1.0/'">
	      Collectable Card Schema
      </xsl:when>
      <xsl:otherwise>
	      <xsl:value-of select="$schema"/>
      </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="srw:recordPosition">
  <b>Position: </b> <xsl:value-of select="."/> <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="srw:nextRecordPosition">
  <!-- Not used -->
</xsl:template>

<xsl:template match="srw:recordData">
  <table width="100%" style="vertical-align: top; border: 1px solid; padding: 3px; border-collapse: collapse; background-color: #eefdff">

<xsl:choose>
<xsl:when test="../srw:recordPacking = 'string'">
<tr><td style="border: 1px solid">
<pre><xsl:value-of select="."/></pre>
</td></tr>
</xsl:when>
<xsl:otherwise>
<xsl:apply-templates/>
</xsl:otherwise>
</xsl:choose>

</table>
</xsl:template>

<xsl:template match="srw:diagnostics">
  <h3 style="font-family: sans-serif; color: #F65500; text-indent: 20px; border-left: solid 1px #3333FF; border-top: solid 1px #3333FF; padding-top: 5px">Diagnostics</h3>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="diag:diagnostic">
  <!-- Just feed down -->
  <table>
    <xsl:apply-templates/>
  </table>
</xsl:template>

<xsl:template match="diag:uri"><tr><td><b>Identifier:</b></td><td><xsl:value-of select="."/></td></tr></xsl:template>
<xsl:template match="diag:details"><tr><td><b>Details:</b></td><td><xsl:value-of select="."/></td></tr></xsl:template>
<xsl:template match="diag:message"><tr><td><b>Message:</b></td><td><xsl:value-of select="."/></td></tr></xsl:template>


<xsl:template match="srw:echoedSearchRetrieveRequest">
  <xsl:if test="//srw:nextRecordPosition">
  <form method="GET">
    <xsl:attribute name="action">
      <xsl:value-of select="$databaseURL"/>
    </xsl:attribute>
    <input type="hidden" value="searchRetrieve" name="operation"/>
    <xsl:apply-templates/>
    <input type="hidden" name="startRecord">
      <xsl:attribute name="value">
        <xsl:value-of select="//srw:nextRecordPosition"/>
      </xsl:attribute>
    </input>
    <input type="submit" value="Next Record(s)"/>
  </form>
  </xsl:if>
</xsl:template>

<xsl:template match="srw:query|srw:echoedSearchRetrieveRequest/srw:recordSchema|srw:maximumRecords|srw:recordPacking|srw:version|srw:recordXPath|srw:resultSetTTL|srw:sortKeys">
  <input type="hidden">
    <xsl:attribute name="name"><xsl:value-of select="local-name()"/></xsl:attribute>
    <xsl:attribute name="value"><xsl:value-of select="."/></xsl:attribute>
  </input>
</xsl:template>

<xsl:template match="srw:startRecord|srw:xQuery"/>
<xsl:template match="srw:echoedScanRequest"/>

</xsl:stylesheet>
