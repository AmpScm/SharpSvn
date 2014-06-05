<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
      xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:msxsl="urn:schemas-microsoft-com:xslt" xmlns:x="inline-doc"
                exclude-result-prefixes="x msxsl">
  <msxsl:script implements-prefix="x" language="c#">
      <![CDATA[
      
    const string html = "http://www.w3.org/1999/xhtml";
    public string DateTitle()
    {
      return (DateTime.Now - new TimeSpan(9,0,0)).ToString("yyyy-MM-dd");
    }
    
    public System.Xml.XPath.XPathNodeIterator MakeMessage(string msg)
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml("<html xmlns=\"http://www.w3.org/1999/xhtml\"></html>");
      
      XPathNavigator n = doc.DocumentElement.CreateNavigator();
      
      if (string.IsNullOrEmpty(msg))
        return n.Select("/qqq");
        
      XmlElement el = doc.DocumentElement;
        
      System.Text.RegularExpressions.Match m
        = System.Text.RegularExpressions.Regex.Match(msg, "^\\s*\\*\\s+", RegexOptions.Multiline);
        
      if(m.Success)
        msg = msg.Substring(0, m.Index);
        
      msg = msg.Trim();
        
      if(string.IsNullOrEmpty(msg))
        return n.Select("/qqq");
        
      System.Text.RegularExpressions.Regex r 
        = new System.Text.RegularExpressions.Regex("(\\b([Ii][sS][sS][uU][eE]\\s*\\#?" +
        "\\s*(?<issue>[0-9]+)" +
        ")|(r(?<rev>[0-9]+))\\b)|(?<nl>\r?\n[ \t]*\r?\n)", RegexOptions.Multiline);
        
      while((m = r.Match(msg)).Success)
      {
        string start = msg.Substring(0, m.Index);
        el.AppendChild(doc.CreateTextNode(start));
        
        System.Text.RegularExpressions.Group g;
        
        if ((g = m.Groups["issue"]).Success && !string.IsNullOrEmpty(g.Value))
        {
          XmlElement a = doc.CreateElement("a", html);
          a.SetAttribute("href", "http://sharpsvn.net/issues/?id=" + g.Value);
          a.InnerText = m.Value;
          el.AppendChild(a);
        }
        else if ((g = m.Groups["rev"]).Success && !string.IsNullOrEmpty(g.Value))
        {
          XmlElement a = doc.CreateElement("a", html);
          a.SetAttribute("href", "http://sharpsvn.net/rev/?r=" + g.Value);
          a.InnerText = m.Value;
          el.AppendChild(a);
        }
        else if ((g = m.Groups["nl"]).Success && !string.IsNullOrEmpty(g.Value))
        {
          XmlElement a = doc.CreateElement("br", html);
          el.AppendChild(a);
          el.AppendChild(doc.CreateTextNode("\r\n"));
        }        
        
        msg = msg.Substring(m.Index+m.Length);
      }      
      
      
      if(!string.IsNullOrEmpty(msg))
        el.AppendChild(doc.CreateTextNode(msg));
      
      return n.Select("./node()");
    }
    ]]>
  </msxsl:script>
  <xsl:param name="latestUrl">http://sharpsvn.net/daily/</xsl:param>
  <xsl:param name="latestx64Url">http://sharpsvn.net/daily/</xsl:param>
  <xsl:param name="latestExUrl">http://sharpsvn.net/daily/</xsl:param>
  <xsl:param name="latestVersion">2.0.0000.0</xsl:param>
  <xsl:param name="latestName">Sharpsvn-Daily-2.0.0000.0.msi</xsl:param>
  <xsl:param name="buildRev">4567</xsl:param>

  <xsl:output media-type="text/html" method="html" indent="no"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <xsl:text>&#13;&#10;</xsl:text>
        <title>SharpSvn-Daily Update - <xsl:value-of select="$latestVersion"/></title>
        <xsl:text>&#13;&#10;</xsl:text>
        <link rel="Stylesheet" type="text/css" href="daily.css" />
      </head>
      <body>
        <xsl:text>&#13;&#10;</xsl:text>
        <h1>SharpSvn-Daily Update - <xsl:value-of select="$latestVersion"/></h1>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <p>
          The SharpSvn project provides frequent builds of the current development tree. These versions
          are not fully tested, but may contain bugfixes (and new bugs) that are not in released versions. If
          you are testing these daily builds (Thanks!) and find issues in it, please report those issues
          with the exact version.
        </p>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <h2>The latest daily build is <a href="{$latestUrl}"><xsl:value-of select="$latestName"/></a>.</h2>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <p>The build script also supplies an <a href="{$latestx64Url}">x64 version</a> of this Win32 build and a
        <a href="{$latestExUrl}">package of all binaries</a> with some extras.</p>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <p>
          You can find older daily releases in the <a href="http://sharpsvn.net/daily/?all=1">daily folder</a> of documents &amp; files.
          The previous version of this report is still available in our <a href="http://sharpsvn.net/daily/?id={$buildRev}">Subversion Repository</a>.
        </p>
        <p>Published daily builds will be available for at least a week after they are build.</p>        
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:text>&#13;&#10;</xsl:text>
        <h2>Recent changes</h2>
        <xsl:text>&#13;&#10;</xsl:text>
        <table>
          <xsl:text>&#13;&#10;</xsl:text>
          <xsl:apply-templates select="/log/logentry" mode="days" />
          <xsl:text>&#13;&#10;</xsl:text>
        </table>
        <xsl:text>&#13;&#10;</xsl:text>
        <xsl:comment>/Recent changes</xsl:comment>
        <xsl:text>&#13;&#10;</xsl:text>
      </body>
    </html>
  </xsl:template>
  <xsl:template match="logentry" mode="days">
    <xsl:variable name="me" select="." />
    <xsl:variable name="myDate" select="substring-before(date,'T')"/>
    <xsl:if test="@revision = ../logentry[substring-before(date,'T') = $myDate][1]/@revision">      
      <tr xmlns="http://www.w3.org/1999/xhtml" class="d">
        <td colspan="4">
          <xsl:value-of select="$myDate"/>
        </td>        
      </tr>
      <xsl:text>&#13;&#10;</xsl:text>
      <xsl:apply-templates select="../logentry[substring-before(date,'T') = $myDate]"/>
    </xsl:if>
  </xsl:template>
  <xsl:template match="logentry">
    <xsl:if test="x:MakeMessage(msg)">
    <tr xmlns="http://www.w3.org/1999/xhtml">
      <td>
        <xsl:value-of select="' '"/>
      </td>
      <td class="r">
        <a id="{@revision}" href="http://sharpsvn.net/rev/?r={@revision}">[r<xsl:value-of select="@revision"/>]</a>
      </td>
      <td class="a">
        <xsl:value-of select="author"/>
      </td>
      <td class="f">
        <xsl:for-each select="paths/path[position() &lt;= 8]">
          <xsl:if test=".!=../path[1]">
            <xsl:value-of select="', '"/>
          </xsl:if>
          <xsl:value-of select="substring-after(., '/trunk/')"/>
        </xsl:for-each>
        <xsl:if test="count(paths/path) &gt; 8">
          <xsl:text> (and </xsl:text>
          <xsl:value-of select="count(paths/path)-8"/>
          <xsl:choose>
            <xsl:when test="count(paths/path) = 9">
              <xsl:text> other path)</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text> other paths)</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
      </td>
    </tr>
    <tr xmlns="http://www.w3.org/1999/xhtml">
      <td colspan="2">
        <xsl:value-of select="' '"/>
      </td>
      <td class="l" colspan="2">
        <xsl:copy-of select="x:MakeMessage(msg)"/>
      </td>
    </tr>
    <xsl:text>&#13;&#10;</xsl:text>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>