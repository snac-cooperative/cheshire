<?xml version='1.0'?>

<xsl:stylesheet 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:srw="http://www.loc.gov/zing/srw/v1.0/" 
  xmlns:dc="http://www.loc.gov/zing/srw/dcschema/v1.0/" 
  xmlns:diag="http://www.loc.gov/zing/srw/v1.0/diagnostic/" 
  xmlns:xsd="http://www.w3.org/2001/XMLSchema" 
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:str="http://xsltsl.org/string"
  xmlns:marc="http://www.loc.gov/marcxml/"
  version="1.0">

<xsl:import href="stdiface.xsl"/>
<xsl:import href="string.xsl"/>

<xsl:variable name="databaseURL" select="'/uol'"/>
<xsl:variable name="databaseTitle" select="'University of Liverpool Library'"/>
<xsl:variable name="defaultRecordSchema" select="'marcxml'"/>

<!-- MarcXML -->

<xsl:template match="marc:record">

  <table bgcolor="#EEEEFF" cellpadding="3" width="100%">
    <tr>
      <td colspan="2">
        <center>
          <h3>
            <xsl:apply-templates select="child::marc:datafield[@tag='245']" mode="display"/>
          </h3>
      </center>
      </td>
    </tr>
    <tr>
      <td width="30%" valign="top">
        <!-- Menu cell -->
        <table>
          <tr>
            <td><b style="color: #F65500">Browse Title</b></td>
          </tr>
          <xsl:apply-templates select="child::marc:datafield[@tag='245']" mode="scan"/>
          <tr>
            <td><b style="color: #F65500">Browse Author</b></td>
          </tr>
          <xsl:apply-templates select="child::marc:datafield[@tag='100']" mode="scan"/>
      </table>
      </td>
      <td bgcolor="#EEFDFF">
        <!-- Content cell -->
        <table>
          <!-- Author -->
          <xsl:apply-templates select="marc:datafield[starts-with(@tag,'1')]" mode="display"/>
          <!-- Other Authors -->
          <xsl:apply-templates select="marc:datafield[starts-with(@tag,'7')]"/>

          <!-- Shelfmark -->
          <xsl:apply-templates select="child::marc:datafield[@tag='090']"/>
          <!-- ISBN -->
          <xsl:apply-templates select="child::marc:datafield[@tag='020']"/>


          <!-- Series -->
          <xsl:apply-templates select="child::marc:datafield[@tag='440']|child::marc:datafield[@tag='490']"/>
          <!-- Edition -->
          <xsl:apply-templates select="child::marc:datafield[starts-with(@tag,'25')]"/>
          <!-- Imprint -->
          <xsl:apply-templates select="marc:datafield[starts-with(@tag,'26')]"/>

          <!-- Pages -->
          <xsl:apply-templates select="child::marc:datafield[@tag='300']/child::marc:subfield[@code='a']"/>
          <!-- Dimensions -->
          <xsl:apply-templates select="child::marc:datafield[@tag='300']/child::marc:subfield[@code='c']"/>
          <!-- Other Physical -->
          <xsl:apply-templates select="child::marc:datafield[@tag='300']/child::marc:subfield[@code='b']"/>

          <!-- Notes -->
          <xsl:apply-templates select="marc:datafield[starts-with(@tag,'5')]"/>

          <!-- Subjects -->
          <xsl:apply-templates select="marc:datafield[starts-with(@tag,'6')]|marc:datafield[@tag='990']"/>

          
        </table>
      </td>
    </tr>
  </table>
</xsl:template>

<!-- Thanks to LoC Marc XML XSLT -->
<xsl:template name="chopPunctuation">
  <xsl:param name="chopString"/>
  <xsl:param name="punctuation"><xsl:text>.:,;/ </xsl:text></xsl:param>
  <xsl:variable name="length" select="string-length($chopString)"/>
  <xsl:choose>
    <xsl:when test="$length=0"/>
    <xsl:when test="contains($punctuation, substring($chopString,$length,1))">
      <xsl:call-template name="chopPunctuation">
        <xsl:with-param name="chopString" select="substring($chopString,1,$length - 1)"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="not($chopString)"/>
    <xsl:otherwise><xsl:value-of select="$chopString"/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="marc:leader|marc:controlfield"/>

<xsl:template match="marc:datafield[@tag='245']" mode="scan">
  <tr>
    <td>
      <a>
        <xsl:attribute name="href">
          <xsl:value-of select="$databaseURL"/>
          <xsl:text>?operation=scan&amp;scanClause=dc.title exact "</xsl:text>
          <xsl:call-template name="str:to-lower">
            <xsl:with-param name="text">
              <xsl:value-of select="./marc:subfield[@code='a']"/>
            </xsl:with-param>
          </xsl:call-template>
          <xsl:text>"&amp;responsePosition=10&amp;maximumTerms=20</xsl:text>
        </xsl:attribute>
        <xsl:apply-templates select="." mode="display"/>
      </a>
    </td>
  </tr>
</xsl:template>

<xsl:template match="marc:datafield[@tag='245']" mode="display">
      <xsl:call-template name="chopPunctuation">
        <xsl:with-param name="chopString">
          <xsl:value-of select="concat(./marc:subfield[@code='a'], ' ', ./marc:subfield[@code='b'])"/>
        </xsl:with-param>
      </xsl:call-template>
</xsl:template>

<xsl:template match="marc:datafield[starts-with(@tag,'1')]" mode="scan">
  <a>
    <xsl:attribute name="href">
      <xsl:value-of select="$databaseURL"/>
      <xsl:text>?operation=scan&amp;scanClause=dc.creator exact "</xsl:text>
      <!-- Ye gods! -->

      <xsl:call-template name="str:subst">
        <xsl:with-param name="text">
          <xsl:call-template name="str:subst">
            <xsl:with-param name="text">
              <xsl:call-template name="str:to-lower">
                <xsl:with-param name="text">
                  <xsl:value-of select="./marc:subfield[@code='a']"/>
                </xsl:with-param>
              </xsl:call-template>
            </xsl:with-param>
            <xsl:with-param name="replace"><xsl:text>,</xsl:text></xsl:with-param>
            <xsl:with-param name="with"><xsl:text></xsl:text></xsl:with-param>
          </xsl:call-template>
        </xsl:with-param>
        <xsl:with-param name="replace"><xsl:text>.</xsl:text></xsl:with-param>
        <xsl:with-param name="with"><xsl:text></xsl:text></xsl:with-param>
      </xsl:call-template>

      <xsl:text>"&amp;responsePosition=10&amp;maximumTerms=20</xsl:text>
    </xsl:attribute>
    <xsl:value-of select="./marc:subfield[@code='a']"/>
  </a>
</xsl:template>

<xsl:template match="marc:datafield[starts-with(@tag,'1')]" mode="display">
  <tr>
    <td>
      <b>Author:</b>
    </td>
    <td>
      <xsl:call-template name="chopPunctuation">
        <xsl:with-param name="chopString">
          <xsl:value-of select="./marc:subfield[@code='a']"/>
        </xsl:with-param>
      </xsl:call-template>
    </td>
  </tr>
</xsl:template>

<xsl:template match="marc:datafield[starts-with(@tag,'7')]">
  <tr>
    <td>
      <b>Other Author:</b>
    </td>
    <td>
      <xsl:call-template name="chopPunctuation">
        <xsl:with-param name="chopString">
          <xsl:value-of select="./marc:subfield[@code='a']"/>
        </xsl:with-param>
      </xsl:call-template>
    </td>
  </tr>
</xsl:template>

<xsl:template match="marc:datafield[@tag='440']|marc:datafield[@tag='490']">
  <tr>
    <td>
      <b>Series:</b>
    </td>
    <td>
      <xsl:value-of select="./marc:subfield[@code='a']"/>
      <xsl:value-of select="./marc:subfield[@code='n']"/>
      <xsl:value-of select="./marc:subfield[@code='p']"/>
      <xsl:value-of select="./marc:subfield[@code='v']"/>
    </td>
  </tr>
</xsl:template>

<xsl:template match="marc:datafield[@tag='090']">
  <tr>
    <td>
      <b>Shelfmark:</b>
    </td>
    <td>
      <xsl:value-of select="."/>
    </td>
  </tr>
</xsl:template>

<xsl:template match="marc:datafield[@tag='020']">
  <tr>
    <td>
      <b>ISBN:</b>
    </td>
    <td>
      <xsl:value-of select="."/>
    </td>
  </tr>
</xsl:template>


<xsl:template match="marc:datafield[starts-with(@tag,'25')]">
  <tr>
    <td>
      <b>Edition:</b>
    </td>
    <td>
      <xsl:value-of select="./marc:subfield[@code='a']"/>
    </td>
  </tr>
</xsl:template>

<xsl:template match="marc:datafield[starts-with(@tag,'26')]">
  <!-- C is date, A B is publisher -->
  <xsl:if test="marc:subfield[@code='c'] and not(marc:subfield[@code='a'])">
    <tr>
      <td>
        <b>Date:</b>
      </td>
      <td>
        <xsl:value-of select="."/>
      </td>
    </tr>
  </xsl:if>
  <xsl:if test="marc:subfield[@code='a']">
    <tr>
      <td>
        <b>Publisher:</b>
      </td>
      <td>
      <xsl:call-template name="chopPunctuation">
        <xsl:with-param name="chopString">
          <xsl:value-of select="."/>
        </xsl:with-param>
      </xsl:call-template>
      </td>
    </tr>
  </xsl:if>
</xsl:template>

<xsl:template match="child::marc:datafield[@tag='300']/child::marc:subfield[@code='a']">
  <tr>
    <td>
      <b>Pages:</b>
    </td>
    <td>
      <xsl:call-template name="chopPunctuation">
        <xsl:with-param name="chopString">
          <xsl:value-of select="."/>
        </xsl:with-param>
      </xsl:call-template>
    </td>
  </tr>
</xsl:template>

<xsl:template match="child::marc:datafield[@tag='300']/child::marc:subfield[@code='c']">
  <tr>
    <td>
      <b>Dimensions:</b>
    </td>
    <td>
      <xsl:call-template name="chopPunctuation">
        <xsl:with-param name="chopString">
          <xsl:value-of select="."/>
        </xsl:with-param>
      </xsl:call-template>
    </td>
  </tr>
</xsl:template>

<xsl:template match="child::marc:datafield[@tag='300']/child::marc:subfield[@code='b']">
  <tr>
    <td>
      <b>Physical Description:</b>
    </td>
    <td>
      <xsl:call-template name="str:subst">
        <xsl:with-param name="text">
          <xsl:value-of select="."/>
        </xsl:with-param>
        <xsl:with-param name="replace"><xsl:text>ill.</xsl:text></xsl:with-param>
        <xsl:with-param name="with">
          Illuminated
        </xsl:with-param>
      </xsl:call-template>
      
    </td>
  </tr>
</xsl:template>

<xsl:template match="marc:datafield[starts-with(@tag,'5')]">
  <!-- Holy Shades of MUSHcode Batman! -->
  <xsl:if test="contains('500X503X504X509X554X546X590X591X', concat(@tag, 'X'))">
    <tr>
      <td>
        <b>Note:</b>
      </td>
      <td>
        <xsl:value-of select="."/>
      </td>
    </tr>
  </xsl:if>
</xsl:template>

<xsl:template match="marc:datafield[starts-with(@tag,'6')]|marc:datafield[@tag='990']">
    <tr>
      <td>
        <b>Subject:</b>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@tag='650' or @tag='690' or @tag='610'">
            <xsl:for-each select="./marc:subfield">
              <xsl:call-template name="subject"/>
            </xsl:for-each>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="."/>
          </xsl:otherwise>
        </xsl:choose>
      </td>
    </tr>
</xsl:template>

<xsl:template name="subject">
  <xsl:value-of select="."/>
  <xsl:if test="not(position()=last())">
    <xsl:text> -- </xsl:text>
  </xsl:if>
</xsl:template>




<!-- Dublin Core -->

<xsl:template match="dc:dc">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="dc:*">
  <xsl:if test="not(name()=dc)">
    <tr>
      <td align="right" width="25%" valign="top">
        <b>
          <xsl:call-template name="str:capitalise">
            <xsl:with-param name="text">
              <xsl:value-of select="name()"/>
            </xsl:with-param>
          </xsl:call-template>
        </b>
        :<xsl:text> </xsl:text> 
      </td>
      <td>
        <xsl:text> </xsl:text> <xsl:value-of select="."/>
      </td>
    </tr>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
