
package gribview;

import jgribapi.*;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.*;
import java.awt.image.*;
import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;
import java.awt.event.*;

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
public class GribMainFrame extends JFrame {
    JPanel contentPane;
    BorderLayout borderLayout1 = new BorderLayout();
    JMenuBar jMenuBar1 = new JMenuBar();
    JMenu jMenuFile = new JMenu();
    JMenuItem jMenuFileExit = new JMenuItem();
    JLabel statusBar = new JLabel();
    JMenuItem jMenuOpen = new JMenuItem();
    GribAnimator animator =  new GribAnimator(this);
    GribHandleSet set = new GribHandleSet();
    String dpath = ".";
    JPanel jMainPane = new JPanel();

    String statusBarDefaultText = "Welcome to gribview 0.1 - ECMWF 2005";

    JPanel jHandleSetPane = new JPanel();

    BorderLayout borderLayout2 = new BorderLayout();

    JTabbedPane jHandleSelected = new JTabbedPane();

    JPanel jHeaderPane = new JPanel();
    JPanel jRequestPane = new JPanel();
    JPanel jInspectPane = new JPanel();
    JPanel jViewPane = new JPanel();
    JPanel jTopDataPane = new JPanel();
    JPanel jTreePane = new JPanel();
    SortButtonRenderer sortedRenderer = new SortButtonRenderer();

    JTable jHandeSelectionTable = new JTable(set);
    JTableHeader selectionTableheader = jHandeSelectionTable.getTableHeader();

    JScrollPane jSelectionScrollPane = new JScrollPane(jHandeSelectionTable);

    JScrollPane jHeaderScrollPane = new JScrollPane(jHeaderPane);
    JScrollPane jRequestScrollPane = new JScrollPane(jRequestPane);

    JScrollPane jInspectScrollPane = new JScrollPane(jInspectPane);
    JScrollPane jTreeScrollPane = new JScrollPane(jTreePane);
    JSplitPane jSplitSelectionPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT, false);

    BorderLayout borderLayout3 = new BorderLayout();
    BorderLayout borderLayout4 = new BorderLayout();
    BorderLayout borderLayout5 = new BorderLayout();
    BorderLayout borderLayout6 = new BorderLayout();
    BorderLayout borderLayout7 = new BorderLayout();

    JGribToolBar toolBar = new JGribToolBar(this);
    JTextArea jTextHeader = new JTextArea();
    JTextArea jTextRequest = new JTextArea();
    GribRenderPane gribRenderPane = new GribRenderPane(this);
    JScrollPane jRenderScrollPane = new JScrollPane(gribRenderPane);


    String requestHandlers[] = {"class",
                               "type",
                               "stream",

                               "expver",
                               "date",
                               "time",
                               "levelist",
                               "channel",

                               "diagnostic",
                               "iteration",
                               "step",

                               "method",
                               "system",
                               "number",
                               "frequency",
                               "direction",
                               "origin",
                               "refdate",
                               "reference",
                               "fcmonth",
                               "leadtime",
                               "opttime",
                               "ident",
                               "instrument",
                               "obstype",
                               "levtype",

                               "fcperiod",
                               "param",
                               "domain",
                               "quantile"

    };
    JMenu jMenuHelp = new JMenu();
    JMenuItem jMenuAbout = new JMenuItem();
    JMenuItem jMenuSaveAll = new JMenuItem();
    JMenuItem jMenuSaveSelect = new JMenuItem();
    JSlider jSliderZoom = new JSlider(JSlider.VERTICAL);

    public GribMainFrame() {
        try {
            setDefaultCloseOperation(EXIT_ON_CLOSE);
            jbInit();
        } catch (Exception exception) {
            exception.printStackTrace();
        }
    }

    public GribMainFrame(String[] args) {
        try {
            setDefaultCloseOperation(EXIT_ON_CLOSE);
            jbInit();
        } catch (Exception exception) {
            exception.printStackTrace();
        }

        for (int i = 0; i < args.length; i++) {
            openGribFile(args[i]);
        }

    }


    /**
     * Component initialization.
     *
     * @throws java.lang.Exception
     */
    private void jbInit() throws Exception {
        contentPane = (JPanel) getContentPane();
        contentPane.setLayout(borderLayout1);
        setSize(new Dimension(600, 600));
        setTitle("Grib View");
        statusBar.setFont(new java.awt.Font("Dialog", Font.PLAIN, 10));
        statusBar.setBorder(BorderFactory.createLoweredBevelBorder());
        statusBar.setDoubleBuffered(true);
        statusBar.setDisplayedMnemonic('0');
        statusBar.setText(" ");
        jMenuFile.setText("File");
        set.setDrivingTable(jHandeSelectionTable);
        jMenuOpen.setText("Open");
        jMenuOpen.addActionListener(new GribMainFrame_jMenuOpen_actionAdapter(this));
        jMenuOpen.addMouseListener(new GribMainFrame_jMenuOpen_mouseAdapter(this));
        jMenuFileExit.setText("Exit");
        jMenuFileExit.addActionListener(new
                                        GribMainFrame_jMenuFileExit_ActionAdapter(this));
        jMainPane.setLayout(borderLayout2);
        ImageIcon topIcon = new ImageIcon(GribMainFrame.class.getResource(
                "apilogo.gif"));
        this.setIconImage(topIcon.getImage());
        selectionTableheader.addMouseListener(new HeaderListener(
                selectionTableheader, sortedRenderer));

        jHandeSelectionTable.addMouseListener(new HandleTableMouseListener(this));
        jHandeSelectionTable.addKeyListener(new HandleTableKeyListener(this));

        jSelectionScrollPane.setMaximumSize(new Dimension(32767, 32767));
        jSelectionScrollPane.setPreferredSize(new Dimension(123, 123));
        jHeaderPane.setLayout(borderLayout3);
        jRequestPane.setLayout(borderLayout4);
        jViewPane.setLayout(borderLayout5);
        jTopDataPane.setLayout(borderLayout6);
        jMenuHelp.setText("Help");
        jMenuAbout.setText("About");
        jMenuAbout.addActionListener(new GribMainFrame_jMenuAbout_actionAdapter(this));
        jMenuSaveAll.setText("Save All");
        jMenuSaveAll.addActionListener(new
                                       GribMainFrame_jMenuSaveAll_actionAdapter(this));
        jMenuSaveSelect.setText("Save Selected");
        jMenuSaveSelect.addActionListener(new
                                          GribMainFrame_jMenuSaveSelect_actionAdapter(this));
        jSliderZoom.addMouseListener(new GribMainFrame_jSliderZoom_mouseAdapter(this));
        jSliderZoom.addMouseMotionListener(new
                GribMainFrame_jSliderZoom_mouseMotionAdapter(this));
        jMenuBar1.add(jMenuFile);
        jMenuBar1.add(jMenuHelp);
        jMenuFile.add(jMenuOpen);
        jMenuFile.add(jMenuSaveAll);
        jMenuFile.add(jMenuFileExit);
        jSliderZoom.setValue(0);
        //   jMenuFile.add(jMenuSaveSelect);

        jSplitSelectionPane.setOneTouchExpandable(true);
        setJMenuBar(jMenuBar1);
        statusBar.setText(statusBarDefaultText);

        jHandeSelectionTable.setShowHorizontalLines(false);

        jHeaderPane.add(jTextHeader, java.awt.BorderLayout.CENTER);
        jRequestPane.add(jTextRequest, java.awt.BorderLayout.CENTER);

        selectionTableheader.setDefaultRenderer(sortedRenderer);
        sortedRenderer.setSelectedColumn(1);

        contentPane.add(statusBar, java.awt.BorderLayout.SOUTH);

        contentPane.add(jMainPane, java.awt.BorderLayout.CENTER);

        jViewPane.add(jRenderScrollPane, java.awt.BorderLayout.CENTER);

        jHandleSelected.add(jHeaderScrollPane, "Header");
        jHandleSelected.add(jRequestScrollPane, "Request");
        jHandleSelected.add(jViewPane, "View");
        jHandleSelected.add(jInspectScrollPane, "Inspect");
       // jHandleSelected.add(jTreeScrollPane, "Tree");
        jViewPane.add(jSliderZoom, java.awt.BorderLayout.EAST);

        jTopDataPane.add(jHandleSelected, java.awt.BorderLayout.CENTER);
        jTopDataPane.add(toolBar, java.awt.BorderLayout.NORTH);



        jSplitSelectionPane.add(jSelectionScrollPane);
        jSplitSelectionPane.add(jTopDataPane);
        jSelectionScrollPane.setMinimumSize(new Dimension(150,100));
        jTopDataPane.setMinimumSize(new Dimension(150,150));

        jMainPane.add(jSplitSelectionPane, java.awt.BorderLayout.CENTER);
        jMenuHelp.add(jMenuAbout);

        dpath = System.getProperty("user.dir");

        jTextHeader.setFont(new Font("Monospaced",Font.PLAIN,14));
        jTextRequest.setFont(new Font("Monospaced",Font.PLAIN,14));


   }


    void updateSelection(){
         jHandeSelectionTable = new JTable(set);
         selectionTableheader = jHandeSelectionTable.getTableHeader();
         jSelectionScrollPane.removeAll();
         jSelectionScrollPane.add(jHandeSelectionTable);
    }

    /**
     * File | Exit action performed.
     *
     * @param actionEvent ActionEvent
     */
    void jMenuFileExit_actionPerformed(ActionEvent actionEvent) {
        System.exit(0);
    }

    public GribRenderPane getGribRenderPane() {
        return gribRenderPane;
    }
     public GribAnimator getAnimator(){

         return animator;
     }

    String makeRequest() {

        GribHandle g = set.getCurrentHandle();
        String rep = "GRIB";
        for (int i = 0; i < requestHandlers.length; i++) {
            String val = g.getString(requestHandlers[i]);
            if (val.length() > 0) {
                rep = rep + "," + "\n\t" + requestHandlers[i] + " = " + val;
            }
        }
        return rep;
    }

    public void jHandleTableListener_selectedChange() {
         int oldv =      jHeaderScrollPane.getVerticalScrollBar().getValue();
        jTextHeader.setText(set.getCurrentHandle().getAscii("GRIB"));
        jHeaderScrollPane.getVerticalScrollBar().setValue(0);

        jTextRequest.setText(makeRequest());
        gribRenderPane.setHandle(set.getCurrentHandle());
        gribRenderPane.repaint();
        jInspectPane.removeAll();
        JGribObjectProducer jo = new JGribViewDescriptorProducer(set.getCurrentHandle(),this);
        Vector v = set.getCurrentHandle().getObjects(jo,"GRIB",null);
        jInspectPane.setLayout(new GridLayout(v.size()-1,1));
        for (int i = 1; i<  v.size(); i++ ) {
            jInspectPane.add((JGribEditComponent) v.elementAt(i));
        }
        jInspectPane.validate();
        jInspectPane.repaint();
    }


      public void resetSelected(){
        set.getCurrentHandle().reset();
        gribRenderPane.setHandle(set.getCurrentHandle());
        jHandleTableListener_selectedChange() ;
      }

    public void selectNext() {
        if(jHandeSelectionTable.getRowCount() <1)return;
        int slctd = jHandeSelectionTable.getSelectedRow();
        toolBar.setEnabled(true);
        if (jHandeSelectionTable.getRowCount()-1 > slctd)
          slctd++;
         else
           slctd = 0;

        jHandeSelectionTable.changeSelection(slctd, -1, false, false);

        jHandleTableListener_selectedChange() ;
     }
     public void selectPrevious() {
                 if(jHandeSelectionTable.getRowCount() <1)return;
         int slctd = jHandeSelectionTable.getSelectedRow();

         if (jHandeSelectionTable.getRowCount() > 0)
           slctd--;
          else
            slctd = 0;

         jHandeSelectionTable.changeSelection(slctd, -1, false, false);

         jHandleTableListener_selectedChange() ;
     }

    public void jHandleTableListener_actionPerformed(MouseEvent e) {
        jHandleTableListener_selectedChange();
    }

    public void openGribFile(String fname) {
        GribFile gf = null;

        try {
            gf = new GribFile(fname);

        } catch (Exception ex) {
            ex.printStackTrace();
        }
        GribHandle g = null;
        if (gf != null) {
            int n = 0;

            while ((g = ((GribHandle) gf.next())) != null) {
                set.addHandle(g);

            }

            jHandeSelectionTable.setSize(100, 100);

            jHandeSelectionTable.repaint();
            selectNext() ;
        }
        jHandleSelected.setSelectedComponent(jViewPane);
       if(jHandeSelectionTable.getRowCount() >1)
           //      toolBar.play();
        this.setTitle( this.getTitle()+" : "+fname);
    }

    public void jMenuOpen_actionPerformed(ActionEvent e) {
        JFileChooser jf = new JFileChooser(dpath);

        int returnVal = jf.showOpenDialog(this);

        if (returnVal == jf.APPROVE_OPTION) {
            openGribFile(jf.getSelectedFile().getAbsolutePath());
        }

        dpath=jf.getSelectedFile().getPath();

    }

public void addAColumn(String key){
    int newset =set.addAKey(key);
    TableColumn column = new TableColumn(newset);
    jHandeSelectionTable.getColumnModel().addColumn(column);

    column.setHeaderValue(key);
    jHandeSelectionTable.repaint();
    selectionTableheader.repaint();
}

public void packImageInFile(){
/*
      JGribTreeNode root = new JGribTreeNode(JGribObjectProducer.GRIBSECTION,"GRIB",set.getCurrentHandle());
      jTreePane.removeAll();
      jTreePane.setLayout(new BorderLayout());
      jTreePane.add(new JTree(root),BorderLayout.CENTER) ;
return;
*/

    JFileChooser jf = new JFileChooser(dpath);
    int returnVal = jf.showOpenDialog(this);
    dpath=jf.getSelectedFile().getPath();
    if (returnVal == jf.APPROVE_OPTION) {

     GribHandle h = set.getCurrentHandle();
     ImageIcon im = new ImageIcon(jf.getSelectedFile().getAbsolutePath());

     int mni =  im.getIconWidth();
     int mnj =  im.getIconHeight();
     System.out.println("ni "+ mni +"nj "+ mnj );

     int vals[] = new int[mni*mnj];
     double dvals[] =  new double[mni*mnj];

     PixelGrabber pr = new PixelGrabber(im.getImage(),0,0,mni,mnj,vals,0,mni);

     try {
         pr.grabPixels();

     } catch (Exception ex) {
         ex.printStackTrace();
     }

     h.setLong("geography.ni",mni);
     h.setLong("geography.nj",mnj);

      for (int j = 0; j < dvals.length; j++) {
           double val = vals[j] & 0x000000FF;
           val += (vals[j] & 0x0000FF00)>>8;
           val += (vals[j] & 0x00FF0000)>>16;
            dvals[j] =  val;
       }


       h.setDoubleArray("values",dvals);


   }
 }
    public void jMenuOpen_mouseEntered(MouseEvent e) {
        statusBar.setText("Opens a gribFile");
    }

    public void jMenuOpen_mouseExited(MouseEvent e) {
        statusBar.setText(statusBarDefaultText);
    }


    public void jMenuAbout_actionPerformed(ActionEvent e) {
        GribViewAboutBox dlg = new GribViewAboutBox(this);
        Dimension dlgSize = dlg.getPreferredSize();
        Dimension frmSize = getSize();
        Point loc = getLocation();
        dlg.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x,
                        (frmSize.height - dlgSize.height) / 2 + loc.y);
        dlg.setModal(true);
        dlg.pack();
        dlg.show();
    }

    public void jMenuSaveSelect_actionPerformed(ActionEvent e) {

    }

    public void jMenuSaveAll_actionPerformed(ActionEvent e) {
        JFileChooser jf = new JFileChooser(dpath);




        jf.setDialogType(JFileChooser.SAVE_DIALOG);

        int returnVal = jf.showSaveDialog(this);

        if (returnVal == jf.APPROVE_OPTION) {
            try {
                dpath = jf.getSelectedFile().getPath();

                GribFile gf = new GribFile(jf.getSelectedFile().getAbsolutePath());
               for (int i = 0; i < set.size(); i++) {
                    gf.writeHandle(set.getHandleAt(i));
              }
               gf.close();

            } catch (Exception ex) {

            }

        }

    }

    public void jSliderZoom_mouseReleased(MouseEvent e) {

    }

    public void jSliderZoom_mouseMoved(MouseEvent e) {

    }

    public void jSliderZoom_mouseDragged(MouseEvent e) {
         gribRenderPane.zoom = jSliderZoom.getValue()+1;
         gribRenderPane.repaint();
    }
    public void setSlider(int val) {
        if (val >100)
            val = 100;
        if (val < 00)
            val = 0;
         jSliderZoom.setValue(val);
         jSliderZoom.repaint();
    }



}


class GribMainFrame_jSliderZoom_mouseAdapter extends MouseAdapter {
    private GribMainFrame adaptee;
    GribMainFrame_jSliderZoom_mouseAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void mouseReleased(MouseEvent e) {
        adaptee.jSliderZoom_mouseReleased(e);
    }
}


class GribMainFrame_jSliderZoom_mouseMotionAdapter extends MouseMotionAdapter {
    private GribMainFrame adaptee;
    GribMainFrame_jSliderZoom_mouseMotionAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void mouseMoved(MouseEvent e) {
        adaptee.jSliderZoom_mouseMoved(e);
    }

    public void mouseDragged(MouseEvent e) {
        adaptee.jSliderZoom_mouseDragged(e);
    }
}


class GribMainFrame_jMenuSaveAll_actionAdapter implements ActionListener {
    private GribMainFrame adaptee;
    GribMainFrame_jMenuSaveAll_actionAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jMenuSaveAll_actionPerformed(e);
    }
}


class GribMainFrame_jMenuSaveSelect_actionAdapter implements ActionListener {
    private GribMainFrame adaptee;
    GribMainFrame_jMenuSaveSelect_actionAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jMenuSaveSelect_actionPerformed(e);
    }
}


class GribMainFrame_jMenuAbout_actionAdapter implements ActionListener {
    private GribMainFrame adaptee;
    GribMainFrame_jMenuAbout_actionAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jMenuAbout_actionPerformed(e);
    }
}


class GribMainFrame_jMenuOpen_mouseAdapter extends MouseAdapter {
    private GribMainFrame adaptee;
    GribMainFrame_jMenuOpen_mouseAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void mouseEntered(MouseEvent e) {
        adaptee.jMenuOpen_mouseEntered(e);
    }

    public void mouseExited(MouseEvent e) {
        adaptee.jMenuOpen_mouseExited(e);
    }

}


class GribMainFrame_jMenuOpen_actionAdapter implements ActionListener {
    private GribMainFrame adaptee;
    GribMainFrame_jMenuOpen_actionAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
        adaptee.jMenuOpen_actionPerformed(e);
    }
}


class GribMainFrame_jMenuFileExit_ActionAdapter implements ActionListener {
    GribMainFrame adaptee;

    GribMainFrame_jMenuFileExit_ActionAdapter(GribMainFrame adaptee) {
        this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent actionEvent) {
        adaptee.jMenuFileExit_actionPerformed(actionEvent);
    }
}


class HandleTableKeyListener extends KeyAdapter {
    GribMainFrame adapter;

    HandleTableKeyListener(GribMainFrame adapter) {
        this.adapter = adapter;
    }

    public void keyReleased(KeyEvent e) {
        adapter.jHandleTableListener_selectedChange();
    }

}


class HandleTableMouseListener extends MouseAdapter {
    GribMainFrame adapter;

    HandleTableMouseListener(GribMainFrame adapter) {
        this.adapter = adapter;
    }

    public void mouseReleased(MouseEvent e) {
        adapter.jHandleTableListener_selectedChange();
    }

}


class HeaderListener extends MouseAdapter {
    JTableHeader header;
    SortButtonRenderer renderer;

    HeaderListener(JTableHeader header, SortButtonRenderer renderer) {
        this.header = header;
        this.renderer = renderer;
    }

    public void mousePressed(MouseEvent e) {
        int col = header.columnAtPoint(e.getPoint());
        int sortCol = header.getTable().convertColumnIndexToModel(col);
        renderer.setPressedColumn(col);
        renderer.setSelectedColumn(col);
        header.repaint();

        if (header.getTable().isEditing()) {
            header.getTable().getCellEditor().stopCellEditing();
        }

        boolean isAscent;
        if (SortButtonRenderer.DOWN == renderer.getState(col)) {
            isAscent = true;
        } else {
            isAscent = false;
        }
        ((GribHandleSet) header.getTable().getModel())
                .sortByColumn(sortCol, isAscent);
    }

    public void mouseReleased(MouseEvent e) {
        int col = header.columnAtPoint(e.getPoint());
        renderer.setPressedColumn( -1); // clear
        header.repaint();
    }
}
