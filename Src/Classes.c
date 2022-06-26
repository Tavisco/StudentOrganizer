#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

static void ClassesListDraw(Int16 itemNum, RectangleType *bounds, Char **unused)
{
	UInt32 pstInt;
	UInt16 numRecs, i;
	Int16 iDoW;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH;
	ClassesVariables *pstVars;
	Boolean hasClass = false;
	Char timeStrFrom[timeStringLength], timeStrTo[timeStringLength];

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);
	FtrGet(appFileCreator, ftrClassesNum, &pstInt);
	pstVars = (ClassesVariables *)pstInt;

	iDoW = 0;

	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->classOcurrence[pstVars->selectedDoW].active)
		{
			if (iDoW == itemNum)
			{
				// Draw class name
				WinDrawTruncChars(rec->className, StrLen(rec->className), bounds->topLeft.x, bounds->topLeft.y, bounds->topLeft.x + 81);
				
				// Parse class hours
				TimeToAscii(rec->classOcurrence[pstVars->selectedDoW].sHour, rec->classOcurrence[pstVars->selectedDoW].sMinute, tfColon24h, timeStrFrom);
				TimeToAscii(rec->classOcurrence[pstVars->selectedDoW].fHour, rec->classOcurrence[pstVars->selectedDoW].fMinute, tfColon24h, timeStrTo);
				
				// Draw class hours
				WinDrawChars(timeStrFrom, StrLen(timeStrFrom), bounds->topLeft.x + 90, bounds->topLeft.y);
				WinDrawChars("-", 1, bounds->topLeft.x + 114, bounds->topLeft.y);
				WinDrawChars(timeStrTo, StrLen(timeStrTo), bounds->topLeft.x + 120, bounds->topLeft.y);
			}
			iDoW += 1;
		}
	}
}

/*
 * FUNCTION: ClassesFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     menu item id
 */
Boolean ClassesFormDoCommand(UInt16 command, ClassesVariables *pstVars)
{
	Boolean handled = false;
	Err error = errNone;

	switch (command)
	{
	case ClassesDoneButton:
		FrmGotoForm(MainForm);
		handled = true;
		break;
	case ClassesNewButton:
		FrmGotoForm(ManageClassForm);
		handled = true;
		break;
	case ClassesEditButton:
		error = LoadSelectedClassIntoMemory(pstVars);
		if (error == errNone)
		{
			FrmGotoForm(ManageClassForm);
		}
		else
		{
			FrmCustomAlert(SelectClassBeforEditAlert, NULL, NULL, NULL);
		}

		handled = true;
		break;
	case ClassesSunPushButton:
		pstVars->selectedDoW = 0;
		LoadClasses(pstVars, false);
		handled = true;
		break;

	case ClassesMonPushButton:
		pstVars->selectedDoW = 1;
		LoadClasses(pstVars, false);
		handled = true;
		break;

	case ClassesTuesPushButton:
		pstVars->selectedDoW = 2;
		LoadClasses(pstVars, false);
		handled = true;
		break;

	case ClassesWedPushButton:
		pstVars->selectedDoW = 3;
		LoadClasses(pstVars, false);
		handled = true;
		break;

	case ClassesThursPushButton:
		pstVars->selectedDoW = 4;
		LoadClasses(pstVars, false);
		handled = true;
		break;

	case ClassesFriPushButton:
		pstVars->selectedDoW = 5;
		LoadClasses(pstVars, false);
		handled = true;
		break;

	case ClassesSatPushButton:
		pstVars->selectedDoW = 6;
		LoadClasses(pstVars, false);
		handled = true;
		break;

	default:
		break;
	}

	return handled;
}

/*
 * FUNCTION: ClassesFormInit
 *
 * DESCRIPTION: This routine initializes the MainForm form.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the MainForm form.
 */
void ClassesFormInit(FormType *frmP, ClassesVariables *pstVars)
{
	ClassesAutoSelectCurrentDay(pstVars);
	LoadClasses(pstVars, true);
}

void LoadClasses(ClassesVariables *pstVars, Boolean firstRun)
{
	UInt32 pstInt;
	DmOpenRef gDB;
	UInt16 numRecs, i;
	ClassDB *rec;
	MemHandle recH;
	UInt16 itemCount = 0;

	FormType *form = FrmGetActiveForm();
	ListType *list = GetObjectPtr(ClassesViewList);

	// Set custom list drawing callback function.
	LstSetDrawFunction(list, ClassesListDraw);

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->classOcurrence[pstVars->selectedDoW].active)
		{
			// TODO: Save the index on a global to fetch later
			// in order to avoid reescaning the whole DB on
			// every freaking line draw
			itemCount += 1;
		}
	}

	LstSetListChoices(list, NULL, itemCount);
	if (!firstRun) {
		LstDrawList(list);
	}
}

Err LoadSelectedClassIntoMemory(ClassesVariables *pstVars)
{
	SharedClassesVariables *vars;
	Int16 selectedItem, itemNum;
	UInt16 numRecs, i;
	ListType *list;
	UInt32 pstInt;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH;

	// Load shared Vars
	vars = (SharedClassesVariables *)MemPtrNew(sizeof(SharedClassesVariables));
	if ((UInt32)vars == 0)
	{
		MemPtrFree(vars);
		return 1;
	}
	MemSet(vars, sizeof(SharedClassesVariables), 0);

	// Get selected item Index
	list = GetObjectPtr(ClassesViewList);
	selectedItem = LstGetSelection(list);

	if (selectedItem == noListSelection)
	{
		// TODO: properly handle error
		MemPtrFree(vars);
		return 1;
	}

	// Load classes DB and get total amount of recs
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	// Iterate on every one
	itemNum = 0;
	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		// Check if the class has current DoW Active
		if (rec->classOcurrence[pstVars->selectedDoW].active)
		{
			// Check if the current item matches the selection
			if (selectedItem == itemNum)
			{
				// If it does, store the DBIndex to sharedVars
				vars->selectedClassDbIndex = i;
				// and memorize the selected DoW for QoL reasons
				vars->selectedDoW = pstVars->selectedDoW;
				break;
			}
			itemNum += 1;
		}
		MemHandleUnlock(recH);
	}

	return FtrSet(appFileCreator, ftrShrdClassesVarsNum, (UInt32)vars);
}

/*
 * FUNCTION: ClassesAutoSelectCurrentDay
 *
 * DESCRIPTION: This routine selects the  pushbutton relative to current DoW
 *
 * PARAMETERS: No parameters
 *
 */
void ClassesAutoSelectCurrentDay(ClassesVariables *pstVars)
{
	DateTimeType now;
	Int16 dowPushButtons[7] = {ClassesSunPushButton, ClassesMonPushButton, ClassesTuesPushButton, ClassesWedPushButton, ClassesThursPushButton, ClassesFriPushButton, ClassesSatPushButton};

	TimSecondsToDateTime(TimGetSeconds(), &now);
	pstVars->selectedDoW = DayOfWeek(now.month, now.day, now.year);

	activateSelector(dowPushButtons[pstVars->selectedDoW]);
}

/*
 * FUNCTION: ClassesActivateSelector
 *
 * DESCRIPTION: This routine make the referenced PushButton active.
 *
 * PARAMETERS:
 *
 * field
 *     The ID of the selected day of week
 *
 */
void ClassesActivateSelector(UInt16 field)
{
	ControlType *ctl = GetObjectPtr(field);
	CtlSetValue(ctl, 1);
}

/*
 * FUNCTION: ClassesFormHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine is the event handler for the "ClassesForm" of this
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
Boolean ClassesFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormPtr frmP;

	switch (eventP->eType)
	{
	case frmOpenEvent:
	{
		ClassesVariables *pstVars;

		frmP = FrmGetActiveForm();

		pstVars = (ClassesVariables *)MemPtrNew(sizeof(ClassesVariables));
		if ((UInt32)pstVars == 0)
			return -1;
		
		MemSet(pstVars, sizeof(ClassesVariables), 0);
		FtrSet(appFileCreator, ftrClassesNum, (UInt32)pstVars);

		ClassesFormInit(frmP, pstVars);

		FrmDrawForm(frmP);
		handled = true;
		break;
	}

	case ctlSelectEvent:
	{
		UInt32 pstInt;
		ClassesVariables *pstVars;
		FtrGet(appFileCreator, ftrClassesNum, &pstInt);
		pstVars = (ClassesVariables *)pstInt;

		return ClassesFormDoCommand(eventP->data.ctlSelect.controlID, pstVars);
	}

	case frmCloseEvent:
	{
		FtrPtrFree(appFileCreator, ftrClassesNum);
		break;
	}

	default:
	{
		break;
	}
	}

	return handled;
}