#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"


static void HomeworksListDraw(Int16 itemNum, RectangleType *bounds, Char **unused)
{
	UInt32 pstInt;
	UInt16 numRecs, i;
	Int16 iNoYear;
	DmOpenRef gDB;
	HomeworkDB *rec;
	MemHandle recH;
	
	// Get the database pointer from feature memory
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	iNoYear = 0;

	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->completedDate.year == 0)
		{
			if (iNoYear == itemNum)
			{
				WinDrawChars(rec->hmwrkName, StrLen(rec->hmwrkName), bounds->topLeft.x, bounds->topLeft.y);
			}
			iNoYear += 1;
		}
	}
}

void FillHomeworksList() {
	UInt32 pstInt;
	DmOpenRef gDB;
	UInt16 numRecs;
	HomeworkDB *rec;
	MemHandle recH;
	ListType *list = GetObjectPtr(HomeworksViewList);
	UInt16 itemCount = 0;
	UInt16 i;

	// The number of choices is equal to the number os classes
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);
	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->completedDate.year == 0)
		{
			// TODO: Save the index on a global to fetch later
			// in order to avoid reescaning the whole DB on
			// every freaking line draw
			itemCount += 1;
		}
	}
	
	// Set custom list drawing callback function.
	LstSetDrawFunction(list, HomeworksListDraw);
	// Set list item number
	LstSetListChoices(list, NULL, itemCount);
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
	case HomeworksEditButton:
	{
		Err error = errNone;
		error = LoadSelectedHomeworkIntoMemory();
		if (error == errNone)
		{
			FrmGotoForm(ManageHomeworkForm);
		}
		else
		{
			FrmCustomAlert(SelectClassBeforEditAlert, NULL, NULL, NULL);
		}

		handled = true;
		break;
	}
	}

	return handled;
}

Err LoadSelectedHomeworkIntoMemory()
{
	SharedHomeworksVariables *sharedVars;
	Int16 selectedItem;
	ListType *list;

	// Load shared Vars
	sharedVars = (SharedHomeworksVariables *)MemPtrNew(sizeof(SharedHomeworksVariables));
	ErrFatalDisplayIf ((!sharedVars), "Out of memory");
	MemSet(sharedVars, sizeof(SharedHomeworksVariables), 0);

	// Get selected item Index
	list = GetObjectPtr(HomeworksViewList);
	selectedItem = LstGetSelection(list);

	if (selectedItem == noListSelection)
	{
		// TODO: properly handle error
		MemPtrFree(sharedVars);
		return 1;
	}

	sharedVars->selectedHomeworkDbIndex = GetDbIndexForSelected(selectedItem);
	return FtrSet(appFileCreator, ftrShrdHomeworksVarsNum, (UInt32)sharedVars);
}

UInt16 GetDbIndexForSelected(UInt16 sel)
{
	UInt32 pstInt;
	UInt16 numRecs, iNoYear, i;
	DmOpenRef gDB;
	HomeworkDB *rec;
	MemHandle recH;
	
	// Get the database pointer from feature memory
	FtrGet(appFileCreator, ftrHmwrkDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	iNoYear = 0;

	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->completedDate.year == 0)
		{
			if (iNoYear == sel)
			{
				// Found it!
				break;
			}
			iNoYear += 1;
		}
	}
	
	return i;
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