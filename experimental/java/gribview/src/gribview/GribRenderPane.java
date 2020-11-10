
package gribview;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;
import java.util.*;
import java.awt.image.*;
import java.awt.print.PrinterJob;
import java.awt.event.*;
import java.awt.print.*;
import jgribapi.*;

/**
 * <p>Title: </p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2005</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class GribRenderPane extends JComponent implements Printable {
    GribHandle handle = null;
    GribHandle cachedHandle = null;
    Image image = null;

    public static final int DRAG_MAP = 0;
    public static final int ADD_MARKER = 1;
    double vals[];
     int ni = 1;
    int batard = 0;
    int mode = DRAG_MAP;
    // projection
    double zoom = 1;
    double centerx = Double.MAX_VALUE;
    double centery = Double.MAX_VALUE;

    // original data
    double olx  = 0;
    double oly  = 0;
    double ofx  = 0;
    double ofy  = 0;

    // first projection
    double plx  = 0;
    double ply  = 0;
    double pfx  = 0;
    double pfy  = 0;

    int oldMousX = 0;
    int oldMousY = 0;

    double refx = 0;
    double refy = 0;
    GribMainFrame mainFrame;

    public GribRenderPane(GribMainFrame mainFrame) {

        this.mainFrame = mainFrame;
        GribRenderPane_jRenderPane_mouseAdapter myAdapter = new
                GribRenderPane_jRenderPane_mouseAdapter(this);
        addMouseMotionListener(myAdapter);
        addMouseWheelListener(myAdapter);
        addMouseListener(myAdapter);
    }

    public void setHandle(GribHandle g) {
        this.handle = g;
        cachedHandle = null;
    }

    public void setMode(int mode) {
        this.mode = mode;
    }





    public void paintImage(Graphics g, Component o, int x, int y, int width,
                           int height) {



        Graphics2D gr = (Graphics2D) g;
/*        gr.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
              RenderingHints.VALUE_ANTIALIAS_ON);
               gr.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
             RenderingHints.VALUE_INTERPOLATION_BICUBIC);*/


        if (cachedHandle == null) {
              boolean sph = false;



             vals = handle.getDoubleArray("values");
             double missvals = handle.getDouble("missingValue");

             ofx = handle.getDouble("geography.loFirst");
             ofy  = handle.getDouble("geography.laFirst");

             olx = handle.getDouble("geography.loLast");
             oly = handle.getDouble("geography.laLast") ;
             if (ofx == ofy) {
                 ofx = 0;
                 ofy = 90;
             }
             if (olx == oly) {
                 olx = 360;
                 oly = -90;
             }
            if((Double.MAX_VALUE == centerx)||(plx!=olx)||(ply!=oly)||(pfx!=ofx)||(pfy!=ofy))
             {
              centerx = (olx+ofx)/2;
              centery = (oly+ofy)/2;
               plx  = olx;
               ply  = oly;
               pfx  = ofx;
               pfy  = ofy;
               zoom = 1;
            }

         //   System.out.println("ox:" + ofx +" oy:" + ofy +" olx:" + olx+" oly:" +oly);
             ni = (int) handle.getLong("geography.ni");
            if (ni == 0 || ni == 65535) {
                ni = (int)handle.getLong("pentagonalResolutionParameterJ");
                if (ni == 0 || ni == 65535) {
                    ni = (int) Math.sqrt(vals.length);
                }
                else
                {
                 sph = true;
                }
            }
            if (ni == 0) {
                return;
            }



        if(sph){
            ni = (int)handle.getLong("pentagonalResolutionParameterJ");
            ni ++;
            double tvals[] = handle.getDoubleArray("values");
            int rp = 0;
            int cp = 1;

            for (int j = 0; j < ni; j++){
                    for (int i = j; i < ni; i++){
                           tvals[rp] *= Math.pow(i*(i+1),1);
                           vals[j*ni+i] = tvals[rp];
                           rp+=2;
                    }
                    for (int i = j; i < ni; i++){
                        tvals[cp]   *= Math.pow(i*(i+1),1);
                        vals[i*ni+j] = tvals[cp];
                        cp+=2;
                    }


            }

        }






            handle.maxVal = vals[0];
            handle.minVal = handle.maxVal;

            for (int i = 0; i < vals.length; i++) {
                if (vals[i] != missvals){
                     if(handle.maxVal == missvals) handle.maxVal = vals[i];
                  if (handle.maxVal < vals[i]) {
                    handle.maxVal = vals[i];
                  }
                  if (handle.minVal > vals[i]) {
                    handle.minVal = vals[i];
                  }
              }
            }
            int pixels[] = new int[vals.length];
            int mint = 0;
            double shade = 1.0;
            mint &= 0;
         //   System.out.println("nvals : "+ vals.length+" ni : "+ ni + " max " +handle.maxVal + " min " + handle.minVal);


            for (int i = 0; i < vals.length; i++) {
                if (vals[i] == missvals) pixels[i] = 255 << 24 | 255 << 16 | 0 << 8 | 0;
                else{

                int v = (int) (((vals[i] - handle.minVal) /
                                (handle.maxVal - handle.minVal)) * (255.0 *shade));
                /*
                            int red = v << 16;
                            int green = v << 8;
                            int blue = v ;*/
                int red = 0;
                int green = 0;
                int blue = 0;

              if (v <= 255) {
                    red = v;
                    green = v;
                    blue = v;
                }
/*
                 if (v <= 255) {
                     red = v;
                     green = 0;
                     blue = 0;
                 }
                if ((v > 255) && (v <= 510)) {
                    v -= 255;

                    red = 255 - v;
                    green = v;
                    blue = 0;
                }

                if ((v > 510) && (v <= 765)) {
                    v -= 510;

                    red = 0;
                    green = 255 - v;
                    blue = v;
                }
                if ((v > 765) && (v <= 1020)) {
                    v -= 765;

                    red = v;
                    green = 0;
                    blue = 255 - v;
                }
                if ((v > 1020) && (v <= 1275)) {
                    v -= 1020;

                    red = 255;
                    green = v;
                    blue = 0;
                }
                if ((v > 1275) && (v <= 1530)) {
                    v -= 1275;
                    red = 255;
                    green = 255 - v;
                    blue = v;
                }
                if ((v > 1530) && (v <= 1785)) {
                    v -= 1530;
                    red = 255 - v;
                    green = 0;
                    blue = 255;
                }
                if ((v > 1785) && (v <= 2040)) {
                    v -= 1785;
                    red = 0;
                    green = v;
                    blue = 255;
                }
                if ((v > 2040) && (v <= 2295)) {
                    v -= 2040;
                    red = v;
                    green = 255;
                    blue = 255;
                }*/



                red = red << 16;
                green = green << 8;

                pixels[i] = 255 << 24 | red | green | blue;
             }
            }

            /*
                    ColorModel cmi = generateColorModel();
                    DataBuffer dbuf = new DataBufferByte(pixels,vals.length,0);
                    int numBanks = dbuf.getNumBanks();
                    int bitMasks[] = new int[]{(byte)255};
                    SampleModel sampleModel = new SinglePixelPackedSampleModel(DataBuffer.TYPE_BYTE , ni, vals.length/ni, bitMasks);
             WritableRaster raster = Raster.createWritableRaster(sampleModel, dbuf, null);
             BufferedImage image = new BufferedImage(cmi, raster, false, null);
             */
            MemoryImageSource mr = new MemoryImageSource(ni, pixels.length / ni,
                    pixels, 0, ni);
            image = Toolkit.getDefaultToolkit().createImage(mr);

            /*        for (int i = 0; i < vals.length; i++) {
                        int vi = (int) (((vals[i] - handle.minVal) /
             (handle.maxVal - handle.minVal)) * 255);
                        gr.setColor(new Color(vi, vi, vi));

                        if (vals[i] == handle.maxVal) {
                            gr.setColor(Color.red);
                        }
                        if (vals[i] == handle.minVal) {
                            gr.setColor(Color.blue);
                        }
                        gr.fillRect((i % ni), i / ni, 1, 1);
                    }
             */
            //   gr.setColor(new Color(100, 255, 200));
            //   gr.fillRect(0,0,width,height);


            cachedHandle = handle;



        }
        paintProjectedImage(gr);
    }

    void paintProjectedImage(Graphics2D gr){
        boolean drawpath=false;

        double lw = getWidth()*zoom;
        double lh = getHeight()*zoom;



        double fx = (centerx-ofx)/(olx - ofx);
        double fy = (centery-ofy)/(oly - ofy);


        double lx = (zoom*getWidth())*fx;
        double ly = (zoom*getHeight())*fy;

       lx = getWidth()/2 - lx;
       ly = getHeight()/2 - ly;
        gr.drawImage(image, (int)(lx), (int)(ly),(int)(lw),(int)(lh), this);
     if(drawpath)
       {
            GeneralPath gpath = new GeneralPath();
          for (int i = 0; i < getComponentCount(); i++) {
            JGribMarker mark =  ((JGribMarker)  getComponent(i));

            if(i>0){
               JGribMarker previous =  ((JGribMarker)  getComponent(i-1));
               gr.setColor(new Color(255,50,50,100));
               gr.setStroke(new BasicStroke(2.0f));
               double x1 = previous.getX()+20;
               double y1 = previous.getY()+20;
               double x2 = mark.getX()+20;
               double y2 = mark.getY()+20;

               double cx1 = x1;
               double cy1 = y1;
               double cx2 = x2;
               double cy2 = y2;

               if(i < getComponentCount()-1){
               JGribMarker next =  ((JGribMarker)  getComponent(i+1));
                 double x3 = next.getX()+20;
                 double y3 = next.getY()+20;

                  cx2 = x2+((x2 - ((x1+x3)/2))/2);
                  cy2 = y2+((y2 - ((y1+y3)/2))/2);

                  x3 = cx2;
                  cx2 = x2+(cy2-y2);
                  cy2 = y2-(x3-x2);

               }

               if(i > 1){
                   JGribMarker prevprevious =  ((JGribMarker)  getComponent(i-2));

                double x0 = prevprevious.getX()+20;
                double y0 = prevprevious.getY()+20;

                  cx1 = x1-((x1 - ((x0+x2)/2))/2);
                  cy1 = y1-((y1 - ((y0+y2)/2))/2);

                  x0 =cx1;
                  cx1 = x1+(cy1-y1);
                  cy1 = y1-(x0-x1);

               }
               gr.fillOval((int)cx1+1,(int)cy1+1,3,3);
               gr.fillOval((int)cx2+1,(int)cy2+1,3,3);
               gpath.curveTo((float)cx1,(float)cy1,(float)cx2,(float)cy2,(float)x2,(float)y2);
             }

             else
             {

                 gpath.moveTo(mark.getX()+20,mark.getY()+20);
             }



         }

           //   if(getComponentCount() > 2) gpath.closePath();
          gr.draw(gpath);
       }
        else{

            for (int i = 0; i < getComponentCount(); i++)
               ((JGribMarker)  getComponent(i)).paint(gr);


           }
   }


    public void paint(Graphics gr) {
        if (handle != null) {
            Graphics2D g = (Graphics2D) gr;

            if (handle != null) {
                paintImage(g, this, 0, 0, this.getWidth(),
                           this.getHeight());
  /*           g.setColor(Color.black);
               g.fillRect(0, this.getHeight() - 20, this.getWidth(), 20);*/

                g.setColor(new Color(255,255,255,100));
                           g.fillRect(0, this.getHeight() - 20, this.getWidth(), 20);

                g.setColor(Color.black);

                g.drawString("Maximum :" + handle.maxVal + " Minimum :" +
                             handle.minVal, 10, this.getHeight() - 5);
            }
        }

    }

    public int print(Graphics g, PageFormat pf, int pi) throws PrinterException {
        Graphics2D g2d = (Graphics2D) g;
        if (pi >= 1) {
            return Printable.NO_SUCH_PAGE;
        }
        g2d.translate(pf.getImageableX(), pf.getImageableY());
        paint(g2d);
        return Printable.PAGE_EXISTS;
    }


    public void jRenderPane_mouseWheelMoved(MouseWheelEvent e) {

            double  nzoom = zoom - ((double)e.getWheelRotation()*Math.abs(zoom))/5;
            if (nzoom > 1){
                  zoom = nzoom;
                }
              else
              {
                  zoom = 1;
                  double dcx = (centerx-(olx+ofx)/2);
                  double dcy = (centery-(oly+ofy)/2);
                  if(dcx < 0.01 && dcy < 0.01)
                      {
                           centerx =  (olx+ofx)/2;
                           centery =  (oly+ofy)/2;
                      }
                    else
                    centerx =  centerx-dcx/5;
                    centery =  centery-dcy/5;

               }
               mainFrame.setSlider((int)zoom);
              repaint();
    }

   public void setDragOrigin(MouseEvent e){
       oldMousX = e.getX();
       oldMousY = e.getY();
   }

   public void moving(MouseEvent e){
       if (mode == ADD_MARKER){
           this.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
      }
      if (mode == DRAG_MAP){
          this.setCursor(Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
     }


    }
    public void exiting(MouseEvent e){

    }
   public void drag(MouseEvent e){
      int dx = oldMousX - e.getX();
      int dy = oldMousY - e.getY();


      centerImage(getWidth()/2+dx, getHeight()/2+dy);

      oldMousX = e.getX();
      oldMousY = e.getY();

     repaint();
   }
   public void doubleClicked(MouseEvent e){
    if (mode == DRAG_MAP)
        centerImage(e.getX(),e.getY());
   }
   public void clicked(MouseEvent e){

     if (mode == ADD_MARKER){
         addMarker(e);
         setMode(DRAG_MAP);
     }
   }



   public double getValueAt(Point2D.Double p){
       return vals[getIndexAt(p)];

   }
   public int getIndexAt(Point2D.Double p){


      if((p.getX() < ofx) || ( p.getY() > ofy) ) return 0;

      if((p.getX() > olx) || ( p.getY() < oly) ) return 0;

        double  x =  (p.getX() -ofx)/ (olx-ofx);
        double  y = (p.getY() -ofy)/ (oly-ofy);

       int  nline =  vals.length/ni;
       int pos = ni*((int)(nline * y)) ;
       pos += x*ni;


        if (pos < 0) return 0;
        if (pos > vals.length) return 0;


       return pos;
    }

   private void centerImage(int x, int y){
     Point2D.Double p =  getLonLat(x,y);
     centerx = p.getX();
     centery = p.getY();
     repaint();
  }

 public  Point getPosition(Point2D.Double p){
    Point rep = new Point(10,10);

    double x = -(p.getX()-centerx);
    double y = -(p.getY()-centery);

    x /= (olx-ofx);
    y /= (oly-ofy);

    x *= getWidth();
    y *= getHeight();

    x *= zoom;
    y *= zoom;

    rep.x = (int)-( x -      (getWidth()/ 2));
    rep.y = (int)-( y -     (getHeight()/ 2));

  return rep;
 }

   public Point2D.Double getLonLat(int x, int y){

      double dx =   (getWidth()/ 2)-x ;
      double dy =   (getHeight()/2)-y;

         dx /= zoom;
         dy /= zoom;

         dx /= getWidth();
         dy /= getHeight();

         dx *= (olx-ofx);
         dy *= (oly-ofy);

         dx = centerx - dx;
         dy = centery - dy;

         return new Point2D.Double(dx, dy);
   }

       void  addMarker(MouseEvent e){
            this.add(new JGribMarker(this,getLonLat(e.getX(),e.getY())));
            repaint();
      }
}

class GribRenderPane_jRenderPane_mouseAdapter implements
        MouseWheelListener, MouseMotionListener , MouseListener{
    private GribRenderPane adaptee;
        GribRenderPane_jRenderPane_mouseAdapter(GribRenderPane adaptee) {
        this.adaptee = adaptee;
    }

    public void mouseWheelMoved(MouseWheelEvent e) {
        adaptee.jRenderPane_mouseWheelMoved(e);
    }
    public void mouseMoved(MouseEvent e) {
        adaptee.moving(e);
    }
    public void mouseDragged(MouseEvent e) {
        adaptee.drag(e);
  }
  public void mouseReleased(MouseEvent e) {

  }
  public void mousePressed(MouseEvent e) {
       adaptee.setDragOrigin(e);
  }
  public void mouseExited(MouseEvent e) {
      adaptee.exiting(e);
  }
  public void mouseEntered(MouseEvent e) {
  }
  public void mouseClicked(MouseEvent e) {
      if(2 == e.getClickCount())
       adaptee.doubleClicked(e);
      else
        adaptee.clicked(e);
  }
}
