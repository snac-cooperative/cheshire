<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:srw="http://www.loc.gov/zing/srw/v1.0/" xmlns:dc="http://www.loc.gov/zing/srw/dcschema/v1.0/" xmlns:card="http://srw.o-r-g.org/schemas/ccg/1.0/" xmlns:diag="http://www.loc.gov/zing/srw/v1.0/diagnostic/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0">

<xsl:import href="http://srw.o-r-g.org:8080/stdiface.xsl"/>

<xsl:variable name="databaseURL" select="'/l5r'"/>
<xsl:variable name="databaseTitle" select="'L5R Card Database'"/>

<!-- Dublin Core -->

<xsl:template match="dc:dc|card:card">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="dc:*">
  <xsl:if test="not(name()=dc)">
    <tr><td align="right" width="25%" valign="top"><b><xsl:value-of select="name()"/></b>:<xsl:text> </xsl:text> </td><td><xsl:text> </xsl:text> <xsl:value-of select="."/></td></tr>
  </xsl:if>
</xsl:template>

<!-- Card Schema -->

<xsl:template match="card:name">
  <tr>
    <td colspan="2">
      <center><h4><xsl:value-of select="."/></h4></center>
    </td>
  </tr>
</xsl:template>

<xsl:template match="card:type">
  <tr>
    <td width="15%" align="right">
      <b>Type:</b>
    </td>
    <td>
      <xsl:value-of select="."/>
    </td>
  </tr>
</xsl:template>

<xsl:template match="card:text[@type='main']">
  <tr>
    <td valign="top" align="right"><b>Card Text:</b></td><td><xsl:value-of select="."/></td>
  </tr>
</xsl:template>

<xsl:template match="card:text[@type='flavour']">
  <xsl:if test="not(.='')">
    <tr>
      <td valign="top" align="right"><b>Flavour Text:</b></td><td><i><xsl:value-of select="."/></i></td>
    </tr>
  </xsl:if>
</xsl:template>

<xsl:template match="card:text[@type='errata']">
  <xsl:if test="not(.='')">
    <tr>
      <td valign="top" align="right"><b>Errata:</b></td><td><xsl:value-of select="."/></td>
    </tr>
  </xsl:if>
</xsl:template>

<xsl:template match="card:artist">
  <tr>
    <td valign="top" align="right"><b>Artist:</b></td><td><xsl:value-of select="."/></td>
  </tr>
</xsl:template>

<xsl:template match="card:statistics|card:costs">
  <xsl:if test="./card:cost">
    <tr><td colspan="2"><center><b>Costs</b></center></td></tr>
  </xsl:if>
  <xsl:if test="./card:stat">
    <tr><td colspan="2"><center><b>Statistics</b></center></td></tr>
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="card:artist">
  <tr>
    <td valign="top" align="right"><b>Artist:</b></td><td><xsl:value-of select="."/></td>
  </tr>
</xsl:template>

<xsl:template match="card:cost|card:stat">
  <tr>
    <td valign="top" align="right"><b><xsl:value-of select="@type"/>:</b></td><td><xsl:value-of select="."/></td>
  </tr>
</xsl:template>

<!-- For the moment don't display them -->
<xsl:template match="card:gameinfo|card:collection">
</xsl:template>

</xsl:stylesheet>
