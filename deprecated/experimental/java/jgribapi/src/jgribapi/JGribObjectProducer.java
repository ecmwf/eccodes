package jgribapi;

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

public interface JGribObjectProducer {
    public static int GRIBLONG        =1;
    public static int GRIBDOUBLE      =2;
    public static int GRIBSTRING      =3;
    public static int GRIBBYTES       =4;
    public static int GRIBSECTION     =5;
    public static int GRIBDEFAULT     =42;


    Object gribFactory(Object userData, int Type, String name);
}
