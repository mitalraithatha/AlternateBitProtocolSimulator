#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <iostream>
#include "../../include/atomics/format_convert.hpp"

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

char* colon="::";
char* model = "generated by model ";

/**
* get components from the original generated output file (unformatted file) in the same line
*/

void getcomponent(char* data, char* component) {
    int i,j;
    char *tmp;
	tmp = strstr(data,model);
	for(i=19,j=0 ; tmp[i]!='\0' && tmp[i]!='\n' ; i++,j++) {
		component[j] = tmp[i];
	}
	component[j] = '\0';
}

/**
* fetch the port name from the file
*/
void getport(char* data, char* port) {

	char *tmp;
	int i,j;
	tmp = strstr(data,colon);

	if(tmp != NULL) {
		for(i=2,j=0;tmp[i]!=':';i++,j++) {
			port[j]=tmp[i];
		}
		port[j]='\0';
		strcpy(data, tmp+2);
    }
}
/**
* get the value and check for '{}' braces from the file
*/
bool getvalue(char *data, char *value) {
	char * tmp;
	int i,j;
	tmp = strstr(data,"{");
	if(tmp[1] == '}') {
		return false;
	}
	for(i=1,j=0;tmp[i]!='}';i++,j++) {
		value[j] = tmp[i];
	}
	value[j] = '\0';
	strcpy(data,tmp);
	return true;
}
/**
 * Converter function calls getvalues(), getports() and getcomponent() functions to create formatted file.
 * */
void converter(char *fin, char *fout){

	char *data,*port="port", ch, *value="value", *component="component";
	char* tmp = NULL;
	char time[17], spaces[20];
	int tmp_count=0, count = 0;
	//char *p_fin = fin;
	//char *p_fout = fout;
	FILE *fp;

	/**
	* open the file in read mode.
	*/
	fp = fopen(fin,"r");

	/**
	* check the if the file is present or not
	*/

	if(fp == NULL) {
		printf("file not present\n");
		return;
	}
	/**
	* check the file characters and check the length of file characters in the same line
	*/
	while((ch=fgetc(fp)) != EOF) {
		if(ch != '\n') {
			tmp_count++;
		}
		else {
			if(count <= tmp_count) {
				count = tmp_count;
			}

			tmp_count = 0;
		}
	}

	bzero(time,sizeof(time));
	strcpy(time,"Time");

	/**
	*open input file in write mode to print output in formated_abp_output.txt or formated_receiver_output.txt
	*or formated_sender_output.txt or formated_subnet_outpat.txt
	*file in current working directory
	*/

	FILE *fp1;
	fp1 = fopen(fout,"w");
	fprintf(fp1,"%s%17s%7s%25s\n",time,value,port,component);

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

	rewind(fp);

	while((fgets(data,count+1,fp))!= NULL) {
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
				if(tmp){
					strcpy(data,tmp);
				}

				getport(data,port);
				if(getvalue(data,value)) {

					/**
					 *print value in file
					 */

					fprintf(fp1,"%s%s%s%s\n",strncat(time, spaces, 16-strlen(time)),
					strncat(value,spaces,8-strlen(value)), strncat(port,spaces,20 - strlen(port)), component);
				}
				else {
					bzero(port,count+1);
				}
			}

			while(tmp=strstr(data,","));
		}
		bzero(data, count+1);
		bzero(port, count+1);
		bzero(component, count+1);
		bzero(value, count+1);
	}
	/**
	 * de-allocate memory to pointers
	 */
	fclose(fp);
	fclose(fp1);
	free(data);
	free(port);
	free(component);
	free(value);
}
