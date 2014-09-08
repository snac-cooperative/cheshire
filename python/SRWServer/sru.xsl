<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:srw="http://www.loc.gov/zing/srw/v1.0/" xmlns:dc="http://www.loc.gov/zing/srw/dcschema/v1.0/" xmlns:card="http://srw.o-r-g.org/schemas/ccg/1.0/" xmlns:zr="http://explain.z3950.org/dtd/2.0/" xmlns:diag="http://www.loc.gov/zing/srw/v1.0/diagnostic/" version="1.0">

  <!-- Author: Rob Sanderson (azaroth@liv.ac.uk)   Version:  0.6   Last Updated:  27/11/2003 -->
  <!-- Licence:  GPL -->

  <xsl:template match="/">
    <html>
      <head>
        <title>
          <xsl:value-of select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:databaseInfo/zr:title"/>
        </title>
        <style>
          H2 {font-family: sans-serif; color: #FF5500; background-color: #eeeeff; padding-top: 10px; padding-bottom: 10px; border: 1px solid #3333FF}
          H3 { font-family: sans-serif; color: #F65500; text-indent: 20px; border-left: solid 1px #3333FF; border-top: solid 1px #3333FF; padding-top: 5px }
          .paramTable { vertical-align: top; border: 1px solid; padding: 3px; border-collapse: collapse }
          .exampleTable { vertical-align: top; border: 1px solid; padding: 3px; border-collapse: collapse; background-color: #eeeeff}
          
          .paramTable TD {border: 1px solid}
          .exampleTable TD {border: 1px solid}
          TH {border: 1px solid; background-color: #eeeeff}

        </style>

        <script>
          <xsl:text>
            function mungeForm() {
            inform = document.getElementById('CQLForm');
            outform = document.getElementById('SRUForm');
            max = inform.maxIndex.value
            cql = ""
            prevIdx = 0;
            // Step through elements in form to create CQL
            for (var idx = 1; idx &lt;= max; idx++) {
              term = inform["term"+idx].value;
              if (term) {
                if (prevIdx) {
                  cql += " " + inform["bool" + prevIdx].value + " "
                }
                if (term.indexOf(' ')) {
                  term = '"' + term + '"';
                }
                cql += inform["index" + idx].value + " " + inform["relat" + idx].value + " " + term
                prevIdx = idx
             }
            }
            if (!cql) {
            alert("At least one term is required to search.");
            return false;
            }
            outform.query.value = cql
            return true;
            }

            function mungeScanForm() {
            inform = document.getElementById('ScanIndexes');
            outform = document.getElementById('ScanSubmit');
            index = inform.scanIndex.value;
            term = inform.term.value;
            outform.scanClause.value = index + " exact \"" + term + "\""
            }

          </xsl:text>
        </script>
      </head>
      <body bgcolor="#FFFFFF">
        <center>
          <h2>
            <xsl:value-of select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:databaseInfo/zr:title"/>
          </h2>
        </center>

        <p>
          <xsl:value-of select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:databaseInfo/zr:description"/>
        </p>

	<xsl:apply-templates select="//srw:explainResponse/srw:diagnostics"/>

        <h3>Search</h3>

        <p>
        <form id="CQLForm">
        <table class="paramTable">
          <tr><th>Index</th><th>Relation</th><th>Term</th><th>Boolean</th></tr>
          <input type="hidden" name="maxIndex">
            <xsl:attribute name="value">
              <xsl:value-of select="count(//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:indexInfo/zr:index)"/>
            </xsl:attribute>
          </input>
          <xsl:for-each select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:indexInfo/zr:index">
            <tr>
              <td align="right">
                <b><xsl:value-of select="zr:title"/></b>
                <input type="hidden">
                  <xsl:attribute name="name">index<xsl:value-of select="position()"/></xsl:attribute>
                  <xsl:attribute name="value"><xsl:value-of select="zr:map[1]/zr:name/@set"/>.<xsl:value-of select="zr:map[1]/zr:name"/></xsl:attribute>
                </input>
              </td>
              <td>
                <select>
                  <xsl:attribute name="name">relat<xsl:value-of select="position()"/></xsl:attribute>
                  <option value="=">=</option>
                  <option value="exact">exact</option>
                  <option value="any">any</option>
                  <option value="all">all</option>
                  <option value="&lt;">&lt;</option>
                  <option value="&gt;">&gt;</option>
                  <option value="&lt;=">&lt;=</option>
                  <option value="&gt;=">&gt;=</option>
                  <option value="&lt;&gt;">not</option>
                </select>
              </td>
              <td>
                <input type="text" value="">
                  <xsl:attribute name="name">term<xsl:value-of select="position()"/></xsl:attribute>
                </input>
              </td>
              <td>
                <select>
                  <xsl:attribute name="name">bool<xsl:value-of select="position()"/></xsl:attribute>
                  <option value="and">and</option>
                  <option value="or">or</option>
                  <option value="not">not</option>
                </select>
              </td>
            </tr>
          </xsl:for-each>
        </table>
      </form>

        <form method="GET" id="SRUForm">
          <xsl:attribute name="action">
            http://<xsl:value-of select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:serverInfo/zr:host"/>:<xsl:value-of select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:serverInfo/zr:port"/><xsl:value-of select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:serverInfo/zr:database"/>
        </xsl:attribute>
        
        <input type="hidden" name="query" value=""/>
        
        <table>
          <tr>
            <td><b>Record Schema:</b>
          </td>
          <td>
            <select name="recordSchema">
              <xsl:for-each select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:schemaInfo/zr:schema">
                <option>
                  <xsl:attribute name="value">
                    <xsl:value-of select="@identifier"/><br/>
                  </xsl:attribute>
                  <xsl:value-of select="zr:title"/><br/>
                </option>
              </xsl:for-each>
            </select>
          </td>
        </tr>
        <tr>
          <td><b>Number of Records:</b></td>
          <td>
            <input type="text" name="maximumRecords">
              <xsl:attribute name="value">
                <xsl:value-of select='//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:configInfo/zr:default[@type="numberOfRecords"]'/>
              </xsl:attribute>
            </input>
          </td>
        </tr>

        <tr>
          <td><b>Record Position:</b></td>
          <td>
            <input type="text" name="startRecord" value="1"/>
          </td>
        </tr>
        <tr>
          <td><b>Result Set TTL:</b></td>
          <td>
            <input type="text" name="resultSetTTL" value="0"/>
          </td>
        </tr>

        <tr>
          <td><b>Record Packing:</b></td>
          <td>
            <select name="recordPacking">
              <option value="xml">XML</option>
              <option value="string">String</option>
            </select>
          </td>
        </tr>

	<tr>
	<td><b>Record XPath:</b></td>
	<td><input type="text" name="recordXPath" value = ""/></td>
	</tr>	

      </table>

	<input type="hidden" name="operation" value="searchRetrieve"/>
	<input type="hidden" name="version" value="1.1"/>
          <input type="submit" value="Search" onClick="return mungeForm();"/>
        </form>
      </p>
        <h3>Browse</h3>
        <p>
	<b>Browse:</b>
          <form id="ScanIndexes">
            <select name="scanIndex">
              <xsl:for-each select="//srw:explainResponse/srw:record/srw:recordData/zr:explain/zr:indexInfo/zr:index">
                <option>
                  <xsl:attribute name="value"><xsl:value-of select="zr:map[1]/zr:name/@set"/>.<xsl:value-of select="zr:map[1]/zr:name"/></xsl:attribute>
                  <xsl:value-of select="zr:title"/>
                </option>
              </xsl:for-each>
            </select>
            at
            <input name="term" type="text" value = ""/>
            </form>
            
            <form id="ScanSubmit">
              <input type="hidden" name="numberOfTerms" value="20"/>
              <input type="hidden" name="responsePosition" value="1"/>
              <input type="hidden" name="operation" value="scan"/>
              <input type="hidden" name="scanClause" value=""/>
	      <input type="hidden" name="version" value="1.1"/>

              <input type="submit" value="Browse" onClick="return mungeScanForm();"/>
            </form>
            

      </p>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="srw:diagnostics">
     <h3>Diagnostics</h3>
     <xsl:apply-templates/>
  </xsl:template>

<xsl:template match="diag:diagnostic">
<table>
<xsl:apply-templates/>
</table>
</xsl:template>
<xsl:template match="diag:code">
<tr><td><b>Code:</b></td><td> <xsl:value-of select="."/></td></tr>
</xsl:template>
<xsl:template match="diag:message">
<tr><td><b>Message:</b></td><td><xsl:value-of select="."/></td></tr>
</xsl:template>
<xsl:template match="diag:details">
<tr><td><b>Details:</b></td><td><xsl:value-of select="."/></td></tr>
</xsl:template>

</xsl:stylesheet>
