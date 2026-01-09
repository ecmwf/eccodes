package gribview;

import jgribapi.*;
import java.util.*;
import javax.swing.event.*;

import javax.swing.*;
import javax.swing.table.*;

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
public class GribHandleSet extends Vector implements TableModel {


    private GribHandle current = null;
    handleComparator handleCompare = new handleComparator();
    private Vector selectionKeys = new Vector();
    JTable driver = null;

    public GribHandleSet() {
        super();

        try {
            jbInit();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        addSelection("ID");
        addSelection("step");
        addSelection("param");
        addSelection("date");
    }


    public void addHandle(GribHandle h) {
        this.add(h);
        if (current == null) {
            current = h;
        }

    }
    public void addSelection(String name){
       selectionKeys.add(name);
   }
    public GribHandle getHandleAt(int index) {
        if (index >= size()) return null;
        if (index < 0) return null;
        return (GribHandle) elementAt(index);
    }

    public void removeHandle(GribHandle h) {
        this.remove(h);
    }

    public void setDrivingTable(JTable driver) {
          this.driver = driver;
    }


    public GribHandle getCurrentHandle() {

        if(driver != null)
            current = getHandleAt(driver.getSelectedRow());

        return current;

    }

    public int addAKey(String s) {
        selectionKeys.add(s);
        return selectionKeys.indexOf(s);
    }
    private void jbInit() throws Exception {
    }

    public int getRowCount() {
        return this.size();
    }

    public int getColumnCount() {
        return selectionKeys.size();
    }

    public String getColumnName(int columnIndex) {
        return selectionKeys.elementAt(columnIndex).toString();
    }

    public Class getColumnClass(int columnIndex) {
        return String.class;
    };

    public boolean isCellEditable(int rowIndex,
                                  int columnIndex) {
        return false;
    }

    public Object getValueAt(int rowIndex,
                             int columnIndex) {

        GribHandle g = getHandleAt(rowIndex);
        if(columnIndex == 0) return Long.toString(g.getID());

        return g.getString(selectionKeys.elementAt(columnIndex).toString());
    }

    public void setValueAt(Object aValue,
                           int rowIndex,
                           int columnIndex) {
        return;
    }


    public void removeTableModelListener(TableModelListener l) {
        return;
    }

    public void addTableModelListener(TableModelListener l) {
        return;
    }

    public void sortByColumn(int sortCol, boolean isAscent) {
        String sortKey = selectionKeys.elementAt(sortCol).toString();
        if(sortCol == 0)
             handleCompare.setCompareKey("Comparing IDS");
        else
            handleCompare.setCompareKey(sortKey);
        handleCompare.setAscentCompare(isAscent);
        Collections.sort(this, handleCompare);
    }
}


class handleComparator implements Comparator {
    boolean ascent = true;
    private String compareKey = null;

    public void setCompareKey(String compareKey) {
        this.compareKey = compareKey;
    }

    public void setAscentCompare(boolean ascent) {
        this.ascent = ascent;
    }

    public int compare(Object o1, Object o2) {
        int rep ;
        if (compareKey == null) {
            return 0;
        }
        if (compareKey.compareTo("Comparing IDS") == 0) {
            rep = (int)(((GribHandle)o2).getID() - ((GribHandle)o1).getID());
        }
        else
        try {
                rep = Double.compare(Double.parseDouble((((GribHandle)o1).getString(compareKey))),Double.parseDouble((((GribHandle)o2).getString(compareKey))));
        } catch (Exception ex) {
               rep = ((GribHandle)o1).getString(compareKey).compareTo(((GribHandle)o2).getString(compareKey));
           }

        if (ascent) {
            return rep;
        } else {
            return -rep;
        }

    }
}
