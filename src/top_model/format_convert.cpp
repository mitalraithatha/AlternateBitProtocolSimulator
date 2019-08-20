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


struct time
{
	int hr,min,sec,millisec;
};

const char* colon = COLON;
const char* model = MODEL_TAG;
int total_subnet1=0, total_subnet2=0; 
struct time subnet1_delay={0,0,0,0}, subnet2_delay={0,0,0,0};

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

struct time difference(struct time sender, struct time receiver) {
	struct time temp;
	temp.hr=temp.min=0;

	sender.sec = sender.sec + (sender.min*60) + (sender.hr*60*60);
	receiver.sec = receiver.sec + (receiver.min*60) + (receiver.hr*60*60);

	temp.sec = receiver.sec - sender.sec;

	if(receiver.millisec >= sender.millisec){
		temp.millisec = receiver.millisec - sender.millisec;
	}
	else if(receiver.millisec < sender.millisec) {
		temp.sec--;
		temp.millisec = receiver.millisec - sender.millisec+1000;
	}
	return temp;
}

void gettime(struct time *sender1, char temp_time[], char time[])
{
	strncpy(temp_time,time,2);
	sender1->hr=atoi(temp_time);

	strncpy(temp_time,time+3,2);
	sender1->min=atoi(temp_time);

	strncpy(temp_time,time+6,2);
	sender1->sec=atoi(temp_time);

	strncpy(temp_time,time+10,2);
	sender1->millisec=atoi(temp_time);
}


/**
 * Converter function calls getvalues(), getports() and getcomponent()
 * functions to create formatted file.
 * */
void converter(const char *file_input, const char *file_output , const char *ftask){

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

	FILE *p_output_file , *p_analytics_file = NULL;
	/*!< pointer to the input file stream*/
	p_output_file = fopen(file_output,"w");
	/*
	 * printing the header of the file
	 */
	 if(ftask){
	 	p_analytics_file = fopen(ftask,"w");
		}
	
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
	char time_sender1[13], temp_time[3];
	temp_time[2]='\0';

	bzero(time_sender1,0);

	int success=0;

	struct time sender1,subnet1,receiver1,subnet2;

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
				if(strcmp(port,"data_output")==0)
					{
						strncpy(time_sender1,time,12);

						gettime(&sender1,temp_time,time);
					}
					else if(strcmp(port,"ack_received_output")==0)
					{
						success++;
					}

					if(strcmp(component,"subnet1")==0)
					{
						gettime(&subnet1,temp_time,time);

						subnet1 = difference(sender1, subnet1);
						total_subnet1++;
						subnet1_delay.sec = subnet1_delay.sec + subnet1.sec;
						subnet1_delay.millisec = subnet1_delay.millisec + subnet1.millisec;

					}
					else if(strcmp(component,"receiver1")==0)
					{
						gettime(&receiver1,temp_time,time);

					
					}
					else if(strcmp(component,"subnet2")==0)
					{
						gettime(&subnet2,temp_time,time);

					

						subnet2 = difference(receiver1, subnet2);
						total_subnet2++;
						subnet2_delay.sec = subnet2_delay.sec + subnet2.sec;
						subnet2_delay.millisec = subnet2_delay.millisec + subnet2.millisec;
					}

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
if(p_analytics_file){
	if(total_subnet1)
	{
		fprintf(p_analytics_file,"Average delay introduced in subnet1 = %g secs and %g milliseconds\n",
				(float)(subnet1_delay.sec)/(float)(total_subnet1),
				(float)subnet1_delay.millisec/(float)(total_subnet1));
	}
	if(total_subnet2)
	{
		fprintf(p_analytics_file,"Average delay introduced in subnet2 = %g secs and %g milliseconds\n",
				(float)(subnet2_delay.sec)/(float)(total_subnet2),
				(float)subnet2_delay.millisec/(float)(total_subnet2));
	}

	fprintf(p_analytics_file,"Total number of packets successfully sent for the entire simulation = %d\n", success);
	fprintf(p_analytics_file,"Total delay introduced from sender to receiver for %d data packets = "
			"%g secs and %g milliseconds\n",success ,
			(float)(subnet1_delay.sec)+(float)(subnet2_delay.sec),
			(float)subnet1_delay.millisec+(float)subnet2_delay.millisec);
	}
	/**
	 * de-allocate memory to pointers
	 */
	fclose(p_input_file);
	fclose(p_output_file);
	if(p_analytics_file) {
	fclose(p_analytics_file);
	}
	free(data);
	free(port);
	free(component);
	free(value);
}
