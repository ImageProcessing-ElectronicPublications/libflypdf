using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

using flypdf;

namespace CSharpExamples
{
    class PdfE6 : Pdf
    {
        Dictionary<string, bool> tags;
        private string HREF;

        public PdfE6()
            : base()
        {
            tags = new Dictionary<string, bool>();
            tags.Add("B", false);
            tags.Add("I", false);
            tags.Add("U", false);
            HREF = "";
        }

        private void OpenTag(string tag, Dictionary<string, string> attr)
        {
            if (tag == "A")
                this.HREF = attr["HREF"];
            else if (tag == "BR")
                this.Ln(5);
            else
                this.SetStyle(tag, true);
        }

        private void CloseTag(string tag)
        {
            if (tag == "A")
                this.HREF = "";
            else
                this.SetStyle(tag, false);
        }

        private void SetStyle(string tag, bool enable)
        {
            string style = "";
            this.tags[tag] = enable;

            foreach(KeyValuePair<string, bool> tag_status in this.tags) { 
                if (tag_status.Value)
                    style += tag_status.Key;
            }

            this.SetFont("", style);
        }

        private void PutLink(string url, string txt)
        {
            this.SetTextColor(0, 0, 255);
            this.SetStyle("U", true);
            this.Write(5, txt, url);
            this.SetStyle("U", false);
            this.SetTextColor(0);
        }

        public void WriteHTML(string html)
        {
            Regex tag_regexp = new Regex("<(.*?)>");
            html = html.Replace("\n", " ");
            int i = 0;

            string[] elements = tag_regexp.Split(html);
            foreach(string e in elements) {
                if (i % 2 == 0) { //Elements at even position are text
                    if (this.HREF.Length > 0)
                        this.PutLink(this.HREF, e);
                    else
                        this.Write(5, e);
                }
                else { //Elements at odd position are tags
                    if (e[0] == '/')
                        this.CloseTag(e.Substring(1).ToUpper());
                    else {
                        Regex attrib_regexp = new Regex("^([^=]*)=[\"']?([^\"']*)[\"']?$");
                        string[] pieces = e.Split(' ');
                        Dictionary<string, string> attrs = new Dictionary<string, string>();

                        for (int j = 1; j < pieces.Length; ++j) {
                            string attribute = pieces[j];
                            Match m = attrib_regexp.Match(attribute);
                            attrs.Add(m.Groups[1].Value, m.Groups[2].Value);
                        }

                        this.OpenTag(pieces[0].ToUpper(), attrs);
                    }
                }
                ++i;
            }
        }
    }

    class Example6
    {
        public static void Run()
        {
            PdfE6 pdf = new PdfE6();
            string html = "You can now easily print text mixing different styles : <B>bold</B>, <I>italic</I>, <U>underlined</U>, or <B><I><U>all at once</U></I></B>!<BR>You can also insert links on text, such as <A HREF=\"http://flypdf.sf.net\">flypdf.sf.net</A>, or on an image: click on the logo.";

            pdf.AddPage ();
            pdf.SetFont ( "Arial", "", 20 );
            pdf.Write ( 5, "To find out what's new in this tutorial, click " );
            pdf.SetFont ( "", "U" );
            int link = pdf.AddLink ();
            pdf.Write ( 5, "here", link.ToString());
            pdf.SetFont ( "" );
            
            pdf.AddPage ();
            pdf.SetLink ( link );
            pdf.Image("..\\src\\examples\\logo.jpg", 10, 10, 30, 0, "", "http://flypdf.sf.net");
            pdf.SetLeftMargin ( 45 );
            pdf.SetFontSize ( 14 );
            pdf.WriteHTML ( html );

            pdf.Output("example6.pdf");
        }
    }
}


