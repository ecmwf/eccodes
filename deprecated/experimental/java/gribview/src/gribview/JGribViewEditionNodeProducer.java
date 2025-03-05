package gribview;

import jgribapi.JGribObjectProducer;
import javax.swing.JComponent;
import jgribapi.GribHandle;
import javax.swing.tree.*;
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

public class JGribViewEditionNodeProducer implements JGribObjectProducer {
    GribHandle h;
    public JGribViewEditionNodeProducer(GribHandle g) {
        this.h =g;

    }

    /**
     * gribFactory
     *
     * @param object Object
     * @param _int int
     * @param string String
     * @return Object
     * @todo Implement this jgribapi.JGribObjectProducer method
     */
    public Object gribFactory(Object object, int type, String name) {
       return new JGribTreeNode(type,name,h);
    }
}
