using System;
using System.IO;
using System.Text.Json;
using Gdk;
using Gtk;
using net_draw.tests;
using Pango;
using Color = System.Drawing.Color;
using Context = Cairo.Context;
using Layout = Pango.Layout;
using Window = Gtk.Window;

namespace net_draw
{
    class Program
    {
        static void Main()
        {
            Application.Init ();
 
            var window = new MainForm();
            window.DeleteEvent += (o, args) => Application.Quit();
            window.Show();
 
            Application.Run ();
        } 
    }
    

    class MainForm : Window
    {
        public MainForm() : base("main")
        {


            SetDefaultSize(1000, 600);
            SetPosition(WindowPosition.Center);
            DeleteEvent += delegate { Application.Quit(); };;
            
            
            var options = new JsonSerializerOptions {
                PropertyNameCaseInsensitive = true,
            };

            var file = @"/Users/rinat/proj/go-gtk/parser/cases/obj_RAM.json";
            
            //var file = @"/Users/rinat/proj/go-gtk/parser/cases/dexter_main.json";

            var text = File.ReadAllText(file);
            var obj = JsonSerializer.Deserialize<ObjectDef>(text, options);

            this.Title = obj.Proto.Name;
        
            var darea = new DrawingArea();
            darea.Drawn += (o, args) => DareaOnDrawn(o, args, obj);

            Add(darea);

            ShowAll();
            
        }

        static Cairo.Color to(Color c) {
            return new Cairo.Color(c.R,c.G, c.B, c.A);
        }
        
        private void draw_text(Context cr, int x, int y, string text)
        {
            var layout = MakeLayout(text);

            int text_width;
            int text_height;

            //get the text dimensions (it updates the variables -- by reference)
            layout.GetPixelSize(out text_width, out text_height);

            // Position the text in the middle
            cr.MoveTo(x, y);

            Pango.CairoHelper.ShowLayout(cr, layout);
        }

        private Layout MakeLayout(string text, bool bold = false) {
            // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
            Pango.FontDescription font = new Pango.FontDescription();
            
            font.Family = "Arial";
            font.Size = Convert.ToInt32(11 * Pango.Scale.PangoScale);

            if (bold) {
                font.Weight = Weight.Semibold;
            }

            // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
            Pango.Layout layout = CreatePangoLayout(text);

            layout.FontDescription = font;
            return layout;
        }

        private void DareaOnDrawn(object o, DrawnArgs args, ObjectDef trg)
        {

            var da = o as DrawingArea;
            using var cr = args.Cr;
            using var target = cr.GetTarget();
            
            
            var width = da.Allocation.Width;
            var height = da.Allocation.Height;

            var ViewScaleDefault = 5;
            var iowidth = 5.41;

            
            cr.SetSourceColor(to(Color.Black));
            foreach (var p in trg.Texts) {
                    var x = p.Gui[0] * 5;
                    var y = p.Gui[1] * 5;
                    draw_text(cr, x, y, p.Text);
            }

            foreach (var t in trg.Net) {
                if (t.Gui.Count > 1) {
                    for (int i = 0; i < t.Gui.Count; i++) {

                        var p = t.Gui[i];

                        var x = p[0] * 5;
                        var y = p[1] * 5;

                        cr.SetSourceColor(to(Color.Blue));
                        cr.LineWidth = 3;

                        if (i == 0) {

                            cr.MoveTo(x, y);
                        }
                        else {
                            cr.LineTo(x, y);
                        }

                        cr.StrokePreserve();
                    }

                }
            }

            foreach (var p in trg.Behavior) {
                
                

                try {
                    
                    var x = p.Gui[0] * 5;
                    var y = p.Gui[1] * 5;
                    
                    if (p.Name.StartsWith("Junction")) {
                        cr.SetSourceColor(to(Color.Red));
                        cr.LineWidth = 3;
                        cr.MoveTo(x-4,y);
                        cr.RelLineTo(8,0);
                        
                        cr.StrokePreserve();
                        cr.MoveTo(x, y-4);
                        cr.RelLineTo(0,8);
                        cr.StrokePreserve();
                        continue;
                    }

                    if (p.Name.StartsWith("Input") ) {
                        cr.MoveTo(x,y);
                        cr.RelLineTo(0,-4);
                        cr.RelLineTo(4,4);
                        cr.RelLineTo(-4, 4);
                        cr.LineTo(x,y);
                        cr.StrokePreserve();

                        var name = p.Outputs[0][1];

                        var l = MakeLayout(name, bold:true);
                        int ph;
                        int pw;
                        l.GetPixelSize(out pw, out ph);
                        
                        cr.MoveTo(x - pw - 9, y - ph/2);
                        
                        cr.SetSourceColor(to(Color.Black));
                        Pango.CairoHelper.ShowLayout(cr, l);
                        
                        continue;
                    }

                    if (p.Name.StartsWith("Output")) {
                        
                        cr.MoveTo(x,y);
                        cr.RelLineTo(4,-4);
                        cr.RelLineTo(0,9);
                        cr.RelLineTo(-4, -4);
                        cr.LineTo(x,y);
                        cr.StrokePreserve();
                        
                        

                        var name = p.Inputs[0][1];

                        var l = MakeLayout(name, bold:true);
                        int ph;
                        int pw;
                        l.GetPixelSize(out pw, out ph);
                        
                        cr.MoveTo(x + 9, y - ph/2);
                        
                        cr.SetSourceColor(to(Color.Black));
                        Pango.CairoHelper.ShowLayout(cr, l);
                        
                        continue;
                        
                        continue;
                    }
                    
                    
                    cr.LineWidth = 2;

                    cr.SetSourceColor(to(Color.Blue));

                    var ios = Math.Max(p.Inputs.Count, p.Outputs.Count);

                    var layout = MakeLayout(p.Name);
                    int tw, th;
                        
                    layout.GetPixelSize(out tw, out th);

                    var box_width = Math.Max(tw + 5, 45);

                    cr.Rectangle(x, y, box_width, 15 * ios + 10);
                    cr.LineWidth = 1;
                    cr.Stroke();

                    const int boxSize = 6;

                    for (int i = 0; i < p.Inputs.Count; i++) {
                        cr.SetSourceColor(to(Color.Red));
                        
                        cr.Rectangle(x-boxSize, y + 7 + 15*i, boxSize, boxSize);
                        cr.Fill();
                        cr.MoveTo(x +1, y+13+15*i ); 
                        cr.SetFontSize(10);
                        cr.ShowText(p.Inputs[i][1]);
                    }
                    
                    
                    for (int i = 0; i < p.Outputs.Count; i++) {
                        cr.SetSourceColor(to(Color.Red));
                          
                        cr.Rectangle(x+box_width, y + 7 + 15*i, boxSize, boxSize);
                        cr.Fill();
                        cr.MoveTo(x +box_width, y+13+15*i ); 
                        cr.SetFontSize(10);
                        cr.ShowText(p.Outputs[i][1]);
                    }

                    cr.SetSourceColor(to(Color.White));
                    cr.Fill();
                    cr.SetSourceColor(to(Color.Red));
                    cr.MoveTo(p.Gui[0] * 5 + 2, p.Gui[1] * 5 - th);
                    Pango.CairoHelper.ShowLayout(cr, layout);
                }
                catch {
                    throw new Exception($"Problem with {p}");
                }
            }
        }
    }
}