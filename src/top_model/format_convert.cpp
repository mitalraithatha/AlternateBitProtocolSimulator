/**
 * @file format_convert.cpp
 * @brief This file is to extract specific information from the output log
 *        generated from sender,subnet,receiver and the ABP simulator
 *
 * Specific information from the output log is extracted and is written
 * to file
 */
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

#define MODEL_TAG "generated by model "
/*!< macro that defines the MODEL TAG */
#define COLON "::"
/*!< macro that defines COLON*/

#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <iostream>
#include "../../include/atomics/format_convert.hpp"



const char* colon = COLON;
const char* model = MODEL_TAG;

/**
* get components from the original generated output file (unformatted file)
* in the same line
*/

void getcomponent(char* data, char* component) {
    int i,j;
    char *temp;
	temp = strstr(data,model);
	for(i=19,j=0 ; temp[i]!='\0' && temp[i]!='\n' ; i++,j++) {
		component[j] = temp[i];
	}
	component[j] = '\0';
}

/**
* fetch the port name from the file
*/
void getport(char* data, char* port) {

	char *temp;
	int i,j;
	temp = strstr(data,colon);

	if(temp != NULL) {
	    for(i=2,j=0;temp[i]!=':';i++,j++) {
			port[j]=temp[i];
		}
		port[j]='\0';
		strcpy(data, temp+2);
    }
}
/**
* get the value and check for '{}' braces from the file
*/
bool getvalue(char *data, char *value) {
	char * temp;
	int i,j;
	temp = strstr(data,"{");
	if(temp[1] == '}') {
		return false;
	}
	for(i=1,j=0;temp[i]!='}';i++,j++) {
		value[j] = temp[i];
	}
	value[j] = '\0';
	strcpy(data,temp);
	return true;
}
/**
 * Converter function calls getvalues(), getports() and getcomponent()
 * functions to create formatted file.
 * */
void converter(const char *file_input, const char *file_output){

    char *data,*port=(char*)"port", character,
           *value=(char*)"value", *component=(char*)"component";
	char* temp = NULL;
	char time[17], spaces[20];
	int temp_count=0, count = 0;
    FILE *p_input_file;
    /*!< pointer to the input file stream*/

	/**
	* open the file in read mode.
	*/
    p_input_file = fopen(file_input,"r");

	/**
	* check the if the file is present or not
	*/

	if(p_input_file == NULL) {
		printf("file not present\n");
		return;
	}
	/**
	* check the file characters and check the length of file characters in the same line
	*/
	while((character=fgetc(p_input_file)) != EOF) {
		if(character != '\n') {
			temp_count++;
		}
		else {
			if(count <= temp_count) {
				count = temp_count;
			}

			temp_count = 0;
		}
	}

	bzero(time,sizeof(time));
	strcpy(time,"Time");

	/**
	*open input file in write mode to print output in formated_abp_output.txt or formated_receiver_output.txt
	*or formated_sender_output.txt or formated_subnet_outpat.txt
	*file in current working directory
	*/

	FILE *p_output_file;
	/*!< pointer to the input file stream*/
	p_output_file = fopen(file_output,"w");
	/*
	 * printing the header of the file
	 */
	fprintf(p_output_file,"%s%17s%7s%25s\n",time,value,port,component);

	memset(spaces,' ', sizeof(spaces)-1);

	/**
	*allocate memory to pointers
	*/

	data = (char*)malloc(count+1);
	port = (char*)malloc(count+1);
	component = (char*)malloc(count+1);
	value = (char*)malloc(count+1);

	/**
	* initialize with null character
	*/

	bzero(data, count+1);
	bzero(port, count+1);
	bzero(component, count+1);
	bzero(value, count+1);

	rewind(p_input_file);

	while((fgets(data,count+1,p_input_file))!= NULL) {
		/**
		*add time
		*/
		if((data[0] != '[') && (data[0]>='0' && data[0] <='9')) {
			strncpy(time,data,strlen(data)-1);
			continue;
		}
		/**
		*check whether packet is present or not
		*/

		if(data[0] == '[' && data[1] !=	']') {
			getcomponent(data,component);
			/**
			iteration for multiple packets
			*/
			do {
				if(temp){
					strcpy(data,temp);
				}

				getport(data,port);
				if(getvalue(data,value)) {

					/**
					 *print value in file
					 */

					fprintf(p_output_file,"%s%s%s%s\n",strncat(time, spaces, 16-strlen(time)),
					strncat(value,spaces,8-strlen(value)), strncat(port,spaces,20 - strlen(port)), component);
				}
				else {
					bzero(port,count+1);
				}
			}

			while(temp=strstr(data,","));
		}
		bzero(data, count+1);
		bzero(port, count+1);
		bzero(component, count+1);
		bzero(value, count+1);
	}
	/**
	 * de-allocate memory to pointers
	 */
	fclose(p_input_file);
	fclose(p_output_file);
	free(data);
	free(port);
	free(component);
	free(value);
}
