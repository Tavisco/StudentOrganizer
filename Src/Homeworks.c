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
	Char dateStr[dateStringLength];
	
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
				// Draw homework name
				WinDrawTruncChars(rec->hmwrkName, StrLen(rec->hmwrkName), bounds->topLeft.x, bounds->topLeft.y, bounds->topLeft.x + 70);
				
				// Draw due in bold font
				FntSetFont(boldFont);
				WinDrawChars("Due: ", StrLen("Due: "), bounds->topLeft.x + 80, bounds->topLeft.y);
				FntSetFont(stdFont);

				// Draw due date
				DateToAscii(rec->dueMonth, rec->dueDay, rec->dueYear, dfDMYWithSlashes, dateStr);
				WinDrawChars(dateStr, StrLen(dateStr), bounds->topLeft.x + 105, bounds->topLeft.y);
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
	LstDrawList(list);
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
		Err error = errNone;
		error = SetHomeworksSharedMemory();
		ErrFatalDisplayIf ((error != errNone), "Error setting feature memory");
		
		FrmGotoForm(ManageHomeworkForm);
		handled = true;
		break;
	}
	case HomeworksEditButton:
	{
		Err error = errNone;
		error = SetHomeworksSharedMemory();
		ErrFatalDisplayIf ((error != errNone), "Error setting feature memory");
		
		if (userHasSelectedAItem())
		{
			FrmGotoForm(ManageHomeworkForm);
		}
		else
		{
			FrmCustomAlert(SelectClassBeforEditAlert, NULL, NULL, NULL);
			FtrPtrFree(appFileCreator, ftrShrdHomeworksVarsNum);
		}

		handled = true;
		break;
	}
	}

	return handled;
}

Boolean userHasSelectedAItem()
{
	return GetUserListSelection(HomeworksViewList) != noListSelection;
}

Err SetHomeworksSharedMemory()
{
	SharedHomeworksVariables *sharedVars;
	Int16 selectedItem;
	
	selectedItem = GetUserListSelection(HomeworksViewList);

	// Load shared Vars
	sharedVars = (SharedHomeworksVariables *)MemPtrNew(sizeof(SharedHomeworksVariables));
	ErrFatalDisplayIf ((!sharedVars), "Out of memory");
	MemSet(sharedVars, sizeof(SharedHomeworksVariables), 0);
	
	if (selectedItem != noListSelection)
	{
		sharedVars->selectedHomeworkDbIndex = GetDbIndexForSelected(selectedItem);
		sharedVars->hasSelectedItem = true;
	} else {
		sharedVars->hasSelectedItem = false;
	}
	
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

		// If the class has no year set, it must be not-finished
		if (rec->completedDate.year == 0)
		{
			// If the no-year index == our selection index...
			if (iNoYear == sel)
			{
				// ... we have found it!
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
		FrmDrawForm(frmP);
		HomeworksFormInit(frmP);

		handled = true;
		break;
	}

	return handled;
}