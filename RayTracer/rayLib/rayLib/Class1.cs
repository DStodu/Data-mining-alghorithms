/**
 * Simple raytracing library 
 * Daniel Stodulka
 * dstodu@gmail.com
 * 
 * !Disclaimer!
 * Reflecting - "bouncing" rays not implemented, got poor results and decided to go just with single ray from camera point
 * Plane intersection might be a bit sketchy
 * Colors using rgb model - blending - c1/2+c2/2, multiple lights - blend all lights and then object
 * 
 * 
 * */


using System;
using System.Collections;

namespace rayLib
{
    // For pure test purposes
    /*public static class Testing
    {
        public static CPoint first;
        public static double second;
        public static bool debug;
        public static int xpl;
        public static int ypl;
    }*/
    

    /**
     * list for objects and lights for easy handling
     * 
     * 
     * 
     * 
     * 
     * */
    
    public class Raytracer
    {
        public static ArrayList objects;
        public static ArrayList lights;
        public Camera cam;
        public int w, h;
        public byte[] pixelData;

        public Raytracer()
        {
            objects = new ArrayList();
            lights = new ArrayList();
          /*  Testing.second = -1;
            Testing.debug = false;
            Testing.xpl = 0;
            Testing.ypl = 0;
            Testing.first = new CPoint(0,0,0);*/

        }

        // init functions pretty self-explanatory add object, set came, canvas
        public void AddObject(CObject foo)
        {
            objects.Add(foo);
        }

        public void AddLight(Light foo)
        {
            lights.Add(foo);
        }

        public void SetCanvas(int w, int h)
        {
            this.w = w;
            this.h = h;
        }

        public void SetCamera(CPoint p1, CPoint foo)
        {
            cam = new Camera(p1,foo);
            cam.fov = 45;
        }


        /**
         * Cast rays from cam point to whole scene and get appropriate colors whethere it hits object and light is nearby
         * Also creates pixel array for bitmap image in GUI
         * 
         * 
         * 
         * */
        public void RenderCanvas()
        {
            pixelData = new byte[w*h*4];
            CPoint eyeVec = CPoint.Subs(cam.vec,cam.p1);
            eyeVec = CPoint.Unit(eyeVec);
            
            CPoint vpRight = CPoint.Cross(eyeVec, new CPoint(0,1,0));
            vpRight = CPoint.Unit(vpRight);

            CPoint vpUP = CPoint.Cross(vpRight,eyeVec);
            vpUP = CPoint.Unit(vpUP);


            double fovRadians = (double)Math.PI * (cam.fov / 2.0) / 180.0;

            double heightWidthRatio = h / w;
            double halfWidth = Math.Tan(fovRadians);

            double halfHeight = heightWidthRatio * halfWidth;
            double camerawidth = halfWidth * 2;
            double cameraheight = halfHeight * 2;
            double pixelWidth = camerawidth / (w- 1);
            double pixelHeight = cameraheight / (h - 1);

            Ray ray = new Ray();
            ray.p1 = cam.p1;
            ray.vec = new CPoint(0,0,0);


            for(int i = 0; i < w; i++)
            {
                for(int j = 0; j < h; j++)
                {
                    CPoint xcomp = CPoint.Scale(vpRight, (i * pixelWidth) - halfWidth);
                    CPoint ycomp = CPoint.Scale(vpUP, (j * pixelHeight) - halfHeight);

                    CPoint temp = CPoint.Add(eyeVec,xcomp);
                    temp = CPoint.Add(temp, ycomp);
                    temp = CPoint.Unit(temp);

                    ray.vec = temp;
                    CColor c = GetColor(ray);

                    int index = (i * 4) + (j * w * 4);
                    pixelData[index + 0] = c.b;
                    pixelData[index + 1] = c.g;
                    pixelData[index + 2] = c.r;
                    pixelData[index + 3] = 255;
                }
            }
        }


        /**
         * Intersection with a object? 
         * If ray hits, call surface which determines color of the point
         * 
         * */
        private CColor GetColor(Ray ray)
        {
            double dist;
            CObject foo = GetClosestInter(ray, out dist);

            if (dist == double.MaxValue)
                return new CColor(0,0,0);

            CPoint tpoint = CPoint.Scale(ray.vec, dist);

            CPoint pointAtTime = CPoint.Add(ray.p1, tpoint);

            CPoint normal = CPoint.Subs(pointAtTime, foo.p1);
            normal = CPoint.Unit(normal);

            return Surface(ray,foo,pointAtTime,normal);
        }


        /**
         * Based on the distance, color of the object and light color, get final point color
         * All object have default ambient and lambert shading values
         * 
         * */

        private CColor Surface(Ray p, CObject o, CPoint pAT, CPoint normal)
        {
            CPoint b = new CPoint(o.c.r, o.c.g, o.c.b);
            CPoint c = new CPoint(0,0,0);

            CPoint lfinal = new CPoint(0,0,0);
            CPoint ltemp;
         
            double lambertAmount = 0;

            foreach(Light l in lights)
            {
                if (!l.Visible(pAT))
                    continue;
                else
                {
                    ltemp = new CPoint(l.c.r, l.c.g, l.c.b);
                    double cont = CPoint.DotProduct(CPoint.Unit(CPoint.Subs(l.p1,pAT)),normal);

                    if (cont > 0)
                        lambertAmount += cont;

                    if (lambertAmount > 1)
                        lambertAmount = 1;

                    ltemp = CPoint.Scale(ltemp,lambertAmount*0.7*0.5);
                    lfinal = CPoint.Add(lfinal,ltemp);
                }
            }

            CPoint go = CPoint.Add(c, CPoint.Scale(b,lambertAmount * 0.7));

            CPoint final = CPoint.Add(go, CPoint.Scale(b, 0.05));
            final = CPoint.Add(CPoint.Scale(final,0.5),lfinal);

            if (final.x > 255)
                final.x = 255;
            if (final.y > 255)
                final.y = 255;
            if (final.z > 255)
                final.z = 255;


            return new CColor((byte)final.x, (byte)final.y, (byte)final.z);
        }

        /**
         * Based on the casted ray returns distance to the closest intersection and object it hit
         * 
         * 
         * */
        public static CObject GetClosestInter(Ray p, out double distance)
        {
            distance = double.MaxValue;
            CObject foo = null;
            
            double lowDist = double.MaxValue;
            CObject lowObj = null;

            foreach (CObject o in objects)
            {
                foo = o.Intersect(p, out distance);

                if (lowDist > distance)
                {
                    lowDist = distance;
                    lowObj = foo;
                }
            }

            distance = lowDist;
            
            return lowObj;
        }
    }

    public struct Ray
    {
        public CPoint p1;
        public CPoint vec;
    };


    // 3D point and essential vector operations
    public class CPoint
    {
        public double x;
        public double y;
        public double z;

        public CPoint(double x, double y, double z)
        {
            this.x = x;
            this.y = y;
            this.z = z; 
        }

        internal static CPoint Subs(CPoint p1,CPoint p2)
        {
            return new CPoint(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
        }

        internal static CPoint Add(CPoint p1,CPoint p2)
        {
            return new CPoint(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
        }

        internal static CPoint Scale(CPoint p, double t)
        {
            return new CPoint(p.x *t, p.y*t, p.z*t);
        }

        internal static double DotProduct(CPoint p1, CPoint p2)
        {
            return  p1.x * p2.x +  p1.y * p2.y + p1.z * p2.z;
        }

        internal static double Length(CPoint p1)
        {
            return Math.Sqrt(DotProduct(p1,p1));
        }

        internal static CPoint Unit(CPoint p1)
        {
            return Scale(p1, (double)1 / Length(p1));
        }

        internal static CPoint Cross(CPoint p1, CPoint p2)
        {
            return new CPoint(p1.y * p2.z - p1.z * p2.y, p1.z * p2.x - p1.x * p2.z, p1.x * p2.y - p1.y * p2.x);
        }

    }

    public class CColor
    {
        public byte r;
        public byte g;
        public byte b;

        public CColor(byte r, byte g, byte b)
        {
            this.r = r;
            this.g = g;
            this.b = b;
        }
    };


    // primitive object contains color, point and intersection method for overloading 
    public abstract class CObject
    {
        public CPoint p1;
        public CColor c;
        internal abstract CObject Intersect(Ray p, out double distance);

        protected CObject()
        {
            p1 = new CPoint(0,0,0);
            c = new CColor(0,0,0);
        }
    }

    public class Sphere: CObject
    {
        public double radius;

        internal override CObject Intersect(Ray p, out double distance)
        {
            distance = double.MaxValue;
            CPoint eye_to_center = CPoint.Subs(this.p1, p.p1);

            double v = CPoint.DotProduct(eye_to_center,p.vec);
            
            double eoDot = CPoint.DotProduct(eye_to_center, eye_to_center);

            double dis = (this.radius * this.radius) - eoDot + (v * v);

            if(dis < 0)
            {
               // distance = double.MaxValue;
                return null;
            }

            distance = v - Math.Sqrt(dis);
            return this;
        }

        public Sphere(CPoint p1, double radius, CColor c) : base()
        {
            this.p1 = p1;
            this.radius = radius;
            this.c = c;
        }
    }

    public class Plane : CObject
    {
        CPoint vec;

        internal override CObject Intersect(Ray p, out double distance)
        {
            double pDist = 0;
            distance = double.MaxValue;
            double denom = CPoint.DotProduct(p.vec,this.vec);

            if(denom != 0)
            {
                double t = -(CPoint.DotProduct(this.p1, this.vec) + pDist) / denom;
                if(t < 0)
                {
                    return null;
                }
                CPoint v0 = CPoint.Scale(p.vec,t);
                CPoint ret = CPoint.Add(v0,p.p1);
                distance = -CPoint.DotProduct(this.vec,ret);
                return this;
            }
            else if((CPoint.DotProduct(this.vec,p.p1) + pDist) == 0)
            {
                //this.p1 = p.p1;
                distance = -CPoint.DotProduct(this.vec, p.p1);
                return this;
            }
            else
            {
                return null;
            }
        }

        public Plane(CPoint p1, CPoint vec, CColor c) : base()
        {
            this.p1 = p1;
            this.vec = vec;
            this.c = c;
        }
    }

    public class Triangle : CObject
    {
        CPoint p2, p3;

        internal override CObject Intersect(Ray p, out double distance)
        {
            distance = double.MaxValue;

            CPoint e1 = CPoint.Subs(this.p2,this.p1);
            CPoint e2 = CPoint.Subs(this.p3, this.p1);

            CPoint pvec = CPoint.Cross(p.vec,e2);

            double det = CPoint.DotProduct(e1,pvec);

            if (det < 1e-8 && det > -1e-8)
                return null;

            double inv_det = 1.0 / det;

            CPoint tvec = CPoint.Subs(p.p1,this.p1);

            double u = CPoint.DotProduct(tvec,pvec) * inv_det;

            if (u < 0 || u > 1)
                return null;

            CPoint qvec = CPoint.Cross(tvec,e1);

            double v = CPoint.DotProduct(p.vec,qvec) * inv_det;

            if (v < 0 || (u + v) > 1)
                return null;

            distance = CPoint.DotProduct(e2,qvec) * inv_det;
            return this;
        }

        public Triangle(CPoint p1, CPoint p2, CPoint p3, CColor c) : base()
        {
            this.p1 = p1;
            this.p2 = p2;
            this.p3 = p3;
            this.c = c;
        }
    }

    public class Light
    {
        public CPoint p1;
        public CColor c;

        public Light(CPoint p1, CColor c)
        {
            this.p1 = p1;
            this.c = c;
        }

        internal bool Visible(CPoint p) //p = pat
        {
            Ray f = new Ray();
            CPoint foo = CPoint.Unit(CPoint.Subs(p, this.p1));
            f.p1 = p;
            f.vec = foo;

            double dist = 0;
            CObject temp = Raytracer.GetClosestInter(f, out dist);

            return dist > -0.005;
        }
    }

    public class Camera
    {
        public CPoint p1;
        public CPoint vec;
        public int fov;

        public Camera(CPoint p1, CPoint vec)
        {
            this.p1 = p1;
            this.vec = vec;
        }
    }

}
