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
public class GribDump {
    public GribDump() {
    }

    public static void main(String[] args) {
        try {

          GribFile gf = new GribFile("/var/tmp/maf/testdata/latlon.grib2");
          GribHandle g =((GribHandle)gf.next());
          System.out.println(g.getLong("step"));
          g.reset();
          System.out.println(g.getLong("step"));

        } catch (Exception ex) {
             ex.printStackTrace();
        }
    }
}
