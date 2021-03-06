/*********************************************************
VIZIC TECHNOLOGIES. COPYRIGHT 2019.
THE DATASHEETS, SOFTWARE AND LIBRARIES ARE PROVIDED "AS IS." 
VIZIC EXPRESSLY DISCLAIM ANY WARRANTY OF ANY KIND, WHETHER 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
OR NONINFRINGEMENT. IN NO EVENT SHALL VIZIC BE LIABLE FOR 
ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF 
PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, 
ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO 
ANY DEFENCE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION,
OR OTHER SIMILAR COSTS.
*********************************************************/

/********************************************************
 IMPORTANT : This Example is created for the Arduino 1.0 Software IDE
********************************************************/

#include <SMARTGPU2.h>             //include the SMARTGPU2 library!

SMARTGPU2 lcd;                     //create our object called LCD

AXIS LCD_WIDTH, LCD_HEIGHT;        //Variables to handle the screen resolution

char songsOnSDCard[20]={0};        //array containing the names of the different called videos, up to 20 characters including .vid extension
unsigned char volume=100;
  
/**********************************/
void drawIntro(void){
  lcd.drawGradientRect(0,0,LCD_WIDTH-1,LCD_HEIGHT-1,MAGENTA,BLACK,VERTICAL);
  lcd.setTextSize(FONT5);
  lcd.string(80,30,LCD_WIDTH-1,LCD_HEIGHT-1,"Jukebox",0);
  lcd.string(40,100,LCD_WIDTH-1,LCD_HEIGHT-1,"SmartGPU 2",0);  
  lcd.setTextSize(FONT3);  
  lcd.string(23,180,LCD_WIDTH-1,LCD_HEIGHT-1,"Vizic Technologies 2019",0);
  delay(2000);
  lcd.drawGradientRect(0,0,LCD_WIDTH-1,LCD_HEIGHT-1,MAGENTA,BLACK,VERTICAL);  
  lcd.string(30,100,LCD_WIDTH-1,LCD_HEIGHT-1,"Searching for songs...",0);    
  delay(500);
}

/**********************************/
void drawControls(ACTIVE stateRR, char *symbol, ACTIVE stateFF){
  //draw button RR
  lcd.objButton(5,LCD_HEIGHT-1-50,((LCD_WIDTH/3)*1)-1,LCD_HEIGHT-1-5,stateRR,"<<<");
  //draw button Play/Pause     
  lcd.objButton(((LCD_WIDTH/3)*1)+1,LCD_HEIGHT-1-50,((LCD_WIDTH/3)*2)-1,LCD_HEIGHT-1-5,SELECTED,symbol);
  //draw button FF
  lcd.objButton(((LCD_WIDTH/3)*2)+1,LCD_HEIGHT-1-50,LCD_WIDTH-1-5,LCD_HEIGHT-1-5,stateFF,">>>");   
}

/**************************************************/
//looks in the current directory for .wav files and returns the name without .wav extension in the *fileName given array
void findNextAudioFile(char *fileName){
  static unsigned int songNumber=0;
  char *pch;
  while(1){
    if(lcd.SDFgetFileName(songNumber++,fileName) == F_INVALID_PARAMETER) songNumber=0; //if the function throws invalid parameter, then reset i
    pch=strstr(fileName,".wav"); //find the .wav extension in the name
    if(pch != 0){                //if .wav extension is present in the name
      strncpy(pch,0x00,1);       //cut/replace the .wav extension for the NULL 0x00 character   
      lcd.erase();      
      return;                    //exit
    }                            //if no .wav extension is found, then continue with the while(1) loop
    lcd.string(0,0,LCD_WIDTH-1,LCD_HEIGHT-1,"No Audio File Found",0);
  }
}


/**********************************/
/**********************************/
void setup() { //initial setup
  //Those two functions must always be called for SMARTGPU2 support
  lcd.init();  //configure the serial and pinout of arduino board for SMARTGPU2 support
  lcd.start(); //initialize the SMARTGPU2 processor
  
  //Obtain screen resolution
  lcd.getWidth(&LCD_WIDTH);
  lcd.getHeight(&LCD_HEIGHT);
}

/**********************************/
/**********************************/
/**********************************/
/**********************************/
void loop() { //main loop
  POINT point;
  unsigned int vid=0,i=0;
  STATE state;
  
  lcd.baudChange(BAUD5);                             //Set a fast baud!, always that we use touch functions is recommended to use fast baud rates
  //Audio Init
  lcd.initDACAudio(ENABLE);                          //Turn on the Audio DACs
  lcd.audioBoost(ENABLE);                            //ENABLE boost
  
  //Text config
  lcd.setTextSize(FONT2);
  //Open music folder
  lcd.SDFopenDir("Music");                           //Open the folder that contains the songs, if commented songs will be searched on root path

  //draw background/Intro
  drawIntro();

  //start playing
  while(1){ //Loop forever in the player!
    lcd.erase();
    findNextAudioFile(songsOnSDCard);                //obtain a xxx.wav file name without the.wav extension
    lcd.stopWAVFile();                               //stop current song if any
    lcd.playWAVFile(songsOnSDCard,0);                //play found .wav file
    lcd.string(0,0,LCD_WIDTH-1,LCD_HEIGHT-1,songsOnSDCard,0); //print file name
    drawControls(DESELECTED,"Pause",DESELECTED);     //draw Buttons
    
    while((lcd.getWAVPlayState(&state)==OK) && (state == ENABLE)){ //while song is still playing
      lcd.drawGradientRect(0,40,LCD_WIDTH-1,LCD_HEIGHT-1-70,random(0,65536),random(0,65536),HORIZONTAL); //draw animation
      delay(100);
      //try to receive a touch...
      if(lcd.touchScreen(&point)==VALID){              //if valid touch...
        if(point.y > (LCD_HEIGHT-1-50)){            //if touch on buttons
          if(point.x < ((LCD_WIDTH/3)*1)){             //touch on <<< button
            drawControls(SELECTED,"Pause",DESELECTED);   
            delay(200);                                //wait
            drawControls(DESELECTED,"Pause",DESELECTED);
            lcd.advanceWAVFile(0);                     //rewind to beginning
          }else if(point.x < ((LCD_WIDTH/3)*2)){       //touch on Play/Pause button
            lcd.getWAVPlayState(&state);
            if(state == ENABLE){                       //if playing
              lcd.pauseWAVFile();                      //pause playing
              drawControls(DESELECTED,"Play",DESELECTED);
              delay(300);              
              while(1){                                //loop until touch on play button is performed
                while(lcd.touchScreen(&point) == INVALID);
                if((point.y >= (LCD_HEIGHT-1-50)) && (point.x > ((LCD_WIDTH/3)*1)) && point.x < ((LCD_WIDTH/3)*2)){ //if touch on Play/Pause button
                  lcd.pauseWAVFile();                  //resume playing
                  drawControls(DESELECTED,"Pause",DESELECTED);
                  delay(300);                                
                  break;
                }
              }
            }else{ break;}
          }else{ //touch on >>> button
            drawControls(DESELECTED,"Pause",SELECTED);
            delay(200);                                //wait
            drawControls(DESELECTED,"Pause",DESELECTED);
            break;                                     //go to next track
          }
        }else{                                         //touch on any other part of the screen = volume up or volume down
          if(point.x < (LCD_WIDTH/2)){                 //touch on left side of the screen
            if(volume >= 10)  volume-=10;              //decrease volume only if it is bigger than 10
          }else{                                       //touch on right side of the screen
            if(volume < 100)  volume+=10;              //increase volume only if it is lower than 100
          } 
          lcd.setVolumeWAV(volume);                    //set new volume
          lcd.string(LCD_WIDTH-1-150,0,LCD_WIDTH-1,LCD_HEIGHT-1,"volume:",0); //print file name
          lcd.drawRectangle(LCD_WIDTH-1-50,0,LCD_WIDTH-1,40,BLACK,FILL);
          lcd.printNumber(LCD_WIDTH-1-50,0,volume);
        } 
      }
    }
  }
}
