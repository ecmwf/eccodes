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
public class GribKey {


  public String name = null;
  public int type = 0;

  public GribKey(int type, String name){
  this.name = name;
  this.type = type;
  }


}
