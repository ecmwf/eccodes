package jgribapi;

import java.util.*;
import java.io.*;
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
public class GribFile implements Iterator{

    String fname = null;
    long fileID =0;

    static {
       System.loadLibrary("ecmgrib");
     }

    private native GribHandle gribGetNextHandle(long fileID);
    private native long       gribEOFReached(long fileID);
    private native long       gribGetGribFile(String fname);
    private native void       gribCloseFile(long fileID);
    private native long       gribNewFile(String fname);
    private native void       gribWriteHandle(long fileID, long handleID);

    public GribFile(String fname) throws FileNotFoundException  {
        this.fname = fname;
        fileID = gribGetGribFile(fname);


        if(fileID == -1){
            fileID = gribNewFile(fname);
            if(fileID == -1){
                throw new FileNotFoundException(fname);
            }
          }

    }

    public Object next() {
        return (GribHandle)gribGetNextHandle(fileID);
    }

    public boolean hasNext() {
        return gribEOFReached(fileID)>0;
    }

    public void writeHandle(GribHandle g) {
        gribWriteHandle(fileID, g.getID());
    }



    public void close() {
       gribCloseFile(fileID);
     }

    public void remove() {
        return;
    }

}
