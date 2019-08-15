﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿# ﻿**ALTERNATE BIT PROTOCOL**

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

----
## **File and Folder Structure Organization**

### **1. data :**   This folder have input and output files for the ABP simulator.

#### 1. Two input files :
 - input_ abp_0.txt
 - input_ abp_1.txt

#### 2. Two output files :
 - abp_ output_0.txt
 - abp_ output_1.txt

----------------------------------

### 2. doc :** This folder have documentation files of the project.

- alternatebitprot.pdf

- Cadmium_Documentation_Ubuntu.pdf

- Cadmium_Windows.pdf

------------------------------------------------

### 3. include :** This folder have the all the header files used in the project. 

#### 1. atomics:
 -  receiver_cadmium.hpp
 - sender_cadmium.hpp
 - subnet_cadmium.hpp

#### 2. data_structures : [contains message header file] 
- message.hpp

-------------------------------------

### **4. lib :**  This folder contains the third party header files.

#### 1. vendor : [This folder contains 3rd party header files needed in the project]
 - iestream.hpp
 - NDTime.hpp

#### 2. cadmium-master[contains cadmium files]
#### 3. DesTimes-master[contains destimes files]

------------------

### 5. src :**  This folder contains the source code for the message and top model.


#### 1. data_structures :  [Contains the source code for the top message]
   - message.cpp

#### 2. top_ model [Contains the source code for the top_model] 
 - main.cpp

-----------------------------------------------------------------

### 6. test :**  This folder contains the unit test for the different include files.

#### 1. data : [This folder contains test input output files]
1. receiver : [This folder contains the input output files for the unit test of the receiver]
     - receiver_ input_test
     - receiver_ test_output
2. sender : [ This folder contains the input output files foe the unit test of the sender]
     - sender_ input_ test_ ack_In
     - sender_ input_ test_ control_ In
     - sender_ test_ output
3. subnet : [This folder contains the input output files foe the unit test of the subnet]
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

### 2. -Run the Simulator**


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
