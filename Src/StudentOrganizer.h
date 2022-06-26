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

#define appFileCreator 'UWSC'
#define appName "Student Organizer"
#define appVersionNum 0x01
#define appPrefID 0x00
#define appPrefVersionNum 0x01

#define kCreator 'UWSC'
#define kClassDBType 'CLSS'
#define kClassesDBName "StudentOrganizerClasses"
#define kHmwrkDBType 'HMWK'
#define kHmwrkDBName "StudentOrganizerHomeworks"

#define ftrManageClassNum (UInt16)0
#define ftrClassesDBNum (UInt16)1
#define ftrClassesNum (UInt16)2
#define ftrShrdClassesVarsNum (UInt16)3
#define ftrManageHomeworkNum (UInt16)4
#define ftrHmwrkDBNum (UInt16)5
#define ftrShrdHomeworksVarsNum (UInt16)6

/*********************************************************************
 * Internal Structures
 *********************************************************************/

typedef struct ClassOccurrenceDB
{
	Boolean active, timeHasBeenSet;
	Int16 dayOfWeekInt, sHour, sMinute, fHour, fMinute;
} ClassOccurrenceDB;

typedef struct ClassDB
{
	struct ClassOccurrenceDB classOcurrence[7];
	Char className[19];
	Char classRoom[9];
} ClassDB;

typedef struct ManageClassVariables
{
	Int16 selectedDoW; // Selected Day of Week by the pushbuttons
	ClassDB record;	   // Current database record
} ManageClassVariables;

typedef struct ClassesVariables
{
	Int16 selectedDoW; // Selected Day of Week by the pushbuttons
	ClassDB records[]; // Records of the database
} ClassesVariables;

typedef struct SharedClassesVariables
{
	UInt16 selectedClassDbIndex; // DB Index of select class on Classes Form
	Int16 selectedDoW; // To remember the selected DoW
} SharedClassesVariables;

typedef struct HomeworkDB
{
	Int16 dueDay, dueMonth, dueYear; // Due date
	Char hmwrkName[19];
	UInt16 classIndex;
	Char hmwrkComments[87];
	DateTimeType completedDate;
	
} HomeworkDB;

typedef struct ManageHomeworkVariables
{
	HomeworkDB record;
	Boolean classSelected;
} ManageHomeworkVariables;

typedef struct SharedHomeworksVariables
{
	UInt16 selectedHomeworkDbIndex; // DB Index of selected homework on the list
} SharedHomeworksVariables;

/*********************************************************************
 * Function Prototypes
 *********************************************************************/

/* Functions in StudentOrganizer.c */
void *GetObjectPtr(UInt16 objectID);
Boolean AppHandleEvent(EventPtr eventP);
void AppEventLoop(void);
void AppStop(void);
Err InitializeClassesDB(void);
Err InitializeHomeworkDB(void);
void CloseClassesDB();
void CloseHmwrksDB();
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
Boolean ClassesFormDoCommand(UInt16 command, ClassesVariables *pstVars);
Boolean ClassesFormHandleEvent(EventPtr eventP);
void ClassesFormInit(FormType *frmP, ClassesVariables *pstVars);
void ClassesAutoSelectCurrentDay(ClassesVariables *pstVars);
void ClassesActivateSelector(UInt16 field);
void LoadClasses(ClassesVariables *pstVars, Boolean firstRun);
Err LoadSelectedClassIntoMemory(ClassesVariables *pstVars);

/* Functions in ManageClass.c */
Boolean ManageClassFormDoCommand(UInt16 command, ManageClassVariables *pstVars);
Boolean ManageClassFormHandleEvent(EventPtr eventP);
void ManageClassFormInit(FormType *frmP, ManageClassVariables *pstVars);
void AskTimeToUser(UInt16 field, ManageClassVariables *pstVars);
void ToggleTimeSelectorTrigger();
void autoSelectCurrentDay();
void activateSelector(UInt16 field);
void LoadDoW();
void SetTimeSelectorVisibility();
void SetTimeSelectorLabels(UInt16 field, ManageClassVariables *pstVars);
Err SaveClassesChanges(ManageClassVariables *pstVars);
Err SaveClassesChangesToDatabase(ManageClassVariables *pstVars);
void CheckForAlreadySelected(ManageClassVariables *pstVars);
Boolean IsScheduleInvalid(ManageClassVariables *pstVars);
Err DeleteClass(ManageClassVariables *pstVars);
Err DeleteAllHomeworksForClass(UInt16 index);
void setFieldValue(UInt16 objectID, char *str);

/* Functions in ManageHomework.c */
Boolean MngHmwrkFormDoCommand(UInt16 command, ManageHomeworkVariables* hmwrkVars);
void MngHmwrkFormInit(FormType *frmP, ManageHomeworkVariables* hmwrkVars);
Boolean MngHmwrkFormHandleEvent(EventPtr eventP);
Boolean AtLeastOneClassExists();
void FillClassesDropdown();
Boolean MngHmwkHandlePopSelected(Int16 selIndex, ManageHomeworkVariables* hmwrkVars);
void AskDateToUser(ManageHomeworkVariables* hmwrkVars);
void UpdateDueDateTriggerLabel(ManageHomeworkVariables* hmwrkVars);
Err SaveHomeworkChanges(ManageHomeworkVariables* hmwrkVars);
Err ParseHmwrkNameField(ManageHomeworkVariables* hmwrkVars);
Err ValidateDueDate(ManageHomeworkVariables* hmwrkVars);
Err ValidateClass(ManageHomeworkVariables* hmwrkVars);
void ParseComments(ManageHomeworkVariables* hmwrkVars);
Err SaveHomeworkChangesToDatabase(ManageHomeworkVariables* hmwrkVars);
void CheckForSelectedHomework(ManageHomeworkVariables* hmwrkVars);
void redirectToCorrectForm();
void FillHomeworksList();
Err DeleteHomework(ManageHomeworkVariables* hmwrkVars);
Err CompleteHomework(ManageHomeworkVariables* hmwrkVars);
Boolean ClassNameIsUnique(Char* className);

/* Functions in Homeworks.c */
Boolean HomeworksFormDoCommand(UInt16 command);
void HomeworksFormInit(FormType *frmP);
Boolean HomeworksFormHandleEvent(EventPtr eventP);
Err LoadSelectedHomeworkIntoMemory();
UInt16 GetDbIndexForSelected(UInt16 sel);
