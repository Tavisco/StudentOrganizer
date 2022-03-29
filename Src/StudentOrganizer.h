/* * StudentOrganizer.h * * header file for Student Organizer * * This wizard-generated code is based on code adapted from the * stationery files distributed as part of the Palm OS SDK 4.0. * * Copyright (c) 1999-2000 Palm, Inc. or its subsidiaries. * All rights reserved. *//********************************************************************* * Internal Constants *********************************************************************/#define appFileCreator			'UWSC'#define appName					"Student Organizer"#define appVersionNum			0x01#define appPrefID				0x00#define appPrefVersionNum		0x01 /********************************************************************* * Internal Structures *********************************************************************/typedef struct ClassOccurrenceDB {	Boolean active;	Int16 dayOfWeekInt;	Int16 sHour, sMinute, fHour, fMinute;	} ClassOccurrenceDB;typedef struct ClassDB {	ClassOccurrenceDB classOcurrence[7];	Char className[19];	Char classRoom[9];	} ClassDB;typedef ClassDB* ClassDBPtr;	/********************************************************************* * Global variables *********************************************************************/extern DmOpenRef gClassesDB;/********************************************************************* * Function Prototypes *********************************************************************/  /* Functions in StudentOrganizer.c */ void * GetObjectPtr(UInt16 objectID); Boolean AppHandleEvent(EventPtr eventP); void AppEventLoop(void); void AppStop(void); Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags); UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlahs);  /* Functions in Main.c */ Boolean MainFormDoCommand(UInt16 command); Boolean MainFormHandleEvent(EventPtr eventP); void MainFormInit(FormType *frmP); void ShowCurrentTime(FormType *frmP); void ShowCurrentWeekday(FormType *frmP);  /* Functions in Classes.c */ Boolean ClassesFormDoCommand(UInt16 command); Boolean ClassesFormHandleEvent(EventPtr eventP); void ClassesFormInit(FormType *frmP); void ClassesAutoSelectCurrentDay(); void ClassesActivateSelector(UInt16 field);   /* Functions in ManageClass.c */ Boolean ManageClassFormDoCommand(UInt16 command); Boolean ManageClassFormHandleEvent(EventPtr eventP); void ManageClassFormInit(FormType *frmP); void AskTimeToUser(UInt16 field); void ToggleTimeSelectorTrigger(); void autoSelectCurrentDay(); void activateSelector(UInt16 field); void LoadDoW(); void SetTimeSelectorVisibility(); void SetTimeSelectorLabels(UInt16 field);