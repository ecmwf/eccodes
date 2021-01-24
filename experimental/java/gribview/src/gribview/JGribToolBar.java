package gribview;

import javax.swing.JToolBar;
import javax.swing.JButton;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;
import java.awt.Component;

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
public class JGribToolBar extends JToolBar {
      boolean playing = false;

    GribMainFrame mainFrame = null;

    public JGribToolBar() {
        try {
            jbInit();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public JGribToolBar(GribMainFrame mainFrame ) {
        this.mainFrame = mainFrame;
    try {
        jbInit();
    } catch (Exception ex) {
        ex.printStackTrace();
    }
}

    private void jbInit() throws Exception {

        jButtonMarker.setText("Add Marker");
        jButtonMarker.addActionListener(new
                JGribToolBar_jButtonMarker_actionAdapter(this));
        jButtonPlay.setText("Play");
        jButtonPlay.addActionListener(new
                                      JGribToolBar_jButtonPlay_actionAdapter(this));
        jButtonFaster.setText("Faster");
        jButtonFaster.addActionListener(new
                JGribToolBar_jButtonFaster_actionAdapter(this));
        jButtonNext.setText("Next");
        jButtonNext.addActionListener(new
                                      JGribToolBar_jButtonNext_actionAdapter(this));
        jButtonPrevious.setText("Previous");
        jButtonPrevious.addActionListener(new
                JGribToolBar_jButtonPrevious_actionAdapter(this));
        jButtonSlower.setText("Slower");
        jButtonSlower.addActionListener(new
                JGribToolBar_jButtonSlower_actionAdapter(this));
    jButtonReset.setText("Reset");
        jButtonReset.addActionListener(new
                                       JGribToolBar_jButtonReset_actionAdapter(this));
        jButtonPackImage.setText("Pack Image");
        jButtonPackImage.addActionListener(new
                JGribToolBar_jButtonPackImage_actionAdapter(this));
        this.add(jButtonMarker);
        this.add(jButtonPackImage);
                this.add(jButtonReset);
        this.add(component1);
        this.add(jButtonPrevious);
        this.add(jButtonPlay);
        this.add(jButtonNext);
        this.add(jButtonSlower);
        this.add(jButtonFaster);
        setEnabled(false);
    }

    JButton jButtonMarker = new JButton();
    JButton jButtonPlay = new JButton();
        JButton jButtonReset = new JButton();
    JButton jButtonFaster = new JButton();
    JButton jButtonNext = new JButton();
    JButton jButtonPrevious = new JButton();
    JButton jButtonSlower = new JButton();
    Component component1 = Box.createVerticalStrut(2);
    JButton jButtonPackImage = new JButton();
    public void jButtonMarker_actionPerformed(ActionEvent e) {
         mainFrame.getGribRenderPane().setMode(GribRenderPane.ADD_MARKER);

    }

    public void jButtonPrevious_actionPerformed(ActionEvent e) {
        mainFrame.selectPrevious();
       // mainFrame.prout();
       /* double[] val = mainFrame.getGribRenderPane().handle.getDoubleArray("values");
        for (int i = 0; i < val.length; i++) {
            val[i] = 10;
        }
        mainFrame.getGribRenderPane().handle.setDoubleArray("values",val);
*/
    }

    public void play() {
        if(!playing){
               mainFrame.getAnimator().start();
               jButtonPlay.setText("Stop");
               playing = true;
            }
    }

    public void stop() {
        if(playing){
               mainFrame.getAnimator().stop();
               jButtonPlay.setText("Play");
               playing = false;
           }
    }

    public void jButtonPlay_actionPerformed(ActionEvent e) {
       if(playing){
           stop();
       }else{
           play();
       }

   }
   public void setEnabled(boolean v) {
     jButtonMarker.setEnabled(v);
     jButtonPackImage.setEnabled(v);
     jButtonPrevious.setEnabled(v);
     jButtonPlay.setEnabled(v);
     jButtonNext.setEnabled(v);
     jButtonSlower.setEnabled(v);
     jButtonFaster.setEnabled(v);
  }



    public void jButtonNext_actionPerformed(ActionEvent e) {
         mainFrame.selectNext() ;
    }

    public void jButtonSlower_actionPerformed(ActionEvent e) {
        mainFrame.getAnimator().slowDown();
    }

    public void jButtonFaster_actionPerformed(ActionEvent e) {
        mainFrame.getAnimator().accelerate();
    }

    public void jButtonPackImage_actionPerformed(ActionEvent e) {
        mainFrame.packImageInFile();
    }

    public void jButtonReset_actionPerformed(ActionEvent e) {
        mainFrame.resetSelected();
    }

}


class JGribToolBar_jButtonReset_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonReset_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButtonReset_actionPerformed(e);
    }
}


class JGribToolBar_jButtonPackImage_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonPackImage_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {

        adaptee.jButtonPackImage_actionPerformed(e);
    }
}


class JGribToolBar_jButtonFaster_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonFaster_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButtonFaster_actionPerformed(e);
    }
}


class JGribToolBar_jButtonSlower_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonSlower_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButtonSlower_actionPerformed(e);
    }
}


class JGribToolBar_jButtonPlay_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonPlay_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButtonPlay_actionPerformed(e);
    }
}


class JGribToolBar_jButtonNext_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonNext_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButtonNext_actionPerformed(e);
    }
}


class JGribToolBar_jButtonMarker_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonMarker_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButtonMarker_actionPerformed(e);
    }
}


class JGribToolBar_jButtonPrevious_actionAdapter implements ActionListener {
    private JGribToolBar adaptee;
    JGribToolBar_jButtonPrevious_actionAdapter(JGribToolBar adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButtonPrevious_actionPerformed(e);
    }
}
