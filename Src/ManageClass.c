#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/*
 * FUNCTION: ManageClassFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     form item id
 */
Boolean ManageClassFormDoCommand(UInt16 command, ManageClassVariables *pstVars)
{
	Boolean handled = false;

	switch (command)
	{
	case ManageClassDoneButton:
		if (SaveClassesChanges(pstVars) == errNone)
		{
			FrmGotoForm(ClassesForm);
		}
		handled = true;
		break;

	case ManageClassCancelButton:
		FrmGotoForm(ClassesForm);
		handled = true;
		break;

	case ManageClassStartSelectorTrigger:
		AskTimeToUser(ManageClassStartSelectorTrigger, pstVars);
		handled = true;
		break;

	case ManageClassFinishSelectorTrigger:
		AskTimeToUser(ManageClassFinishSelectorTrigger, pstVars);
		handled = true;
		break;

	case ManageClassHasClassCheckbox:
		ToggleTimeSelectorTrigger(pstVars);
		handled = true;
		break;

	case ManageClassSunPushButton:
		pstVars->selectedDoW = 0;
		LoadDoW(pstVars);
		handled = true;
		break;

	case ManageClassMonPushButton:
		pstVars->selectedDoW = 1;
		LoadDoW(pstVars);
		handled = true;
		break;

	case ManageClassTuesPushButton:
		pstVars->selectedDoW = 2;
		LoadDoW(pstVars);
		handled = true;
		break;

	case ManageClassWedPushButton:
		pstVars->selectedDoW = 3;
		LoadDoW(pstVars);
		handled = true;
		break;

	case ManageClassThursPushButton:
		pstVars->selectedDoW = 4;
		LoadDoW(pstVars);
		handled = true;
		break;

	case ManageClassFriPushButton:
		pstVars->selectedDoW = 5;
		LoadDoW(pstVars);
		handled = true;
		break;

	case ManageClassSatPushButton:
		pstVars->selectedDoW = 6;
		LoadDoW(pstVars);
		handled = true;
		break;

	case OptionsDeleteClassManageClassBar:
		if (DeleteClass(pstVars) == errNone)
		{
			FrmGotoForm(ClassesForm);
		}
		handled = true;
		break;

	default:
		break;
	}

	return handled;
}

Err DeleteClass(ManageClassVariables *pstVars)
{
	Err error = errNone;
	UInt32 pstSharedInt, pstInt;
	SharedClassesVariables *pSharedPrefs;
	UInt16 deleteConf;
	DmOpenRef gDB;

	// Check if we are editing, and get the index.
	if (FtrGet(appFileCreator, ftrShrdClassesVarsNum, &pstSharedInt) != errNone)
	{
		FrmCustomAlert(SelectClassBeforDeleteAlert, NULL, NULL, NULL);
		return 1;
	}
	pSharedPrefs = (SharedClassesVariables *)pstSharedInt;

	// Ask for confirmation before deletion
	deleteConf = FrmCustomAlert(ConfirmDeleteClassAlert, NULL, NULL, NULL);
	if (deleteConf != 0)
	{
		error = 1;
		return error;
	}

	error = DeleteAllHomeworksForClass(pSharedPrefs->selectedClassDbIndex);
	if (error != errNone) {
		return error;
	}

	pstInt = 0;
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	return DmRemoveRecord(gDB, pSharedPrefs->selectedClassDbIndex);
}

Err DeleteAllHomeworksForClass(UInt16 classIndex) {
	Int16 deleteIndex = -2;
	UInt32 pstInt;
	UInt16 numRecs, i;
	DmOpenRef gDB;
	HomeworkDB *rec;
	MemHandle recH;
	Err error = errNone;

	// Get the database pointer from feature memory
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	while (deleteIndex != -1) {
		numRecs = DmNumRecords(gDB);
		deleteIndex = -2;
		// Find the index we want to delete
		for (i = 0; i < numRecs; i++)
		{
			recH = DmQueryRecord(gDB, i);
			rec = (HomeworkDB *)MemHandleLock(recH);
			if (classIndex == rec->classIndex)
			{
				deleteIndex = i;
			}
			MemHandleUnlock(recH);
		}
		if (deleteIndex >= 0)
		{
			error = DmRemoveRecord(gDB, deleteIndex);
			if (error != errNone) {
				return error;
			}
		} else {
			// If we itareated over every entry on DB, and nothing
			// was found, we set this to -1 to end the while loop.
			deleteIndex = -1;
		}
	}

	return errNone;
}

Err SaveClassesChanges(ManageClassVariables *pstVars)
{
	UInt32 tmp;
	Boolean isEditing;
	Err error = errNone;
	Char *fldNameTxt, *fldRoomTxt = "\n";
	FieldType *fldNameP = GetObjectPtr(ManageClassNameField);
	FieldType *fldRoomP = GetObjectPtr(ManageClassRoomField);

	fldNameTxt = FldGetTextPtr(fldNameP);
	if (fldNameTxt == NULL)
	{
		FrmCustomAlert(ManageClassMissingFieldAlert, "Class name", NULL, NULL);
		error = 1;
		return error;
	}
	StrCopy(pstVars->record.className, fldNameTxt);

	fldRoomTxt = FldGetTextPtr(fldRoomP);
	if (fldRoomTxt == NULL)
	{
		FrmCustomAlert(ManageClassMissingFieldAlert, "Class room", NULL, NULL);
		error = 1;
		return error;
	}
	StrCopy(pstVars->record.classRoom, fldRoomTxt);

	if (IsScheduleInvalid(pstVars))
	{
		FrmCustomAlert(ManageClassOneWeekdayNeededAlert, NULL, NULL, NULL);
		error = 1;
		return error;
	}
	
	isEditing = FtrGet(appFileCreator, ftrShrdClassesVarsNum, &tmp) == errNone;

	if (!isEditing && !ClassNameIsUnique(fldNameTxt))
	{
		FrmCustomAlert(ClassNameMustBeUniqueAlert, NULL, NULL, NULL);
		error = 1;
		return error;
	}

	return SaveClassesChangesToDatabase(pstVars);
}

Boolean ClassNameIsUnique(Char *className)
{
	UInt32 pstInt;
	UInt16 numRecs, i;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH;
	Boolean hasClass = false;

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (StrCompare(className, rec->className) == 0)
		{
			return false;
		}
	}
	return true;
}

Boolean IsScheduleInvalid(ManageClassVariables *pstVars)
{
	Boolean invalid = false;
	Boolean oneActive = false;
	Int16 i;

	for (i = 0; (i < 7) && !invalid; i++)
	{
		if (pstVars->record.classOcurrence[i].active)
		{
			oneActive = true;

			invalid = !pstVars->record.classOcurrence[i].timeHasBeenSet; // TODO: Update this to have set status by start and finish fields
		}
	}

	if (!oneActive)
	{
		invalid = true;
	}

	return invalid;
}

Err SaveClassesChangesToDatabase(ManageClassVariables *pstVars)
{
	Err error = errNone;
	UInt16 recIndex = dmMaxRecordIndex;
	MemHandle recH;
	MemPtr recP;
	UInt32 pstInt, pstSharedInt;
	DmOpenRef gDB;
	SharedClassesVariables *pSharedPrefs;
	UInt16 newSize;
	Boolean isEditing;

	// Check if we are editing, and get the index.
	isEditing = FtrGet(appFileCreator, ftrShrdClassesVarsNum, &pstSharedInt) == errNone;
	if (isEditing)
	{
		pSharedPrefs = (SharedClassesVariables *)pstSharedInt;
	}

	recP = MemPtrNew(sizeof(ClassDB));
	MemPtrFree(recP);

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	if (isEditing)
	{
		// Edit record
		newSize = sizeof(pstVars->record);
		recH = DmResizeRecord(gDB, pSharedPrefs->selectedClassDbIndex, newSize);
		recP = MemHandleLock(recH);
		DmWrite(recP, 0, &(pstVars->record), sizeof(pstVars->record));
		MemHandleUnlock(recH);
	}
	else
	{
		// New record
		recH = DmNewRecord(gDB, &recIndex, sizeof(pstVars->record));
		ErrFatalDisplayIf ((!recH), "Out of memory");
		recP = MemHandleLock(recH);
		DmWrite(recP, 0, &(pstVars->record), sizeof(pstVars->record));
		error = DmReleaseRecord(gDB, recIndex, true);
		MemHandleUnlock(recH);
	}
	return error;
}

void LoadDoW(ManageClassVariables *pstVars)
{
	SetTimeSelectorLabels(ManageClassStartSelectorTrigger, pstVars);
	SetTimeSelectorLabels(ManageClassFinishSelectorTrigger, pstVars);
	SetTimeSelectorVisibility(pstVars);
}

/*
 * FUNCTION: ToggleTimeSelectorTrigger
 *
 * DESCRIPTION: This routine shows or hides the time selectors and labels
 *   based on the state of the checkbox
 *
 * PARAMETERS: No parameters
 *
 */
void ToggleTimeSelectorTrigger(ManageClassVariables *pstVars)
{
	pstVars->record.classOcurrence[pstVars->selectedDoW].active = !pstVars->record.classOcurrence[pstVars->selectedDoW].active;
	SetTimeSelectorVisibility(pstVars);
	SetTimeSelectorLabels(ManageClassStartSelectorTrigger, pstVars);
	SetTimeSelectorLabels(ManageClassFinishSelectorTrigger, pstVars);
}

void SetTimeSelectorVisibility(ManageClassVariables *pstVars)
{
	Boolean status = pstVars->record.classOcurrence[pstVars->selectedDoW].active;
	FormType *formP = FrmGetActiveForm();
	UInt16 startLabelIndex = FrmGetObjectIndex(formP, ManageClassStartLabel);
	UInt16 startSelectorIndex = FrmGetObjectIndex(formP, ManageClassStartSelectorTrigger);
	UInt16 finishLabelIndex = FrmGetObjectIndex(formP, ManageClassFinishLabel);
	UInt16 finishSelectorIndex = FrmGetObjectIndex(formP, ManageClassFinishSelectorTrigger);
	ControlType *chkBoxCtl = GetObjectPtr(ManageClassHasClassCheckbox);

	if (status)
	{
		CtlSetValue(chkBoxCtl, 1);
		FrmShowObject(formP, startLabelIndex);
		FrmShowObject(formP, startSelectorIndex);
		FrmShowObject(formP, finishSelectorIndex);
		FrmShowObject(formP, finishLabelIndex);

	}
	else
	{
		CtlSetValue(chkBoxCtl, 0);
		FrmHideObject(formP, startLabelIndex);
		FrmHideObject(formP, startSelectorIndex);
		FrmHideObject(formP, finishSelectorIndex);
		FrmHideObject(formP, finishLabelIndex);
	}
}

/*
 * FUNCTION: AskTimeToUser
 *
 * DESCRIPTION: This routine calls a system pop-up that asks the user
 *   to input time, in this case, the class start/finish time.
 *
 *   After a time has been choosen, it then converts it to ASCII and
 *   updates the selector field label to it.
 *
 *
 * PARAMETERS:
 *
 * field
 *     selector item id
 */
void AskTimeToUser(UInt16 field, ManageClassVariables *pstVars)
{
	Boolean ok = false;
	DateTimeType now;
	Int16 hour, minute;

	TimSecondsToDateTime(TimGetSeconds(), &now);
	hour = now.hour;
	minute = 0;

	ok = SelectOneTime(&hour, &minute, "Select time");
	if (ok)
	{
		if (field == ManageClassStartSelectorTrigger)
		{
			pstVars->record.classOcurrence[pstVars->selectedDoW].sHour = hour;
			pstVars->record.classOcurrence[pstVars->selectedDoW].sMinute = minute;
		}
		else
		{
			pstVars->record.classOcurrence[pstVars->selectedDoW].fHour = hour;
			pstVars->record.classOcurrence[pstVars->selectedDoW].fMinute = minute;
		}

		pstVars->record.classOcurrence[pstVars->selectedDoW].timeHasBeenSet = true; // TODO: Update this to have set status by start and finish fields
	}

	SetTimeSelectorLabels(field, pstVars);
}

void SetTimeSelectorLabels(UInt16 field, ManageClassVariables *pstVars)
{
	ControlPtr ctl;
	char *label;

	// Get the pointer of our object
	ctl = GetObjectPtr(field);
	// and get the pointer to it's label
	label = (Char *)CtlGetLabel(ctl);
	
	// If the time has not been set
	// (ie. when the form is starting and the class is new)
	// set the label to that string
	if (!pstVars->record.classOcurrence[pstVars->selectedDoW].timeHasBeenSet)
	{
		StrCopy(label, "Select time...");
	}
	else if (field == ManageClassStartSelectorTrigger)
	{
		TimeToAscii(
			pstVars->record.classOcurrence[pstVars->selectedDoW].sHour,
			pstVars->record.classOcurrence[pstVars->selectedDoW].sMinute,
			tfColon24h, label);
	}
	else
	{
		TimeToAscii(
			pstVars->record.classOcurrence[pstVars->selectedDoW].fHour,
			pstVars->record.classOcurrence[pstVars->selectedDoW].fMinute,
			tfColon24h, label);
	}

	CtlSetLabel(ctl, label);
}

/*
 * FUNCTION: ManageClassFormInit
 *
 * DESCRIPTION: This routine initializes the ManageClassForm form.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the ManageClass form.
 */
void ManageClassFormInit(FormType *frmP, ManageClassVariables *pstVars)
{
	Char *startSelectorTriggetLabelStr, *finishSelectorTriggetLabelStr;
	ControlPtr ctl, ctl2;
	
	startSelectorTriggetLabelStr = MemPtrNew(15);
	finishSelectorTriggetLabelStr = MemPtrNew(15);
	
	ctl = GetObjectPtr(ManageClassStartSelectorTrigger);
	CtlSetLabel(ctl, startSelectorTriggetLabelStr);
	
	ctl2 = GetObjectPtr(ManageClassFinishSelectorTrigger);
	CtlSetLabel(ctl2, finishSelectorTriggetLabelStr);
	
	CheckForAlreadySelected(pstVars);
	autoSelectCurrentDay(pstVars);
}

void CheckForAlreadySelected(ManageClassVariables *pstVars)
{
	UInt32 pstSharedInt, pstDbInt;
	SharedClassesVariables *pSharedPrefs;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH;

	if (FtrGet(appFileCreator, ftrShrdClassesVarsNum, &pstSharedInt) == ftrErrNoSuchFeature)
	{
		// There is no class already selected
		// so nothing to do here
		return;
	}
	
	pSharedPrefs = (SharedClassesVariables *)pstSharedInt;

	// Load selected class data
	FtrGet(appFileCreator, ftrClassesDBNum, &pstDbInt);
	gDB = (DmOpenRef)pstDbInt;
	recH = DmQueryRecord(gDB, pSharedPrefs->selectedClassDbIndex);
	rec = MemHandleLock(recH);
	// Point current form data, to database data
	pstVars->record = *rec;
	MemHandleUnlock(recH);
	
	// Update Class Name field
	SetFieldValue(ManageClassNameField, pstVars->record.className);
	
	// Update Class Room field
	SetFieldValue(ManageClassRoomField, pstVars->record.classRoom);
	
	// Use selected DoW from previous form, for better QoL
	pstVars->selectedDoW = pSharedPrefs->selectedDoW;
	
	// Load DoW
	LoadDoW(pstVars);
}

/*
 * FUNCTION: AutoSelectCurrentDay
 *
 * DESCRIPTION: This routine selects the  pushbutton relative to current DoW
 *
 * PARAMETERS: No parameters
 *
 */
void autoSelectCurrentDay(ManageClassVariables *pstVars)
{
	DateTimeType now;
	Int16 dowPushButtons[7] = {ManageClassSunPushButton, ManageClassMonPushButton, ManageClassTuesPushButton, ManageClassWedPushButton, ManageClassThursPushButton, ManageClassFriPushButton, ManageClassSatPushButton};

	if (pstVars->selectedDoW == NULL)
	{
		TimSecondsToDateTime(TimGetSeconds(), &now);
		pstVars->selectedDoW = DayOfWeek(now.month, now.day, now.year);
	}

	activateSelector(dowPushButtons[pstVars->selectedDoW]);
}

/*
 * FUNCTION: ActivateSelector
 *
 * DESCRIPTION: This routine make the referenced PushButton active.
 *
 * PARAMETERS:
 *
 * field
 *     The ID of the selected day of week
 *
 */
void activateSelector(UInt16 field)
{
	ControlType *ctl = GetObjectPtr(field);
	CtlSetValue(ctl, 1);
}

/*
 * FUNCTION: ManageClassFormHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine is the event handler for the "ManageClassForm" of this
 * application.
 *
 * PARAMETERS:
 *
 * eventP
 *     a pointer to an EventType structure
 *
 * RETURNED:
 *     true if the event was handled and should not be passed to
 *     FrmHandleEvent
 */
Boolean ManageClassFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormPtr frmP;
	ManageClassVariables *pstVars;

	switch (eventP->eType)
	{
	case frmOpenEvent:
	{
		frmP = FrmGetActiveForm();

		pstVars = (ManageClassVariables *)MemPtrNew(sizeof(ManageClassVariables));
		ErrFatalDisplayIf (((UInt32)pstVars == 0), "Out of memory");
		MemSet(pstVars, sizeof(ManageClassVariables), 0);
		FtrSet(appFileCreator, ftrManageClassNum, (UInt32)pstVars);
		
		ManageClassFormInit(frmP, pstVars);
		
		FrmDrawForm(frmP);
		handled = true;
		break;
	}

	case frmCloseEvent:
	{
		void *temp;
		UInt32 *labelPtr;
		ControlPtr ctl;
		
		if (FtrGet(appFileCreator, ftrShrdClassesVarsNum, (UInt32 *)&temp) == errNone)
		{
			FtrPtrFree(appFileCreator, ftrShrdClassesVarsNum);
		}

		// Free ManageClass variables
		FtrPtrFree(appFileCreator, ftrManageClassNum);
		
		ctl = GetObjectPtr(ManageClassStartSelectorTrigger);
		labelPtr = (UInt32 *)CtlGetLabel(ctl);
		MemPtrFree(labelPtr);
		
		ctl = GetObjectPtr(ManageClassFinishSelectorTrigger);
		labelPtr = (UInt32 *)CtlGetLabel(ctl);
		MemPtrFree(labelPtr);
		break;
	}

	case ctlSelectEvent:
	{
		UInt32 pstInt;
		ManageClassVariables *pstVars;
		FtrGet(appFileCreator, ftrManageClassNum, &pstInt);
		pstVars = (ManageClassVariables *)pstInt;
		return ManageClassFormDoCommand(eventP->data.ctlSelect.controlID, pstVars);
		break;
	}

	case menuEvent:
	{
		UInt32 pstInt;
		ManageClassVariables *pstVars;
		FtrGet(appFileCreator, ftrManageClassNum, &pstInt);
		pstVars = (ManageClassVariables *)pstInt;
		return ManageClassFormDoCommand(eventP->data.menu.itemID, pstVars);
		break;
	}

	default:
	{
		break;
	}
	}

	return handled;
}