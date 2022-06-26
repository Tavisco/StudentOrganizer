/*
 * StudentOrganizer.c
 *
 * main file for Student Organizer
 *
 * This wizard-generated code is based on code adapted from the
 * stationery files distributed as part of the Palm OS SDK 4.0.
 *
 * Copyright (c) 1999-2000 Palm, Inc. or its subsidiaries.
 * All rights reserved.
 */

#include <PalmOS.h>
#include <PalmOSGlue.h>

#include "StudentOrganizer.h"
#include "Rsc/StudentOrganizer_Rsc.h"

/*********************************************************************
 * Entry Points
 *********************************************************************/

/*********************************************************************
 * Global variables
 *********************************************************************/

/*********************************************************************
 * Internal Constants
 *********************************************************************/

/* Define the minimum OS version we support */
#define ourMinVersion sysMakeROMVersion(3, 0, 0, sysROMStageDevelopment, 0)
#define kPalmOS20Version sysMakeROMVersion(2, 0, 0, sysROMStageDevelopment, 0)

/*********************************************************************
 * Internal Functions
 *********************************************************************/

/*
 * FUNCTION: GetObjectPtr
 *
 * DESCRIPTION:
 *
 * This routine returns a pointer to an object in the current form.
 *
 * PARAMETERS:
 *
 * formId
 *     id of the form to display
 *
 * RETURNED:
 *     address of object as a void pointer
 */

void *GetObjectPtr(UInt16 objectID)
{
	FormType *frmP;

	frmP = FrmGetActiveForm();
	return FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, objectID));
}


Int16 GetUserListSelection(UInt16 listID)
{
	ListType *list = GetObjectPtr(listID);
	return LstGetSelection(list);
}


void SetFieldValue(UInt16 objectID, char *newText) {
	FieldType *fldP;
	MemHandle newTextH, oldTextH;
	char *str;
	
	fldP = GetObjectPtr(objectID);
	// Get the current text handle for the field, if any
	oldTextH = FldGetTextHandle(fldP);
	// Have the field stop using that handle
	FldSetTextHandle(fldP, NULL);
	
	// If there is a handle, free it
	if (oldTextH != NULL)
	{
		MemHandleFree(oldTextH);
	}
	
	// Create a new memory chunk
	// the +1 on the length is for
	// the null terminator
	newTextH = MemHandleNew(StrLen(newText) + 1);
	// Allocate it, and lock
	str = MemHandleLock(newTextH);
	
	// Copy our new text to the memory chunk
	StrCopy(str, newText);
	// and unlock it
	MemPtrUnlock(str);
	
	// Have the field use that new handle
	FldSetTextHandle(fldP, newTextH);
}

/*
 * FUNCTION: AppHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine loads form resources and set the event handler for
 * the form loaded.
 *
 * PARAMETERS:
 *
 * event
 *     a pointer to an EventType structure
 *
 * RETURNED:
 *     true if the event was handled and should not be passed
 *     to a higher level handler.
 */

Boolean AppHandleEvent(EventPtr eventP)
{
	UInt16 formId;
	FormPtr frmP;

	if (eventP->eType == frmLoadEvent)
	{
		/* Load the form resource. */
		formId = eventP->data.frmLoad.formID;
		frmP = FrmInitForm(formId);
		FrmSetActiveForm(frmP);

		/*
		 * Set the event handler for the form.  The handler of the
		 * currently active form is called by FrmHandleEvent each
		 * time is receives an event.
		 */
		switch (formId)
		{
		case MainForm:
			FrmSetEventHandler(frmP, MainFormHandleEvent);
			break;
		case ClassesForm:
			FrmSetEventHandler(frmP, ClassesFormHandleEvent);
			break;
		case ManageClassForm:
			FrmSetEventHandler(frmP, ManageClassFormHandleEvent);
			break;
		case ManageHomeworkForm:
			FrmSetEventHandler(frmP, MngHmwrkFormHandleEvent);
			break;
		case HomeworksForm:
			FrmSetEventHandler(frmP, HomeworksFormHandleEvent);
		}
		return true;
	}

	return false;
}

/*
 * FUNCTION: AppEventLoop
 *
 * DESCRIPTION: This routine is the event loop for the application.
 */

void AppEventLoop(void)
{
	UInt16 error;
	EventType event;

	do
	{
		/* change timeout if you need periodic nilEvents */
		EvtGetEvent(&event, evtWaitForever);

		if (!SysHandleEvent(&event))
		{
			if (!MenuHandleEvent(0, &event, &error))
			{
				if (!AppHandleEvent(&event))
				{
					FrmDispatchEvent(&event);
				}
			}
		}
	} while (event.eType != appStopEvent);
}

/*
 * FUNCTION: AppStop
 *
 * DESCRIPTION: Close all forms.
 */

void AppStop(void)
{
	/* Close all the open forms.*/
	FrmCloseAllForms();

	/* Close all the databases.*/
	CloseClassesDB();
	CloseHmwrksDB();
}

void CloseHmwrksDB() {
	UInt32 pstInt;
	DmOpenRef GDB;
	
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	GDB = (DmOpenRef)pstInt;
	DmCloseDatabase(GDB);
}

void CloseClassesDB() {
	UInt32 pstInt;
	DmOpenRef GDB;
	
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	GDB = (DmOpenRef)pstInt;
	DmCloseDatabase(GDB);
}

Err AppStart(void)
{
	Err error;
	
	// Clean all feature memory
	FtrPtrFree(appFileCreator, ftrManageClassNum);
	FtrPtrFree(appFileCreator, ftrClassesDBNum);
	FtrPtrFree(appFileCreator, ftrClassesNum);
	FtrPtrFree(appFileCreator, ftrShrdClassesVarsNum);
	FtrPtrFree(appFileCreator, ftrManageHomeworkNum);
	FtrPtrFree(appFileCreator, ftrHmwrkDBNum);
	FtrPtrFree(appFileCreator, ftrShrdHomeworksVarsNum);
	
	error = InitializeClassesDB();
	if (error != errNone)
	{
		return error;
	}
	
	return InitializeHomeworkDB();
}

Err InitializeClassesDB(void)
{
	Err error = errNone;
	DmOpenRef gDB = 0;

	gDB = DmOpenDatabaseByTypeCreator(kClassDBType, kCreator, dmModeReadWrite);
	if (!gDB)
	{
		error = DmCreateDatabase(0, kClassesDBName, kCreator, kClassDBType, false);
		if (error)
			return error;

		gDB = DmOpenDatabaseByTypeCreator(kClassDBType, kCreator, dmModeReadWrite);
		if (!gDB)
			return DmGetLastErr();
	}

	FtrSet(appFileCreator, ftrClassesDBNum, (UInt32)gDB);
	return error;
}

Err InitializeHomeworkDB(void)
{
	Err error = errNone;
	DmOpenRef gDB = 0;

	gDB = DmOpenDatabaseByTypeCreator(kHmwrkDBType, kCreator, dmModeReadWrite);
	if (!gDB)
	{
		error = DmCreateDatabase(0, kHmwrkDBName, kCreator, kHmwrkDBType, false);
		if (error)
			return error;

		gDB = DmOpenDatabaseByTypeCreator(kHmwrkDBType, kCreator, dmModeReadWrite);
		if (!gDB)
			return DmGetLastErr();
	}

	FtrSet(appFileCreator, ftrHmwrkDBNum, (UInt32)gDB);
	return error;
}

/*
 * FUNCTION: RomVersionCompatible
 *
 * DESCRIPTION:
 *
 * This routine checks that a ROM version is meet your minimum
 * requirement.
 *
 * PARAMETERS:
 *
 * requiredVersion
 *     minimum rom version required
 *     (see sysFtrNumROMVersion in SystemMgr.h for format)
 *
 * launchFlags
 *     flags that indicate if the application UI is initialized
 *     These flags are one of the parameters to your app's PilotMain
 *
 * RETURNED:
 *     error code or zero if ROM version is compatible
 */

Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags)
{
	UInt32 romVersion;

	/* See if we're on in minimum required version of the ROM or later. */
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion < requiredVersion)
	{
		if ((launchFlags &
			 (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
			(sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp))
		{
			FrmAlert(RomIncompatibleAlert);

			/* Palm OS versions before 2.0 will continuously relaunch this
			 * app unless we switch to another safe one. */
			if (romVersion < kPalmOS20Version)
			{
				AppLaunchWithCommand(
					sysFileCDefaultApp,
					sysAppLaunchCmdNormalLaunch, NULL);
			}
		}

		return sysErrRomIncompatible;
	}

	return errNone;
}

/*
 * FUNCTION: PilotMain
 *
 * DESCRIPTION: This is the main entry point for the application.
 *
 * PARAMETERS:
 *
 * cmd
 *     word value specifying the launch code.
 *
 * cmdPB
 *     pointer to a structure that is associated with the launch code
 *
 * launchFlags
 *     word value providing extra information about the launch.
 *
 * RETURNED:
 *     Result of launch, errNone if all went OK
 */

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	Err error;

	error = RomVersionCompatible(ourMinVersion, launchFlags);
	if (error)
		return (error);

	switch (cmd)
	{
	case sysAppLaunchCmdNormalLaunch:
		/*
		 * start application by opening the main form
		 * and then entering the main event loop
		 */
		error = AppStart();
		if (error)
			return error;
		FrmGotoForm(MainForm);
		AppEventLoop();

		AppStop();
		break;
	}

	return errNone;
}

UInt32 __attribute__((section(".vectors"))) __Startup__(void)
{
	SysAppInfoPtr appInfoP;
	void *prevGlobalsP;
	void *globalsP;
	UInt32 ret;

	SysAppStartup(&appInfoP, &prevGlobalsP, &globalsP);
	ret = PilotMain(appInfoP->cmd, appInfoP->cmdPBP, appInfoP->launchFlags);
	SysAppExit(appInfoP, prevGlobalsP, globalsP);

	return ret;
}
