#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

static Char* GetClassNameFromDbIndex(Int16 i)
{
	UInt32 pstInt;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH;
	Char *itemTextP;

	// Get the database pointer from feature memory
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	// Open and lock the correct record on the DB
	recH = DmQueryRecord(gDB, i);
	rec = MemHandleLock(recH);
	itemTextP = rec->className;
	// Unlock record and database
	MemPtrUnlock(rec);

	return itemTextP;
}

static void ClassesListDraw(Int16 itemNum, RectangleType *bounds, Char **unused)
{
	Char* itemTextP = GetClassNameFromDbIndex(itemNum);

	// Draw the className on the list
	WinDrawChars(itemTextP, StrLen(itemTextP), bounds->topLeft.x, bounds->topLeft.y);
}

void MngHmwrkFormInit(FormType *frmP, ManageHomeworkVariables* hmwrkVars)
{
	// Do not open the form if there are no classes in DB
	if (!AtLeastOneClassExists())
	{
		FrmCustomAlert(AddClassBeforeHomeworkAlert, NULL, NULL, NULL);
		FrmGotoForm(MainForm);
	}
	
	FillClassesDropdown();

	CheckForSelectedHomework(hmwrkVars);
}

void CheckForSelectedHomework(ManageHomeworkVariables* hmwrkVars)
{
	UInt32 pstSharedInt, pstDbInt;
	SharedHomeworksVariables *sharedVars;
	DmOpenRef gDB;
	HomeworkDB *rec;
	MemHandle recH;
	Err error;
	FormPtr formP;

	error = FtrGet(appFileCreator, ftrShrdHomeworksVarsNum, &pstSharedInt);
	if (error != errNone)
	{
		// TODO: Properly handle errors
		return;
	}

	sharedVars = (SharedHomeworksVariables *)pstSharedInt;
	
	if (!sharedVars->hasSelectedItem)
	{
		return;
	}

	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstDbInt);
	gDB = (DmOpenRef)pstDbInt;
	recH = DmQueryRecord(gDB, sharedVars->selectedHomeworkDbIndex);
	rec = MemHandleLock(recH);

	hmwrkVars->record = *rec;

	MemHandleUnlock(recH);
	
	// Update Class Name field
	SetFieldValue(NameMngHomeworkField, hmwrkVars->record.hmwrkName);
	
	// Update Comments Room field
	SetFieldValue(CommentsMngHmwrkField, hmwrkVars->record.hmwrkComments);

	// Update class trigger label
	MngHmwkHandlePopSelected(hmwrkVars->record.classIndex, hmwrkVars);

	UpdateDueDateTriggerLabel(hmwrkVars);
	
	// Set complete button usable
	if (hmwrkVars->record.completedDate.year == 0)
	{
		formP = FrmGetActiveForm();
		FrmShowObject(formP, FrmGetObjectIndex(formP, CompleteMngHmwrkButton));	
	}
}

void FillClassesDropdown() {
	UInt32 pstInt;
	DmOpenRef gDB;
	UInt16 numRecs, size;
	ListType *list = GetObjectPtr(ClassesMngHmwrkList);

	// The number of choices is equal to the number os classes
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);
	size = numRecs;

	// Limit list sizenumRecs
	if (size > 4)
		size = 4; 
	LstSetHeight(list, size);
	
	// Set custom list drawing callback function.
	LstSetDrawFunction(list, ClassesListDraw);
	// Set list item number
	LstSetListChoices(list, NULL, numRecs);
}

Boolean AtLeastOneClassExists() {
	UInt32 pstInt;
	UInt16 numRecs;
	DmOpenRef gDB;
	
	// Get database pointer
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	// Get records qty
	numRecs = DmNumRecords(gDB);
	
	return numRecs != 0;
}

Boolean MngHmwkHandlePopSelected(Int16 selIndex, ManageHomeworkVariables* hmwrkVars)
{
	ControlType *popTrig = GetObjectPtr(ClassMngHmwrkTrigger);
	Char *itemTextP = GetClassNameFromDbIndex(selIndex);

	CtlSetLabel(popTrig, itemTextP);
	hmwrkVars->record.classIndex = selIndex;
	hmwrkVars->classSelected = true;

	return true;
}

void AskDateToUser(ManageHomeworkVariables *hmwrkVars)
{
	Int16 day, month, year;
	DateTimeType now;
	Boolean selected;

	TimSecondsToDateTime(TimGetSeconds(), &now);
	day = now.day;
	month = now.month;
	year = now.year;

	selected = SelectDay(selectDayByDay, &month, &day, &year, "Select due date");
	if (selected)
	{
		hmwrkVars->record.dueDay = day;
		hmwrkVars->record.dueMonth = month;
		hmwrkVars->record.dueYear = year;
		
		UpdateDueDateTriggerLabel(hmwrkVars);
	} 
}

Boolean MngHmwrkFormDoCommand(UInt16 command, ManageHomeworkVariables *hmwrkVars)
{
	Boolean handled = false;

	switch (command)
	{
	case DoneMngHmwrkButton:
	{
		if (SaveHomeworkChanges(hmwrkVars) == errNone)
		{
			redirectToCorrectForm();
		}// TODO: Better error management
		handled = true;
		break;
	}
	case CancelMngHmwrkButton:
	{
		redirectToCorrectForm();
		handled = true;
		break;
	}

	case DueMngHmwrkSelector:
	{
		AskDateToUser(hmwrkVars);
		handled = true;
		break;
	}
	case CompleteMngHmwrkButton:
	{
		if (CompleteHomework(hmwrkVars) == errNone)
		{
			redirectToCorrectForm();
		}
		handled = true;
		break;
	}
	case OptionsDeleteHomeworHrmwrksBar:
	{
		if (DeleteHomework(hmwrkVars) == errNone)
		{
			redirectToCorrectForm();
		}
		handled = true;
		break;
	}
	}

	return handled;
}

void redirectToCorrectForm() {
	UInt32 pstSharedInt = 0;
	
	if (FtrGet(appFileCreator, ftrShrdHomeworksVarsNum, &pstSharedInt) == errNone)
	{
		// If it has shared vars, the user must come from
		// the homeworks form
		FrmGotoForm(HomeworksForm);	
	} else {
		// Otherwise, it come from the main form
		FrmGotoForm(MainForm);
	}
}


Err CompleteHomework(ManageHomeworkVariables *hmwrkVars)
{
	Int8 deleteConf;
	DateTimeType now;
	
	// Ask for confirmation before deletion
	deleteConf = FrmCustomAlert(ConfirmActionHomeworkAlert, "complete", NULL, NULL);
	if (deleteConf != 0)
	{
		// TODO: Better error management
		return 1;
	}
	
	TimSecondsToDateTime(TimGetSeconds(), &now);
	hmwrkVars->record.completedDate = now;
	
	return SaveHomeworkChangesToDatabase(hmwrkVars);
}

Err DeleteHomework(ManageHomeworkVariables *hmwrkVars)
{
	Err error = errNone;
	UInt32 pstSharedInt, pstInt;
	SharedHomeworksVariables *pSharedPrefs;
	UInt16 index;
	Int8 deleteConf;
	DmOpenRef gDB;

	// Check if we are editing, and get the index.
	if (FtrGet(appFileCreator, ftrShrdHomeworksVarsNum, &pstSharedInt) != errNone)
	{
		FrmCustomAlert(DeleteNewHomeworkAlert, NULL, NULL, NULL);
		return 1;
	}
	
	pSharedPrefs = (SharedHomeworksVariables *)pstSharedInt;
	index = pSharedPrefs->selectedHomeworkDbIndex;

	// Ask for confirmation before deletion
	deleteConf = FrmCustomAlert(ConfirmActionHomeworkAlert, "delete", NULL, NULL);
	if (deleteConf != 0)
	{
		error = 1;
		return error;
	}

	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	return DmRemoveRecord(gDB, index);
}

Err SaveHomeworkChanges(ManageHomeworkVariables *hmwrkVars)
{
	// Parse and validate Name field
	Err error = errNone;
	error = ParseHmwrkNameField(hmwrkVars);
	if (error != errNone)
	{
		FrmCustomAlert(HmwrkNameNeededAlert, NULL, NULL, NULL);
		return error;
	}
	
	// Validate class
	if (!hmwrkVars->classSelected)
	{
		FrmCustomAlert(HmwrkClassNeededAlert, NULL, NULL, NULL);
		return 1;
	}
	
	// Validate due date
	error = ValidateDueDate(hmwrkVars);
	if (error != errNone)
	{
		FrmCustomAlert(HmwrkDueDateNeededAlert, NULL, NULL, NULL);
		return error;
	}

	// Parse comments field
	ParseComments(hmwrkVars);
	
	//return errNone;
	return SaveHomeworkChangesToDatabase(hmwrkVars);
}

Err SaveHomeworkChangesToDatabase(ManageHomeworkVariables *hmwrkVars)
{
	Err error = errNone;
	UInt16 recIndex = dmMaxRecordIndex;
	MemHandle recH;
	MemPtr recP;
	UInt32 pstInt, pstSharedInt;
	DmOpenRef gDB;
	SharedHomeworksVariables *sharedVars;
	UInt16 newSize;
	Boolean isEditing;

	error = FtrGet(appFileCreator, ftrShrdHomeworksVarsNum, &pstSharedInt);
	
	// Check if we are editing		
	if (error == ftrErrNoSuchFeature)
	{
		isEditing = false;
	}
	else
	{
		sharedVars = (SharedHomeworksVariables *)pstSharedInt;
		isEditing = sharedVars->hasSelectedItem;
	}

	recP = MemPtrNew(sizeof(HomeworkDB));
	MemPtrFree(recP);

	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	if (isEditing)
	{
		// Edit record
		newSize = sizeof(hmwrkVars->record);
		recH = DmResizeRecord(gDB, sharedVars->selectedHomeworkDbIndex, newSize);
		recP = MemHandleLock(recH);
		DmWrite(recP, 0, &(hmwrkVars->record), sizeof(hmwrkVars->record));
		MemHandleUnlock(recH);
	}
	else
	{
		// New record
		recH = DmNewRecord(gDB, &recIndex, sizeof(hmwrkVars->record));
		ErrFatalDisplayIf ((!recH), "Out of memory");

		recP = MemHandleLock(recH);
		DmWrite(recP, 0, &(hmwrkVars->record), sizeof(hmwrkVars->record));
		error = DmReleaseRecord(gDB, recIndex, true);
		MemHandleUnlock(recH);
	}
	return error;
}

void ParseComments(ManageHomeworkVariables *hmwrkVars)
{
	Char *fldCommentsTxt;
	FieldType *fldCommentsP = GetObjectPtr(CommentsMngHmwrkField);
	
	fldCommentsTxt = FldGetTextPtr(fldCommentsP);
	if (fldCommentsTxt != NULL)
	{
		StrCopy(hmwrkVars->record.hmwrkComments, fldCommentsTxt);	
	}
}

Err ValidateDueDate(ManageHomeworkVariables *hmwrkVars)
{
	return hmwrkVars->record.dueDay == NULL;
}

Err ParseHmwrkNameField(ManageHomeworkVariables *hmwrkVars)
{
	Char *fldNameTxt;
	FieldType *fldNameP = GetObjectPtr(NameMngHomeworkField);

	fldNameTxt = FldGetTextPtr(fldNameP);
	if (fldNameTxt == NULL)
	{
		return 1;
	}
	StrCopy(hmwrkVars->record.hmwrkName, fldNameTxt);
	return errNone;
}

void UpdateDueDateTriggerLabel(ManageHomeworkVariables *hmwrkVars) {
	ControlPtr ctl;
	Char *label;

	ctl = GetObjectPtr(DueMngHmwrkSelector);
	label = (Char *)CtlGetLabel (ctl);
	
	DateToDOWDMFormat(hmwrkVars->record.dueMonth, hmwrkVars->record.dueDay, hmwrkVars->record.dueYear, dfDMYWithSlashes, label);
	CtlSetLabel(ctl, label);
}

Boolean MngHmwrkFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormType *frmP;
	ManageHomeworkVariables *hmwrkVarsP;

	switch (eventP->eType)
	{
	case ctlSelectEvent:
	{
		UInt32 ptrInt;
		FtrGet(appFileCreator, ftrManageHomeworkNum, &ptrInt);
		hmwrkVarsP = (ManageHomeworkVariables *)ptrInt;
		return MngHmwrkFormDoCommand(eventP->data.menu.itemID, hmwrkVarsP);
	}
	
	case menuEvent:
	{
		UInt32 ptrInt;
		FtrGet(appFileCreator, ftrManageHomeworkNum, &ptrInt);
		hmwrkVarsP = (ManageHomeworkVariables *)ptrInt;
		return MngHmwrkFormDoCommand(eventP->data.menu.itemID, hmwrkVarsP);
	}

	case popSelectEvent:
	{
		UInt32 ptrInt;
		FtrGet(appFileCreator, ftrManageHomeworkNum, &ptrInt);
		hmwrkVarsP = (ManageHomeworkVariables *)ptrInt;
		return MngHmwkHandlePopSelected(eventP->data.popSelect.selection, hmwrkVarsP);
	}

	case frmOpenEvent:
	{
		frmP = FrmGetActiveForm();

		hmwrkVarsP = (ManageHomeworkVariables *)MemPtrNew(sizeof(ManageHomeworkVariables));
		ErrFatalDisplayIf ((!hmwrkVarsP), "Out of memory");
		MemSet(hmwrkVarsP, sizeof(ManageHomeworkVariables), 0);
		MngHmwrkFormInit(frmP, hmwrkVarsP);
		FtrSet(appFileCreator, ftrManageHomeworkNum, (UInt32)hmwrkVarsP);

		FrmDrawForm(frmP);
		handled = true;
		break;
	}
	case frmCloseEvent:
	{
		// Free ManageHomework variables
		FtrPtrFree(appFileCreator, ftrManageHomeworkNum);
		
		// Free shared variables
		FtrPtrFree(appFileCreator, ftrShrdHomeworksVarsNum);
		break;
	}
	}

	return handled;
}