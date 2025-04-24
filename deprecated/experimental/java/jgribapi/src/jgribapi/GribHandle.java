package jgribapi;
import java.io.FileNotFoundException;
import java.awt.*;
import java.awt.image.*;
import java.util.*;
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
public class GribHandle {

    private long handleID;

    private  String fname;


    public double maxVal = Double.MIN_VALUE;
    public double minVal = Double.MAX_VALUE ;


    public GribHandle() {
        try {
            jbInit();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }


    static {
      System.loadLibrary("ecmgrib");
    }

    private native long     getIteratorID(long hid) ;

    private native String   gribGetString(long id, String key);

    private native long     gribGetLong  (long id, String key) ;

    private native double   gribGetDouble(long id, String key) ;

    private native long[]   gribGetLongArray  (long id, String key) ;

    private native double[] gribGetDoubleArray(long id, String key) ;

    private native int      gribSetString(long id, String key, String val) ;

    private native GribKey  gribCreateNext(long searchid);

    private native long     gribSetSearch(long id, String key) ;

    private native int      gribSetLong  (long id, String key, long val) ;

    private native int      gribSetDouble(long id, String key, double val) ;

    private native int      gribSetLongArray  (long id, String key, long[]   val) ;
    private native int      gribSetDoubleArray(long id, String key, double[] val) ;

    private native String   gribGetXml(long id) ;

    private native String   gribGetXml(long id, String key) ;

    private native String   gribGetAscii(long id, String key) ;

    private native byte[]   gribGetBytes(long id, String key) ;

    private native int      gribDeleteHandle(long handleID) ;
    private native void      gribResetHandle(long handleID) ;

    public void reset(){
        gribResetHandle(handleID);
    }
    public String getString(String key){
      return  gribGetString(handleID, key);
    }
    public long getLong(String key){
      return  gribGetLong(handleID, key);
    }
    public byte[] geByteArray(String key){
      return  gribGetBytes(handleID, key);
   }
   public String getAscii(String key){
     return  gribGetAscii(handleID, key);
    }
    public double getDouble(String key){
      return  gribGetDouble(handleID, key);
    }
    public long[] getLongArray(String key){
      return  gribGetLongArray(handleID, key);
    }
    public double[] getDoubleArray(String key){
      return  gribGetDoubleArray(handleID, key);
    }
    public long  gribGetIteratorID(){
      return getIteratorID(handleID);
    }

    public GribIterator iterator(){
      return null;
    }
    public String getGribXmlDocument(){
        return gribGetXml(handleID);
    }

    public String getGribXml(String key){
      return gribGetXml(handleID,key);
   }

    public GribHandle(long id){
        handleID = id;
    }

    public long getID(){
      return handleID ;
   }
    public Vector getObjects(JGribObjectProducer o,String skey, Object udata){
        Vector v = new Vector();

        long searchid = gribSetSearch(getID(),skey);
        if(searchid == 0) return v;
        GribKey key = gribCreateNext(searchid) ;

        while(key != null){
            v.add(o.gribFactory(udata,key.type,key.name));
            key = gribCreateNext(searchid);
        }
        return v ;
    }

    public int      setDoubleArray(String key, double[] val) {
         return gribSetDoubleArray(handleID, key, val);
    }

    public int      setLong(String key, long val) {
         return gribSetLong(handleID, key, val);
    }

    public int      setDouble(String key, double val) {
         return gribSetDouble(handleID, key, val);
     }

    public int      setString(String key, String val) {
          return gribSetString(handleID, key, val);
    }

    private void jbInit() throws Exception {
    }


}
