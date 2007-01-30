/*****************************************************************************
 *
 * OBJECTS.H - Header file for object addition/search functions
 *
 * Copyright (c) 1999-2006 Ethan Galstad (nagios@nagios.org)
 * Last Modified: 07-18-2006
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


#ifndef _OBJECTS_H
#define _OBJECTS_H

#include "config.h"
#include "common.h"

#ifdef __cplusplus
  extern "C" {
#endif



/*************** CURRENT OBJECT REVISION **************/

#define CURRENT_OBJECT_STRUCTURE_VERSION        9999    /* set to 3 once Nagios 3.x development stabilizes... */



/***************** OBJECT SIZE LIMITS *****************/

#define MAX_STATE_HISTORY_ENTRIES		21	/* max number of old states to keep track of for flap detection */
#define MAX_CONTACT_ADDRESSES                   6       /* max number of custom addresses a contact can have */



/***************** CHAINED HASH LIMITS ****************/

#define SERVICE_HASHSLOTS                      1024
#define HOST_HASHSLOTS                         1024
#define COMMAND_HASHSLOTS                      256
#define TIMEPERIOD_HASHSLOTS                   64
#define CONTACT_HASHSLOTS                      128
#define CONTACTGROUP_HASHSLOTS                 64
#define HOSTGROUP_HASHSLOTS                    128
#define SERVICEGROUP_HASHSLOTS                 128

#define HOSTDEPENDENCY_HASHSLOTS               1024
#define SERVICEDEPENDENCY_HASHSLOTS            1024
#define HOSTESCALATION_HASHSLOTS               1024
#define SERVICEESCALATION_HASHSLOTS            1024



/****************** DATA STRUCTURES *******************/

typedef struct host_struct host;
typedef struct service_struct service;
typedef struct contact_struct contact;

/* TIMERANGE structure */
typedef struct timerange_struct{
	unsigned long range_start;
	unsigned long range_end;
	struct timerange_struct *next;
        }timerange;


/* TIMEPERIOD structure */
typedef struct timeperiod_struct{
	char    *name;
	char    *alias;
	timerange *days[7];
	struct 	timeperiod_struct *next;
	struct 	timeperiod_struct *nexthash;
	}timeperiod;


/* CONTACTGROUPMEMBER structure */
typedef struct contactgroupmember_struct{
	char    *contact_name;
#ifdef NSCORE
	contact *contact_ptr;
#endif
	struct  contactgroupmember_struct *next;
        }contactgroupmember;


/* CONTACTGROUP structure */
typedef struct contactgroup_struct{
	char	*group_name;
	char    *alias;
	contactgroupmember *members;
	struct	contactgroup_struct *next;
	struct	contactgroup_struct *nexthash;
	}contactgroup;


/* CONTACTGROUPSMEMBER structure */
typedef struct contactgroupsmember_struct{
	char    *group_name;
#ifdef NSCORE
	contactgroup *group_ptr;
#endif
	struct contactgroupsmember_struct *next;
        }contactgroupsmember;


/* CUSTOMVARIABLESMEMBER structure */
typedef struct customvariablesmember_struct{
	char    *variable_name;
	char    *variable_value;
	int     has_been_modified;
	struct customvariablesmember_struct *next;
        }customvariablesmember;


/* COMMAND structure */
typedef struct command_struct{
	char    *name;
	char    *command_line;
	struct command_struct *next;
	struct command_struct *nexthash;
        }command;


/* COMMANDSMEMBER structure */
typedef struct commandsmember_struct{
	char	*command;
#ifdef NSCORE
	command *command_ptr;
#endif
	struct	commandsmember_struct *next;
	}commandsmember;


/* CONTACT structure */
struct contact_struct{
	char	*name;
	char	*alias;
	char	*email;
	char	*pager;
	char    *address[MAX_CONTACT_ADDRESSES];
	commandsmember *host_notification_commands;
	commandsmember *service_notification_commands;	
	int     notify_on_service_unknown;
	int     notify_on_service_warning;
	int     notify_on_service_critical;
	int     notify_on_service_recovery;
	int     notify_on_service_flapping;
	int     notify_on_service_downtime;
	int 	notify_on_host_down;
	int	notify_on_host_unreachable;
	int	notify_on_host_recovery;
	int     notify_on_host_flapping;
	int     notify_on_host_downtime;
	char	*host_notification_period;
	char	*service_notification_period;
	int     host_notifications_enabled;
	int     service_notifications_enabled;
	int     can_submit_commands;
	int     retain_status_information;
	int     retain_nonstatus_information;
	customvariablesmember *custom_variables;
#ifdef NSCORE
	time_t  last_host_notification;
	time_t  last_service_notification;
	unsigned long modified_attributes;
	unsigned long modified_host_attributes;
	unsigned long modified_service_attributes;

	timeperiod *host_notification_period_ptr;
	timeperiod *service_notification_period_ptr;
#endif
	struct	contact_struct *next;
	struct	contact_struct *nexthash;
        };


/* CONTACTSMEMBER structure */
typedef struct contactsmember_struct{
	char    *contact_name;
#ifdef NSCORE
	contact *contact_ptr;
#endif
	struct contactsmember_struct *next;
        }contactsmember;


/* HOSTSMEMBER structure */
typedef struct hostsmember_struct{
	char    *host_name;
#ifdef NSCORE
	host    *host_ptr;
#endif
	struct hostsmember_struct *next;
        }hostsmember;


/* HOSTGROUPMEMBER structure */
typedef struct hostgroupmember_struct{
	char    *host_name;
#ifdef NSCORE
	host    *host_ptr;
#endif
	struct  hostgroupmember_struct *next;
        }hostgroupmember;


/* HOSTGROUP structure */
typedef struct hostgroup_struct{
	char 	*group_name;
	char    *alias;
	hostgroupmember *members;
	struct	hostgroup_struct *next;
	struct	hostgroup_struct *nexthash;
	}hostgroup;


/* HOST structure */
struct host_struct{
	char    *name;
	char    *display_name;
	char	*alias;
	char    *address;
        hostsmember *parent_hosts;
	char    *host_check_command;
	double  check_interval;
	double  retry_interval;
	int     max_attempts;
	char    *event_handler;
	contactgroupsmember *contact_groups;
	contactsmember *contacts;
	double  notification_interval;
	double  first_notification_delay;
	int	notify_on_down;
	int	notify_on_unreachable;
	int     notify_on_recovery;
	int     notify_on_flapping;
	int     notify_on_downtime;
	char	*notification_period;
	char    *check_period;
	int     flap_detection_enabled;
	double  low_flap_threshold;
	double  high_flap_threshold;
	int     flap_detection_on_up;
	int     flap_detection_on_down;
	int     flap_detection_on_unreachable;
	int     stalk_on_up;
	int     stalk_on_down;
	int     stalk_on_unreachable;
	int     check_freshness;
	int     freshness_threshold;
	int     process_performance_data;
	int     checks_enabled;
	int     accept_passive_host_checks;
	int     event_handler_enabled;
	int     retain_status_information;
	int     retain_nonstatus_information;
	int     failure_prediction_enabled;
	char    *failure_prediction_options;
	int     obsess_over_host;
	char    *notes;
	char    *notes_url;
	char    *action_url;
	char    *icon_image;
	char    *icon_image_alt;
	char    *vrml_image;
	char    *statusmap_image;
	int     have_2d_coords;
	int     x_2d;
	int     y_2d;
	int     have_3d_coords;
	double  x_3d;
	double  y_3d;
	double  z_3d;
	int     should_be_drawn;
	customvariablesmember *custom_variables;
#ifdef NSCORE
	int     problem_has_been_acknowledged;
	int     acknowledgement_type;
	int     check_type;
	int     current_state;
	int     last_state;
	int     last_hard_state;
	char	*plugin_output;
	char    *long_plugin_output;
	char    *perf_data;
        int     state_type;
	int     current_attempt;
	unsigned long current_event_id;
	unsigned long last_event_id;
	double  latency;
	double  execution_time;
	int     is_executing;
	int     check_options;
	int     notifications_enabled;
	time_t  last_host_notification;
	time_t  next_host_notification;
	time_t  next_check;
	int     should_be_scheduled;
	time_t  last_check;
	time_t	last_state_change;
	time_t	last_hard_state_change;
	time_t  last_time_up;
	time_t  last_time_down;
	time_t  last_time_unreachable;
	int     has_been_checked;
	int     is_being_freshened;
	int     notified_on_down;
	int     notified_on_unreachable;
	int     current_notification_number;
	int     no_more_notifications;
	unsigned long current_notification_id;
	int     check_flapping_recovery_notification;
	int     scheduled_downtime_depth;
	int     pending_flex_downtime;
	int     state_history[MAX_STATE_HISTORY_ENTRIES];    /* flap detection */
	int     state_history_index;
	time_t  last_state_history_update;
	int     is_flapping;
	unsigned long flapping_comment_id;
	double  percent_state_change;
	int     total_services;
	unsigned long total_service_check_interval;
	unsigned long modified_attributes;
	int     circular_path_checked;
	int     contains_circular_path;

	command *event_handler_ptr;
	command *check_command_ptr;
	timeperiod *check_period_ptr;
	timeperiod *notification_period_ptr;
	hostgroup *first_hostgroup_ptr;
#endif
	struct  host_struct *next;
	struct  host_struct *nexthash;
        };


/* SERVICEGROUPMEMBER structure */
typedef struct servicegroupmember_struct{
	char    *host_name;
	char    *service_description;
#ifdef NSCORE
	service *service_ptr;
#endif
	struct  servicegroupmember_struct *next;
        }servicegroupmember;


/* SERVICEGROUP structure */
typedef struct servicegroup_struct{
	char 	*group_name;
	char    *alias;
	servicegroupmember *members;
	struct	servicegroup_struct *next;
	struct	servicegroup_struct *nexthash;
	}servicegroup;


/* SERVICE structure */
struct service_struct{
	char	*host_name;
	char	*description;
	char    *display_name;
        char    *service_check_command;
	char    *event_handler;
	double	check_interval;
	double  retry_interval;
	int	max_attempts;
	int     parallelize;
	contactgroupsmember *contact_groups;
	contactsmember *contacts;
	double	notification_interval;
	double  first_notification_delay;
	int     notify_on_unknown;
	int	notify_on_warning;
	int	notify_on_critical;
	int	notify_on_recovery;
	int     notify_on_flapping;
	int     notify_on_downtime;
	int     stalk_on_ok;
	int     stalk_on_warning;
	int     stalk_on_unknown;
	int     stalk_on_critical;
	int     is_volatile;
	char	*notification_period;
	char	*check_period;
	int     flap_detection_enabled;
	double  low_flap_threshold;
	double  high_flap_threshold;
	int     flap_detection_on_ok;
	int     flap_detection_on_warning;
	int     flap_detection_on_unknown;
	int     flap_detection_on_critical;
	int     process_performance_data;
	int     check_freshness;
	int     freshness_threshold;
	int     accept_passive_service_checks;
	int     event_handler_enabled;
	int	checks_enabled;
	int     retain_status_information;
	int     retain_nonstatus_information;
	int     notifications_enabled;
	int     obsess_over_service;
	int     failure_prediction_enabled;
	char    *failure_prediction_options;
	char    *notes;
	char    *notes_url;
	char    *action_url;
	char    *icon_image;
	char    *icon_image_alt;
	customvariablesmember *custom_variables;
#ifdef NSCORE
	int     problem_has_been_acknowledged;
	int     acknowledgement_type;
	int     host_problem_at_last_check;
	int     check_type;
	int	current_state;
	int	last_state;
	int	last_hard_state;
	char	*plugin_output;
	char    *long_plugin_output;
	char    *perf_data;
        int     state_type;
	time_t	next_check;
	int     should_be_scheduled;
	time_t	last_check;
	int	current_attempt;
	unsigned long current_event_id;
	unsigned long last_event_id;
	time_t	last_notification;
	time_t  next_notification;
	int     no_more_notifications;
	int     check_flapping_recovery_notification;
	time_t	last_state_change;
	time_t	last_hard_state_change;
	time_t  last_time_ok;
	time_t  last_time_warning;
	time_t  last_time_unknown;
	time_t  last_time_critical;
	int     has_been_checked;
	int     is_being_freshened;
	int     notified_on_unknown;
	int     notified_on_warning;
	int     notified_on_critical;
	int     current_notification_number;
        unsigned long current_notification_id;
	double  latency;
	double  execution_time;
	int     is_executing;
	int     check_options;
	int     scheduled_downtime_depth;
	int     pending_flex_downtime;
	int     state_history[MAX_STATE_HISTORY_ENTRIES];    /* flap detection */
	int     state_history_index;
	int     is_flapping;
	unsigned long flapping_comment_id;
	double  percent_state_change;
	unsigned long modified_attributes;

	host *host_ptr;
	command *event_handler_ptr;
	char *event_handler_args;
	command *check_command_ptr;
	char *check_command_args;
	timeperiod *check_period_ptr;
	timeperiod *notification_period_ptr;
	servicegroup *first_servicegroup_ptr;
#endif
	struct service_struct *next;
	struct service_struct *nexthash;
	};


/* SERVICE ESCALATION structure */
typedef struct serviceescalation_struct{
	char    *host_name;
	char    *description;
	int     first_notification;
	int     last_notification;
	double  notification_interval;
	char    *escalation_period;
	int     escalate_on_recovery;
	int     escalate_on_warning;
	int     escalate_on_unknown;
	int     escalate_on_critical;
	contactgroupsmember *contact_groups;
	contactsmember *contacts;
#ifdef NSCORE
	service *service_ptr;
	timeperiod *escalation_period_ptr;
#endif
	struct  serviceescalation_struct *next;
	struct  serviceescalation_struct *nexthash;
        }serviceescalation;


/* SERVICE DEPENDENCY structure */
typedef struct servicedependency_struct{
	int     dependency_type;
	char    *dependent_host_name;
	char    *dependent_service_description;
	char    *host_name;
	char    *service_description;
	char    *dependency_period;
	int     inherits_parent;
	int     fail_on_ok;
	int     fail_on_warning;
	int     fail_on_unknown;
	int     fail_on_critical;
	int     fail_on_pending;
#ifdef NSCORE
	int     circular_path_checked;
	int     contains_circular_path;

	service *master_service_ptr;
	service *dependent_service_ptr;
	timeperiod *dependency_period_ptr;
#endif
	struct servicedependency_struct *next;
	struct servicedependency_struct *nexthash;
        }servicedependency;


/* HOST ESCALATION structure */
typedef struct hostescalation_struct{
	char    *host_name;
	int     first_notification;
	int     last_notification;
	double  notification_interval;
	char    *escalation_period;
	int     escalate_on_recovery;
	int     escalate_on_down;
	int     escalate_on_unreachable;
	contactgroupsmember *contact_groups;
	contactsmember *contacts;
#ifdef NSCORE
	host    *host_ptr;
	timeperiod *escalation_period_ptr;
#endif
	struct  hostescalation_struct *next;
	struct  hostescalation_struct *nexthash;
        }hostescalation;


/* HOST DEPENDENCY structure */
typedef struct hostdependency_struct{
	int     dependency_type;
	char    *dependent_host_name;
	char    *host_name;
	char    *dependency_period;
	int     inherits_parent;
	int     fail_on_up;
	int     fail_on_down;
	int     fail_on_unreachable;
	int     fail_on_pending;
#ifdef NSCORE
	int     circular_path_checked;
	int     contains_circular_path;

	host    *master_host_ptr;
	host    *dependent_host_ptr;
	timeperiod *dependency_period_ptr;
#endif
	struct hostdependency_struct *next;
	struct hostdependency_struct *nexthash;
        }hostdependency;


/* OBJECT LIST STRUCTURE */
typedef struct objectlist_struct{
	void      *object_ptr;
	struct objectlist_struct *next;
        }objectlist;



/****************** HASH STRUCTURES ********************/

typedef struct host_cursor_struct{
	int     host_hashchain_iterator;
	host    *current_host_pointer;
        }host_cursor;





/********************* FUNCTIONS **********************/

/**** DEBUG functions ****/
/* RMO: 9/25/01
   Send debug output to stdout. Does nothing if 'level' is
   not enabled by a corresponding 'DEBUGn' define.
   Accepts format string (fmt) and variable-length arg list
   (as printf does). Prints to stdout and, if NSCGI environment,
   surrounded with HTML comment delimiters to be viewed through
   browser's 'view source' option.

   Use as: dbg_print((level,fmt,...)); [NOTE double parens]
 
   The macro def below causes dbg_print(()) calls to vaporize
   if none of the DEBUGn levels are defined.
*/
#if defined(DEBUG0) || defined(DEBUG1) || defined(DEBUG2) || defined(DEBUG3) || defined(DEBUG4) || defined(DEBUG5) || defined(DEBUG6) || defined(DEBUG7) || defined(DEBUG8) || defined(DEBUG9) || defined(DEBUG10) || defined(DEBUG11)
#define dbg_print(args) dbg_print_x args
#else
#define dbg_print(args)
#endif


/**** Top-level input functions ****/
int read_object_config_data(char *,int,int,int);        /* reads all external configuration data of specific types */

/**** Object Creation Functions ****/
contact *add_contact(char *,char *,char *,char *,char **,char *,char *,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int);	/* adds a contact definition */
commandsmember *add_service_notification_command_to_contact(contact *,char *);				/* adds a service notification command to a contact definition */
commandsmember *add_host_notification_command_to_contact(contact *,char *);				/* adds a host notification command to a contact definition */
customvariablesmember *add_custom_variable_to_contact(contact *,char *,char *);                         /* adds a custom variable to a service definition */
 host *add_host(char *,char *,char *,char *,char *,double,double,int,int,int,int,int,int,double,double,char *,int,char *,int,int,char *,int,int,double,double,int,int,int,int,int,int,int,int,char *,int,int,char *,char *,char *,char *,char *,char *,char *,int,int,int,double,double,double,int,int,int,int,int);	/* adds a host definition */
hostsmember *add_parent_host_to_host(host *,char *);							/* adds a parent host to a host definition */
contactgroupsmember *add_contactgroup_to_host(host *,char *);					        /* adds a contactgroup to a host definition */
contactsmember *add_contact_to_host(host *,char *);                                                     /* adds a contact to a host definition */
customvariablesmember *add_custom_variable_to_host(host *,char *,char *);                               /* adds a custom variable to a host definition */
timeperiod *add_timeperiod(char *,char *);								/* adds a timeperiod definition */
timerange *add_timerange_to_timeperiod(timeperiod *,int,unsigned long,unsigned long);			/* adds a timerange to a timeperiod definition */
hostgroup *add_hostgroup(char *,char *);								/* adds a hostgroup definition */
hostgroupmember *add_host_to_hostgroup(hostgroup *, char *);						/* adds a host to a hostgroup definition */
servicegroup *add_servicegroup(char *,char *);                                                          /* adds a servicegroup definition */
servicegroupmember *add_service_to_servicegroup(servicegroup *,char *,char *);                          /* adds a service to a servicegroup definition */
contactgroup *add_contactgroup(char *,char *);								/* adds a contactgroup definition */
contactgroupmember *add_contact_to_contactgroup(contactgroup *,char *);					/* adds a contact to a contact group definition */
command *add_command(char *,char *);									/* adds a command definition */
service *add_service(char *,char *,char *,char *,int,int,int,double,double,double,double,char *,int,int,int,int,int,int,int,int,char *,int,char *,int,int,double,double,int,int,int,int,int,int,int,int,int,int,char *,int,int,char *,char *,char *,char *,char *,int,int,int);	/* adds a service definition */
contactgroupsmember *add_contactgroup_to_service(service *,char *);					/* adds a contact group to a service definition */
contactsmember *add_contact_to_service(service *,char *);                                               /* adds a contact to a host definition */
serviceescalation *add_serviceescalation(char *,char *,int,int,double,char *,int,int,int,int);          /* adds a service escalation definition */
contactgroupsmember *add_contactgroup_to_serviceescalation(serviceescalation *,char *);                 /* adds a contact group to a service escalation definition */
contactsmember *add_contact_to_serviceescalation(serviceescalation *,char *);                           /* adds a contact to a service escalation definition */
customvariablesmember *add_custom_variable_to_service(service *,char *,char *);                         /* adds a custom variable to a service definition */
servicedependency *add_service_dependency(char *,char *,char *,char *,int,int,int,int,int,int,int,char *);     /* adds a service dependency definition */
hostdependency *add_host_dependency(char *,char *,int,int,int,int,int,int,char *);                             /* adds a host dependency definition */
hostescalation *add_hostescalation(char *,int,int,double,char *,int,int,int);                           /* adds a host escalation definition */
contactsmember *add_contact_to_hostescalation(hostescalation *,char *);                                 /* adds a contact to a host escalation definition */
contactgroupsmember *add_contactgroup_to_hostescalation(hostescalation *,char *);                       /* adds a contact group to a host escalation definition */

contactsmember *add_contact_to_object(contactsmember **,char *);                                        /* adds a contact to an object */ 
customvariablesmember *add_custom_variable_to_object(customvariablesmember **,char *,char *);           /* adds a custom variable to an object */



/**** Object Hash Functions ****/
int add_host_to_hashlist(host *);
int add_service_to_hashlist(service *);
int add_command_to_hashlist(command *);
int add_timeperiod_to_hashlist(timeperiod *);
int add_contact_to_hashlist(contact *);
int add_contactgroup_to_hashlist(contactgroup *);
int add_hostgroup_to_hashlist(hostgroup *);
int add_servicegroup_to_hashlist(servicegroup *);
int add_hostdependency_to_hashlist(hostdependency *);
int add_servicedependency_to_hashlist(servicedependency *);
int add_hostescalation_to_hashlist(hostescalation *);
int add_serviceescalation_to_hashlist(serviceescalation *);


/**** Object Search Functions ****/
timeperiod * find_timeperiod(char *);						                /* finds a timeperiod object */
host * find_host(char *);									/* finds a host object */
hostgroup * find_hostgroup(char *);						                /* finds a hostgroup object */
servicegroup * find_servicegroup(char *);					                /* finds a servicegroup object */
contact * find_contact(char *);							                /* finds a contact object */
contactgroup * find_contactgroup(char *);					                /* finds a contactgroup object */
contactgroupmember *find_contactgroupmember(char *,contactgroup *);	                        /* finds a contactgroup member object */
command * find_command(char *);							                /* finds a command object */
service * find_service(char *,char *);								/* finds a service object */


/**** Object Traversal Functions ****/
void move_first_service(void);									/* sets up the static memory area for get_next_service */
service *get_next_service(void);								/* returns the next service, NULL at the end of the list */
int find_all_services_by_host(char *);							        /* sets up the static memory area for get_next_service_by_host */
service *get_next_service_by_host(void);							/* returns the next service for the host, NULL at the end of the list */
void move_first_host(void);									/* sets up the static memory area for get_next_host */
host *get_next_host(void);									/* returns the next host, NULL at the end of the list */
void *get_host_cursor(void);					                                /* allocate memory for the host cursor */
host *get_next_host_cursor(void *v_cursor);							/* return the next host, NULL at the end of the list */
void free_host_cursor(void *cursor);								/* free allocated cursor memory */
void *get_next_N(void **hashchain, int hashslots, int *iterator, void *current, void *next);

hostescalation *get_first_hostescalation_by_host(char *);
hostescalation *get_next_hostescalation_by_host(char *,hostescalation *);
serviceescalation *get_first_serviceescalation_by_service(char *,char *);
serviceescalation *get_next_serviceescalation_by_service(char *,char *,serviceescalation *);
hostdependency *get_first_hostdependency_by_dependent_host(char *);
hostdependency *get_next_hostdependency_by_dependent_host(char *,hostdependency *);
servicedependency *get_first_servicedependency_by_dependent_service(char *,char *);
servicedependency *get_next_servicedependency_by_dependent_service(char *,char *,servicedependency *);

#ifdef NSCORE
int add_object_to_objectlist(objectlist **,void *);
int free_objectlist(objectlist **);
#endif


/**** Object Query Functions ****/
int is_host_immediate_child_of_host(host *,host *);	                /* checks if a host is an immediate child of another host */	
int is_host_primary_immediate_child_of_host(host *,host *);             /* checks if a host is an immediate child (and primary child) of another host */
int is_host_immediate_parent_of_host(host *,host *);	                /* checks if a host is an immediate child of another host */	
int is_host_member_of_hostgroup(hostgroup *,host *);		        /* tests whether or not a host is a member of a specific hostgroup */
int is_host_member_of_servicegroup(servicegroup *,host *);	        /* tests whether or not a service is a member of a specific servicegroup */
int is_service_member_of_servicegroup(servicegroup *,service *);	/* tests whether or not a service is a member of a specific servicegroup */
int is_contact_member_of_contactgroup(contactgroup *, contact *);	/* tests whether or not a contact is a member of a specific contact group */
int is_contact_for_hostgroup(hostgroup *,contact *);	                /* tests whether or not a contact is a member of a specific hostgroup */
int is_contact_for_servicegroup(servicegroup *,contact *);	        /* tests whether or not a contact is a member of a specific servicegroup */
int is_contact_for_host(host *,contact *);			        /* tests whether or not a contact is a contact member for a specific host */
int is_escalated_contact_for_host(host *,contact *);                    /* checks whether or not a contact is an escalated contact for a specific host */
int is_contact_for_service(service *,contact *);		        /* tests whether or not a contact is a contact member for a specific service */
int is_escalated_contact_for_service(service *,contact *);              /* checks whether or not a contact is an escalated contact for a specific service */
int is_host_immediate_parent_of_host(host *,host *);		        /* tests whether or not a host is an immediate parent of another host */

int number_of_immediate_child_hosts(host *);		                /* counts the number of immediate child hosts for a particular host */
int number_of_total_child_hosts(host *);				/* counts the number of total child hosts for a particular host */
int number_of_immediate_parent_hosts(host *);				/* counts the number of immediate parents hosts for a particular host */
int number_of_total_parent_hosts(host *);				/* counts the number of total parents hosts for a particular host */

#ifdef NSCORE
int check_for_circular_host_path(host *,host *);                             /* checks if a circular path exists for a given host */
int check_for_circular_servicedependency_path(servicedependency *,servicedependency *,int);   /* checks if a circular dependency exists for a given service */
int check_for_circular_hostdependency_path(hostdependency *,hostdependency *,int);   /* checks if a circular dependency exists for a given host */
#endif


/**** Object Cleanup Functions ****/
int free_object_data(void);                             /* frees all allocated memory for the object definitions */

#ifdef __cplusplus
  }
#endif

#endif