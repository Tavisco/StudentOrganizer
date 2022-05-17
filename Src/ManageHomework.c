#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"


static void ClassesListDraw(Int16 itemNum, RectangleType *bounds, Char **unused)
{
	UInt32 pstInt;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH;

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	recH = DmQueryRecord(gDB, itemNum);
	rec = MemHandleLock(recH);
	
	WinDrawChars(rec->className, StrLen(rec->className), bounds->topLeft.x, bounds->topLeft.y);
	
	MemHandleUnlock(recH);
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
	UInt32 pstInt;
	DmOpenRef gDB;
	UInt16 numRecs;
	
	FormType *form = FrmGetActiveForm();
	ListType *list = GetObjectPtr(ClassesMngHmwrkList);

	// Set custom list drawing callback function.
	LstSetDrawFunction(list, ClassesListDraw);

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	LstSetListChoices(list, NULL, numRecs);
	LstDrawList(list);
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
		FrmDrawForm(frmP);
		MngHmwrkFormInit(frmP);
		handled = true;
		break;
	}

	return handled;
}