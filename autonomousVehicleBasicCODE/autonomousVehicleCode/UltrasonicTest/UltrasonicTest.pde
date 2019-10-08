String port_num = "COM3";

import processing.serial.*;

PImage carImage;
Serial serial;
GuiProgress fcProgress;
GuiProgress flProgress;
GuiProgress frProgress;
GuiProgress lProgress;
GuiProgress rProgress;
float f_center;
float f_left;
float f_right;
float left;
float right;

float maxDistance = 800;

void setup()
{
  //size(640, 480);
  size(520, 350);
  
  carImage = loadImage("car.png");
  
  fcProgress = new GuiProgress(0, 0, carImage.height, carImage.width / 5, false, 0);
  flProgress = new GuiProgress(0, 0, carImage.height, carImage.width / 5, false, 0);
  frProgress = new GuiProgress(0, 0, carImage.height, carImage.width / 5, false, 0);
  lProgress = new GuiProgress(0, 0, carImage.height, carImage.width / 5, false, 0);
  rProgress = new GuiProgress(0, 0, carImage.height, carImage.width / 5, false, 0);
  
  printArray(Serial.list());
  serial = new Serial(this, port_num, 9600);
}

void draw()
{
  if(serial.available() > 0)
  {
    String packet = serial.readStringUntil('\n');
    if(packet != null)
    {
      packet = trim(packet);
      String[] tokens = splitTokens(packet, ":");
      if(tokens.length == 2)
      {
        if(tokens[0].equals("FC"))
        {
          f_center = Float.parseFloat(tokens[1]);
          fcProgress.value = f_center / maxDistance;
        }
        else if(tokens[0].equals("FL"))
        {
          f_left = Float.parseFloat(tokens[1]);
          flProgress.value = f_left / maxDistance;
        }
        else if(tokens[0].equals("FR"))
        {
          f_right = Float.parseFloat(tokens[1]);
          frProgress.value = f_right / maxDistance;
        }
        else if(tokens[0].equals("L"))
        {
          left = Float.parseFloat(tokens[1]);
          lProgress.value = left / maxDistance;
        }
        else if(tokens[0].equals("R"))
        {
          right = Float.parseFloat(tokens[1]);
          rProgress.value = right / maxDistance;
        }
      }
    }
  }
  
  background(255);
  
  pushMatrix();
  translate(150, 160);
    pushMatrix();
    scale(0.2); 
    image(carImage, 0, 0, carImage.width, carImage.height);
      pushMatrix();
      translate(carImage.width / 2 - 44, 0);
      rotate(PI * 1.5);
      fcProgress.draw();
      popMatrix();
      
      pushMatrix();
      translate(0, 50);
      rotate(PI * 1.25);
      flProgress.draw();
      popMatrix();
      
      pushMatrix();
      translate(carImage.width - 60, 0);
      rotate(PI * 1.75);
      frProgress.draw();
      popMatrix();      
      
      pushMatrix();
      translate(60, carImage.height / 2 + 90);
      rotate(PI * 1.0);
      lProgress.draw();
      popMatrix();
      
      pushMatrix();
      translate(carImage.width - 65, carImage.height / 2);
      rotate(PI * 0);
      rProgress.draw();
      popMatrix();
    popMatrix();
  popMatrix();
  
  fill(0);
  textSize(14);
  text("FC: " + nf(f_center, 3, 1) + " mm", 400, 20);
  text("FL: " + nf(f_left, 3, 1) + " mm", 400, 40);
  text("FR: " + nf(f_right, 3, 1) + " mm", 400, 60);
  text("L: " + nf(left, 3, 1) + " mm", 400, 120);
  text("R: " + nf(right, 3, 1) + " mm", 400, 140);
}