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
	Char *	itemTextP;

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
	MemHandle recH, oldTextH, newTextH;
	FieldType *fldP;
	char *str;

	if (FtrGet(appFileCreator, ftrShrdHomeworksVarsNum, &pstSharedInt) == 0)
	{
		sharedVars = (SharedHomeworksVariables *)pstSharedInt;

		FtrGet(appFileCreator, ftrHmwrkDBNum, &pstDbInt);
		gDB = (DmOpenRef)pstDbInt;
		recH = DmQueryRecord(gDB, sharedVars->selectedHomeworkDbIndex);
		rec = MemHandleLock(recH);

		hmwrkVars->record = *rec;

		MemHandleUnlock(recH);
		
		// Update Class Name field
		// TODO: Extract this to a function
		fldP = GetObjectPtr(NameMngHomeworkField);
		oldTextH = FldGetTextHandle(fldP);
		newTextH = MemHandleNew(sizeof(hmwrkVars->record.hmwrkName));
		str = MemHandleLock(newTextH);
		StrCopy(str, hmwrkVars->record.hmwrkName);
		MemHandleUnlock(newTextH);
		FldSetTextHandle(fldP, newTextH);
		FldDrawField(fldP);
		if (oldTextH != NULL)
		{
			MemHandleFree(oldTextH);
		}

		// Update Comments Room field
		fldP = GetObjectPtr(CommentsMngHmwrkField);
		oldTextH = FldGetTextHandle(fldP);
		newTextH = MemHandleNew(sizeof(hmwrkVars->record.hmwrkComments));
		str = MemHandleLock(newTextH);
		StrCopy(str, hmwrkVars->record.hmwrkComments);
		MemHandleUnlock(newTextH);
		FldSetTextHandle(fldP, newTextH);
		FldDrawField(fldP);
		if (oldTextH != NULL)
		{
			MemHandleFree(oldTextH);
		}
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
	Char* itemTextP = GetClassNameFromDbIndex(selIndex);

	CtlSetLabel(popTrig, itemTextP);
	StrCopy(hmwrkVars->record.className, itemTextP);

	return true;
}

void AskDateToUser(ManageHomeworkVariables* hmwrkVars)
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

Boolean MngHmwrkFormDoCommand(UInt16 command, ManageHomeworkVariables* hmwrkVars)
{
	Boolean handled = false;

	switch (command)
	{
	case DoneMngHmwrkButton:
	{
		if (SaveHomeworkChanges(hmwrkVars) == errNone)
		{
			FrmGotoForm(MainForm);
		}
		handled = true;
		break;
	}
	case CancelMngHmwrkButton:
	{
		FrmGotoForm(MainForm);
		handled = true;
		break;
	}

	case DueMngHmwrkSelector:
	{
		AskDateToUser(hmwrkVars);
		handled = true;
		break;
	}
	}

	return handled;
}

Err SaveHomeworkChanges(ManageHomeworkVariables* hmwrkVars)
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
	error = ValidateClass(hmwrkVars);
	if (error != errNone)
	{
		FrmCustomAlert(HmwrkClassNeededAlert, NULL, NULL, NULL);
		return error;
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

Err SaveHomeworkChangesToDatabase(ManageHomeworkVariables* hmwrkVars)
{
	Err error = errNone;
	UInt16 recIndex = dmMaxRecordIndex;
	MemHandle recH;
	MemPtr recP;
	UInt32 pstInt, pstSharedInt;
	DmOpenRef gDB;
	// SharedClassesVariables *pSharedPrefs;
	UInt16 index = -1;
	UInt16 newSize;

	// Check if we are editing, and get the index.
	// if (FtrGet(appFileCreator, ftrShrdClassesVarsNum, &pstSharedInt) == 0)
	// {
	// 	pSharedPrefs = (SharedClassesVariables *)pstSharedInt;
	// 	index = pSharedPrefs->selectedClassDbIndex;
	// }

	recP = MemPtrNew(sizeof(HomeworkDB));
	MemPtrFree(recP);

	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	// if (index == (UInt16)-1)
	// {pstVars
		// New record
		//recIndex = DmNumRecords(gDB);
		recH = DmNewRecord(gDB, &recIndex, sizeof(hmwrkVars->record));
		if (recH)
		{
			recP = MemHandleLock(recH);
			DmWrite(recP, 0, &(hmwrkVars->record), sizeof(hmwrkVars->record));
			error = DmReleaseRecord(gDB, recIndex, true);
			MemHandleUnlock(recH);
		}
	// }
	// else
	// {
	// 	// Edit record
	// 	newSize = sizeof(pstVars->record);
	// 	recH = DmResizeRecord(gDB, index, newSize);
	// 	recP = MemHandleLock(recH);
	// 	DmWrite(recP, 0, &(pstVars->record), sizeof(pstVars->record));
	// 	error = DmReleaseRecord(gDB, index, true);
	// 	MemHandleUnlock(recH);
	// }
	return error;
}

void ParseComments(ManageHomeworkVariables* hmwrkVars)
{
	Char *fldCommentsTxt;
	FieldType *fldCommentsP = GetObjectPtr(CommentsMngHmwrkField);
	
	fldCommentsTxt = FldGetTextPtr(fldCommentsP);
	if (fldCommentsTxt != NULL)
	{
		StrCopy(hmwrkVars->record.hmwrkComments, fldCommentsTxt);	
	}
}

Err ValidateClass(ManageHomeworkVariables* hmwrkVars)
{
	return StrLen(hmwrkVars->record.className) == 0;

}

Err ValidateDueDate(ManageHomeworkVariables* hmwrkVars)
{
	return hmwrkVars->record.dueDay == NULL;
}

Err ParseHmwrkNameField(ManageHomeworkVariables* hmwrkVars)
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

void UpdateDueDateTriggerLabel(ManageHomeworkVariables* hmwrkVars) {
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
		FrmDrawForm(frmP);

		hmwrkVarsP = (ManageHomeworkVariables *)MemPtrNew(sizeof(ManageHomeworkVariables));
		ErrFatalDisplayIf ((!hmwrkVarsP), "Out of memory");
		MemSet(hmwrkVarsP, sizeof(ManageHomeworkVariables), 0);
		MngHmwrkFormInit(frmP, hmwrkVarsP);
		FtrSet(appFileCreator, ftrManageHomeworkNum, (UInt32)hmwrkVarsP);

		handled = true;
		break;
	}
	case frmCloseEvent:
	{
		// Free ManageHomework variables
		FtrPtrFree(appFileCreator, ftrManageHomeworkNum);
		FtrPtrFree(appFileCreator, ftrShrdHomeworksVarsNum);
		break;
	}
	}

	return handled;
}