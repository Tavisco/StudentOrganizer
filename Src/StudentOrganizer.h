/*
 * StudentOrganizer.h
 *
 * header file for Student Organizer
 *
 * This wizard-generated code is based on code adapted from the
 * stationery files distributed as part of the Palm OS SDK 4.0.
 *
 * Copyright (c) 1999-2000 Palm, Inc. or its subsidiaries.
 * All rights reserved.
 */

/*********************************************************************
 * Internal Constants
 *********************************************************************/

#define appFileCreator			'UWSC'
#define appName					"Student Organizer"
#define appVersionNum			0x01
#define appPrefID				0x00
#define appPrefVersionNum		0x01
 

/*********************************************************************
 * Internal Structures
 *********************************************************************/



/*********************************************************************
 * Global variables
 *********************************************************************/



/*********************************************************************
 * Function Prototypes
 *********************************************************************/
 
 /* Functions in StudentOrganizer.c */
 void * GetObjectPtr(UInt16 objectID);
 Boolean AppHandleEvent(EventPtr eventP);
 void AppEventLoop(void);
 void AppStop(void);
 Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags);
 UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlahs);
 
 /* Functions in Main.c */
 Boolean MainFormDoCommand(UInt16 command);
 Boolean MainFormHandleEvent(EventPtr eventP);
 void MainFormInit(FormType *frmP);