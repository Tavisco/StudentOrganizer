#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

static Char *GetHomeworkNameFromDbIndex(Int16 i)
{
	UInt32 pstInt;
	DmOpenRef gDB;
	HomeworkDB *rec;
	MemHandle recH;
	Char *itemTextP;

	// Get the database pointer from feature memory
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;

	// Open and lock the correct record on the DB
	recH = DmQueryRecord(gDB, i);
	rec = MemHandleLock(recH);
	itemTextP = rec->hmwrkName;
	// Unlock record and database
	MemPtrUnlock(rec);

	return itemTextP;
}

static void HomeworksListDraw(Int16 itemNum, RectangleType *bounds, Char **unused)
{
	Char* itemTextP = GetHomeworkNameFromDbIndex(itemNum);

	// Draw the className on the list
	WinDrawChars(itemTextP, StrLen(itemTextP), bounds->topLeft.x, bounds->topLeft.y);
}

void FillHomeworksList() {
	UInt32 pstInt;
	DmOpenRef gDB;
	UInt16 numRecs;
	ListType *list = GetObjectPtr(HomeworksViewList);

	// The number of choices is equal to the number os classes
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);
	
	// Set custom list drawing callback function.
	LstSetDrawFunction(list, HomeworksListDraw);
	// Set list item number
	LstSetListChoices(list, NULL, numRecs);
	LstSetSelection(list, -1);
}

Boolean HomeworksFormDoCommand(UInt16 command)
{
	Boolean handled = false;

	switch (command)
	{
	case HomeworksDoneButton:
	{
		FrmGotoForm(MainForm);
		handled = true;
		break;
	}
    case HomeworksNewButton:
	{
		FrmGotoForm(ManageHomeworkForm);
		handled = true;
		break;
	}
	}

	return handled;
}


void HomeworksFormInit(FormType *frmP)
{
	FillHomeworksList();
}


Boolean HomeworksFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormType *frmP;

	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return HomeworksFormDoCommand(eventP->data.menu.itemID);
	
	case menuEvent:
		return HomeworksFormDoCommand(eventP->data.menu.itemID);

	case frmOpenEvent:
		frmP = FrmGetActiveForm();
		HomeworksFormInit(frmP);
		FrmDrawForm(frmP);
		handled = true;
		break;
	}

	return handled;
}