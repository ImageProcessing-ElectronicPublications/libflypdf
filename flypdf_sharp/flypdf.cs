using System;
using System.Runtime.InteropServices;

namespace flypdf
{
    public class Pdf
    {
        private const string flib = "flypdf_c";
        private IntPtr self_;
        private static Type[] MethodType = new Type[] { };

        private bool DerivedClassHasMethod(string methodName, Type[] methodTypes)
        {
            System.Reflection.MethodInfo methodInfo = this.GetType().GetMethod(methodName, System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance, null, methodTypes, null);
            bool hasDerivedMethod = methodInfo.DeclaringType.IsSubclassOf(typeof(Pdf));
            return hasDerivedMethod;
        }

        private delegate void Handler();
        [DllImport(flib)]
        private static extern IntPtr flypdf_SetHeaderDelegate(IntPtr p, Handler h);
        public virtual void Header() { }
        private void HeaderCaller()
        {
            Header();
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetFooterDelegate(IntPtr p, Handler h);
        public virtual void Footer() { }
        private void FooterCaller()
        {
            Footer();
        }

        private delegate int PBreakHandler();
        [DllImport(flib)]
        private static extern IntPtr flypdf_SetAcceptPageBreakDelegate(IntPtr p, PBreakHandler h);
        [DllImport(flib)]
        private static extern int flypdf_AcceptPageBreak(IntPtr p);
        public virtual int AcceptPageBreak() { return flypdf_AcceptPageBreak(this.self_);  }
        private int AcceptPageBreakCaller()
        {
            return AcceptPageBreak();
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_newPdf(string orientation, string unit, string format);
        public Pdf(string orientation, string unit, string page_size)
        {
            this.self_ = flypdf_newPdf(orientation, unit, page_size);
            if (DerivedClassHasMethod("Header", MethodType))
                flypdf_SetHeaderDelegate(this.self_, new Handler(HeaderCaller));
            if (DerivedClassHasMethod("Footer", MethodType))
                flypdf_SetFooterDelegate(this.self_, new Handler(FooterCaller));
            if (DerivedClassHasMethod("AcceptPageBreak", MethodType))
                flypdf_SetAcceptPageBreakDelegate(this.self_, new PBreakHandler(AcceptPageBreakCaller));
        }

        public Pdf(string orientation, string unit)
            : this(orientation, unit, "A4") { }

        public Pdf(string orientation)
            : this(orientation, "mm") { }

        public Pdf()
            : this("P") { } 

        [DllImport(flib)]
        private static extern void flypdf_destroyPdf(IntPtr p);
        ~Pdf()
        {
            flypdf_destroyPdf(this.self_);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetMargins(IntPtr p, float left, float top, float right);
        public Pdf SetMargins(float left, float top, float right)
        {
            flypdf_SetMargins(this.self_, left, top, right);
            return this;
        }

        public Pdf SetMargins(float left, float top)
        {
            return SetMargins(left, top, -1);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetLeftMargin(IntPtr p, float margin);
        public Pdf SetLeftMargin(float margin)
        {
            flypdf_SetLeftMargin(this.self_, margin);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetTopMargin(IntPtr p, float margin);
        public Pdf SetTopMargin(float margin)
        {
            flypdf_SetTopMargin(this.self_, margin);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetRightMargin(IntPtr p, float margin);
        public Pdf SetRightMargin(float margin)
        {
            flypdf_SetRightMargin(this.self_, margin);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetAutoPageBreak(IntPtr p, int autopb, float margin);
        public Pdf SetAutoPageBreak(int autopb, float margin)
        {
            flypdf_SetAutoPageBreak(this.self_, autopb, margin);
            return this;
        }

        public Pdf SetAutoPageBreak(int autopb)
        {
            return SetAutoPageBreak(autopb, 0);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetDisplayMode(IntPtr p, string zoom, string layout);
        public Pdf SetDisplayMode(string zoom, string layout)
        {
            flypdf_SetDisplayMode(this.self_, zoom, layout);
            return this;
        }

        public Pdf SetDisplayMode(string zoom)
        {
            return SetDisplayMode(zoom, "continuous");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetTitle(IntPtr p, string title);
        public Pdf SetTitle(string title)
        {
            flypdf_SetTitle(this.self_, title);
            return this;
        }


        [DllImport(flib)]
        private static extern IntPtr flypdf_SetSubject(IntPtr p, string subject);
        public Pdf SetSubject(string subject)
        {
            flypdf_SetSubject(this.self_, subject);
            return this;
        }


        [DllImport(flib)]
        private static extern IntPtr flypdf_SetAuthor(IntPtr p, string author);
        public Pdf SetAuthor(string author)
        {
            flypdf_SetAuthor(this.self_, author);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetKeywords(IntPtr p, string keywords);
        public Pdf SetKeywords(string keywords)
        {
            flypdf_SetKeywords(this.self_, keywords);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetCreator(IntPtr p, string creator);
        public Pdf SetCreator(string creator)
        {
            flypdf_SetCreator(this.self_, creator);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetAliasNbPages(IntPtr p, string alias);
        public Pdf SetAliasNbPages(string alias)
        {
            flypdf_SetAliasNbPages(this.self_, alias);
            return this;
        }

        public Pdf SetAliasNbPages()
        {
            return SetAliasNbPages("{nb}");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_AddPage(IntPtr p, string orientation);
        public Pdf AddPage(string orientation)
        {
            flypdf_AddPage(this.self_, orientation);
            return this;
        }

        public Pdf AddPage()
        {
            return AddPage("");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Open(IntPtr p);
        public Pdf Open()
        {
            flypdf_Open(this.self_);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Close(IntPtr p);
        public Pdf Close()
        {
            flypdf_Close(this.self_);
            return this;
        }

        [DllImport(flib)]
        private static extern int flypdf_PageNo(IntPtr p);
        public int PageNo()
        {
            return flypdf_PageNo(this.self_);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Output(IntPtr p, string name);
        public Pdf Output(string name)
        {
            flypdf_Output(this.self_, name);
            return this;
        }

        public Pdf Output()
        {
            return Output("dest.pdf");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Line(IntPtr p, double x1, double y1, double x2, double y2);
        public Pdf Line(double x1, double y1, double x2, double y2)
        {
            flypdf_Line(this.self_, x1, y1, x2, y2);
            return this;
        }


        [DllImport(flib)]
        private static extern IntPtr flypdf_SetLineWidth(IntPtr p, float width);
        public Pdf SetLineWidth(float width)
        {
            flypdf_SetLineWidth(this.self_, width);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetTextColor(IntPtr p, int r, int g, int b);
        public Pdf SetTextColor(int r, int g, int b)
        {
            flypdf_SetTextColor(this.self_, r, g, b);
            return this;
        }

        public Pdf SetTextColor(int r, int g)
        {
            return SetTextColor(r, g, -1);
        }

        public Pdf SetTextColor(int r)
        {
            return SetTextColor(r, -1);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetFillColor(IntPtr p, int r, int g, int b);
        public Pdf SetFillColor(int r, int g, int b)
        {
            flypdf_SetFillColor(this.self_, r, g, b);
            return this;
        }

        public Pdf SetFillColor(int r, int g)
        {
            return SetFillColor(r, g, -1);
        }

        public Pdf SetFillColor(int r)
        {
            return SetFillColor(r, -1);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetDrawColor(IntPtr p, int r, int g, int b);
        public Pdf SetDrawColor(int r, int g, int b)
        {
            flypdf_SetDrawColor(this.self_, r, g, b);
            return this;
        }

        public Pdf SetDrawColor(int r, int g)
        {
            return SetDrawColor(r, g, -1);
        }

        public Pdf SetDrawColor(int r)
        {
            return SetDrawColor(r, -1);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Rect(IntPtr p, double x, double y, double w, double h, string style);
        public Pdf Rect(double x, double y, double w, double h, string style)
        {
            flypdf_Rect(this.self_, x, y, w, h, style);
            return this;
        }

        public Pdf Rect(double x, double y, double w, double h)
        {
            return Rect(x, y, w, h, "");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Text(IntPtr p, double x, double y, string txt);
        public Pdf Text(double x, double y, string txt)
        {
            flypdf_Text(this.self_, x, y, txt);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Ln(IntPtr p, int h);
        public Pdf Ln(int h)
        {
            flypdf_Ln(this.self_, h);
            return this;
        }

        public Pdf Ln()
        {
            return Ln(-1);
        }

        [DllImport(flib)]
        private static extern double flypdf_GetX(IntPtr p);
        public double GetX()
        {
            return flypdf_GetX(this.self_);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetX(IntPtr p, double x);
        public Pdf SetX(double x)
        {
            flypdf_SetX(this.self_, x);
            return this;
        }

        [DllImport(flib)]
        private static extern double flypdf_GetY(IntPtr p);
        public double GetY()
        {
            return flypdf_GetY(this.self_);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetY(IntPtr p, double y);
        public Pdf SetY(double y)
        {
            flypdf_SetY(this.self_, y);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetXY(IntPtr p, double x, double y);
        public Pdf SetXY(double x, double y)
        {
            flypdf_SetXY(this.self_, x, y);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Cell(IntPtr p, double w, double h, string txt, string border, double ln, string align, int fill, string link);
        public Pdf Cell(double w, double h, string txt, string border, double ln, string align, int fill, string link)
        {
            flypdf_Cell(this.self_, w, h, txt, border, ln, align, fill, link);
            return this;
        }

        public Pdf Cell(double w, double h, string txt, string border, double ln, string align, int fill)
        {
            return Cell(w, h, txt, border, ln, align, fill, "");
        }

        public Pdf Cell(double w, double h, string txt, string border, double ln, string align)
        {
            return Cell(w, h, txt, border, ln, align, 0);
        }

        public Pdf Cell(double w, double h, string txt, string border, double ln)
        {
            return Cell(w, h, txt, border, ln, "");
        }

        public Pdf Cell(double w, double h, string txt, string border)
        {
            return Cell(w, h, txt, border, 0);
        }

        public Pdf Cell(double w, double h, string txt)
        {
            return Cell(w, h, txt, "");
        }

        public Pdf Cell(double w, double h)
        {
            return Cell(w, h, "");
        }

        public Pdf Cell(double w)
        {
            return Cell(w, 0);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Write(IntPtr p, double h, string txt, string link);
        public Pdf Write(double h, string txt, string link)
        {
            flypdf_Write(this.self_, h, txt, link);
            return this;
        }

        public Pdf Write(double h, string txt)
        {
            return Write(h, txt, "");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetFont(IntPtr p, string family, string style, double size);
        public Pdf SetFont(string family, string style, double size)
        {
            flypdf_SetFont(this.self_, family, style, size);
            return this;
        }

        public Pdf SetFont(string family, string style)
        {
            return SetFont(family, style, 0);
        }

        public Pdf SetFont(string family)
        {
            return SetFont(family, "");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetFontSize(IntPtr p, double size);
        public Pdf SetFontSize(double size)
        {
            flypdf_SetFontSize(this.self_, size);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_AddTrueTypeFont(IntPtr p, string font_file, string charset, string ofamily, string ostyle);
        public Pdf AddTrueTypeFont(string font_file, string family, string style, string charset)
        {
            flypdf_AddTrueTypeFont(this.self_, font_file, charset, family, style);
            return this;
        }

        public Pdf AddTrueTypeFont(string font_file, string family, string style)
        {
            return AddTrueTypeFont(font_file, family, style, "");
        }

        public Pdf AddTrueTypeFont(string font_file, string family)
        {
            return AddTrueTypeFont(font_file, family, "");
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_AddType1Font(IntPtr p, string afm_file, string font_file, string charset, string ofamily, string ostyle);
        public Pdf AddType1Font(string afm_file, string font_file, string family, string style, string charset)
        {
            flypdf_AddType1Font(this.self_, afm_file, font_file, charset, family, style);
            return this;
        }

        public Pdf AddType1Font(string afm_file, string font_file, string family, string style)
        {
            return AddType1Font(afm_file, font_file, family, style, "");
        }

        public Pdf AddType1Font(string afm_file, string font_file, string family)
        {
            return AddType1Font(afm_file, font_file, family, "");
        }

        [DllImport(flib)]
        private static extern string flypdf_csharp_get_string_from_ss(IntPtr p);

        [DllImport(flib)]
        private static extern void flypdf_csharp_dealloc_string_from_ss(IntPtr p);

        [DllImport(flib)]
        private static extern IntPtr flypdf_csharp_SaveReducedTrueTypeFont(IntPtr p, string font_file, string charset);
        public string SaveReducedTrueTypeFont(string font_file, string charset)
        {
            IntPtr tmp_result_file = flypdf_csharp_SaveReducedTrueTypeFont(this.self_, font_file, charset);
            string result_file = flypdf_csharp_get_string_from_ss(tmp_result_file);
            flypdf_csharp_dealloc_string_from_ss(tmp_result_file);
            return result_file;
        }

        [DllImport(flib)]
        private static extern double flypdf_GetStringWidth(IntPtr p, string s);
        public double GetStringWidth(string s)
        {
            return flypdf_GetStringWidth(this.self_, s);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Image(IntPtr p, string image, double x, double y, double w, double h, string type, string link);
        public Pdf Image(string image_path, double x, double y, double w, double h, string type, string link)
        {
            flypdf_Image(this.self_, image_path, x, y, w, h, type, link);
            return this;
        }

        public Pdf Image(string image_path, double x, double y, double w, double h, string type)
        {
            return Image(image_path, x, y, w, h, type, "");
        }

        public Pdf Image(string image_path, double x, double y, double w, double h)
        {
            return Image(image_path, x, y, w, h, "");
        }

        public Pdf Image(string image_path, double x, double y)
        {
            return Image(image_path, x, y, 0, 0);
        }

        [DllImport(flib)]
        private static extern int flypdf_AddLink(IntPtr p);
        public int AddLink()
        {
            return flypdf_AddLink(this.self_);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_SetLink(IntPtr p, int link, double y, int page);
        public Pdf SetLink(int link, double y, int page)
        {
            flypdf_SetLink(this.self_, link, y, page);
            return this;
        }

        public Pdf SetLink(int linkid, double y)
        {
            return SetLink(linkid, y, -1);
        }

        public Pdf SetLink(int linkid)
        {
            return SetLink(linkid, 0);
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_ExternalLink(IntPtr p, double x, double y, double w, double h, string url);
        public Pdf ExternalLink(double x, double y, double w, double h, string url)
        {
            flypdf_ExternalLink(this.self_, x, y, w, h, url);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_InternalLink(IntPtr p, double x, double y, double w, double h, int linkid);
        public Pdf InternalLink(double x, double y, double w, double h, int linkid)
        {
            flypdf_InternalLink(this.self_, x, y, w, h, linkid);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_TextWithDirection(IntPtr p, double x, double y, string txt, string direction);
        public Pdf TextWithDirection(double x, double y, string txt, string direction)
        {
            flypdf_TextWithDirection(this.self_, x, y, txt, direction);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_TextWithRotation(IntPtr p, double x, double y, string text, double txt_angle, double font_angle);
        public Pdf TextWithRotation(double x, double y, string text, double txt_angle, double font_angle)
        {
            flypdf_TextWithRotation(this.self_, x, y, text, txt_angle, font_angle);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_RoundedRect(IntPtr p, double x, double y, double w, double h, double r, string style, string angle);
        public Pdf RoundedRect(double x, double y, double w, double h, double r, string style, string angle)
        {
            flypdf_RoundedRect(this.self_, x, y, w, h, r, style, angle);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Circle(IntPtr p, double x, double y, double r, string style);
        public Pdf Circle(double x, double y, double r, string style)
        {
            flypdf_Circle(this.self_, x, y, r, style);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_Ellipse(IntPtr p, double x, double y, double rx, double ry, string style);
        public Pdf Ellipse(double x, double y, double rx, double ry, string style)
        {
            flypdf_Ellipse(this.self_, x, y, rx, ry, style);
            return this;
        }

        [DllImport(flib)]
        private static extern IntPtr flypdf_MultiCell(IntPtr p, double w, double h, string txt, string border, string align, int fill);
        public Pdf MultiCell(double w, double h, string txt, string border, string align, int fill)
        {
            flypdf_MultiCell(this.self_, w, h, txt, border, align, fill);
            return this;
        }

        public Pdf MultiCell(double w, double h, string txt, string border, string align)
        {
            return MultiCell(w, h, txt, border, align, 0);
        }

        public Pdf MultiCell(double w, double h, string txt, string border)
        {
            return MultiCell(w, h, txt, border, "J");
        }

        public Pdf MultiCell(double w, double h, string txt)
        {
            return MultiCell(w, h, txt, "");
        }
    }
}
