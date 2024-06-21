<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:output method="xml" indent="yes" />
    <xsl:include href="file.xsl" />

    <!-- ######################################################### -->
    <!-- #                       Pagination                      # -->
    <!-- ######################################################### -->

    <xsl:template match="/">
        <fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format">
            <xsl:call-template name="page-sequence-def" />
            <xsl:call-template name="bookmarks" />
            <xsl:call-template name="first-page" />
            <xsl:call-template name="doc-introduction" />
            <xsl:call-template name="book" />
        </fo:root>
    </xsl:template>
    
    <!-- Document Page Setup -->
    <xsl:template name="page-sequence-def">
        <fo:layout-master-set>
            <fo:simple-page-master master-name="cover" page-height="29.7cm" page-width="21cm" margin-top="0.5cm" margin-bottom="0.5cm"
            margin-left="2cm" margin-right="2cm">
                <fo:region-body margin-top="2cm" />
            </fo:simple-page-master>
            <fo:simple-page-master master-name="leftPage" page-height="29.7cm" page-width="21cm" margin-top="1.2cm" margin-bottom="1.4cm"
            margin-left="2cm" margin-right="2cm">
                <fo:region-body margin-top="2cm" margin-bottom="1.5cm" />
                <fo:region-before extent="1.2cm" />
                <fo:region-after extent="1.3cm" />
            </fo:simple-page-master>
            <fo:simple-page-master master-name="rightPage" page-height="29.7cm" page-width="21cm" margin-top="1.2cm" margin-bottom="1.4cm"
            margin-left="2cm" margin-right="2cm">
                <fo:region-body margin-top="2cm" margin-bottom="1.5cm" />
                <fo:region-before extent="1.2cm" />
                <fo:region-after extent="1.3cm" />
            </fo:simple-page-master>
            <fo:page-sequence-master master-name="contents">
                <fo:repeatable-page-master-alternatives>
                    <fo:conditional-page-master-reference master-reference="leftPage" odd-or-even="even" />
                    <fo:conditional-page-master-reference master-reference="rightPage" odd-or-even="odd" />
                </fo:repeatable-page-master-alternatives>
            </fo:page-sequence-master>
        </fo:layout-master-set>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                       BOOKMARKS                       # -->
    <!-- ######################################################### -->

    <xsl:template name="bookmarks">
        <fo:bookmark-tree>
            <xsl:apply-templates select="//section[@niv='1']/title" mode="book" />
        </fo:bookmark-tree>
    </xsl:template>
    <xsl:template mode="book" match="section/title">
        <xsl:variable name="doc_level" select="../@niv" />
        <fo:bookmark internal-destination="{generate-id(.)}">
            <fo:bookmark-title>
                <xsl:call-template name="title_num">
                    <xsl:with-param name="level" select="$doc_level" />
                    <xsl:with-param name="type" select="doc" />
                </xsl:call-template>
            </fo:bookmark-title>
            <xsl:call-template name="title_bookmarks">
                <xsl:with-param name="book_level">
                    <xsl:number value="$doc_level + 1" />
                </xsl:with-param>
            </xsl:call-template>
        </fo:bookmark>
    </xsl:template>
    <xsl:template name="title_bookmarks">
        <xsl:param name="book_level" select="2" />
        <xsl:if test="..//section[@niv = $book_level]/title">
            <xsl:for-each select="..//section[@niv = $book_level]/title">
                <fo:bookmark internal-destination="{generate-id(.)}">
                    <fo:bookmark-title>
                        <xsl:call-template name="title_num">
                            <xsl:with-param name="level" select="../@niv" />
                            <xsl:with-param name="type" select="doc" />
                        </xsl:call-template>
                    </fo:bookmark-title>
                    <xsl:call-template name="title_bookmarks">
                        <xsl:with-param name="book_level">
                            <xsl:number value="$book_level + 1" />
                        </xsl:with-param>
                    </xsl:call-template>
                </fo:bookmark>
            </xsl:for-each>
        </xsl:if>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                       FIRST PAGE                      # -->
    <!-- ######################################################### -->

    <!-- Release version -->
    <xsl:template match="version">
        <fo:block font-family="Helvetica" font-style="normal" font-weight="normal" font-size="20pt" text-align="right" space-after="5mm">
        Software Release 
        <xsl:value-of select="$release_num" />
        <xsl:apply-templates /></fo:block>
    </xsl:template>

    <!-- Information copyright / footer -->
    <xsl:template mode="footer" match="legal">
        <fo:block font-family="Helvetica" font-size="8pt" text-align="right">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Tool name -->
    <xsl:template match="tool">
        <fo:block font-family="Helvetica" font-style="normal" font-weight="normal" font-size="18pt" text-align="right">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Logo -->
    <xsl:template match="logo">
        <xsl:call-template name="image">
            <xsl:with-param name="name" select="@namepdf" />
            <xsl:with-param name="height">41pt</xsl:with-param>
            <xsl:with-param name="width">61pt</xsl:with-param>
            <xsl:with-param name="align">left</xsl:with-param>
        </xsl:call-template>
    </xsl:template>

    <!-- Cover Title -->
    <xsl:template mode="page" match="booktitle">
        <fo:block font-family="Helvetica" font-style="normal" font-weight="bold" font-size="36pt">
            <xsl:apply-templates />
        </fo:block>
        <xsl:if test="../titleextension">
            <fo:block font-family="Helvetica" font-style="normal" font-weight="bold" font-size="36pt">
                <xsl:apply-templates select="../titleextension" />
            </fo:block>
        </xsl:if>
    </xsl:template>

    <!-- Date -->
    <xsl:template match="date">
        <fo:block font-family="Helvetica" font-style="normal" font-weight="normal" font-size="20pt" text-align="right">
            <xsl:value-of select="$date" />
        </fo:block>
    </xsl:template>

    <!-- Cover page -->
    <xsl:template name="first-page">
        <fo:page-sequence master-reference="cover">
            <fo:flow flow-name="xsl-region-body">
                <fo:block font-family="Helvetica" font-style="normal" font-weight="bold" font-size="36pt" text-align="center">Avertec
                Tools</fo:block>
                <fo:block space-after="3cm">
                    <fo:leader leader-pattern="rule" leader-length="17cm" rule-thickness="1pt" />
                </fo:block>
                <fo:table table-layout="fixed" width="17cm">
                    <fo:table-column column-width="17cm" />
                    <fo:table-body>
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:block text-align="right">
                                    <xsl:apply-templates mode="page" select="//booktitle" />
                                </fo:block>
                                <fo:block text-align="right" space-before="0.8cm" space-after="1.4cm">
                                    <fo:external-graphic src="file:{$pictpdf}/Photo_yagle.jpg" height="260pt" width="auto"
                                    content-height="220pt" content-width="auto" />
                                </fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                    </fo:table-body>
                </fo:table>
                <fo:block space-after="0.1cm">
                    <xsl:apply-templates select="//tool" />
                </fo:block>
                <fo:block space-after="2pt">
                    <xsl:apply-templates select="//version" />
                </fo:block>
                <fo:block space-after="2.7cm">
                    <xsl:apply-templates select="//date" />
                </fo:block>
                <fo:block space-after="0.6cm">
                    <xsl:apply-templates select="//logo" />
                </fo:block>
                <fo:block font-family="Helvetica" font-size="14pt" font-style="normal" font-weight="normal" text-align="left"
                space-after="2pt" break-after="page">
                    <xsl:value-of select="//legal" />
                </fo:block>
            </fo:flow>
        </fo:page-sequence>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                      INTRODUCTION                     # -->
    <!-- ######################################################### -->

    <!-- page break -->
    <xsl:template match="introduction">
        <fo:block break-before="page">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <xsl:template match="intro">
        <fo:block font-size="15pt" font-family="Times" font-style="normal" font-weight="normal" text-align="justify">
            <xsl:apply-templates />
        </fo:block>
        <fo:block space-before="12.5cm" text-align="center" font-family="Helvetica" font-size="12pt" font-style="normal"
        font-weight="normal">Documentation issued and compliant with Avertec Tools Release 
        <xsl:value-of select="$release_num" />.</fo:block>
        <fo:block space-before="1cm" text-align="center" font-family="Helvetica" font-size="12pt" font-style="normal" font-weight="normal">
        Please contact support@avertec.com for comments relating to this manual.</fo:block>
    </xsl:template>
    <xsl:template match="intro_title">
        <fo:block text-align="right" space-after="40pt" font-family="Helvetica" font-style="normal" font-weight="bold" font-size="26pt"
        color="grey">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Introduction page -->
    <xsl:template name="doc-introduction">
        <fo:page-sequence master-reference="contents" initial-page-number="0">
            <fo:static-content flow-name="xsl-region-before">
                <fo:block font-family="Helvetica" font-size="10pt" text-align="right">
                    <xsl:apply-templates mode="banner" select="//booktitle" />
                </fo:block>
                <fo:block text-align='center'>
                    <fo:leader leader-pattern="rule" leader-length="17cm" />
                </fo:block>
            </fo:static-content>
            <fo:static-content flow-name="xsl-region-after">
                <fo:block text-align='center'>
                    <fo:leader leader-pattern="rule" leader-length="17cm" />
                </fo:block>
                <fo:block text-align="center">
                    <fo:page-number />
                </fo:block>
            </fo:static-content>
            <fo:flow flow-name="xsl-region-body">
                <xsl:apply-templates select="//intro_title" />
                <xsl:apply-templates select="//intro" />
            </fo:flow>
        </fo:page-sequence>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                    Table of Contents                  # -->
    <!-- ######################################################### -->

    <xsl:template name="toc">
        <fo:block text-align="right" space-after="52pt" id="tof" font-family="Helvetica" font-size="26pt" font-weight="bold" color="grey">
        Table of Contents</fo:block>
        <fo:block text-align="justify">
            <xsl:apply-templates mode="toc" select="//title" />
            <fo:block space-after="12pt" text-indent="0.3in" text-align-last="justify">
                <fo:basic-link internal-destination="{generate-id(.)}">Index</fo:basic-link>
                <fo:leader leader-pattern="dots" />
                <fo:page-number-citation ref-id="{generate-id(.)}" />
            </fo:block>
        </fo:block>
    </xsl:template>

    <!-- titles  -->
    <xsl:template mode="toc" match="section/title">
        <xsl:variable name="doc_level" select="../@niv" />
        <fo:block text-indent="0.3in" text-align-last="justify">
            <fo:basic-link internal-destination="{generate-id(.)}">
                <xsl:call-template name="title_num">
                    <xsl:with-param name="level" select="$doc_level" />
                    <xsl:with-param name="type" select="toc" />
                </xsl:call-template>
            </fo:basic-link>
            <fo:leader leader-pattern="dots" />
            <fo:page-number-citation ref-id="{generate-id(.)}" />
        </fo:block>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                    Title Numbering                    # -->
    <!-- ######################################################### -->

    <xsl:template name="title_num">
        <xsl:param name="level" select="1" />
        <xsl:param name="type" select="doc" />
        <xsl:choose>
			<xsl:when test="$level = 1"><xsl:number count="//section[@niv='1']/title" level="any"/>.<xsl:text>  </xsl:text></xsl:when>
			<xsl:when test="$level = 2"><xsl:number count="//section[@niv='1']/title" level="any"/>.<xsl:number count="//section[@niv='2']/title" from="//section[@niv='1']" level="any"/>.  </xsl:when>
			<xsl:when test="$level = 3"><xsl:number count="//section[@niv='1']/title" level="any"/>.<xsl:number count="//section[@niv='2']/title" from="//section[@niv='1']" level="any"/>.<xsl:number count="//section[@niv='3']/title" from="//section[@niv='2']" level="any" format="1"/>.  </xsl:when>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="$type = toc">
                <xsl:choose>
                    <xsl:when test="not(./index)">
                        <xsl:apply-templates />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:apply-templates mode="toc" select="./index" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <xsl:when test="not(./index)">
                        <xsl:value-of select="." />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:apply-templates mode="title" select="./index" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                    Index                              # -->
    <!-- ######################################################### -->

    <!-- Index des variables dans le document / plusieurs mod disponibles -->
    <xsl:template mode="title" match="//index">
        <xsl:value-of select="text()" />
    </xsl:template>
    <xsl:template mode="toc" match="//index">
        <fo:block text-indent="0.3in" text-align-last="justify">
            <fo:basic-link internal-destination="{generate-id(.)}">
                <xsl:value-of select="text()" />
            </fo:basic-link>
            <fo:leader leader-pattern="dots" />
            <fo:page-number-citation ref-id="{generate-id(.)}" />
        </fo:block>
    </xsl:template>
    <xsl:template mode="iov" match="//index">
        <fo:block space-after="10pt" text-align-last="justify" text-indent="0.6in" font-weight="bold" font-style="normal">
            <xsl:choose>
                <xsl:when test="../../title">
                    <fo:basic-link internal-destination="{generate-id(..)}">
                        <xsl:apply-templates />
                    </fo:basic-link>
                    <fo:leader leader-pattern="dots" />
                    <fo:page-number-citation ref-id="{generate-id(..)}" />
                </xsl:when>
                <xsl:otherwise>
                    <fo:basic-link internal-destination="{generate-id(.)}">
                        <xsl:apply-templates />
                    </fo:basic-link>
                    <fo:leader leader-pattern="dots" />
                    <fo:page-number-citation ref-id="{generate-id(.)}" />
                </xsl:otherwise>
            </xsl:choose>
        </fo:block>
    </xsl:template>
    <xsl:template match="//index">
        <xsl:param name="in">
            <fo:page-number />
        </xsl:param>
        <xsl:value-of select="item[number($in)]" />
        <fo:block id="{generate-id(.)}">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Mise en forme de l'index -->
    <xsl:template name="doc_index">
        <fo:block break-before="page" text-align="right" space-after="52pt" id="iov" font-family="Helvetica" font-size="16pt"
        font-weight="bold">
            <fo:block font-family="Times" font-style="normal" font-weight="bold" id="{generate-id(.)}">Index</fo:block>
        </fo:block>
        <xsl:if test="//index">
            <fo:block text-align="justify">
                <xsl:apply-templates mode="iov" select="//index">
                    <xsl:sort order="ascending" data-type="text" case-order="upper-first" lang="en" />
                </xsl:apply-templates>
            </fo:block>
        </xsl:if>
        <xsl:if test="not(//index)">
            <fo:block space-before="4.5cm" text-align="center" font-family="Helvetica" font-style="normal" font-weight="bold"
            font-size="12pt">No index for this document.</fo:block>
        </xsl:if>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                      MAIN PART                        # -->
    <!-- ######################################################### -->

    <xsl:template name="book">
        <fo:page-sequence master-reference="contents">
            <fo:static-content flow-name="xsl-region-before">
                <fo:block font-family="Helvetica" font-size="10pt" text-align="right">
                    <xsl:apply-templates mode="banner" select="//booktitle" />
                </fo:block>
                <fo:block text-align='center'>
                    <fo:leader leader-pattern="rule" leader-length="17cm" />
                </fo:block>
            </fo:static-content>
            <fo:static-content flow-name="xsl-region-after">
                <fo:block text-align='center'>
                    <fo:leader leader-pattern="rule" leader-length="17cm" />
                </fo:block>
                <fo:block text-align="center">
                    <fo:page-number />
                </fo:block>
            </fo:static-content>
            <fo:flow flow-name="xsl-region-body">
                <xsl:call-template name="toc" />
                <xsl:apply-templates select="//chapter" />
                <xsl:call-template name="doc_index" />
            </fo:flow>
        </fo:page-sequence>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                    HEADINGS                           # -->
    <!-- ######################################################### -->

    <!-- pagebreak on new chapter  -->
    <xsl:template match="chapter">
        <fo:block break-before="page">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- sections -->
    <xsl:template match="section">
        <xsl:choose>
            <xsl:when test="@niv &lt; 6">
                <fo:block>
                    <xsl:apply-templates />
                </fo:block>
            </xsl:when>
            <xsl:otherwise>
                <fo:block keep-together.within-page="always">
                    <xsl:apply-templates />
                </fo:block>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- titles -->
    <xsl:template match="section/title">
        <xsl:param name="n">
            <fo:page-number />
        </xsl:param>
        <xsl:variable name="doc_level" select="../@niv" />
        <xsl:choose>
            <xsl:when test="$doc_level = 1">
                <fo:block id="{generate-id(.)}" space-before="18pt" space-after="36pt" font-family="Helvetica" color="grey"
                font-size="26pt" font-style="normal" font-weight="bold" text-align="left" keep-with-next.within-page="always">Chapter 
                <xsl:call-template name="title_num">
                    <xsl:with-param name="level" select="$doc_level" />
                    <xsl:with-param name="type" select="doc" />
                </xsl:call-template></fo:block>
                <xsl:value-of select="item[number($n)]" />
            </xsl:when>
            <xsl:when test="$doc_level = 2">
                <fo:block id="{generate-id(.)}" space-before="14pt" space-after="14pt" font-family="Helvetica" color="grey"
                font-size="20pt" font-style="normal" font-weight="bold" text-align="left" keep-with-next.within-page="always">
                    <xsl:call-template name="title_num">
                        <xsl:with-param name="level" select="$doc_level" />
                        <xsl:with-param name="type" select="doc" />
                    </xsl:call-template>
                </fo:block>
            </xsl:when>
            <xsl:when test="$doc_level = 3">
                <fo:block id="{generate-id(.)}" space-before="10pt" space-after="10pt" font-family="Helvetica" font-size="16pt"
                font-style="normal" font-weight="bold" text-align="left" keep-with-next.within-page="always">
                    <xsl:call-template name="title_num">
                        <xsl:with-param name="level" select="$doc_level" />
                        <xsl:with-param name="type" select="doc" />
                    </xsl:call-template>
                </fo:block>
            </xsl:when>
            <xsl:when test="$doc_level = 4">
                <fo:block id="{generate-id(.)}" space-before="8pt" space-after="8pt" font-family="Helvetica" font-size="12pt"
                font-style="normal" font-weight="bold" text-align="left" keep-with-next.within-page="always">
                    <xsl:call-template name="title_num">
                        <xsl:with-param name="level" select="$doc_level" />
                        <xsl:with-param name="type" select="doc" />
                    </xsl:call-template>
                </fo:block>
            </xsl:when>
        </xsl:choose>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                    List Styles                        # -->
    <!-- ######################################################### -->

    <!-- Array with article and 2-line definition -->
    <xsl:template priority="1" match="//row[@type='split']">
        <fo:table-row start-indent="0cm" keep-with-next.within-page="always">
            <fo:table-cell number-columns-spanned="2">
                <fo:block font-family="Times" text-align="justify" font-style="normal" font-weight="bold">
                    <xsl:apply-templates select="article" />
                </fo:block>
            </fo:table-cell>
        </fo:table-row>
        <fo:table-row start-indent="0cm">
            <fo:table-cell column-number="2" padding-after="10pt">
                <fo:block font-family="Times" text-align="justify" font-style="normal" font-weight="normal" space-after.optimum="10pt">
                    <xsl:apply-templates select="def" />
                </fo:block>
            </fo:table-cell>
        </fo:table-row>
    </xsl:template>

    <!-- Defaulted array lines -->
    <xsl:template match="//row">
        <fo:table-row start-indent="0cm">
            <fo:table-cell padding-after="10pt">
                <fo:block font-family="Times" text-align="justify" font-style="normal" font-weight="bold">
                    <xsl:apply-templates select="article" />
                </fo:block>
            </fo:table-cell>
            <fo:table-cell padding-after="10pt">
                <fo:block font-family="Times" text-align="justify" font-style="normal" font-weight="normal" space-after.optimum="10pt">
                    <xsl:apply-templates select="def" />
                </fo:block>
            </fo:table-cell>
        </fo:table-row>
    </xsl:template>

    <!-- Array lines displaying a picture as article -->
    <xsl:template mode="button" match="//row">
        <fo:table-row start-indent="0cm">
            <fo:table-cell>
                <fo:block text-align="right" font-family="Times" font-style="normal" font-weight="bold" vertical-align="middle">
                    <xsl:apply-templates select="article" />
                </fo:block>
            </fo:table-cell>
            <fo:table-cell>
                <fo:block text-align="justify" font-family="Times" font-style="normal" font-weight="normal" space-before="12pt"
                vertical-align="middle">
                    <xsl:apply-templates select="def" />
                </fo:block>
            </fo:table-cell>
        </fo:table-row>
    </xsl:template>

    <!-- List -->
    <xsl:template match="//list">
        <fo:block space-before="6pt" space-after="6pt">
            <xsl:apply-templates select="article" />
            <fo:list-block start-indent="1cm">
                <xsl:apply-templates select="item" />
            </fo:list-block>
        </fo:block>
    </xsl:template>

    <!-- List within a array -->
    <xsl:template mode="table" match="//list">
        <fo:block space-before="6pt" space-after="6pt" keep-together="always">
            <xsl:apply-templates select="article" />
            <fo:list-block>
                <xsl:apply-templates mode="table" select="item" />
            </fo:list-block>
        </fo:block>
    </xsl:template>

    <!-- Items of a list within a array -->
    <xsl:template mode="table" match="item">
        <fo:list-item space-before="4pt">
            <fo:list-item-label>
                <fo:block>
                    <xsl:text>&#8226;</xsl:text>
                </fo:block>
            </fo:list-item-label>
            <fo:list-item-body>
                <fo:block font-family="Times" text-align="justify" font-weight="normal" font-style="normal">
                    <xsl:apply-templates />
                </fo:block>
            </fo:list-item-body>
        </fo:list-item>
    </xsl:template>

    <!-- Items of an indented list -->
    <xsl:template match="item">
        <fo:list-item space-before="4pt">
            <fo:list-item-label>
                <fo:block>
                    <xsl:text>&#8226;</xsl:text>
                </fo:block>
            </fo:list-item-label>
            <fo:list-item-body>
                <fo:block start-indent="0.6in" font-family="Times" text-align="justify" font-weight="normal" font-style="normal">
                    <xsl:apply-templates />
                </fo:block>
            </fo:list-item-body>
        </fo:list-item>
    </xsl:template>

    <!-- Glossary -->
    <xsl:template match="//glossary">
        <xsl:if test="./row[@type='half']">
            <fo:block space-before="12pt">
                <fo:table table-layout="fixed" width="16cm" start-indent="1cm">
                    <fo:table-column column-width="8cm" />
                    <fo:table-column column-width="8cm" />
                    <fo:table-body>
                        <xsl:apply-templates />
                    </fo:table-body>
                </fo:table>
            </fo:block>
        </xsl:if>
        <xsl:if test="./row[@type='split']">
            <fo:block space-before="6pt" space-after="6pt">
                <fo:table table-layout="fixed" width="16cm" start-indent="1cm">
                    <fo:table-column column-width="1.5cm" />
                    <fo:table-column column-width="14.5cm" />
                    <fo:table-body>
                        <xsl:apply-templates />
                    </fo:table-body>
                </fo:table>
            </fo:block>
        </xsl:if>
        <xsl:if test="not(./row[@type='half']) and not (./row[@type='split'])">
            <fo:block space-before="6pt" space-after="6pt">
                <fo:table table-layout="fixed" width="16cm" start-indent="1cm">
                    <fo:table-column column-width="6cm" />
                    <fo:table-column column-width="10cm" />
                    <fo:table-body>
                        <xsl:apply-templates />
                    </fo:table-body>
                </fo:table>
            </fo:block>
        </xsl:if>
    </xsl:template>

    <!-- Definition de table / format reduit -->
    <xsl:template priority="1" match="//glossary[@width='small']">
        <fo:block space-before="6pt" keep-together.within-page="always">
            <fo:table table-layout="fixed" width="14.5cm">
                <fo:table-column column-width="3.5cm" />
                <fo:table-column column-width="11cm" />
                <fo:table-body>
                    <xsl:apply-templates />
                </fo:table-body>
            </fo:table>
        </fo:block>
    </xsl:template>
    <xsl:template priority="1" match="//glossary[@width='small2']">
        <fo:block space-before="6pt" keep-together.within-page="always">
            <fo:table table-layout="fixed" width="14.5cm">
                <fo:table-column column-width="5.5cm" />
                <fo:table-column column-width="9cm" />
                <fo:table-body>
                    <xsl:apply-templates />
                </fo:table-body>
            </fo:table>
        </fo:block>
    </xsl:template>

    <!-- article d'un tableau -->
    <xsl:template match="article">
        <fo:block id="{generate-id(.)}" font-family="Times" font-style="normal" font-weight="bold">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- partie gauche d'un tableau -->
    <xsl:template match="def">
        <fo:block font-family="Times" font-style="normal" font-weight="normal">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                    Text Styles                        # -->
    <!-- ######################################################### -->

    <!-- page blanche en fin de document -->
    <xsl:template match="//last">
        <fo:block break-before="page" text-align="center">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Saut de page force -->
    <xsl:template match="//pagebreak">
        <fo:block break-after="page">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Paragraphe -->
    <xsl:template match="//d">
        <fo:block start-indent="0.4in" space-after="12pt" space-before="12pt" font-family="Times" font-size="12pt" font-style="normal"
        font-weight="normal" text-align="justify" keep-together.within-page="always">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Paragraphe -->
    <xsl:template match="//p">
        <fo:block space-after="6pt" font-family="Times" font-size="12pt" font-style="normal" font-weight="normal" text-align="justify"
        keep-together.within-page="always">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Paragraphe -->
    <xsl:template match="//br">
        <fo:block></fo:block>
        <xsl:apply-templates />
    </xsl:template>

    <!-- Titre de l'ouvrage en-tete/pied de page -->
    <xsl:template mode="banner" match="booktitle">
        <fo:block font-family="Helvetica" font-style="normal" font-weight="normal" font-size="12pt">
            <xsl:apply-templates />
            <xsl:if test="../titleextension">&#160;
            <xsl:apply-templates select="../titleextension" /></xsl:if>
        </fo:block>
    </xsl:template>

    <!-- ligne d'un code / conserve la mise en forme -->
    <xsl:template match="//cl">
        <fo:block white-space="pre">
            <xsl:apply-templates />
        </fo:block>
    </xsl:template>

    <!-- Delimite une partie de code deja mis en forme / defaut est identique a type=box -->
    <xsl:template match="//bnf">
        <fo:block font-family="Courier" font-size="10pt" space-after="12pt" font-style="normal" font-weight="normal" color="blue"
        start-indent="0.4in" space-before="12pt" keep-together="always">
            <fo:block>
                <xsl:apply-templates />
            </fo:block>
        </fo:block>
    </xsl:template>

    <!-- Delimite une partie de code deja mis en forme / defaut est identique a type=box -->
    <xsl:template match="//code">
        <fo:block font-family="Courier" font-size="9pt" space-after="12pt" font-style="normal" font-weight="normal" start-indent="0.4in"
        space-before="12pt" widows="4" orphans="4">
            <fo:block>
                <xsl:apply-templates />
            </fo:block>
        </fo:block>
    </xsl:template>

    <!-- Delimite une partie de code deja mis en forme / defaut est identique a type=box -->
    <xsl:template match="//smallcode">
        <fo:block font-family="Courier" font-size="8pt" space-after="12pt" font-style="normal" font-weight="normal" space-before="12pt"
        widows="4" orphans="4">
            <fo:block>
                <xsl:apply-templates />
            </fo:block>
        </fo:block>
    </xsl:template>

    <!-- partie document / positionnement d'indentation et de font size par defaut du document -->
    <xsl:template match="document">
        <xsl:apply-templates />
    </xsl:template>

    <!--  mise en forme dans une ligne -->
    <xsl:template match="//f">
        <fo:inline font-family="Courier" font-size="10pt" font-style="normal" font-weight="normal">
            <xsl:apply-templates />
        </fo:inline>
    </xsl:template>

    <!--  mise en forme dans une ligne -->
    <xsl:template match="//b">
        <fo:inline font-family="Helvetica" font-size="11pt" font-style="normal" font-weight="bold">
            <xsl:apply-templates />
        </fo:inline>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                  FUNCTIONS                            # -->
    <!-- ######################################################### -->

    <xsl:template match="//functions">
        <fo:block space-before="12pt">
            <fo:table table-layout="fixed" width="17cm">
                <fo:table-column column-width="17cm" />
                <fo:table-body>
                    <xsl:for-each select="func">
                        <!-- funcname -->
                        <fo:table-row>
                            <fo:table-cell padding-before="12pt">
                                <fo:block font-family="Courier" font-style="normal" font-size="13pt" font-weight="bold"
                                text-align="left" space-before="12pt">
                                    <xsl:apply-templates select="funcname" />
                                </fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                        <!-- proto -->
                        <fo:table-row>
                            <fo:table-cell padding-before="12pt">
                                <fo:block font-family="Courier" font-style="normal" font-size="11pt" font-weight="normal"
                                text-align="left" space-before="12pt">
                                    <xsl:apply-templates select="proto" />
                                </fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                        <!-- desc -->
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:table table-layout="fixed" width="17cm">
                                    <fo:table-column column-width="4.5cm" />
                                    <fo:table-column column-width="12.5cm" />
                                    <fo:table-body>
                                        <fo:table-row>
                                            <fo:table-cell column-number="2" padding-before="12pt">
                                                <fo:block font-family="Times" font-style="italic" font-weight="normal" text-align="justify"
                                                space-before="12pt">
                                                    <xsl:apply-templates select="desc" />
                                                </fo:block>
                                            </fo:table-cell>
                                        </fo:table-row>
                                    </fo:table-body>
                                </fo:table>
                            </fo:table-cell>
                        </fo:table-row>
                        <!-- arg + argdef-->
                        <xsl:for-each select="arg">
                            <fo:table-row>
                                <fo:table-cell>
                                    <fo:table table-layout="fixed" width="17cm">
                                        <fo:table-column column-width="4.5cm" />
                                        <fo:table-column column-width="12.5cm" />
                                        <fo:table-body>
                                            <fo:table-row>
                                                <fo:table-cell padding-before="12pt">
                                                    <fo:block font-family="Courier" font-size="11pt" font-style="normal"
                                                    font-weight="normal" text-align="justify" space-before="12pt">
                                                        <xsl:apply-templates select="argn" />
                                                    </fo:block>
                                                </fo:table-cell>
                                                <fo:table-cell padding-before="12pt">
                                                    <fo:block font-family="Times" font-style="normal" font-weight="normal"
                                                    text-align="justify" space-before="12pt">
                                                        <xsl:apply-templates select="argdef" />
                                                    </fo:block>
                                                </fo:table-cell>
                                            </fo:table-row>
                                        </fo:table-body>
                                    </fo:table>
                                </fo:table-cell>
                            </fo:table-row>
                        </xsl:for-each>
                        <!-- line -->
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:block text-align='right' space-after="12pt">
                                    <fo:leader leader-pattern="rule" leader-length="17cm" />
                                </fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                    </xsl:for-each>
                </fo:table-body>
            </fo:table>
        </fo:block>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                  ERROR MESSAGES                       # -->
    <!-- ######################################################### -->
    <!-- Delimite un message d'erreur -->
    <xsl:template match="//messages">
        <xsl:choose>
            <xsl:when test="./err/num = ''">
                <fo:block space-before="12pt" font-family="Times" font-style="italic" font-weight="normal" text-align="center">Error
                messages description not available yet.</fo:block>
            </xsl:when>
            <xsl:otherwise>
                <fo:block space-before="12pt">
                    <fo:table table-layout="fixed" width="17cm">
                        <fo:table-column column-width="17cm" />
                        <fo:table-body>
                            <xsl:for-each select="err">
                                <xsl:variable name="msg_buf" select="msg" />
                                <fo:table-row>
                                    <fo:table-cell>
                                        <fo:table table-layout="fixed" width="17cm">
                                            <fo:table-column column-width="3cm" />
                                            <fo:table-column column-width="14cm" />
                                            <fo:table-body>
                                                <fo:table-row>
                                                    <fo:table-cell>
                                                        <fo:block font-family="Courier" font-size="10pt" font-style="normal"
                                                        font-weight="normal" text-align="justify">
                                                            <xsl:apply-templates select="lib" />
                                                            <xsl:apply-templates select="num" />
                                                        </fo:block>
                                                    </fo:table-cell>
                                                    <fo:table-cell>
                                                        <fo:block font-family="Courier" font-size="10pt" font-style="normal"
                                                        font-weight="normal" text-align="justify">
                                                            <xsl:for-each select="tokenize(msg,'\\n')">
                                                                <fo:block>
                                                                    <xsl:for-each select="tokenize(.,'\\t')">
                                                                        <xsl:for-each select="replace(.,'%s','&lt;string&gt;')">
                                                                            <xsl:for-each select="replace(.,'%d','&lt;decimal&gt;')">
                                                                                <xsl:for-each select="replace(.,'%i','&lt;integer&gt;')">
                                                                                    <xsl:for-each select="replace(.,'%c','&lt;character&gt;')">
                                                                                        <xsl:value-of select="." />
                                                                                    </xsl:for-each>
                                                                                </xsl:for-each>
                                                                            </xsl:for-each>
                                                                        </xsl:for-each>
                                                                    </xsl:for-each>
                                                                </fo:block>
                                                            </xsl:for-each>
                                                        </fo:block>
                                                    </fo:table-cell>
                                                </fo:table-row>
                                            </fo:table-body>
                                        </fo:table>
                                    </fo:table-cell>
                                </fo:table-row>
                                <fo:table-row>
                                    <fo:table-cell>
                                        <fo:table table-layout="fixed" width="17cm">
                                            <fo:table-column column-width="3cm" />
                                            <fo:table-column column-width="14cm" />
                                            <fo:table-body>
                                                <fo:table-row>
                                                    <fo:table-cell column-number="2">
                                                        <fo:block font-family="Times" font-style="normal" font-weight="normal"
                                                        text-align="justify" space-before="12pt">
                                                            <xsl:apply-templates select="doc" />
                                                        </fo:block>
                                                    </fo:table-cell>
                                                </fo:table-row>
                                            </fo:table-body>
                                        </fo:table>
                                    </fo:table-cell>
                                </fo:table-row>
                                <fo:table-row>
                                    <fo:table-cell>
                                        <fo:block text-align='right' space-after="12pt">
                                            <fo:leader leader-pattern="rule" leader-length="17cm" />
                                        </fo:block>
                                    </fo:table-cell>
                                </fo:table-row>
                            </xsl:for-each>
                        </fo:table-body>
                    </fo:table>
                </fo:block>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- ######################################################### -->
    <!-- #                    Images                             # -->
    <!-- ######################################################### -->

    <!-- Image hauteur (H) imposee -->
    <xsl:template match="//imgHsize">
        <xsl:call-template name="image">
            <xsl:with-param name="name" select="@namepdf" />
            <xsl:with-param name="height" select="@hpdf" />
            <xsl:with-param name="align">center</xsl:with-param>
        </xsl:call-template>
    </xsl:template>

    <!-- Images avec tailles predefinie en fonction des attributs -->
    <xsl:template match="//imgfix">
        <xsl:call-template name="image">
            <xsl:with-param name="name" select="@ref" />
            <xsl:with-param name="height">39pt</xsl:with-param>
            <xsl:with-param name="width">39pt</xsl:with-param>
            <xsl:with-param name="align">center</xsl:with-param>
        </xsl:call-template>
    </xsl:template>

    <!-- Image hauteur (H) et largeur (W) imposees -->
    <xsl:template match="//imgsize">
        <xsl:call-template name="image">
            <xsl:with-param name="name" select="@namepdf" />
            <xsl:with-param name="height" select="@hpdf" />
            <xsl:with-param name="width" select="@wpdf" />
            <xsl:with-param name="align">center</xsl:with-param>
        </xsl:call-template>
    </xsl:template>

    <!-- Affichage des images en fonction des attributs disponibles -->
    <xsl:template name="image">
        <xsl:param name="name" />
        <xsl:param name="width" />
        <xsl:param name="height" />
        <xsl:param name="align" select="center" />
        <fo:block text-align="{$align}" space-before="12pt" space-after="12pt">
            <xsl:choose>
                <xsl:when test="not( $width and $height )">
                    <xsl:choose>
                        <xsl:when test="$width">
                            <fo:external-graphic src="file:{$pictpdf}/{$name}" scaling="uniform" height="auto" width="{$width}"
                            content-height="auto" content-width="{$width}" />
                        </xsl:when>
                        <xsl:when test="$height">
                            <fo:external-graphic src="file:{$pictpdf}/{$name}" scaling="uniform" height="{$height}" width="auto"
                            content-height="{$height}" content-width="auto" />
                        </xsl:when>
                        <xsl:otherwise>
                            <fo:external-graphic src="file:{$pictpdf}/{$name}" scaling="uniform" height="auto" width="auto"
                            content-height="auto" content-width="auto" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:otherwise>
                    <fo:external-graphic src="file:{$pictpdf}/{$name}" height="{$height}" width="{$width}" content-height="{$height}"
                    content-width="{$width}" />
                </xsl:otherwise>
            </xsl:choose>
        </fo:block>
    </xsl:template>

	<!-- ######################################################### -->
	<!-- #                    FAQ                                # -->
	<!-- ######################################################### -->

	<xsl:template  match="//faq[@link]">
  		<xsl:variable name="itarget"><xsl:value-of select="./@link"/></xsl:variable>
    	<xsl:for-each select="//faq[@id]">
      		<xsl:variable name="faqid"><xsl:number count="index" from="//glossary"  level="any"/></xsl:variable>
      		<xsl:if test="number($itarget) = number($faqid)">
        		<fo:basic-link internal-destination="{generate-id(..)}">
          			<fo:inline font-family="Times" font-style="normal" font-weight="bold">Q<xsl:value-of select="$faqid"/></fo:inline>
        		</fo:basic-link>
      		</xsl:if>
    	</xsl:for-each>
	</xsl:template>

	<xsl:template  match="faq">
  		<xsl:variable name="locid"><xsl:number count="index" from="//glossary"  level="any"/></xsl:variable>
  		Q<xsl:value-of select="$locid"/> :
	</xsl:template>

</xsl:stylesheet>
