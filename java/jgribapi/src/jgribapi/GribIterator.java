package jgribapi;

import java.util.*;
import java.lang.ArrayStoreException;
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

public class GribIterator implements Iterator {
    GribHandle h = null;
    long iterID = 0;
    private double lats[] = null;
    private double lons[] = null;
    private double vals[] = null;

    static {
       System.loadLibrary("ecmgrib");
    }

   private native GribPoint gribIteratorGetNext(long iterID);
   private native long gribIteratorHasNext(long iterID);

    public GribIterator(GribHandle h) throws ArrayStoreException{
        this.h = h;

        iterID = h.gribGetIteratorID();
        if (this.iterID == -1) {
             throw new ArrayStoreException();
        }
    }

    public Object next() {
        return (GribPoint)gribIteratorGetNext(iterID);
    }

    public boolean hasNext() {
        return gribIteratorHasNext(iterID)>0;
    }

    public void remove() {
        return;
    }

}
