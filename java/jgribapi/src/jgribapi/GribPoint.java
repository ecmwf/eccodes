package jgribapi;


import java.awt.*;
import javax.swing.JComponent;

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
public class GribPoint {

    public double lat;
    public double lon;
    public double val;

    static double max = Double.MIN_VALUE;
    static double min = Double.MAX_VALUE;

    static double minlat = Double.MAX_VALUE;
    static double minlon = Double.MAX_VALUE;

    static double maxlon = Double.MIN_VALUE;
    static double maxlat = Double.MIN_VALUE;


    public GribPoint() {
        lat = 0;
        lon = 0;
        val = 0;
    }

    public GribPoint(double lat, double lon, double val) {
        this.lat = lat;
        this.lon = lon;
        this.val = val;
      //  this.setToolTipText(this.toString());

        if (val > max) {
            max = val;
        }
        if (val < min) {
            min = val;
        }

        if (lon > maxlon) {
            maxlon = lon;
        }
        if (lon < minlon) {
            minlon = lon;
        }

        if (lat > maxlat) {
            maxlat = lat;
        }
        if (lat < minlat) {
            minlat = lat;
        }

    }

    public String toString() {
        return "lat:" + lat + " lon:" + lon + " val:" + val;
    }

}
