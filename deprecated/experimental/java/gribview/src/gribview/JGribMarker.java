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
public class JGribMarker extends JComponent {
    public JGribMarker() {
        try {
            jbInit();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private GribRenderPane rpane = null;

    private boolean modeEdit = true;

    Point2D.Double p;

    public JGribMarker(GribRenderPane parentPane, Point2D.Double p) {
        rpane = parentPane;
        this.setSize(140, 41);

        this.p = p;

        JGribMarker_mouseAdapter myAdapter = new JGribMarker_mouseAdapter(this);
        addMouseListener(myAdapter);
        addMouseMotionListener(myAdapter);
    }

    public void paint(Graphics2D g) {

        Point lp = rpane.getPosition(p);
        lp.x -=21;
        lp.y -=21;
        setLocation(lp);

        if (modeEdit) {
            paintEdit(g);
        } else {
            paintIdle(g);
        }
    }

    private void paintEdit(Graphics2D g) {

        g.setColor(new Color(255, 255, 255, 20));

        g.fillOval(getX(), getY(), 41,  41);
        g.setColor(new Color(255, 255, 255, 150));
        g.fillRect(getX()+21, getY(), getWidth()-21, getHeight());

        g.setColor(new Color(255, 255, 255, 255));

       g.fillRect(getX(), getY()+19, 40, 3);
       g.fillRect(getX()+19, getY(), 3, 40);

       g.setColor(new Color(255, 0, 0, 255));

       g.fillRect(getX(), getY()+20, 40, 1);
       g.fillRect(getX()+20, getY(), 1, 40);

       g.setColor(new Color(0, 0, 0, 250));

             g.drawString("lat: "+ ((int)(p.getX()*10))/10.0 +" lon: "+ ((int)(p.getY()*10))/10.0 ,getX()+25, getY()+15);
            double val = rpane.getValueAt(p);
        g.drawString("val: "+ ((int)(val*1000))/1000.0,getX()+25, getY()+35);
        if((p.getX() > 8.6)    && (p.getX() < 9.5))
          if((p.getY() > 41.4) && (p.getY() < 42.6)){
            g.setColor(new Color(255, 255, 255, 100));
            g.drawString("- Best place in the world -",getX()+25, getY()+25);
            g.setColor(new Color(0, 0, 0, 100));
            g.drawString("- Best place in the world -",getX()+24, getY()+24);
          }

   }


    private void paintIdle(Graphics2D g) {

        g.setColor(new Color(255, 255, 255, 80));



        g.fillRect(getX()+21, getY(), getWidth()-21, getHeight());

    g.setColor(new Color(255, 255, 255, 50));

   g.fillRect(getX(), getY()+19, 40, 3);
   g.fillRect(getX()+19, getY(), 3, 40);

   g.setColor(new Color(255, 0, 0, 50));

   g.fillRect(getX(), getY()+20, 40, 1);
   g.fillRect(getX()+20, getY(), 1, 40);

   g.setColor(new Color(0, 0, 0, 100));

      g.drawString("lat: "+ ((int)(p.getX()*10))/10.0 +" lon: "+ ((int)(p.getY()*10))/10.0 ,getX()+25, getY()+15);
      double val = rpane.getValueAt(p);
      g.drawString("val: "+ val,getX()+25, getY()+35);

    if((p.getX() > 8.6)    && (p.getX() < 9.5))
      if((p.getY() > 41.4) && (p.getY() < 42.6)){
        g.setColor(new Color(255, 255, 255, 100));
        g.drawString("- Best place in the world -",getX()+25, getY()+25);
        g.setColor(new Color(0, 0, 0, 100));
        g.drawString("- Best place in the world -",getX()+24, getY()+24);
      }
    }

    public void setEditable(boolean edit) {
        modeEdit = edit;
        setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
        repaint();
    }
    public void removeMe() {
      rpane.remove(this);

      rpane.repaint();
   }

    private void jbInit() throws Exception {
    }

    public void moving(){}
    public void drag(MouseEvent e){
       p.setLocation(rpane.getLonLat(e.getX()+getX(),e.getY()+getY()));
       rpane.repaint();
    }

}


class JGribMarker_mouseAdapter implements
        MouseListener, MouseMotionListener {
    private JGribMarker adaptee;
    JGribMarker_mouseAdapter(JGribMarker adaptee) {
        this.adaptee = adaptee;
    }

    public void mouseReleased(MouseEvent e) {

    }

    public void mouseMoved(MouseEvent e) {
    }

    public void mouseDragged(MouseEvent e) {
           adaptee.drag(e);
    }
    public void mousePressed(MouseEvent e) {

    }

    public void mouseExited(MouseEvent e) {
        adaptee.setEditable(false);

    }

    public void mouseEntered(MouseEvent e) {
        adaptee.setEditable(true);

    }

    public void mouseClicked(MouseEvent e) {
        if (2 == e.getClickCount()) {
            adaptee.removeMe();
        }

    }
}
