/*****************************************************************************
 *
 * XPDDEFAULT.C - Default performance data routines
 *
 * Copyright (c) 2000-2006 Ethan Galstad (nagios@nagios.org)
 * Last Modified:   09-07-2006
 *
 * License:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *****************************************************************************/


/*********** COMMON HEADER FILES ***********/

#include "../include/config.h"
#include "../include/common.h"
#include "../include/objects.h"
#include "../include/nagios.h"


/**** DATA INPUT-SPECIFIC HEADER FILES ****/

#include "xpddefault.h"


int     xpddefault_perfdata_timeout;

char    *xpddefault_host_perfdata_command=NULL;
char    *xpddefault_service_perfdata_command=NULL;
command *xpddefault_host_perfdata_command_ptr=NULL;
command *xpddefault_service_perfdata_command_ptr=NULL;

char    *xpddefault_host_perfdata_file_template=NULL;
char    *xpddefault_service_perfdata_file_template=NULL;

char    *xpddefault_host_perfdata_file=NULL;
char    *xpddefault_service_perfdata_file=NULL;

int     xpddefault_host_perfdata_file_append=TRUE;
int     xpddefault_service_perfdata_file_append=TRUE;

unsigned long xpddefault_host_perfdata_file_processing_interval=0L;
unsigned long xpddefault_service_perfdata_file_processing_interval=0L;

char    *xpddefault_host_perfdata_file_processing_command=NULL;
char    *xpddefault_service_perfdata_file_processing_command=NULL;
command *xpddefault_host_perfdata_file_processing_command_ptr=NULL;
command *xpddefault_service_perfdata_file_processing_command_ptr=NULL;

FILE    *xpddefault_host_perfdata_fp=NULL;
FILE    *xpddefault_service_perfdata_fp=NULL;

extern char *macro_x[MACRO_X_COUNT];




/******************************************************************/
/***************** COMMON CONFIG INITIALIZATION  ******************/
/******************************************************************/

/* grabs configuration information from main config file */
int xpddefault_grab_config_info(char *config_file){
	char *input=NULL;
	char *temp_buffer=NULL;
	mmapfile *thefile=NULL;

	/* open the config file for reading */
	if((thefile=mmap_fopen(config_file))==NULL){
		asprintf(&temp_buffer,"Error: Could not open main config file '%s' for reading performance variables!\n",config_file);
		write_to_logs_and_console(temp_buffer,NSLOG_CONFIG_ERROR,TRUE);
		my_free((void **)&temp_buffer);
		return ERROR;
	        }

	/* read in all lines from the config file */
	while(1){

		/* free memory */
		my_free((void **)&input);

		/* read the next line */
		if((input=mmap_fgets(thefile))==NULL)
			break;

		/* skip blank lines and comments */
		if(input[0]=='#' || input[0]=='\x0')
			continue;

		strip(input);

		xpddefault_grab_config_directives(input);
	        }

	/* free memory and close the file */
	my_free((void **)&input);
	mmap_fclose(thefile);

	return OK;
        }


/* processes a single directive */
int xpddefault_grab_config_directives(char *input){
	char *temp_ptr=NULL;
	char *varname=NULL;
	char *varvalue=NULL;

	/* get the variable name */
	if((temp_ptr=my_strtok(input,"="))==NULL)
		return ERROR;
	if((varname=(char *)strdup(temp_ptr))==NULL)
		return ERROR;

	/* get the variable value */
	if((temp_ptr=my_strtok(NULL,"\n"))==NULL){
		my_free((void **)&varname);
		return ERROR;
	        }
	if((varvalue=(char *)strdup(temp_ptr))==NULL){
		my_free((void **)&varname);
		return ERROR;
	        }

	if(!strcmp(varname,"perfdata_timeout")){
		strip(varvalue);
		xpddefault_perfdata_timeout=atoi(varvalue);
	        }

	else if(!strcmp(varname,"host_perfdata_command"))
		xpddefault_host_perfdata_command=(char *)strdup(varvalue);

	else if(!strcmp(varname,"service_perfdata_command"))
		xpddefault_service_perfdata_command=(char *)strdup(varvalue);

	else if(!strcmp(varname,"host_perfdata_file_template"))
		xpddefault_host_perfdata_file_template=(char *)strdup(varvalue);

	else if(!strcmp(varname,"service_perfdata_file_template"))
		xpddefault_service_perfdata_file_template=(char *)strdup(varvalue);

	else if(!strcmp(varname,"host_perfdata_file"))
		xpddefault_host_perfdata_file=(char *)strdup(varvalue);

	else if(!strcmp(varname,"service_perfdata_file"))
		xpddefault_service_perfdata_file=(char *)strdup(varvalue);

	else if(!strcmp(varname,"host_perfdata_file_mode")){
		if(!strstr(varvalue,"w"))
			xpddefault_host_perfdata_file_append=FALSE;
		else
			xpddefault_host_perfdata_file_append=TRUE;
	        }

	else if(!strcmp(varname,"service_perfdata_file_mode")){
		if(!strstr(varvalue,"w"))
			xpddefault_service_perfdata_file_append=FALSE;
		else
			xpddefault_service_perfdata_file_append=TRUE;
	        }

	else if(!strcmp(varname,"host_perfdata_file_processing_interval"))
		xpddefault_host_perfdata_file_processing_interval=strtoul(varvalue,NULL,0);

	else if(!strcmp(varname,"service_perfdata_file_processing_interval"))
		xpddefault_service_perfdata_file_processing_interval=strtoul(varvalue,NULL,0);

	else if(!strcmp(varname,"host_perfdata_file_processing_command"))
		xpddefault_host_perfdata_file_processing_command=(char *)strdup(varvalue);

	else if(!strcmp(varname,"service_perfdata_file_processing_command"))
		xpddefault_service_perfdata_file_processing_command=(char *)strdup(varvalue);

	/* free memory */
	my_free((void **)&varname);
	my_free((void **)&varvalue);

	return OK;
        }



/******************************************************************/
/************** INITIALIZATION & CLEANUP FUNCTIONS ****************/
/******************************************************************/

/* initializes performance data */
int xpddefault_initialize_performance_data(char *config_file){
	char *buffer=NULL;
	char *temp_buffer=NULL;
	char *temp_command_name=NULL;
	command *temp_command=NULL;
	time_t current_time;

	time(&current_time);

	/* reset vars */
	xpddefault_host_perfdata_command_ptr=NULL;
	xpddefault_service_perfdata_command_ptr=NULL;
	xpddefault_host_perfdata_file_processing_command_ptr=NULL;
	xpddefault_service_perfdata_file_processing_command_ptr=NULL;

	/* grab config info from main config file */
	xpddefault_grab_config_info(config_file);

	/* make sure we have some templates defined */
	if(xpddefault_host_perfdata_file_template==NULL)
		xpddefault_host_perfdata_file_template=(char *)strdup(DEFAULT_HOST_PERFDATA_FILE_TEMPLATE);
	if(xpddefault_service_perfdata_file_template==NULL)
		xpddefault_service_perfdata_file_template=(char *)strdup(DEFAULT_SERVICE_PERFDATA_FILE_TEMPLATE);

	/* process special chars in templates */
	xpddefault_preprocess_file_templates(xpddefault_host_perfdata_file_template);
	xpddefault_preprocess_file_templates(xpddefault_service_perfdata_file_template);

	/* open the performance data files */
	xpddefault_open_host_perfdata_file();
	xpddefault_open_service_perfdata_file();

	/* verify that performance data commands are valid */
	if(xpddefault_host_perfdata_command!=NULL){

		temp_buffer=(char *)strdup(xpddefault_host_perfdata_command);

		/* get the command name, leave any arguments behind */
		temp_command_name=my_strtok(temp_buffer,"!");

		if((temp_command=find_command(temp_command_name))==NULL){
			asprintf(&buffer,"Warning: Host performance command '%s' was not found - host performance data will not be processed!\n",temp_command_name);
			write_to_logs_and_console(buffer,NSLOG_RUNTIME_WARNING,TRUE);
			my_free((void **)&buffer);
			my_free((void **)&xpddefault_host_perfdata_command);
		        }

		/* save the command pointer for later */
		xpddefault_host_perfdata_command_ptr=temp_command;
	        }
	if(xpddefault_service_perfdata_command!=NULL){

		temp_buffer=(char *)strdup(xpddefault_service_perfdata_command);

		/* get the command name, leave any arguments behind */
		temp_command_name=my_strtok(temp_buffer,"!");

		if((temp_command=find_command(temp_command_name))==NULL){
			asprintf(&buffer,"Warning: Service performance command '%s' was not found - service performance data will not be processed!\n",temp_command_name);
			write_to_logs_and_console(buffer,NSLOG_RUNTIME_WARNING,TRUE);
			my_free((void **)&buffer);
			my_free((void **)&xpddefault_service_perfdata_command);
		        }

		/* save the command pointer for later */
		xpddefault_service_perfdata_command_ptr=temp_command;
	        }
	if(xpddefault_host_perfdata_file_processing_command!=NULL){

		temp_buffer=(char *)strdup(xpddefault_host_perfdata_file_processing_command);

		/* get the command name, leave any arguments behind */
		temp_command_name=my_strtok(temp_buffer,"!");

		if((temp_command=find_command(temp_command_name))==NULL){
			asprintf(&buffer,"Warning: Host performance file processing command '%s' was not found - host performance data file will not be processed!\n",temp_command_name);
			write_to_logs_and_console(buffer,NSLOG_RUNTIME_WARNING,TRUE);
			my_free((void **)&buffer);
			my_free((void **)&xpddefault_host_perfdata_file_processing_command);
		        }

		/* save the command pointer for later */
		xpddefault_host_perfdata_file_processing_command_ptr=temp_command;
	        }
	if(xpddefault_service_perfdata_file_processing_command!=NULL){

		temp_buffer=(char *)strdup(xpddefault_service_perfdata_file_processing_command);

		/* get the command name, leave any arguments behind */
		temp_command_name=my_strtok(temp_buffer,"!");

		if((temp_command=find_command(temp_command_name))==NULL){
			asprintf(&buffer,"Warning: Service performance file processing command '%s' was not found - service performance data file will not be processed!\n",temp_command_name);
			write_to_logs_and_console(buffer,NSLOG_RUNTIME_WARNING,TRUE);
			my_free((void **)&buffer);
			my_free((void **)&xpddefault_service_perfdata_file_processing_command);
		        }

		/* save the command pointer for later */
		xpddefault_host_perfdata_file_processing_command_ptr=temp_command;
	        }

	/* periodically process the host perfdata file */
	if(xpddefault_host_perfdata_file_processing_interval>0 && xpddefault_host_perfdata_file_processing_command!=NULL)
		schedule_new_event(EVENT_USER_FUNCTION,TRUE,current_time+xpddefault_host_perfdata_file_processing_interval,TRUE,xpddefault_host_perfdata_file_processing_interval,NULL,TRUE,xpddefault_process_host_perfdata_file,NULL);

	/* periodically process the service perfdata file */
	if(xpddefault_service_perfdata_file_processing_interval>0 && xpddefault_service_perfdata_file_processing_command!=NULL)
		schedule_new_event(EVENT_USER_FUNCTION,TRUE,current_time+xpddefault_service_perfdata_file_processing_interval,TRUE,xpddefault_service_perfdata_file_processing_interval,NULL,TRUE,xpddefault_process_service_perfdata_file,NULL);

	/* save the host perf data file macro */
	my_free((void **)&macro_x[MACRO_HOSTPERFDATAFILE]);
	if(xpddefault_host_perfdata_file!=NULL){
		if((macro_x[MACRO_HOSTPERFDATAFILE]=(char *)strdup(xpddefault_host_perfdata_file)))
			strip(macro_x[MACRO_HOSTPERFDATAFILE]);
	        }

	/* save the service perf data file macro */
	my_free((void **)&macro_x[MACRO_SERVICEPERFDATAFILE]);
	if(xpddefault_service_perfdata_file!=NULL){
		if((macro_x[MACRO_SERVICEPERFDATAFILE]=(char *)strdup(xpddefault_service_perfdata_file)))
			strip(macro_x[MACRO_SERVICEPERFDATAFILE]);
	        }

	/* free memory */
	my_free((void **)&temp_buffer);
	my_free((void **)&buffer);

	return OK;
        }



/* cleans up performance data */
int xpddefault_cleanup_performance_data(char *config_file){

	/* free memory */
	my_free((void **)&xpddefault_host_perfdata_command);
	my_free((void **)&xpddefault_service_perfdata_command);
	my_free((void **)&xpddefault_host_perfdata_file_template);
	my_free((void **)&xpddefault_service_perfdata_file_template);
	my_free((void **)&xpddefault_host_perfdata_file);
	my_free((void **)&xpddefault_service_perfdata_file);
	my_free((void **)&xpddefault_host_perfdata_file_processing_command);
	my_free((void **)&xpddefault_service_perfdata_file_processing_command);

	/* close the files */
	xpddefault_close_host_perfdata_file();
	xpddefault_close_service_perfdata_file();

	return OK;
        }



/******************************************************************/
/****************** PERFORMANCE DATA FUNCTIONS ********************/
/******************************************************************/


/* updates service performance data */
int xpddefault_update_service_performance_data(service *svc){

	/* run the performance data command */
	xpddefault_run_service_performance_data_command(svc);

	/* update the performance data file */
	xpddefault_update_service_performance_data_file(svc);

	return OK;
        }


/* updates host performance data */
int xpddefault_update_host_performance_data(host *hst){

	/* run the performance data command */
	xpddefault_run_host_performance_data_command(hst);

	/* update the performance data file */
	xpddefault_update_host_performance_data_file(hst);

	return OK;
        }




/******************************************************************/
/************** PERFORMANCE DATA COMMAND FUNCTIONS ****************/
/******************************************************************/


/* runs the service performance data command */
int xpddefault_run_service_performance_data_command(service *svc){
	char raw_command_line[MAX_INPUT_BUFFER]="";
	char processed_command_line[MAX_INPUT_BUFFER]="";
	char *temp_buffer=NULL;
	host *temp_host;
	int early_timeout=FALSE;
	double exectime;
	int result=OK;
	int macro_options=STRIP_ILLEGAL_MACRO_CHARS|ESCAPE_MACRO_CHARS;

	/* we don't have a command */
	if(xpddefault_service_perfdata_command==NULL)
		return OK;

	/* find the associated host */
	temp_host=find_host(svc->host_name);

	/* update service macros */
	clear_volatile_macros();
	grab_host_macros(temp_host);
	grab_service_macros(svc);
	grab_datetime_macros();
	grab_summary_macros(NULL);

	/* get the raw command line */
	get_raw_command_line(xpddefault_service_perfdata_command_ptr,xpddefault_service_perfdata_command,raw_command_line,sizeof(raw_command_line),macro_options);

#ifdef DEBUG3
	printf("\tRaw service performance data command line: %s\n",raw_command_line);
#endif

	/* process any macros in the raw command line */
	process_macros(raw_command_line,processed_command_line,(int)sizeof(processed_command_line),macro_options);

#ifdef DEBUG3
	printf("\tProcessed service performance data command line: %s\n",processed_command_line);
#endif

	/* set environment variables */
	set_all_macro_environment_vars(TRUE);

	/* run the command */
	my_system(processed_command_line,xpddefault_perfdata_timeout,&early_timeout,&exectime,NULL,0);

	/* unset environment variables */
	set_all_macro_environment_vars(FALSE);

	/* check to see if the command timed out */
	if(early_timeout==TRUE){
		asprintf(&temp_buffer,"Warning: Service performance data command '%s' for service '%s' on host '%s' timed out after %d seconds\n",processed_command_line,svc->description,svc->host_name,xpddefault_perfdata_timeout);
		write_to_logs_and_console(temp_buffer,NSLOG_RUNTIME_WARNING,TRUE);
		my_free((void **)&temp_buffer);
	        }

	return result;
        }


/* runs the host performance data command */
int xpddefault_run_host_performance_data_command(host *hst){
	char raw_command_line[MAX_INPUT_BUFFER]="";
	char processed_command_line[MAX_INPUT_BUFFER]="";
	char *temp_buffer=NULL;
	int early_timeout=FALSE;
	double exectime;
	int result=OK;
	int macro_options=STRIP_ILLEGAL_MACRO_CHARS|ESCAPE_MACRO_CHARS;

	/* we don't have a command */
	if(xpddefault_host_perfdata_command==NULL)
		return OK;

	/* update host macros */
	clear_volatile_macros();
	grab_host_macros(hst);
	grab_datetime_macros();
	grab_summary_macros(NULL);

	/* get the raw command line */
	get_raw_command_line(xpddefault_host_perfdata_command_ptr,xpddefault_host_perfdata_command,raw_command_line,sizeof(raw_command_line),macro_options);

#ifdef DEBUG3
	printf("\tRaw host performance data command line: %s\n",raw_command_line);
#endif

	/* process any macros in the raw command line */
	process_macros(raw_command_line,processed_command_line,(int)sizeof(processed_command_line),macro_options);

#ifdef DEBUG3
	printf("\tProcessed host performance data command line: %s\n",processed_command_line);
#endif

	/* set environment variables */
	set_all_macro_environment_vars(TRUE);

	/* run the command */
	my_system(processed_command_line,xpddefault_perfdata_timeout,&early_timeout,&exectime,NULL,0);

	/* unset environment variables */
	set_all_macro_environment_vars(FALSE);

	/* check to see if the command timed out */
	if(early_timeout==TRUE){
		asprintf(&temp_buffer,"Warning: Host performance data command '%s' for host '%s' timed out after %d seconds\n",processed_command_line,hst->name,xpddefault_perfdata_timeout);
		write_to_logs_and_console(temp_buffer,NSLOG_RUNTIME_WARNING,TRUE);
		my_free((void **)&temp_buffer);
	        }

	return result;
        }



/******************************************************************/
/**************** FILE PERFORMANCE DATA FUNCTIONS *****************/
/******************************************************************/

/* open the host performance data file for writing */
int xpddefault_open_host_perfdata_file(void){
	char *buffer=NULL;

	if(xpddefault_host_perfdata_file!=NULL){

		xpddefault_host_perfdata_fp=fopen(xpddefault_host_perfdata_file,(xpddefault_host_perfdata_file_append==TRUE)?"a":"w");

		if(xpddefault_host_perfdata_fp==NULL){
			asprintf(&buffer,"Warning: File '%s' could not be opened - host performance data will not be written to file!\n",xpddefault_host_perfdata_file);
			write_to_logs_and_console(buffer,NSLOG_RUNTIME_WARNING,TRUE);
			my_free((void **)&buffer);
			return ERROR;
		        }
	        }

	return OK;
        }


/* open the service performance data file for writing */
int xpddefault_open_service_perfdata_file(void){
	char *buffer=NULL;

	if(xpddefault_service_perfdata_file!=NULL){

		xpddefault_service_perfdata_fp=fopen(xpddefault_service_perfdata_file,(xpddefault_service_perfdata_file_append==TRUE)?"a":"w");

		if(xpddefault_service_perfdata_fp==NULL){
			asprintf(&buffer,"Warning: File '%s' could not be opened - service performance data will not be written to file!\n",xpddefault_service_perfdata_file);
			write_to_logs_and_console(buffer,NSLOG_RUNTIME_WARNING,TRUE);
			my_free((void **)&buffer);
			return ERROR;
		        }
	        }

	return OK;
        }


/* close the host performance data file */
int xpddefault_close_host_perfdata_file(void){

	if(xpddefault_host_perfdata_fp!=NULL)
		fclose(xpddefault_host_perfdata_fp);

	return OK;
        }


/* close the service performance data file */
int xpddefault_close_service_perfdata_file(void){

	if(xpddefault_service_perfdata_fp!=NULL)
		fclose(xpddefault_service_perfdata_fp);

	return OK;
        }


/* processes delimiter characters in templates */
int xpddefault_preprocess_file_templates(char *template){
	char *tempbuf;
	int x=0;
	int y=0;

	if(template==NULL)
		return OK;

	/* allocate temporary buffer */
	tempbuf=(char *)malloc(strlen(template)+1);
	if(tempbuf==NULL)
		return ERROR;
	strcpy(tempbuf,"");

	for(x=0,y=0;x<strlen(template);x++,y++){
		if(template[x]=='\\'){
			if(template[x+1]=='t'){
				tempbuf[y]='\t';
				x++;
			        }
			else if(template[x+1]=='r'){
				tempbuf[y]='\r';
				x++;
			        }
			else if(template[x+1]=='n'){
				tempbuf[y]='\n';
				x++;
			        }
			else
				tempbuf[y]=template[x];
		        }
		else
			tempbuf[y]=template[x];
	        }
	tempbuf[y]='\x0';

	strcpy(template,tempbuf);
	my_free((void **)&tempbuf);

	return OK;
        }


/* updates service performance data file */
int xpddefault_update_service_performance_data_file(service *svc){
	char *raw_output=NULL;
	char processed_output[MAX_INPUT_BUFFER]="";
	host *temp_host=NULL;
	int result=OK;

	/* we don't have a file to write to*/
	if(xpddefault_service_perfdata_fp==NULL || xpddefault_service_perfdata_file_template==NULL)
		return OK;

	/* find the associated host */
	temp_host=find_host(svc->host_name);

	/* update service macros */
	clear_volatile_macros();
	grab_host_macros(temp_host);
	grab_service_macros(svc);
	grab_datetime_macros();
	grab_summary_macros(NULL);

	/* get the raw line to write */
	raw_output=(char *)strdup(xpddefault_service_perfdata_file_template);

#ifdef DEBUG3
	printf("\tRaw service performance data output: %s\n",raw_output);
#endif

	/* process any macros in the raw output line */
	process_macros(raw_output,processed_output,(int)sizeof(processed_output),0);

#ifdef DEBUG3
	printf("\tProcessed service performance data output: %s\n",processed_output);
#endif

	/* write the processed output line containing performance data to the service perfdata file */
	fputs(processed_output,xpddefault_service_perfdata_fp);
	fputs("\n",xpddefault_service_perfdata_fp);
	fflush(xpddefault_service_perfdata_fp);

	/* free memory */
	my_free((void **)&raw_output);

	return result;
        }


/* updates host performance data file */
int xpddefault_update_host_performance_data_file(host *hst){
	char *raw_output=NULL;
	char processed_output[MAX_INPUT_BUFFER];
	int result=OK;

	/* we don't have a host perfdata file */
	if(xpddefault_host_perfdata_fp==NULL || xpddefault_host_perfdata_file_template==NULL)
		return OK;

	/* update host macros */
	clear_volatile_macros();
	grab_host_macros(hst);
	grab_datetime_macros();
	grab_summary_macros(NULL);

	/* get the raw output */
	raw_output=(char *)strdup(xpddefault_host_perfdata_file_template);

#ifdef DEBUG3
	printf("\tRaw host performance output: %s\n",raw_output);
#endif

	/* process any macros in the raw output */
	process_macros(raw_output,processed_output,(int)sizeof(processed_output),0);

#ifdef DEBUG3
	printf("\tProcessed host performance data output: %s\n",processed_output);
#endif

	/* write the processed output line containing performance data to the host perfdata file */
	fputs(processed_output,xpddefault_host_perfdata_fp);
	fputs("\n",xpddefault_host_perfdata_fp);
	fflush(xpddefault_host_perfdata_fp);

	/* free memory */
	my_free((void **)&raw_output);

	return result;
        }


/* periodically process the host perf data file */
int xpddefault_process_host_perfdata_file(void){
	char raw_command_line[MAX_INPUT_BUFFER]="";
	char processed_command_line[MAX_INPUT_BUFFER]="";
	char *temp_buffer=NULL;
	int early_timeout=FALSE;
	double exectime=0.0;
	int result=OK;
	int macro_options=STRIP_ILLEGAL_MACRO_CHARS|ESCAPE_MACRO_CHARS;

	/* we don't have a command */
	if(xpddefault_host_perfdata_file_processing_command==NULL)
		return OK;

	/* close the performance data files */
	xpddefault_close_host_perfdata_file();
	xpddefault_close_service_perfdata_file();

	/* update macros */
	clear_volatile_macros();
	grab_datetime_macros();
	grab_summary_macros(NULL);

	/* get the raw command line */
	get_raw_command_line(xpddefault_host_perfdata_file_processing_command_ptr,xpddefault_host_perfdata_file_processing_command,raw_command_line,sizeof(raw_command_line),macro_options);
	strip(raw_command_line);

#ifdef DEBUG3
	printf("\tRaw host performance data file processing command line: %s\n",raw_command_line);
#endif

	/* process any macros in the raw command line */
	process_macros(raw_command_line,processed_command_line,(int)sizeof(processed_command_line),macro_options);

#ifdef DEBUG3
	printf("\tProcessed host performance data file processing command line: %s\n",processed_command_line);
#endif

	/* run the command */
	my_system(processed_command_line,xpddefault_perfdata_timeout,&early_timeout,&exectime,NULL,0);

	/* check to see if the command timed out */
	if(early_timeout==TRUE){
		asprintf(&temp_buffer,"Warning: Host performance data file processing command '%s' timed out after %d seconds\n",processed_command_line,xpddefault_perfdata_timeout);
		write_to_logs_and_console(temp_buffer,NSLOG_RUNTIME_WARNING,TRUE);
		my_free((void **)&temp_buffer);
	        }

	/* re-open the performance data files */
	xpddefault_open_service_perfdata_file();
	xpddefault_open_host_perfdata_file();

	return result;
        }


/* periodically process the service perf data file */
int xpddefault_process_service_perfdata_file(void){
	char raw_command_line[MAX_INPUT_BUFFER]="";
	char processed_command_line[MAX_INPUT_BUFFER]="";
	char *temp_buffer=NULL;
	int early_timeout=FALSE;
	double exectime=0.0;
	int result=OK;
	int macro_options=STRIP_ILLEGAL_MACRO_CHARS|ESCAPE_MACRO_CHARS;

	/* we don't have a command */
	if(xpddefault_service_perfdata_file_processing_command==NULL)
		return OK;

	/* close the performance data files */
	xpddefault_close_host_perfdata_file();
	xpddefault_close_service_perfdata_file();

	/* update macros */
	clear_volatile_macros();
	grab_datetime_macros();
	grab_summary_macros(NULL);

	/* get the raw command line */
	get_raw_command_line(xpddefault_service_perfdata_file_processing_command_ptr,xpddefault_service_perfdata_file_processing_command,raw_command_line,sizeof(raw_command_line),macro_options);
	strip(raw_command_line);

#ifdef DEBUG3
	printf("\tRaw service performance data file processing command line: %s\n",raw_command_line);
#endif

	/* process any macros in the raw command line */
	process_macros(raw_command_line,processed_command_line,(int)sizeof(processed_command_line),macro_options);

#ifdef DEBUG3
	printf("\tProcessed service performance data file processing command line: %s\n",processed_command_line);
#endif

	/* run the command */
	my_system(processed_command_line,xpddefault_perfdata_timeout,&early_timeout,&exectime,NULL,0);

	/* check to see if the command timed out */
	if(early_timeout==TRUE){
		asprintf(&temp_buffer,"Warning: Service performance data file processing command '%s' timed out after %d seconds\n",processed_command_line,xpddefault_perfdata_timeout);
		write_to_logs_and_console(temp_buffer,NSLOG_RUNTIME_WARNING,TRUE);
		my_free((void **)&temp_buffer);
	        }

	/* re-open the performance data files */
	xpddefault_open_service_perfdata_file();
	xpddefault_open_host_perfdata_file();

	return result;
        }
