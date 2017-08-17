/**
 * Daniel Stodulka
 * dstodu@gmail.com
 * 
 * Sample GUI for render library, default values for rendering sphere and triangle - Set -> Add -> Render
 * no need for pushing set if adding more stuff
 * 
 * 
 * 
 * */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using rayLib;

namespace RayGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        // our tracing library variable
        public Raytracer rt;


        // init library and init comboboxes for colors
        public MainWindow()
        {
            rt = new Raytracer();
            InitializeComponent();
            InitCombo();
        }

        private string[] LoadComboBoxData()
        {
            string[] strArray =
            {
                "Red",
                "Green",
                "Blue"
            };
            return strArray;
        }

        public void InitCombo()
        {
            lightCombo.ItemsSource = LoadComboBoxData();
            lightCombo.SelectedIndex = 0;

            triangleCombo.ItemsSource = LoadComboBoxData();
            triangleCombo.SelectedIndex = 0;

            sphereCombo.ItemsSource = LoadComboBoxData();
            sphereCombo.SelectedIndex = 0;

            planeCombo.ItemsSource = LoadComboBoxData();
            planeCombo.SelectedIndex = 0;
        }

       

        // render pixel array returned by raytracing
        public void RenderCanvas ()
        {
            try
            {
                rt.RenderCanvas();
            }
            catch(Exception ex)
            {
                textBlock.Text = ex.ToString();
            }

            var height = rt.h;
            var width = rt.w;

            rImg.Width = width;
            rImg.Height = height;

            var pixelFormat = PixelFormats.Bgra32;

            try
            {
                var bitmap = BitmapSource.Create(width, height, 96, 96, pixelFormat, null, rt.pixelData, width * 4);
                rImg.Source = bitmap;
            }
            catch(Exception ex)
            {
                textBlock.Text = ex.ToString();
            }
        }


        // sets camera and canvas size
        private void bSet_Click(object sender, RoutedEventArgs e)
        {
           ParseCamera();
           ParseCanvas();
       } 

        // parse camera pos, vec and set camera
        public void ParseCamera()
        {
            CPoint p1 = ParseText(camPointTBox.Text.Split(';'));
            CPoint vec = ParseText(camVectorTBox.Text.Split(';'));
            rt.SetCamera(p1,vec);
        }

        // parse canvas size and set it
        public void ParseCanvas()
        {
            int w = 0, h = 0;

            string[] foo = canvasTBox.Text.Split(';');
            if (foo.Length == 2)
            {
                int.TryParse(foo[0], out w);
                int.TryParse(foo[1], out h);
            }

            rt.SetCanvas(w,h);
        }


        // small function for parsing text boxes containing 3d points
        CPoint ParseText(string[] foo)
        {
            CPoint p = new CPoint(0,0,0);
            
            try
            {
                p.x = float.Parse(foo[0]);
                p.y = float.Parse(foo[1]);
                p.z = float.Parse(foo[2]);
            }
            catch(Exception ex)
            {
                textBlock.Text = ex.ToString();
            }
            return p;
        }

        // parse color from combobox
        CColor ParseColor(string foo)
        {
            CColor c = new CColor(0,0,0);

            if (foo == "Red")
                c.r = 255;
            else if (foo == "Green")
                c.g = 255;
            else
                c.b = 255;

            return c;
        }

        // adding object and lights into appropriate lists
        private void bLightAdd_Click(object sender, RoutedEventArgs e)
        {
            CPoint p1 = ParseText(lightPointTBox.Text.Split(';'));
            CColor c = ParseColor(lightCombo.SelectedItem.ToString());
            rt.AddLight(new Light(p1,c));
        }

        private void bTriangleAdd_Click(object sender, RoutedEventArgs e)
        {
            CPoint p1 = ParseText(trianglePointTBox.Text.Split(';'));
            CPoint p2 = ParseText(trianglePointTBox2.Text.Split(';'));
            CPoint p3 = ParseText(trianglePointTBox3.Text.Split(';'));
            CColor c = ParseColor(triangleCombo.SelectedItem.ToString());
            rt.AddObject(new Triangle(p1,p2,p3,c));
        }

        private void bSphereAdd_Click(object sender, RoutedEventArgs e)
        {
            CPoint p1 = ParseText(spherePointTBox.Text.Split(';'));
            CColor c = ParseColor(sphereCombo.SelectedItem.ToString());

            float radius;
            float.TryParse(spehreRadiusTBox.Text,out radius);

            rt.AddObject(new Sphere(p1,radius,c));
        }

        private void bPlaneAdd_Click(object sender, RoutedEventArgs e)
        {
            CPoint p1 = ParseText(planePointTBox.Text.Split(';'));
            CPoint vec = ParseText(planeVectorTBox.Text.Split(';'));
            CColor c = ParseColor(planeCombo.SelectedItem.ToString());

            rt.AddObject(new Plane(p1,vec,c));

        }


        // render scene
        private void bRender_Click(object sender, RoutedEventArgs e)
        {
            RenderCanvas();
        }
    }
}