<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <!-- Author: Rob Sanderson (azaroth@liv.ac.uk)   Version:  0.5   Last Updated:  28/01/03 -->
  <!-- Licence:  GPL -->

  <xsl:template match="/">
    <html>
      <head>
        <title>
          <xsl:value-of select="//explain/databaseInfo/title"/>
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
            outform.clause.value = index + " exact \"" + term + "\""
            }

          </xsl:text>
        </script>
      </head>
      <body bgcolor="#FFFFFF">
        <center>
          <h2>
            <xsl:value-of select="//explain/databaseInfo/title"/>
          </h2>
        </center>

        <p>
          <xsl:value-of select="//explain/databaseInfo/description"/>
        </p>

        <h3>Search</h3>

        <p>
        <form id="CQLForm">
        <table class="paramTable">
          <tr><th>Index</th><th>Relation</th><th>Term</th><th>Boolean</th></tr>
          <input type="hidden" name="maxIndex">
            <xsl:attribute name="value">
              <xsl:value-of select="count(//explain/indexInfo/index)"/>
            </xsl:attribute>
          </input>
          <xsl:for-each select="//explain/indexInfo/index">
            <tr>
              <td align="right">
                <b><xsl:value-of select="title"/></b>
                <input type="hidden">
                  <xsl:attribute name="name">index<xsl:value-of select="position()"/></xsl:attribute>
                  <xsl:attribute name="value"><xsl:value-of select="map[1]/name/@indexSet"/>.<xsl:value-of select="map[1]/name"/></xsl:attribute>
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
            http://<xsl:value-of select="//explain/serverInfo/host"/>:<xsl:value-of select="//explain/serverInfo/port"/><xsl:value-of select="//explain/serverInfo/database"/>
        </xsl:attribute>
        
        <input type="hidden" name="query" value=""/>
        
        <table>
          <tr>
            <td><b>Record Schema:</b>
          </td>
          <td>
            <select name="recordSchema">
              <xsl:for-each select="//explain/schemaInfo/schema">
                <option>
                  <xsl:attribute name="value">
                    <xsl:value-of select="@identifier"/><br/>
                  </xsl:attribute>
                  <xsl:value-of select="title"/><br/>
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
                <xsl:value-of select='//explain/configInfo/default[@type="numberOfRecords"]'/>
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

      </table>


          <input type="submit" value="Search" onClick="return mungeForm();"/>
        </form>
      </p>
        <h3>Browse</h3>
        <p>
          Browse 
          <form id="ScanIndexes">
            <select name="scanIndex">
              <xsl:for-each select="//explain/indexInfo/index">
                <option>
                  <xsl:attribute name="value"><xsl:value-of select="map[1]/name/@indexSet"/>.<xsl:value-of select="map[1]/name"/></xsl:attribute>
                  <xsl:value-of select="title"/>
                </option>
              </xsl:for-each>
            </select>
            at
            <input name="term" type="text" value = ""/>
            </form>
            
            <form id="ScanSubmit">
              <input type="hidden" name="numberOfEntries" value="20"/>
              <input type="hidden" name="responsePosition" value="1"/>
              <input type="hidden" name="operation" value="scan"/>
              <input type="hidden" name="clause" value=""/>

              <input type="submit" value="Browse" onClick="return mungeScanForm();"/>
            </form>
            

      </p>
      </body>
    </html>
  </xsl:template>

</xsl:stylesheet>
