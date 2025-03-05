package gribview;


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
public class GribAnimator implements Runnable{
    Thread t = null;
    GribMainFrame view = null;
    public long speed = 10;
    boolean playing = false;

    public GribAnimator(GribMainFrame rp) {
        this.view = rp;


      }

      public void accelerate(){
       if (speed > 1) speed --;
      }

      public void slowDown(){
        speed ++;
      }



      public void start(){
        playing = true;
        t = new Thread(this);
        t.start();
    }

    public void stop(){
        playing = false;
    }

    public void run(){
        while(playing){
        try {
          view.selectNext();
          t.sleep(speed*100);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
       }

    }



}
