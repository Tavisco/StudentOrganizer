#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

static void ClassesListDraw(Int16 itemNum, RectangleType *bounds, Char **unused) {
	UInt32 pstInt;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH;
	
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef) pstInt;
	
	recH = DmQueryRecord(gDB, itemNum);
	rec = MemHandleLock(recH);
	
	WinDrawChars(rec->className, StrLen(rec->className), bounds->topLeft.x, bounds->topLeft.y);
	
	MemHandleUnlock(recH);
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
Boolean ClassesFormDoCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case ClassesDoneButton:
			FrmGotoForm (MainForm);
			handled = true;
			break;
		case ClassesNewButton:
			FrmGotoForm (ManageClassForm);
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
void ClassesFormInit(FormType *frmP, ClassesVariables* pstVars) {
	ClassesAutoSelectCurrentDay(pstVars);
	LoadClasses(pstVars);
}

void LoadClasses(ClassesVariables* pstVars) {
	MemHandle recH;
	UInt32 pstInt;
	DmOpenRef gDB;
	UInt16 i, numRecs;
	ClassDB *rec;
	FormType *form = FrmGetActiveForm();
	ListType *list = GetObjectPtr(ClassesViewList);
	//int numChoices = 0;
	
	// Set custom list drawing callback function.
	LstSetDrawFunction(list, ClassesListDraw);
	
	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef) pstInt;

	numRecs = DmNumRecords(gDB);
	
	LstSetListChoices(list, NULL, numRecs);
	LstDrawList(list);

	//for (i = 0; i < numRecs; i++) {
	//	recH = DmQueryRecord(gDB, i);
	//	rec = MemHandleLock(recH);
		
	//	MemHandleUnlock(recH);
	//	pst
	//}
}

/*
 * FUNCTION: ClassesAutoSelectCurrentDay
 *
 * DESCRIPTION: This routine selects the  pushbutton relative to current DoW
 *
 * PARAMETERS: No parameters
 *
 */
void ClassesAutoSelectCurrentDay(ClassesVariables* pstVars) {
	DateTimeType now;

	TimSecondsToDateTime(TimGetSeconds(), &now);
	pstVars->selectedDoW = DayOfWeek(now.month, now.day, now.year);
	
	switch (pstVars->selectedDoW) {
		case 0 :
			activateSelector(ClassesSunPushButton);
			break;
		case 1 :
			activateSelector(ClassesMonPushButton);
			break;
		case 2 :
			activateSelector(ClassesTuesPushButton);
			break;
		case 3 :
			activateSelector(ClassesWedPushButton);
			break;
		case 4 :
			activateSelector(ClassesThursPushButton);
			break;
		case 5 :
			activateSelector(ClassesFriPushButton);
			break;
		case 6 :
			activateSelector(ClassesSatPushButton);
			break;
	}
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
void ClassesActivateSelector(UInt16 field) {
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
Boolean ClassesFormHandleEvent(EventPtr eventP) {
	Boolean handled = false;
	FormPtr frmP;

	switch (eventP->eType) 
	{
		case frmOpenEvent: 
		{
			ClassesVariables* pstVars;
			
			frmP = FrmGetActiveForm();
			FrmDrawForm(frmP);

			pstVars = (ClassesVariables*)MemPtrNew(sizeof(ClassesVariables));
			if ((UInt32)pstVars == 0) return -1;
			MemSet(pstVars, sizeof(ClassesVariables), 0);
			FtrSet(appFileCreator, ftrClassesNum, (UInt32)pstVars);
			ClassesFormInit(frmP, pstVars);
			handled = true;
			break;
        }    
			
		case ctlSelectEvent:
		{
			return ClassesFormDoCommand(eventP->data.ctlSelect.controlID);
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