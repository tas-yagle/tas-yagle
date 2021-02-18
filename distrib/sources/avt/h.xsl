<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<xsl:output method="text" indent="yes"/>

<xsl:template match="/notice">/****************************************************************************/
/*                                                                          */
/*                            LIB ERROR MESSAGES                            */
/*                                                                          */
/*    Fichier : all_errmsg.h                                                */
/*    WARNING!!! This file is generated from XML with java - do not edit    */
/*                                                                          */
/****************************************************************************/
#include &lt;stdlib.h&gt;

<xsl:apply-templates select="//messages"/>
</xsl:template>

<xsl:template name="vartab">
<xsl:param name="last_err"/>
char *<xsl:value-of select="@libname" />_errmsgtab[] = {<xsl:for-each select="err">
    <xsl:variable name="err_num"><xsl:number count="err" from="/lib" /></xsl:variable>
    <xsl:apply-templates select="num" /> <xsl:apply-templates select="msg"><xsl:with-param name="last_err" select="$last_err"/><xsl:with-param name="err_num" select="$err_num"/></xsl:apply-templates>
<xsl:if test="number($err_num) = number($last_err)">
    };
</xsl:if>
</xsl:for-each>
</xsl:template>

<xsl:template match="messages">
<xsl:variable name="nb_err"><xsl:value-of select="count(./err)"/></xsl:variable>

<xsl:call-template name="vartab"><xsl:with-param name="last_err" select="$nb_err"/></xsl:call-template>

</xsl:template>

<xsl:template match="num">
    "<xsl:apply-templates/>", </xsl:template>

<xsl:template match="msg">
<xsl:param name="last_err"/><xsl:param name="err_num"/>"<xsl:apply-templates/>",<xsl:if test="number($err_num) = number($last_err)">
    NULL</xsl:if> 
</xsl:template>

<xsl:template match="doc">
    <xsl:apply-templates/> 
</xsl:template>

</xsl:stylesheet>
