package gribview;

import java.util.Enumeration;

import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.*;
import jgribapi.*;
import java.util.Vector;

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
public class JGribTreeNode extends DefaultMutableTreeNode {

    private boolean areChildrenDefined = false;
    private int outlineNum =2;
    private int numChildren =2;
    private int type;
    private String keyname ;
    private GribHandle g ;





     public JGribTreeNode(int type,String keyname ,GribHandle g) {
       this.g = g;
       this.type = type;
       this.keyname = keyname;

     }

     public boolean isLeaf() {

       if(this.type == JGribObjectProducer.GRIBSECTION) return false;
       return true;
     }

     public int getChildCount() {
       if (!areChildrenDefined)
         defineChildNodes();
       return(super.getChildCount());
     }

     private void defineChildNodes() {
       // You must set the flag before defining children if you
       // use "add" for the new children. Otherwise you get an infinite
       // recursive loop, since add results in a call to getChildCount.
       // However, you could use "insert" in such a case.
      areChildrenDefined = true;

      JGribViewEditionNodeProducer jo = new JGribViewEditionNodeProducer(g);
      Vector v = g.getObjects(jo,keyname,null);

      for(int i=1; i<v.size(); i++)
         add(( JGribTreeNode)v.elementAt(i));

     }

     public String toString() {
       TreeNode parent = getParent();
       String val = new String();

       if(type == JGribObjectProducer.GRIBSECTION)
          val = "Section";

      if(type == JGribObjectProducer.GRIBLONG)
         val = Long.toString(g.getLong(keyname));

      if(type == JGribObjectProducer.GRIBDOUBLE)
         val = Double.toString(g.getDouble(keyname));

      if(type == JGribObjectProducer.GRIBSTRING)
          val = g.getString(keyname);

      if(type == JGribObjectProducer.GRIBBYTES)
          val = "Bytes";

      if(type == JGribObjectProducer.GRIBDEFAULT)
          val = g.getAscii(keyname);

       return(keyname + " " +val );

  }

}
