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

#define kCreator                'UWSC'
#define kClassDBType            'CLSS'
#define kClassesDBName          "StudentOrganizerClasses"

#define ftrManageClassNum       (UInt16)0
#define ftrClassesDBNum         (UInt16)1
#define ftrClassesNum           (UInt16)2
#define ftrShrdClassesVarsNum   (UInt16)3
 

/*********************************************************************
 * Internal Structures
 *********************************************************************/

typedef struct ClassOccurrenceDB {
	Boolean active, timeHasBeenSet;
	Int16 dayOfWeekInt, sHour, sMinute, fHour, fMinute;
} ClassOccurrenceDB;

typedef struct ClassDB {
	struct ClassOccurrenceDB classOcurrence[7];
	Char className[19];
	Char classRoom[9];
} ClassDB;

typedef struct ManageClassVariables {
	Int16 selectedDoW; // Selected Day of Week by the pushbuttons
	ClassDB record; // Current database record
} ManageClassVariables;

typedef struct ClassesVariables {
	Int16 selectedDoW; // Selected Day of Week by the pushbuttons
	ClassDB records[]; // Records of the database
} ClassesVariables;

typedef struct SharedClassesVariables {
	UInt16 selectedClassDbIndex; // DB Index of select class on Classes Form
} SharedClassesVariables;

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
 Err AppStart(void);
 UInt32 __attribute__((section(".vectors"))) __Startup__(void);
 
 /* Functions in Main.c */
 Boolean MainFormDoCommand(UInt16 command);
 Boolean MainFormHandleEvent(EventPtr eventP);
 void MainFormInit(FormType *frmP);
 void ShowCurrentTime(FormType *frmP);
 void ShowCurrentWeekday(FormType *frmP);
 void SetCurrentClass(FormType *frmP, Char *className);
 void SetNextClass(FormType *frmP, Char *currClass);
 
 /* Functions in Classes.c */
 Boolean ClassesFormDoCommand(UInt16 command, ClassesVariables* pstVars);
 Boolean ClassesFormHandleEvent(EventPtr eventP);
 void ClassesFormInit(FormType *frmP, ClassesVariables* pstVars);
 void ClassesAutoSelectCurrentDay(ClassesVariables* pstVars);
 void ClassesActivateSelector(UInt16 field);
 void LoadClasses(ClassesVariables* pstVars);
 Err LoadSelectedClassIntoMemory(ClassesVariables* pstVars);
 
  /* Functions in ManageClass.c */
 Boolean ManageClassFormDoCommand(UInt16 command, ManageClassVariables* pstVars);
 Boolean ManageClassFormHandleEvent(EventPtr eventP);
 void ManageClassFormInit(FormType *frmP, ManageClassVariables* pstVars);
 void AskTimeToUser(UInt16 field, ManageClassVariables* pstVars);
 void ToggleTimeSelectorTrigger();
 void autoSelectCurrentDay();
 void activateSelector(UInt16 field);
 void LoadDoW();
 void SetTimeSelectorVisibility();
 void SetTimeSelectorLabels(UInt16 field, ManageClassVariables* pstVars);
 Err SaveChanges(ManageClassVariables* pstVars);
 Err SaveChangesToDatabase(ManageClassVariables* pstVars);
 void CheckForAlreadySelected(ManageClassVariables* pstVars);
 Boolean IsScheduleInvalid(ManageClassVariables* pstVars);
 Err DeleteClass(ManageClassVariables* pstVars);