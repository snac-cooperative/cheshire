<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:srw_dc="info:srw/schema/1/dc-v1.1" 
  xmlns:card="http://srw.o-r-g.org/schemas/ccg/1.0/" 
  xmlns:dc="http://purl.org/dc/elements/1.1/" version="1.0">

  <xsl:template match="card">
    <srw_dc:dc>
      <xsl:apply-templates/>
    </srw_dc:dc>
  </xsl:template>

  <xsl:template match="name">
    <dc:title>
      <xsl:value-of select="."/>
    </dc:title>
  </xsl:template>
  <xsl:template match="type">
    <dc:type>
      <xsl:value-of select="."/>
    </dc:type>
  </xsl:template>
  <xsl:template match="text[@type='main']">
    <dc:description>
      <xsl:value-of select="."/>
    </dc:description>
  </xsl:template>
  <xsl:template match="artist">
    <dc:creator>
      <xsl:value-of select="."/>
    </dc:creator>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
