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

void MngHmwrkFormInit(FormType *frmP)
{
	// Do not open the form if there are no classes in DB
	if (!AtLeastOneClassExists())
	{
		FrmCustomAlert(AddClassBeforeHomeworkAlert, NULL, NULL, NULL);
		FrmGotoForm(MainForm);
	}
	
	FillClassesDropdown();
}

void FillClassesDropdown() {
	UInt32 pstInt;
	DmOpenRef gDB;
	UInt16 numRecs;
	ListType *list = GetObjectPtr(ClassesMngHmwrkList);

	// The number of choices is equal to the number os classes
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	// Resize the list vertically to fit all items
	if (numRecs <= 3) {
		LstSetHeight(list, numRecs);
	} else {
		LstSetHeight(list, 4);
	}
	
	// Set custom list drawing callback function.
	LstSetDrawFunction(list, ClassesListDraw);
	// Set list size
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

	return true;
}

void AskDateToUser(ManageHomeworkVariables* hmwrkVars)
{
	Int16 day, month, year;
	DateTimeType now;
	//Boolean selected;

	TimSecondsToDateTime(TimGetSeconds(), &now);
	day = now.day;
	month = now.month;
	year = now.year;

	SelectDay(selectDayByDay, &month, &day, &year, "Select due date");
}

Boolean MngHmwrkFormDoCommand(UInt16 command, ManageHomeworkVariables* hmwrkVars)
{
	Boolean handled = false;

	switch (command)
	{
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
		MngHmwrkFormInit(frmP);
		FrmDrawForm(frmP);

		hmwrkVarsP = (ManageHomeworkVariables *)MemPtrNew(sizeof(ManageHomeworkVariables));
		ErrFatalDisplayIf ((!hmwrkVarsP), "Out of memory");
		MemSet(hmwrkVarsP, sizeof(ManageHomeworkVariables), 0);
		FtrSet(appFileCreator, ftrManageHomeworkNum, (UInt32)hmwrkVarsP);

		handled = true;
		break;
	}
	case frmCloseEvent:
	{
		// Free ManageHomework variables
		FtrPtrFree(appFileCreator, ftrManageHomeworkNum);
		break;
	}
	}

	return handled;
}