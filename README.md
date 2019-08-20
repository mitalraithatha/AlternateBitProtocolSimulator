# ALTERNATE BIT PROTOCOL

----
## **Organization :** 
- Carleton University
----
## **Authors :**

- Originally Developed By : [Cristina Ruiz Martin](https://github.com/cruizm)
- Modified by : [Mital Raithatha](https://github.com/mitalraithatha) & [Sreejith Unnithan](https://github.com/SREEJITH1991)
----
## **Table of contents**

- [Introduction](#Introduction)

- [Files and Folder Structure Organization](#File-and-Folder-Structure-Organization)

- [Build code and run the test](#Build-code-and-run-the-test)

  
-----

## **Introduction**

ABP (Alternating Bit Protocol) is a communication protocol to ensure reliable transmission through unreliable network. The sender sends a packet and waits for an acknowledgement. If the acknowledgement doesn't arrive within a predefined time, the sender re-sends this packet until it receives an expected acknowledgement and then sends the next packet. In order to distinguish two consecutive packets, the sender adds an additional bit on each packet (called alternating bit because the sender uses 0 and 1 alternatively). This software is a simulator to replicate the behavior of the Alternating Bit Protocol.

The ABP Simulator consists of 3 components: sender, network and receiver :

**Receiver :** The behavior of receiver is to receive the data and send back an acknowledgement extracted from the received data after a time period.

**Subnet :** The subnets just pass the packets after a time delay. However, in order to simulate the unreliability of the network, only 95% of the packets will be passed in each of the subnet, i.e. 5% of the data will be lost through the subnet

**Sender :** Every time when sender is sent a packet, it waits for an acknowledgement during an specific waiting time. It the acknowledgment does not arrive within the time window, the sender will re-send the previous packet with the alternating bit. If the expected acknowledgement is received within the time window, the sender will send the next packet

----
## **File and Folder Structure Organization**

### **1. data :**   This folder have input and output files for the ABP simulator.

#### 1. Two input files :
 - input_abp_0.txt
 - input_abp_1.txt

#### 2. Two output files :
 - abp_output_0.txt
 - abp_output_1.txt
 - abp_output.txt
 - formatted_abp_output.txt
 - average_output.txt

----------------------------------

### 2. doc : This folder have documentation files of the project.

- alternatebitprot.pdf
- Cadmium_Documentation_Ubuntu.pdf
- Cadmium_Windows.pdf
- Doxygen_docs_sender_main.zip
- Doxygen_docs_subnet_main.zip
- Doxygen_receiver_cadmium.hpp_&_receiver_main.cpp

------------------------------------------------

### 3. include : This folder have the all the header files used in the project. 

#### 1. atomics:

 -  format_convert.hpp
 -  receiver_cadmium.hpp
 - sender_cadmium.hpp
 - subnet_cadmium.hpp

#### 2. data_structures : [contains message header file] 
- message.hpp

-------------------------------------

### **4. lib :**  This folder contains the third party header files.

#### 1. cadmium-master[contains cadmium files as submodules]

#### 2. DesTimes-master[contains destimes files as submodules]

#### 3. vendor : [This folder contains 3rd party header files needed in the project]

- iestream.hpp

------------------

### 5. src : This folder contains the source code for the message and top model.


#### 1. data_structures :  [Contains the source code for the top message]
   - message.cpp

#### 2. top_ model [Contains the source code for the top_model] 
 - format_convert.cpp
 - main.cpp

-----------------------------------------------------------------

### 6. test : This folder contains the unit test for the different include files.

#### 1. data : [This folder contains test input output files]
1. receiver : [This folder contains the input output files for the unit test of the receiver]
     - formatted_receiver_output
     - receiver_ input_test
     - receiver_ test_output
2. sender : [ This folder contains the input output files foe the unit test of the sender]
     - formatted_sender_output
     - sender_ input_ test_ ack_In
     - sender_ input_ test_ control_ In
     - sender_ test_ output
3. subnet : [This folder contains the input output files foe the unit test of the subnet]
     - formatted_subnet_output
     - subnet_ input_test
     - subnet_ test_output

#### 2. src : [This folder contains the source code for the sender,receiver and subnet]
1. receiver : [This folder contains the source code for the receiver]
   - main.cpp
2. sender : [This folder contains the source code for the sender]
   - main.cpp
3. [This folder contains the source code for the subnet]
   - main.cpp

---------------------

## **Build code and run the test**

### **1. - Run the unit tests**

#### 1.1. Run subnet test	

1. Open the terminal. Press in your keyboard Ctrl+Alt+t

2. Set the command prompt in the test/src/subnet folder. To do so, type in the terminal the path to this folder.

   > Example: cd Documents/AlternateBitProtocol/test/src/subnet
   
3. To compile the test, type in the terminal:

   > make clean; make all					

4. To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE". For this specific test you need to type:		

     > ./SUBNET_TEST
5. To check the output of the test, open  **"test/data/subnet/subnet_test_output.txt"**


#### 1.2. To run receiver and sender tests, the steps are analogous to 1.1, just select the appropriate path.

-------------------------------

### 2. - Run the Simulator


1. Open the terminal. Press in your keyboard Ctrl+Alt+t

2. Set the command prompt in the src/top_model folder. To do so, type in the terminal the path to this folder.

   > Example: cd ../../top_model
3.  To compile the project, type in the terminal:
		
    
    > make clean; make all

4 - To run the simulation, type in the terminal "./NAME_OF_THE_COMPILED_FILE NAME_OF_THE_INPUT_FILE". For this test you need to type:
		

```c
./ABP.txt
```

5 - To check the output of the simulation, open  **"data/abp_output.txt"**

6 - To execute the simulator with different inputs

   - 6.1. Create new .txt files with the same structure as input_abp_0.txt or input_abp_1.txt
   - 6.2. Run the simulator using the instructions in step 4

   - 6.3. If you want to keep the output, rename abp_output.txt. To do so, type in the terminal: "mv abp_output.txt NEW_NAME"

     > Example: mv abp_output.txt abp_output_0.txt

### 3.- Installation steps for Cadmium

#### 3.1 Installing Cygwin and GCC 

##### 3.1.1 apt-cyg INSTALLATION  :

1. Open Cygwin in administrator mode and type the following commands 

   > a) “wget rawgit.com/transcode-open/apt-cyg/master/apt-cyg” 
   >
   > b) “install apt-cyg /bin” "apt-cyg" in Cygwin is similar to "sudo apt-get" command in Linux. (Similar to Linux you can use this command to install packages, update, list them etc.)

#### 3.2 INSTALLING CADMIUM DEPENDENCIES 

##### 3.2.2 Installing Boost Library 

1. Go to the location where you have downloaded the setup (.exe) file for Cygwin and open the Cygwin Terminal there. 

2. Type **“./setup-x86_64.exe -q -P libboost-devel” (OR) Type “apt-cyg install libboost-devel”** 

3. These steps install the boost library. 

4. Download Cadmium & DESTimes Select a folder to store these files and your new projects. 

5. Open Cygwin window inside this ‘folder’ location. 

   > 1. Type “git clone https://github.com/SimulationEverywhere/cadmium” 
   > 2. 2. Type “git clone https://github.com/Laouen/DESTimes.git” These steps download Cadmium and DESTimes.



### 3.- Download the Cadmium & DESTimes as submodule

1. Git clone the repository using the cygwin terminal or git bash.

   > git clone <repository link>

2. run the command :

   > git submodule init
   >
   > git submodule update
