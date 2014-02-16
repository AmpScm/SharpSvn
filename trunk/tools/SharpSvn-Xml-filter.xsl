﻿<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" />
    <xsl:template match="/doc">
      <doc>
        <assembly>
          <name>
          <xsl:value-of select="normalize-space(translate(assembly,'&quot;',''))"/>
          </name>
        </assembly>
        <members>
          <xsl:for-each select="members/member[contains(@name, ':SharpSvn.')]">
            <member name="{@name}">
              <xsl:copy-of select="*"/>
              <xsl:if test="starts-with(@name, 'T:') and not(threadsafety)">
                <threadsafety static="true" instance="false" />
              </xsl:if>
            </member>
          </xsl:for-each>
        </members>
      </doc>
    </xsl:template>
</xsl:stylesheet>
