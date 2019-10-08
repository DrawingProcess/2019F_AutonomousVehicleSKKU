class GuiBase
{
  float sWidth, sHeight;
  float xPos, yPos;
  float xClick, yClick;
  boolean focus = false;
  boolean over = false;
  boolean press = false;
  
  GuiBase(float x, float y, float w, float h)
  {
    xPos = x;
    yPos = y;
    sWidth = w;
    sHeight = h;
  }
  
  void draw()
  {
    if(focus)
      stroke(255, 0, 0);
    else
      stroke(0, 0, 0);
    
    if(over)
      fill(200, 200, 200);
    else
      fill(50, 50, 50);    
    
    rect(xPos, yPos, sWidth, sHeight);
    
    if(press)
    {
      fill(0, 255, 0, 50);
      rect(xPos + (mouseX - xClick), yPos + (mouseY - yClick), sWidth, sHeight);
    }
  }
  
  boolean keyPressed()
  {
    return focus;
  }
  
  boolean keyReleased()
  {
    return focus;
  }
  
  boolean mouseClicked()
  {
    return over;
  }
  
  boolean mousePressed()
  {
    if(over)
    {
      focus = true;
      press = true;
      xClick = mouseX;
      yClick = mouseY;
      return true;
    }
    else
    {
      focus = false;
      return false;
    }
  }
  
  boolean mouseReleased()
  {
    press = false;
    
    if(over)      
      return true;
    else
      return false;
  }
  
  void mouseMoved()
  {
    if (mouseX > xPos && mouseX < xPos + sWidth &&
       mouseY > yPos && mouseY < yPos + sHeight)
    {
      over = true;
    }
    else
    {
      over = false;
    }
  }
  
  boolean mouseDragged()
  {
    return press;
  }
}