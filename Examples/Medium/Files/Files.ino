/*********************************************************
VIZIC TECHNOLOGIES. COPYRIGHT 2014.
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

/********************************************************
 This simple sketch does the next:
 1.- try open the "test file.txt" file in the microSD root path in write only mode 
 2.- if doesn't exist, create the file - if exist overwritte the file
 3.- open again the "test file.txt" file in the microSD root path in write only mode 
 4.- write "Data Written by the SmartDRIVE Processor" to the .txt file
 5.- save contents and close file
 6.- open again the file in read only mode
 7.- read from file to a buffer
 8.- verify data read to be equal to message
 9.- if different erase the created file else keep the file
 10- umount drive
 11.- end
 
- remove microSD card and search for the file on a PC with the written contents
********************************************************/

#include <SMARTGPU2.h>      //include the SMARTGPU2 library!

SMARTGPU2 lcd;              //create our object called lcd

AXIS LCD_WIDTH, LCD_HEIGHT; //Variables to handle the screen resolution

FILERESULT res;             //create the variable that will store all SMARTGPU2 commands responses

char message[41]="Data Written by the SmartGPU 2 Processor";
unsigned int row=10;

//function that loops forever on error
void die(FILERESULT response){ //if the response is different than OK, print and loop forever
  if(response!=F_OK){
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Error... forever loop @",0);
    while(1);  
  }
}

/********************************************/
/********************************************/
void setup() { //initial setup
  //Those two functions must always be called for SMARTGPU2 support
  lcd.init();  //configure the serial and pinout of arduino board for SMARTGPU2 support
  lcd.start(); //initialize the SMARTGPU2 processor
  
  //Obtain screen resolution
  lcd.getWidth(&LCD_WIDTH);
  lcd.getHeight(&LCD_HEIGHT);
}

/********************************************/
/********************************************/
/********************************************/
/********************************************/
void loop() { //main loop 
    char buffer[50]={0};
    unsigned int writtenBytes=0, readbytes=0, i=0;        
    
    //strings config
    lcd.setTextColour(GREEN);  
    
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"FAT file open, read, write demo!",0);        row+=15;
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Open File <test file.txt> in WRITE mode...",0); row+=15;
    
    //try to open the file
    res=lcd.SDFopenFile("test file.txt", WRITEONLY, WORKSPACE0);  //Try to open a file called "testFile.txt" in write only mode in the workspace block 0    
    if(res!=F_OK){                       //If the file doesn't Open is because it doesn't exist      
      lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"File doesn't exist, creating file...",0); row+=15;    
      res=lcd.SDFnewFile("test file.txt");  //Try to create the file 
      die(res);                        //If any error loop forever
      res=lcd.SDFopenFile("test file.txt", WRITEONLY, WORKSPACE0);  //Try to open the created file      
      die(res);                        //If any error loop forever
    }
    
    //Up to here the file exist and is open    
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"File Successfully Open in WRITE mode...",0); row+=15;    
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Write <Data Written by the SmartGPU 2 Processor>",0); row+=15;    
    res=lcd.SDFwriteFile(message, sizeof(message), &writtenBytes, WORKSPACE0); //write to the open file in WORKSPACE0 size of message in bytes and store the successfully written Bytes on writtenBytes variable
    die(res);                         //If any error loop forever    
    lcd.SDFsaveFile(WORKSPACE0);          //Save changes in the file contained in WORKSPACE0
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Closing File...",0); row+=15;    
    lcd.SDFcloseFile(WORKSPACE0);         //Close the file --------------------
    
    //Now lets verify contents
    //open again the file in read only mode
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Open File <test file.txt> in READ mode...",0); row+=15;    
    res=lcd.SDFopenFile("test file.txt", READONLY, WORKSPACE0);  //Try to open again the file read only mode in the workspace block 0
    die(res);                         //If any error loop forever
    //read the file  
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"File Successfully Open in READ mode...",0); row+=15;    
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Read bytes from file to buffer...",0); row+=15;    
    res=lcd.SDFreadFile(buffer, sizeof(message), &readbytes, WORKSPACE0); //read size of message in bytes from the open file in WORKSPACE0 and store the successfully read Bytes on readbytes variable
    die(res);                         //If any error loop forever        
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Closing File...",0); row+=15;        
    lcd.SDFcloseFile(WORKSPACE0);         //Close the file --------------------
    
    //check contents
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Verify/Compare contents...",0); row+=15;            
    for(i=0;i<sizeof(message);i++){      
      if(message[i]!=buffer[i]){ //if contents are different
        lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Contents differ, erasing the created file...END",0); row+=10;                  
        lcd.SDFeraseDirFile("test file.txt"); //erase the File
        while(1);
      }
    }
    lcd.string(10,row,LCD_WIDTH-1,LCD_HEIGHT-1,"Contents are equal...END",0);                     
    while(1);
}
