class GuiProgress extends GuiBase
{
  float value;
  boolean isVertical;
  boolean disable = false;
  color backgroundColor = color(204);
  color fillColor = color(0);
  
  GuiProgress(float x, float y, float w, float h, boolean vertical, float initValue)
  {
    super(x, y, w, h);
    value = initValue;
    isVertical = vertical;
  }
  
  void draw()
  {
    noStroke();
    fill(backgroundColor);
    rect(xPos, yPos, sWidth, sHeight);
    
    fill(fillColor);
    if(isVertical)
      rect(xPos, yPos, sWidth, sHeight * value);
    else
      rect(xPos, yPos, sWidth * value, sHeight);
  }
}