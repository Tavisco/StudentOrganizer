#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"


static void ClassesListDraw(Int16 itemNum, RectangleType *bounds, Char **unused)
{
	if (itemNum == 0) {
		WinDrawChars("1ello", 5, bounds->topLeft.x, bounds->topLeft.y);
	} else if (itemNum == 1) {
		WinDrawChars("2ello", 5, bounds->topLeft.x, bounds->topLeft.y);
	} else if (itemNum == 2) {
		WinDrawChars("3ello", 5, bounds->topLeft.x, bounds->topLeft.y);
	}
	// UInt32 pstInt;
	// DmOpenRef gDB;
	// ClassDB *rec;
	// MemHandle recH;
	// Char *	itemTextP;

	// // Get the database pointer from feature memory
	// FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	// gDB = (DmOpenRef)pstInt;

	// // Open and lock the correct record on the DB
	// recH = DmQueryRecord(gDB, itemNum);
	// rec = MemHandleLock(recH);
	// itemTextP = rec->className;

	// // Draw the className on the list
	// WinDrawChars(itemTextP, StrLen(itemTextP), bounds->topLeft.x, bounds->topLeft.y);

	// // Unlock record and database
	// MemPtrUnlock(rec);
}

Boolean MngHmwrkFormDoCommand(UInt16 command)
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
	}

	return handled;
}


void MngHmwrkFormInit(FormType *frmP)
{
	if (!AtLeastOneClassExists())
	{
		FrmCustomAlert(AddClassBeforeHomeworkAlert, NULL, NULL, NULL);
		FrmGotoForm(MainForm);
	}
	
	FillClassesDropdown();
}

void FillClassesDropdown() {
	//UInt32 pstInt;
	//DmOpenRef gDB;
	//UInt16 numRecs;

	ListType *list = GetObjectPtr(ClassesMngHmwrkList);

	// The number of choices is equal to the number os classes
	//FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	//gDB = (DmOpenRef)pstInt;
	//numRecs = DmNumRecords(gDB);
	LstSetHeight(list, 2);
	LstSetListChoices(list, NULL, 2);
	// Set custom list drawing callback function.
	LstSetDrawFunction(list, ClassesListDraw);
}

Boolean AtLeastOneClassExists() {
	UInt32 pstInt;
	UInt16 numRecs;
	DmOpenRef gDB;
	
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);
	
	return numRecs != 0;
}

Boolean MngHmwrkFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormType *frmP;

	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return MngHmwrkFormDoCommand(eventP->data.menu.itemID);
	
	case menuEvent:
		return MngHmwrkFormDoCommand(eventP->data.menu.itemID);

	case frmOpenEvent:
		frmP = FrmGetActiveForm();
		MngHmwrkFormInit(frmP);
		FrmDrawForm(frmP);
		handled = true;
		break;
	}

	return handled;
}