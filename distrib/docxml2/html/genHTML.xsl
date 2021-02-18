<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="file.xsl" />

<xsl:preserve-space elements="title cl"/> 
<xsl:strip-space elements="p def"/> 

<xsl:output name="html" method="html"/>


<!-- ######################################################### -->
<!-- #         GENERATION DE DIFFERENTES PAGES HTML          # -->
<!-- ######################################################### -->
<xsl:param name="banner_name"><xsl:value-of select="$fichier"/>_banner</xsl:param>
<xsl:param name="doc_name"><xsl:value-of select="$fichier"/>_doc</xsl:param>
<xsl:param name="toc_name"><xsl:value-of select="$fichier"/>_toc</xsl:param>
<xsl:param name="info_name"><xsl:value-of select="$fichier"/>_info</xsl:param>
<xsl:param name="index_name"><xsl:value-of select="$fichier"/>_index</xsl:param>

<xsl:template match="/">
  <!-- Fichier: {$fichier}_banner.html / Banniere avec titre de la doc -->
  <xsl:call-template name="banner"/>
  <!-- Fichier: {$fichier}_doc[#chapter_num].html / Frameset -->
  <xsl:call-template name="doc"/>
  <!-- Fichier: {$fichier}.html / Frameset -->
  <xsl:call-template name="frameset"/>
  <!-- Fichier: {$fichier}_index.html / Index des variables de la doc -->
  <xsl:call-template name="indexfile"/>
  <!-- Fichier: {$fichier}_info.html / Introduction ("About") de la doc -->
  <xsl:call-template name="infofile"/>
 <!-- Fichier: {$fichier}_toc.html / Tables des matieres de la doc -->
  <xsl:call-template name="toc"/>
</xsl:template>

<!-- ######################################################### -->
<!-- #       REGLES APLIQUEES AUX BALISES DES DOCUMENTS      # -->
<!-- #           Attention au classement alphabetique        # -->
<!-- #      Classement par nom de balise / pas par path      # -->
<!-- ######################################################### -->

<!-- partie gauche du tableau 75% -->
<xsl:template priority="1" match="//glossary/row[@type='split']/article">
    <TD colspan='2' class="article_split"><a name="{generate-id(.)}"><xsl:apply-templates/></a></TD>
</xsl:template>

<!-- partie gauche du tableau 50% -->
<xsl:template priority="1" match="//glossary/row[@type='half']/article">
    <TD colspan='2' class="article_half"><a name="{generate-id(.)}"><xsl:apply-templates/></a></TD>
</xsl:template>

<!-- partie gauche du tableau par defaut -->
<xsl:template match="//glossary/row/article">
    <TD class="article_row"><a name="{generate-id(.)}"><xsl:apply-templates/></a></TD>
</xsl:template>

<!-- marque un des elements d'une liste a point -->
<xsl:template match="//list/article">
    <b><xsl:apply-templates/></b>
</xsl:template>

<!-- marque un des elements d'une liste enumeree -->
<xsl:template match="//numlist/article">
    <b><xsl:apply-templates/></b>
</xsl:template>

<!-- corps de texte -->
<xsl:template match="//body">
    <xsl:apply-templates/>
</xsl:template>

<!-- Titre de l'ouvrage -->
<xsl:template match="//booktitle">
  <p class="booktitle"><xsl:apply-templates/>
    <xsl:if test="../titleextension">&#xA0;<xsl:apply-templates select="../titleextension"/></xsl:if>
  </p>
</xsl:template>


<!-- ligne d'un code -->
<xsl:template match="//cl">
        <xsl:apply-templates/>
</xsl:template>

<!-- Delimite une partie de code deja mis en forme / defaut est identique a type=box -->
<xsl:template match="//bnf">
  <p class="center">
  <table BORDER="2" CELLPADDING="10"><tr><td class="bluecode">
    <pre><xsl:apply-templates/></pre>
  </td></tr></table>
  </p>
</xsl:template>

<!-- Delimite une partie de code deja mis en forme -->
<xsl:template match="//smallcode">
  <p class="justify">
  <table BORDER="0" CELLPADDING="10"><tr><td class="smallcode">
    <pre><xsl:apply-templates/></pre>
  </td></tr></table>
  </p>
</xsl:template>

<!-- Delimite une partie de code deja mis en forme -->
<xsl:template match="//code">
  <p class="justify">
  <table BORDER="0" CELLPADDING="10"><tr><td class="code">
    <pre><xsl:apply-templates/></pre>
  </td></tr></table>
  </p>
</xsl:template>

<!-- fait apparaitre une ligne de command hors d'un paragraphe -->
<xsl:template match="//command">
    <p class="command"><xsl:apply-templates/></p>
</xsl:template>

<!--  Affichage de la date -->
<xsl:template match="presentation/date">
    <p class="date"><xsl:apply-templates  select="$date"/></p>
</xsl:template>

<!-- partie gauche du tableau 75% -->
<xsl:template priority="1" match="//glossary/row[@type='split']/def">
    <TD class="def_split">
    <xsl:apply-templates/></TD>
</xsl:template>

<!-- partie gauche du tableau 50% -->
<xsl:template priority="1" match="//glossary/row[@type='half']/def">
    <TD class="def_half">
    <xsl:apply-templates/></TD>
</xsl:template>

<!-- partie gauche du tableau par defaut -->
<xsl:template match="//glossary/row/def">
    <TD class="def_row">
    <xsl:apply-templates/></TD>
</xsl:template>

<!-- partie document / positionnement d'indentation et de font size par defaut du document -->
<xsl:template match="document">
    <DIV class="document">
    <xsl:apply-templates/>
    </DIV>
</xsl:template>

<!--  mise en forme dans une ligne -->
<xsl:template match="//b">
    <font  class="font"><xsl:apply-templates/></font>
</xsl:template>

<!--  mise en forme dans une ligne -->
<xsl:template match="//f">
    <font  class="font"><span style="color: rgb(255, 102, 0);"><xsl:apply-templates/></span></font>
</xsl:template>

<!-- Gestion des balises de FAQ avec lien -->
<xsl:template  match="//faq[@link]">
    <xsl:variable name="position_ref"><xsl:number count="//chapter" level="any"/></xsl:variable>
    <xsl:variable name="doc_filename"><xsl:value-of select="$doc_name"/><xsl:value-of select="$position_ref"/></xsl:variable>
    <xsl:variable name="itarget"><xsl:value-of select="./@link"/></xsl:variable>
    <xsl:for-each select="//faq[@id]">
            <xsl:variable name="faqid"><xsl:number count="index" from="//glossary"  level="any"/></xsl:variable>
            <xsl:if test="number($itarget) = number($faqid)">
                <a href="{$doc_filename}.html#{generate-id(..)}" target="doc">
                <b>Q<xsl:value-of select="$faqid"/></b></a>
            </xsl:if>
    </xsl:for-each>
</xsl:template>

<!-- FAQ -->
<xsl:template  match="faq">
    <xsl:variable name="locid"><xsl:number count="index" from="//glossary"  level="any"/></xsl:variable>
    Q<xsl:value-of select="$locid"/> :
</xsl:template>

<!-- Image taille reelle -->
<xsl:template match="//imgfix">
    <xsl:call-template name="image">
      <xsl:with-param name="name" select="@ref"/>
    </xsl:call-template>
</xsl:template>

<!-- Logo de la societe -->
<xsl:template match="//imglogo">
    <xsl:call-template name="image">
      <xsl:with-param name="name" select="@namehtml"/>
    </xsl:call-template>
</xsl:template>

<!-- Image hauteur (H) imposee -->
<xsl:template match="//imgHsize">
    <xsl:call-template name="image">
      <xsl:with-param name="name" select="@namehtml"/>
      <xsl:with-param name="height" select="@hpdf"/>
    </xsl:call-template>
</xsl:template>

<!-- Image hauteur (H) et largeur (W) imposees -->
<xsl:template match="//imgsize">
    <xsl:call-template name="image">
      <xsl:with-param name="name" select="@namehtml"/>
      <xsl:with-param name="height" select="@hpdf"/>
      <xsl:with-param name="width" select="@wpdf"/>
    </xsl:call-template>
</xsl:template>

<!-- Image largeur (W) imposee -->
<xsl:template match="//imgWsize">
    <xsl:call-template name="image">
      <xsl:with-param name="name" select="@namehtml"/>
      <xsl:with-param name="width" select="@wpdf"/>
    </xsl:call-template>
</xsl:template>

<!-- Index des variables dans le document -->
<xsl:template match="//index">
  <a name="{generate-id(.)}"><xsl:apply-templates/></a>
</xsl:template>

<!-- Resumer du contenu de la doc -->
<xsl:template match="introduction/intro">
    <p class="intro"><xsl:apply-templates/></p>
    <br/><br/><br/><br/><br/>
    <p class="intro_fixe">
      Documentation issued and compliant with Avertec Release <xsl:value-of select="$release_num" />.
      <br/>
      Please, contact support@avertec.com for any problems relating to this manual.
      <br/><br/><br/>
      <xsl:apply-templates select="//legal"/>
    </p>
</xsl:template>

<!-- elements composants une liste a point -->
<xsl:template match="//list/item">
    <LI><xsl:apply-templates/></LI>
</xsl:template>

<!-- elements composants une liste enumeree -->
<xsl:template match="//numlist/item">
    <LI><xsl:apply-templates/></LI>
</xsl:template>

<!-- mise en forme de tableau -->
<xsl:template match="//glossary">
  <p class="center">
    <xsl:if test="./row[@type='half']">
        <TABLE width="90%" cellspacing="1" border="1">
        <xsl:apply-templates/></TABLE>
    </xsl:if>
    <xsl:if test="not(./row[@type='half'])">
        <TABLE width="90%" cellspacing="20">
        <xsl:apply-templates/></TABLE>
    </xsl:if>
  </p>
</xsl:template>

<!-- fait ressortir un path d'un paragraphe -->
<xsl:template match="//path">
    <p class="path"><xsl:apply-templates/></p>
</xsl:template>

<!-- Information copyright -->
<xsl:template match="presentation/legal">
    <p class="legal"><xsl:apply-templates/></p>
</xsl:template>

<!-- Delimite une liste d'elements par points -->
<xsl:template match="//list">
    <UL class="pnt_list"><xsl:apply-templates/></UL>
</xsl:template>

<!-- TCL API Functions -->
<xsl:template match="//functions">
    <table class="function_tab">
    <xsl:for-each select="func">
        <tr><td colspan='2'><p class="func_name"><xsl:apply-templates select="funcname"/></p></td></tr>
        <tr><td colspan='2'><p class="func_proto"><xsl:apply-templates select="proto"/><br/><br/></p></td></tr>
        <tr><td></td><td><p class="func_desc"><xsl:apply-templates select="desc"/><br/><br/></p></td></tr>
        <xsl:for-each select="arg">
            <tr>
                <td class="func_argn"><xsl:apply-templates select="argn"/></td>
                <td class="func_argdef"><xsl:apply-templates select="argdef"/></td>
            </tr>
        </xsl:for-each>
        <tr><td colspan='2'><hr/><br/></td></tr>
    </xsl:for-each>
    </table>
</xsl:template>

<!-- Delimite un message d'erreur -->
<xsl:template match="//messages">
  <xsl:choose>
  <xsl:when test="./err/num = ''">
  <p>Error messages description not available yet.</p>
  </xsl:when>
  <xsl:otherwise>
  <table class="errmsg_tab">
     <tr>
       <td class="errmsg_num">Error Num</td>
       <td class="errmsg_head">Error Message and description</td>
     </tr>
     <tr><td colspan='2'><hr/><br/></td></tr>
  <xsl:for-each select="err">
  <xsl:variable name="msg_buf" select="msg"/>
     <tr>
       <td class="errmsg_num"><xsl:apply-templates select="lib"/><xsl:apply-templates select="num"/></td>
       <td class="errmsg_msg">
         <xsl:for-each select="tokenize(msg,'\\n')">
         <xsl:for-each select="tokenize(.,'\\t')">&#xA0;&#xA0;&#xA0;&#xA0;
         <xsl:for-each select="replace(.,'%s','&lt;string&gt;')">
         <xsl:for-each select="replace(.,'%d','&lt;decimal&gt;')">
         <xsl:for-each select="replace(.,'%i','&lt;integer&gt;')">
         <xsl:for-each select="replace(.,'%c','&lt;character&gt;')">
         <xsl:for-each select="replace(.,'%g','&lt;float&gt;')">
           <xsl:value-of select="." />
         </xsl:for-each>
         </xsl:for-each>
         </xsl:for-each>
         </xsl:for-each>
         </xsl:for-each>
         </xsl:for-each>
         <br/>
         </xsl:for-each>
       </td>
     </tr>
     <tr><td colspan='2'><p class="document"><xsl:apply-templates select="doc"/></p><br/></td></tr>
     <tr><td colspan='2'><hr/><br/></td></tr>
   </xsl:for-each>
   </table>
  </xsl:otherwise>
  </xsl:choose>   
</xsl:template>

<!-- Delimite une liste d'elements enuemres -->
<xsl:template match="//numlist">
    <OL class="ord_list"><xsl:apply-templates/></OL>
</xsl:template>

<!-- Nom de l'outil associer a la doc -->
<xsl:template match="presentation/outil">
    <p clas="outil"><xsl:apply-templates/></p>
</xsl:template>

<!-- Paragraphe -->
<xsl:template match="//d">
    <p><xsl:apply-templates/></p>
</xsl:template>

<!-- Paragraphe -->
<xsl:template match="//p">
    <p><xsl:apply-templates/></p>
</xsl:template>

<!-- Paragraphe -->
<xsl:template match="//br">
    <br/>
</xsl:template>

<!-- lignes des tableaux 75% -->
<xsl:template priority="1" match="//glossary/row[@type='split']">
    <TR><xsl:apply-templates select="article"/></TR>
    <TR><TD></TD><xsl:apply-templates select="def"/></TR>
</xsl:template>

<!-- lignes des tableaux 50% -->
<xsl:template priority="1" match="//glossary/row[@type='half']">
    <TR><xsl:apply-templates select="article"/>
        <xsl:apply-templates select="def"/></TR>
</xsl:template>

<!-- lignes des tableaux par defaut -->
<xsl:template match="//row">
    <TR><xsl:apply-templates/></TR>
</xsl:template>

<!-- Titres et chapitres de la table des matieres -->
<xsl:template mode="toc" match="//section/title">
  <xsl:param name="expand_toc" select="0"/>
  <xsl:variable name="toc_level" select="../@niv"/>
  <xsl:variable name="next_level"><xsl:number value="$toc_level + 1"/></xsl:variable>
  <xsl:variable name="toc_target"><xsl:value-of select="$toc_name"/><xsl:number count="//chapter" level="any"/>.html</xsl:variable>
  <xsl:variable name="doc_target"><xsl:value-of select="$doc_name"/><xsl:number count="//chapter" level="any"/>.html</xsl:variable>
  <xsl:if test="$toc_level &lt; 5">
    <tr align='justify'><td>
     <xsl:choose>
      <xsl:when test="$toc_level = 1">
      <a href="javascript: loadframe('{$toc_target}','toc');loadframe('{$doc_target}#{generate-id(.)}','doc');" class="toc{$toc_level}">
        <xsl:call-template name="title_num">
          <xsl:with-param name="level" select="$toc_level"/>
          <xsl:with-param name="type" select="toc"/>
        </xsl:call-template>
      </a>
     </xsl:when>
     <xsl:otherwise>
      <a href="{$doc_target}#{generate-id(.)}" target="doc" class="toc{$toc_level}">
        <xsl:call-template name="title_num">
          <xsl:with-param name="level" select="$toc_level"/>
          <xsl:with-param name="type" select="toc"/>
        </xsl:call-template>
      </a>
     </xsl:otherwise>
     </xsl:choose>
    </td></tr>
    <xsl:if test="$expand_toc = 1">
      <xsl:apply-templates mode="toc" select="..//section[@niv = $next_level]/title">
      <xsl:with-param name="expand_toc" select="1"/></xsl:apply-templates>
    </xsl:if>  
  </xsl:if>  
</xsl:template>

<!-- Titres et chapitres du document -->
<xsl:template match="section/title">
  <xsl:variable name="doc_level" select="../@niv"/>
  <a name="{generate-id(.)}">
  <xsl:choose>
    <xsl:when test="$doc_level = 1"><h1><xsl:call-template name="title_num">
                                    <xsl:with-param name="level" select="$doc_level"/>
                                    <xsl:with-param name="type" select="doc"/></xsl:call-template></h1></xsl:when>
    <xsl:when test="$doc_level = 2"><h2><xsl:call-template name="title_num">
                                    <xsl:with-param name="level" select="$doc_level"/>
                                    <xsl:with-param name="type" select="doc"/></xsl:call-template></h2></xsl:when>
    <xsl:when test="$doc_level = 3"><h3><xsl:call-template name="title_num">
                                    <xsl:with-param name="level" select="$doc_level"/>
                                    <xsl:with-param name="type" select="doc"/></xsl:call-template></h3></xsl:when>
    <xsl:when test="$doc_level = 4"><h4><xsl:call-template name="title_num">
                                    <xsl:with-param name="level" select="$doc_level"/>
                                    <xsl:with-param name="type" select="doc"/></xsl:call-template></h4></xsl:when>
  </xsl:choose>
  </a>
</xsl:template>

<!-- titre associer a l'introduction -->
<xsl:template match="introduction/titre_intro">
    <h1 class="titre_intro"><xsl:apply-templates/><hr NOSHADE="any" size='1'  width="100%" /></h1>
</xsl:template>

<!-- Fait apparaitre les variables dans une ligne -->
<xsl:template match="//var">
    <i><xsl:apply-templates/></i>
</xsl:template>

<!-- Version de la distrib associer a la doc -->
<xsl:template match="presentation/version">
    <p class="version"><xsl:apply-templates/></p>
</xsl:template>

<!-- ######################################################### -->
<!-- # DEFINITIONS DES TEMPLATES DE LA FEUILLE DE STYLE XSLT # -->
<!-- ######################################################### -->

<!-- ######################################################### -->
<!-- #               TEMPLATES TYPES "FONCTIONS"             # -->
<!-- ######################################################### -->

<!-- Index des variables apparaissant en fin de document -->
<xsl:template name="alpha_index">
  <tr><td>
    <table cellpadding="10" align="center" valign="middle" width="80%">
    <tr>
      <xsl:for-each-group select="//index" group-by="substring(.,1,1)">
        <xsl:sort order="ascending" data-type="text" case-order="upper-first" lang="en" />
        <td><p class="index"><a href="#{generate-id(.)}">
          <xsl:call-template name="convert_case">
            <xsl:with-param name="toconvert" select="substring(.,1,1)"/>
          </xsl:call-template>
        </a></p></td>
      </xsl:for-each-group>
    </tr>
    </table>
  </td></tr>
  <xsl:for-each-group select="//index" group-by="substring(.,1,1)">
    <xsl:sort order="ascending" data-type="text" case-order="upper-first" lang="en" />
    <TR><TD bgcolor="lightgrey">
      <table width="100%" cellpadding="0" cellspacing="0"><tr>
      <td width="5%" bgcolor="lightgrey" class="index">
        <a name="{generate-id(.)}">
          <xsl:call-template name="convert_case">
            <xsl:with-param name="toconvert" select="substring(.,1,1)"/>
          </xsl:call-template>
        </a>
      </td>
      <td width="85%" bgcolor="lightgrey" align="right"><a href="#index_top">Back to Index</a></td>
      <td width="10%" bgcolor="lightgrey"></td>
      </tr>
      </table>
    </TD></TR>
    <xsl:for-each select="current-group()">
      <xsl:sort order="ascending" data-type="text" case-order="upper-first" lang="en" />
      <xsl:variable name="position_ref"><xsl:number count="//chapter" level="any"/></xsl:variable>
      <xsl:variable name="doc_target"><xsl:value-of select="$doc_name"/><xsl:value-of select="$position_ref"/>.html</xsl:variable>
      <xsl:variable name="toc_target"><xsl:value-of select="$toc_name"/><xsl:value-of select="$position_ref"/>.html</xsl:variable>
      <TR><TD valign="top"><p>
      <a href="javascript: loadframe('{$toc_target}','toc');loadframe('{$doc_target}#{generate-id(.)}','doc');">
        <xsl:value-of select="."/>
      </a>
      </p></TD></TR>
    </xsl:for-each>
  <tr><td><br/></td></tr>
  </xsl:for-each-group>
</xsl:template>

<xsl:template name="convert_case">
  <xsl:param name='toconvert' />
  <xsl:variable name="lower_case">abcdefghijklmnopqrstuvwxyz</xsl:variable>
  <xsl:variable name="upper_case">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>
  <xsl:choose>
    <xsl:when test="contains($upper_case,$toconvert)">
      <xsl:value-of select="translate($toconvert,$upper_case,$lower_case)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="translate($toconvert,$lower_case,$upper_case)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Feuille de style CSS des fichiers HTML -->
<xsl:template name="css">
  <STYLE TYPE="text/css">
    h1 {font-family:times; font-size:16pt; font-weight:bold; text-align:left; margin-left:10pt; font-style:normal} 
    h2 {font-family:times; font-size:14pt; font-weight:bold; text-align:left; margin-left:10pt; font-style:normal} 
    h3 {font-family:times; font-size:13pt; font-weight:bold; text-align:left; margin-left:10pt; font-style:normal} 
    h4 {font-family:times; font-size:12pt; font-weight:bold; text-align:left; margin-left:10pt; font-style:normal} 
    p  {text-indent:0%; text-align:justify; font-family:times; margin-left:10pt; margin-right:10pt}
    d  {text-indent:10%; text-align:justify; font-family:times; margin-left:10pt; margin-right:10pt}
    
    .article_split {width:75%; vertical-align:top;}
    .article_half  {width:50%; vertical-align:top;text-align:center}
    .article_row   {width:40%; vertical-align:top;background-color: rgb(255, 245, 245);}
    .bluecode      {text-indent:0%; font-family:times; font-size:10pt; text-align:justify; color:blue}
    .booktitle     {font-family:times; font-size:28pt; text-align:left; font-weight:bold ; text-indent:0%}
    .code          {margin-left:10pt; text-indent:0%; font-family:times; font-size:10pt; text-align:justify}
    .smallcode     {margin-left:10pt; text-indent:0%; font-family:times; font-size:8pt; text-align:justify}
    .command       {font-family:times; font-size:10pt; text-align:justify; font-weight:bold ; margin-left:10%}
    .date          {font-family:times; font-size:10pt; text-align:right}
    .def_split     {text-align:justify; vertical-align:top; width:75%}
    .def_half      {text-align:justify; vertical-align:top; width:75%}
    .def_row       {font-family:times; font-size:8pt;text-align:justify; vertical-align:top; width:60%;background-color: rgb(245, 245, 255);}
    .document      {margin-left:5px; font-size:10pt; text-align:justify}
    .errmsg_tab    {width:75%; vertical-align:top; margin-left: 12%}
    .errmsg_head   {width:80%; vertical-align:top; text-align:center; font-style:italic; font-weight:bold}
    .errmsg_num    {width:20%; vertical-align:top; text-align:center; font-weight:bold }
    .errmsg_msg    {width:80%; vertical-align:top; text-align:justify;font-style:italic}
    .function_tab  {width:80%; vertical-align:top; margin-left:10%}
    .func_name     {font-family:times; font-size:10pt; text-align:left; font-weight:bold; background-color: rgb(245, 245, 245);}
    .func_proto    {font-family:times; font-size:10pt; text-align:left}
    .func_desc     {font-family:times; font-size:10pt; text-align:justify;font-style:italic; color:rgb(51, 153, 153);}
    .func_argn     {width:20%; font-family:courier; vertical-align:top; font-size:8pt; text-align:left;background-color: rgb(255, 245, 245);}
    .func_argdef   {font-family:times; vertical-align:top; font-size:8pt; text-align:left;background-color: rgb(245, 245, 255);}
    .font          {font-family:courier; font-size:8pt}
    .intro         {font-family:times; font-size:10pt; text-indent:10%}
    .intro_fixe    {font-family:times; font-size:10pt; text-align:center; font-weight:bold}
    .legal         {font-family:times; font-size:10pt; text-align:right; font-weight:bold}
    .ord_list      {list-style-type: lower-roman; margin-left: 10%; text-align: justify;}
    .outil         {font-family:times; font-size:10pt; text-align:right; font-weight:bold}
    .path          {font-family:times; font-size:10pt; text-align:center}
    .pnt_list      {list-style-type: square; margin-left: 10%; text-align: justify}
    .titre_intro   {font-family:times; font-size:10pt; text-align:right; font-weight:bold} 
    .version       {font-family:times; font-size:20pt; text-align:right; font-weight:bold} 
    
    .center{text-align:center}  
    .index{text-align:center; font-weight:bold} 
    .right{text-align:right}  
    .toc1{margin-left:0pt;  font-size:10pt; text-align:left; font-family:times; white-space:nowrap; font-weight:normal}
    .toc2{margin-left:10pt; font-size:10pt; text-align:left; font-family:times; white-space:nowrap; font-weight:normal}
    .toc3{margin-left:16pt; font-size:8pt;  text-align:left; font-family:times; white-space:nowrap; font-weight:normal}
    .toc4{margin-left:22pt; font-size:8pt;  text-align:left; font-family:times; white-space:nowrap; font-weight:normal}
  </STYLE>
</xsl:template>

<!-- Entete <HEAD> des fichiers HTML -->
<xsl:template name="head">
  <xsl:param name="filename"><xsl:value-of select="$fichier"/></xsl:param>
  <xsl:param name="java" select="0"/>
  <HEAD>
    <TITLE><xsl:value-of select="//booktitle"/></TITLE>
    <META name="description" content="The file {$filename}.html is part of the Avertec Documentation Platform {$release_num}"/>
    <META name="keywords" content="Avertec HiTAS YAGLE {$fichier} guide html documentation"/>
    <META name="revised" content="Generated from XML : {$date}" />
    <xsl:call-template name="css"/>
    <xsl:if test="$java = 1">
      <script language="JavaScript">
      &lt;!--
      function loadframe(url,target)
         {
         parent.frames[target].location.href = url;
         }
      //--&gt;
      </script>
    </xsl:if>
    </HEAD>
</xsl:template>

<!-- Affichage des images en fonction des attributs disponibles -->
<xsl:template name="image">
  <xsl:param name="name"/>
  <xsl:param name="width"/>
  <xsl:param name="height"/>
  <xsl:variable name="img">imgs/<xsl:value-of select="$name"/></xsl:variable>
  <center>
  <xsl:choose>
    <xsl:when test="not( $width and $height )">
      <xsl:choose>
        <xsl:when test="$width">
          <IMG src="{$img}" width="{$width}"/>
        </xsl:when>
        <xsl:when test="$height">
          <IMG src="{$img}" height="{$height}"/>
        </xsl:when>
        <xsl:otherwise>
          <IMG src="{$img}"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <IMG src="{$img}" width="{$width}" height="{$height}"/>
    </xsl:otherwise>
  </xsl:choose>
  </center>
</xsl:template>

<!-- Numerotation des chapites chapitres -->
<xsl:template name="title_num">
  <xsl:param name="level" select="1"/>
  <xsl:param name="type" select="doc"/>
  <xsl:choose>
    <xsl:when test="$level &lt; 4">
      <xsl:number count="//section[@niv='1']/title" level="any"/>.
      <xsl:if test="$level &gt; 1">
        <xsl:number count="//section[@niv='2']/title" from="//section[@niv='1']" level="any"/>.
      </xsl:if>
      <xsl:if test="$level &gt; 2">
        <xsl:number count="//section[@niv='3']/title" from="//section[@niv='2']" level="any" format="1"/>.
      </xsl:if>
      <xsl:if test="$level &gt; 3">
        <xsl:number count="//section[@niv='4']/title" from="//section[@niv='3']" level="any" format="a"/>.
      </xsl:if>
      <xsl:choose>
        <xsl:when test="$type = toc">
          <xsl:choose>
            <xsl:when test="not(./index)"><xsl:apply-templates /></xsl:when>
            <xsl:otherwise><xsl:value-of select="./index" /></xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise><xsl:value-of select="." /></xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
        <xsl:when test="$type = toc">
          <xsl:choose>
            <xsl:when test="not(./index)"><xsl:apply-templates /></xsl:when>
            <xsl:otherwise><xsl:value-of select="./index" /></xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise><xsl:value-of select="." /></xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- ######################################################### -->
<!-- #         TEMPLATES GENERANT DES FICHIERS HTML          # -->
<!-- ######################################################### -->

<!-- Fichiers "*_banner.html" - Banniere de navigation -->
<xsl:template name="banner">
  <xsl:result-document href="file:{$dochtmldir}/{$fichier}_banner.html" format="html">
    <HTML>
      <xsl:call-template name="head"><xsl:with-param name="filename"><xsl:value-of select="$banner_name"/></xsl:with-param></xsl:call-template>
      <BODY bgcolor="white" MARGINWIDTH='0' MARGINHEIGHT='0'>
      <span style="font-family:times; font-size:10pt;">
      <TABLE WIDTH="100%"  HEIGHT="5%" CELLSPACING="0" CELLPADDING="0" BORDER="0">
        <tr>
          <td rowspan="2" width="20%" align="center" valign="top">
            <xsl:apply-templates select="//presentation/imglogo" />
          </td>
          <td colspan="2" width="*">
            <xsl:apply-templates select="//presentation/booktitle" />
          </td>
          <td rowspan="2" width="20%">
            <br/><br/>
            <a href="http://www.avertec.com" target="new">website</a><br/>
            <a href="{$info_name}.html" target="doc">About</a><br/>
            <a href="{$index_name}.html" target="doc">Index</a>
          </td>
        </tr>
        <tr height="45%">
          <td width="20%" valign="middle">
            <FORM name="navform" >
            <select name="dest" size="1" 
              onChange="parent.location.href=(this.options[this.selectedIndex].value); index=this.options[this.selectedIndex].value;">
              <OPTION >Choose a User Guide</OPTION>
              <OPTION >----------------</OPTION>
              <OPTION VALUE="hitas_user.html">HITAS User Guide</OPTION>
              <OPTION VALUE="hitas_reference.html">HITAS Reference Guide</OPTION>
              <OPTION VALUE="yagle_user.html">YAGLE User Guide</OPTION>
              <OPTION VALUE="yagle_reference.html">YAGLE Reference Guide</OPTION>
              <OPTION VALUE="gns_user.html">GNS User Guide</OPTION>
              <OPTION >----------------</OPTION>
              <OPTION VALUE="../docavertec.html">User Guides Page</OPTION>
              <OPTION VALUE="faq.html">FAQ Page</OPTION>
            </select>
            </FORM>
          </td>
          <td valign="middle">
            <FORM name="navform" >
            <select name="dest" size="1" 
              onChange="parent.location.href=(this.options[this.selectedIndex].value); index=this.options[this.selectedIndex].value;">
              <OPTION >Choose a Tutorial</OPTION>
              <OPTION >----------------</OPTION>
              <OPTION VALUE="hitas_tutorial.html">HITAS Tutorial</OPTION>
              <OPTION VALUE="yagle_tutorial.html">YAGLE Tutorial</OPTION>
              <OPTION >----------------</OPTION>
              <OPTION VALUE="tutorialavertec.html">Tutorials Page</OPTION>
            </select>
            </FORM>
          </td>
        </tr>
      </TABLE>
      </span>
      </BODY>
    </HTML>
  </xsl:result-document>
</xsl:template>

<!-- Fichiers "*_doc(#num).html" - Correspondants aux chapitres -->
<xsl:template name="doc">
  <xsl:for-each select="//chapter">
    <xsl:variable name="position_ref"><xsl:number count="//chapter" level="any"/></xsl:variable>
    <xsl:variable name="doc_filename"><xsl:value-of select="$doc_name"/><xsl:value-of select="$position_ref"/></xsl:variable>
    <xsl:result-document href="file:{$dochtmldir}/{$doc_filename}.html" format="html">
    <HTML>
      <xsl:call-template name="head"><xsl:with-param name="filename"><xsl:value-of select="$doc_filename"/></xsl:with-param></xsl:call-template>
      <BODY bgcolor="white" MARGINWIDTH='0' MARGINHEIGHT='0'>
        <span style="font-family:times; font-size:10pt;">
          <xsl:apply-templates/>
        </span>
      </BODY>
    </HTML>
    </xsl:result-document>
  </xsl:for-each>
</xsl:template>

<!-- Fichier "{$fichier}.html" - Frameset -->
<xsl:template name="frameset">
  <xsl:result-document href="file:{$dochtmldir}/{$fichier}.html" format="html">
    <HTML>
      <xsl:call-template name="head"><xsl:with-param name="filename"><xsl:value-of select="$fichier"/></xsl:with-param></xsl:call-template>
      <FRAMESET rows="90,*" border="2" frameborder="2" framespacing="0">
        <NOFRAMES>
          <BODY>
            <P>Sorry but your browser does not support Frame pages.<br/>
            Be informed that Avertec documentation is also available in PDF format.
            </P>
          </BODY>
        </NOFRAMES>
        <FRAME SRC="{$fichier}_banner.html" scrolling="no" name="banner"/>
          <FRAMESET cols="200,*" framespacing="0" border="1" frameborder="2">
            <FRAME SRC="{$fichier}_toc1.html" name="toc"/>
            <FRAME SRC="{$fichier}_doc1.html" name="doc"/>
          </FRAMESET>
      </FRAMESET>
    </HTML>
  </xsl:result-document>
</xsl:template>
  
<!-- Fichier "*_index.html" - Index des variables -->
<xsl:template name="indexfile">
  <xsl:result-document href="file:{$dochtmldir}/{$index_name}.html" format="html">
    <HTML>
      <xsl:call-template name="head">
        <xsl:with-param name="filename"><xsl:value-of select="$index_name"/></xsl:with-param>
        <xsl:with-param name="java">1</xsl:with-param>
      </xsl:call-template>
      <BODY bgcolor="white" MARGINWIDTH='0' MARGINHEIGHT='0'>
        <span style="font-family:times; font-size:10pt;">
        <table width="100%">
          <tr><td>
            <h1 class="right">
              <a name="index_top">Index</a>
              <hr NOSHADE="any" size='1'  width="100%" />
            </h1>
          </td></tr>
        <xsl:if test="//index">
          <xsl:if test="not(//index/faq)">
            <xsl:call-template name="alpha_index"/>
          </xsl:if>
          <xsl:if test="//index/faq">
            <p>No index for this document.</p>
          </xsl:if>
        </xsl:if>
        <xsl:if test="not(//index)">
           <p>No index for this document.</p>
        </xsl:if>
        </table>
      </span>
      </BODY>
    </HTML>
  </xsl:result-document>
</xsl:template>
  
<!-- Fichier "*_info.html" - Section "About" du document -->
<xsl:template name="infofile">
  <xsl:result-document href="file:{$dochtmldir}/{$info_name}.html" format="html">
    <HTML>
      <xsl:call-template name="head">
        <xsl:with-param name="filename"><xsl:value-of select="$index_name"/></xsl:with-param>
      </xsl:call-template>
      <BODY bgcolor="white" MARGINWIDTH='0' MARGINHEIGHT='0'>
        <span style="font-family:times; font-size:10pt;">
        <table width="100%">
          <tr><td>
            <xsl:apply-templates select="//introduction/titre_intro" />
          </td></tr>
          <tr><td>
            <xsl:apply-templates select="//introduction/intro" />
          </td></tr>
        </table>
        </span>
      </BODY>
    </HTML>
  </xsl:result-document>
</xsl:template>

<!-- Fichiers "*_toc(#num).html" - Table des matieres des fichiers HTML -->
<xsl:template name="toc">
  <xsl:for-each select="//chapter">
    <xsl:variable name="position_ref"><xsl:number count="//chapter" level="any"/></xsl:variable>
    <xsl:variable name="toc_filename"><xsl:value-of select="$toc_name"/><xsl:value-of select="$position_ref"/></xsl:variable>
    <xsl:result-document href="file:{$dochtmldir}/{$toc_filename}.html" format="html">
    <HTML>
      <xsl:call-template name="head">
        <xsl:with-param name="filename"><xsl:value-of select="$toc_filename"/></xsl:with-param>
        <xsl:with-param name="java">1</xsl:with-param>
      </xsl:call-template>
      <BODY bgcolor="white" MARGINWIDTH='0' MARGINHEIGHT='0'>
        <span style="font-family:times; font-size:10pt;">
        <table cellpadding='15pt' width="200">
          <tr><td>
            <nobr><DIV STYLE="font-family:times; font-size:16pt;text-indent:0pt;align=center"><a name="content">Table of Contents</a></DIV></nobr>
          </td></tr>
        </table>
        <table cellpadding='0'>
          <xsl:apply-templates mode="toc" select="//chapter/section[@niv=1]/title"><xsl:with-param name="expand_toc" select="0"/></xsl:apply-templates>
        </table>
        <table width="200">
          <tr><td>
            <hr NOSHADE="any" size='2'  width="100%" />
            <nobr><DIV STYLE="font-family:times; font-size:10pt;text-indent:15pt;font-weight:bold;font-style:italic">
            Chapter <xsl:number count="//chapter" level="any"/> Subsections
            </DIV></nobr>
            <hr NOSHADE="any" size='2'  width="100%" />
          </td></tr>
        </table>
        <table>
          <xsl:apply-templates mode="toc" select="./section[@niv=1]/title"><xsl:with-param name="expand_toc" select="1"/></xsl:apply-templates>
        </table>
        </span>
      </BODY>
    </HTML>
    </xsl:result-document>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
