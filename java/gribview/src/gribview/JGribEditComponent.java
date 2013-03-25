package gribview;

import javax.swing.*;
import jgribapi.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.GridLayout;
import java.util.Iterator;
import java.util.Vector;
import java.awt.BorderLayout;
import java.awt.*;

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
public class JGribEditComponent extends JPanel {
    GribHandle g = null;
    int type;
    String key;
    JButton jLabelKey = new JButton();
    JTextField jTextValue = new JTextField();
    JButton jButton1 = new JButton();
    JButton jButton2 = new JButton();
    JLabel  defaultLabel = new JLabel();
    GridLayout gridLayout1 = new GridLayout(1,3);
    BorderLayout borderLayout1 = new BorderLayout();


    GribMainFrame obs;

    public JGribEditComponent() {
        try {
            jbInit();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public JGribEditComponent(GribMainFrame obs,GribHandle g,String key, int type) {
        jButton2.setBackground(UIManager.getColor(
                "ComboBox.selectionBackground"));
        jButton1.setBackground(UIManager.getColor("Desktop.background"));
        jLabelKey.setBackground(Color.orange);

        defaultLabel.setBackground(UIManager.getColor(
                "CheckBoxMenuItem.foreground"));

        this.g = g;
        this.type =type;
        this.key = key;
        this.obs = obs;

        jLabelKey.setPreferredSize(new Dimension(150,10));


        jButton1.setPreferredSize(new Dimension(100,10));
        jButton2.setPreferredSize(new Dimension(100,10));

        jLabelKey.setText(key);
        this.setLayout(borderLayout1);
        this.add(jLabelKey,BorderLayout.WEST);

        jButton2.setText("pack");
        jButton1.setText("+");
        jButton2.addActionListener(new
                                   JGribEditComponent_jButton2_actionAdapter(this));
        if(type == JGribObjectProducer.GRIBDEFAULT){
            defaultLabel.setText(g.getAscii(key));
           this.add(defaultLabel,BorderLayout.CENTER);
           this.add(new JLabel("-"),BorderLayout.EAST);
    }

        if(type == JGribObjectProducer.GRIBSECTION){
            defaultLabel.setText("Section");
            this.add(defaultLabel);

            jButton1.addActionListener(new
                                   JGribEditComponent_jButton1_actionAdapter(this));


           this.add(jButton1,BorderLayout.EAST);
        }
        if(type == JGribObjectProducer.GRIBLONG){
            jTextValue.setText(g.getString(key));
            jLabelKey.addActionListener(new
                                    JGribEditComponent_jLabelKey_actionAdapter(this));
            this.add(jTextValue,BorderLayout.CENTER);
            this.add(jButton2,BorderLayout.EAST);

        }
        if(type == JGribObjectProducer.GRIBDOUBLE){
            jTextValue.setText(g.getString(key));
            jLabelKey.addActionListener(new
                                    JGribEditComponent_jLabelKey_actionAdapter(this));
            this.add(jTextValue,BorderLayout.CENTER);
            this.add(jButton2,BorderLayout.EAST);

        }
        if(type == JGribObjectProducer.GRIBSTRING){
           jTextValue.setText(g.getString(key));
           jLabelKey.addActionListener(new
                                    JGribEditComponent_jLabelKey_actionAdapter(this));
           this.add(jTextValue,BorderLayout.CENTER);
           this.add(jButton2,BorderLayout.EAST);
        }

        if(type == JGribObjectProducer.GRIBBYTES){
            defaultLabel.setText("Bytes");
           this.add(defaultLabel,BorderLayout.CENTER);
           this.add(new JLabel(),BorderLayout.EAST);

        }


    }
    private void jbInit() throws Exception {
        jLabelKey.setBackground(UIManager.getColor("CheckBox.focus"));
        jLabelKey.setText("Key");
        jLabelKey.addActionListener(new
                                    JGribEditComponent_jLabelKey_actionAdapter(this));
        jTextValue.setText("value");
        jTextValue.setText("+");
        jButton1.addActionListener(new
                                   JGribEditComponent_jButton1_actionAdapter(this));
        this.setLayout(gridLayout1);
        jButton2.addActionListener(new
                                   JGribEditComponent_jButton2_actionAdapter(this));
        this.setLayout(gridLayout1);
        jButton2.setBackground(UIManager.getColor(
                "ComboBox.selectionBackground"));
        jButton1.setBackground(UIManager.getColor("Desktop.background"));
        defaultLabel.setBackground(UIManager.getColor(
                "CheckBoxMenuItem.foreground"));

        this.add(jLabelKey);
                this.add(defaultLabel);
        this.add(jTextValue);
        this.add(jButton2);
        this.add(jButton1);

    }


    public void jButton1_actionPerformed(ActionEvent e) {
         JPanel jInspectPane = new JPanel();
         JGribObjectProducer jo = new JGribViewDescriptorProducer(g,obs);
         Vector v = g.getObjects(jo,key,null);
         JFrame jw = new JFrame("Section " + key);
         jInspectPane.setLayout(new GridLayout(v.size()-1,4));
         for (int i = 1; i<  v.size(); i++ ) {
             jInspectPane.add((JGribEditComponent) v.elementAt(i));
         }

        JScrollPane jInspectScrollPane = new JScrollPane(jInspectPane);
        jw.getContentPane().add(jInspectScrollPane, java.awt.BorderLayout.CENTER);
        jw.setSize(jInspectScrollPane.getPreferredSize());
        jw.setVisible(true);
/*        obs.validate();
        obs.repaint();*/
    }

    public void jButton2_actionPerformed(ActionEvent e) {
        if(type == JGribObjectProducer.GRIBLONG){
                 long a  = Integer.parseInt(jTextValue.getText());
                 g.setLong(key,a);
        }
        if(type == JGribObjectProducer.GRIBDOUBLE)
                g.setDouble(key,Double.parseDouble(jTextValue.getText()));


        if(type == JGribObjectProducer.GRIBSTRING)
                g.setString(key,jTextValue.getText());

    }

    public void jLabelKey_actionPerformed(ActionEvent e) {
       obs.addAColumn(jLabelKey.getText());
    }
}


class JGribEditComponent_jLabelKey_actionAdapter implements ActionListener {
    private JGribEditComponent adaptee;
    JGribEditComponent_jLabelKey_actionAdapter(JGribEditComponent adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jLabelKey_actionPerformed(e);
    }
}


class JGribEditComponent_jButton2_actionAdapter implements ActionListener {
    private JGribEditComponent adaptee;
    JGribEditComponent_jButton2_actionAdapter(JGribEditComponent adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButton2_actionPerformed(e);
    }
}


class JGribEditComponent_jButton1_actionAdapter implements ActionListener {
    private JGribEditComponent adaptee;
    JGribEditComponent_jButton1_actionAdapter(JGribEditComponent adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jButton1_actionPerformed(e);
    }
}
