package gribview;

import jgribapi.JGribObjectProducer;
import jgribapi.*;
import javax.swing.*;
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
public class JGribViewDescriptorProducer implements JGribObjectProducer {
    GribHandle h;
    GribMainFrame obs;
    public JGribViewDescriptorProducer(GribHandle g, GribMainFrame obs) {
        this.h =g;
        this.obs = obs;
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
        return new JGribEditComponent(obs,h,name,type);
    }
}
