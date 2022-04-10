#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "Rsc/StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/*
 * FUNCTION: ManageClassFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     form item id
 */
Boolean ManageClassFormDoCommand(UInt16 command, ClassVariables* pstVars) {
	Boolean handled = false;
	
	switch(command) {
		case ManageClassDoneButton:
			SaveChanges(pstVars);
			FrmGotoForm (ClassesForm);
			handled = true;
			break;
			
		case ManageClassCancelButton:
			FrmGotoForm (ClassesForm);
			handled = true;
			break;
			
		case ManageClassStartSelectorTrigger:
			AskTimeToUser(ManageClassStartSelectorTrigger, pstVars);
			handled = true;
			break;
			
		case ManageClassFinishSelectorTrigger:
			AskTimeToUser(ManageClassFinishSelectorTrigger, pstVars);
			handled = true;
			break;
		
		case ManageClassHasClassCheckbox:
			ToggleTimeSelectorTrigger(pstVars);
			handled = true;
			break;
		
		case ManageClassSunPushButton:
			pstVars->selectedDoW = 0;
			LoadDoW(pstVars);
			handled = true;
			break;
		
		case ManageClassMonPushButton:
			pstVars->selectedDoW = 1;
			LoadDoW(pstVars);
			handled = true;
			break;
		
		case ManageClassTuesPushButton:
			pstVars->selectedDoW = 2;
			LoadDoW(pstVars);
			handled = true;
			break;
			
		case ManageClassWedPushButton:
			pstVars->selectedDoW = 3;
			LoadDoW(pstVars);
			handled = true;
			break;
			
		case ManageClassThursPushButton:
			pstVars->selectedDoW = 4;
			LoadDoW(pstVars);
			handled = true;
			break;
			
		case ManageClassFriPushButton:
			pstVars->selectedDoW = 5;
			LoadDoW(pstVars);
			handled = true;
			break;
			
		case ManageClassSatPushButton:
			pstVars->selectedDoW = 6;
			LoadDoW(pstVars);
			handled = true;
			break;
			
		default:
			break;
	}

	return handled;
}

void SaveChanges(ClassVariables* pstVars) {
	Err error;
	Char *fldNameTxt, *fldRoomTxt = "\n";
	FieldType *fldNameP = GetObjectPtr(ManageClassNameField);
	FieldType *fldRoomP = GetObjectPtr(ManageClassRoomField);

	fldNameTxt = FldGetTextPtr(fldNameP);
	StrCopy(pstVars->record.className, fldNameTxt);

	fldRoomTxt = FldGetTextPtr(fldRoomP);
	StrCopy(pstVars->record.classRoom, fldRoomTxt);

	error = SaveChangesToDatabase(pstVars);
	if (!error) {
		return;
	}
}

Err SaveChangesToDatabase(ClassVariables* pstVars) {
	Err error = errNone;
	UInt16 recIndex = dmMaxRecordIndex;
	MemHandle recH;
	MemPtr recP;
	UInt32 pstInt, pstSharedInt;
	DmOpenRef gDB;
	SharedClassesVariables* pSharedPrefs;
	UInt16 index = -1;
	UInt16 newSize;

	// Check if we are editing, and get the index.
	//if (FtrGet(appFileCreator, ftrShrdClassesVarsNum, &pstSharedInt) == 0) {
	//	pSharedPrefs = (SharedClassesVariables *)pstSharedInt;
	//	index = pSharedPrefs->selectedClassIndex;
	//}

	// TODO: Check if the class name is unique

	recP = MemPtrNew(sizeof(ClassDB));
	MemPtrFree(recP);

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef) pstInt;

	if (index == (UInt16)-1) {
		// New record	
		//recIndex = DmNumRecords(gDB);
		recH = DmNewRecord(gDB, &recIndex, sizeof(pstVars->record));
		if (recH) {
			recP = MemHandleLock(recH);
			DmWrite(recP, 0, &(pstVars->record), sizeof(pstVars->record));
			error = DmReleaseRecord(gDB, recIndex, true);
			MemHandleUnlock(recH);
		}
	} else {
		// Edit record
		newSize = sizeof(pstVars->record);
		recH = DmResizeRecord(gDB, index, newSize);
		recP = MemHandleLock(recH);
		DmWrite(recP, 0, &(pstVars->record), sizeof(pstVars->record));
		error = DmReleaseRecord(gDB, index, true);
		MemHandleUnlock(recH);
	}
	return error;
}

void LoadDoW(ClassVariables* pstVars) {
	SetTimeSelectorLabels(ManageClassStartSelectorTrigger, pstVars);
	SetTimeSelectorLabels(ManageClassFinishSelectorTrigger, pstVars);
	SetTimeSelectorVisibility(pstVars);
}

/*
 * FUNCTION: ToggleTimeSelectorTrigger
 *
 * DESCRIPTION: This routine shows or hides the time selectors and labels
 *   based on the state of the checkbox
 *
 * PARAMETERS: No parameters
 *
 */
void ToggleTimeSelectorTrigger(ClassVariables* pstVars) {
	pstVars->record.classOcurrence[pstVars->selectedDoW].active = !pstVars->record.classOcurrence[pstVars->selectedDoW].active;
	SetTimeSelectorVisibility(pstVars);
}

void SetTimeSelectorVisibility(ClassVariables* pstVars) {
	Boolean status = pstVars->record.classOcurrence[pstVars->selectedDoW].active;
	FormType *formP = FrmGetActiveForm();
	UInt16 startLabelIndex = FrmGetObjectIndex(formP, ManageClassStartLabel);
	UInt16 startSelectorIndex = FrmGetObjectIndex(formP, ManageClassStartSelectorTrigger);
	UInt16 finishLabelIndex = FrmGetObjectIndex(formP, ManageClassFinishLabel);
	UInt16 finishSelectorIndex = FrmGetObjectIndex(formP, ManageClassFinishSelectorTrigger);
	ControlType *chkBoxCtl = GetObjectPtr(ManageClassHasClassCheckbox);
	
	if (status) {
		CtlSetValue(chkBoxCtl, 1);
		FrmShowObject(formP, startLabelIndex);
		FrmShowObject(formP, startSelectorIndex);
		FrmShowObject(formP, finishSelectorIndex);
		FrmShowObject(formP, finishLabelIndex);
		SetTimeSelectorLabels(ManageClassStartSelectorTrigger, pstVars);
		SetTimeSelectorLabels(ManageClassFinishSelectorTrigger,pstVars);	
	} else {
		CtlSetValue(chkBoxCtl, 0);
		FrmHideObject(formP, startLabelIndex);
		FrmHideObject(formP, startSelectorIndex);
		FrmHideObject(formP, finishSelectorIndex);
		FrmHideObject(formP, finishLabelIndex);
	}
}

/*
 * FUNCTION: AskTimeToUser
 *
 * DESCRIPTION: This routine calls a system pop-up that asks the user
 *   to input time, in this case, the class start/finish time.
 *
 *   After a time has been choosen, it then converts it to ASCII and
 *   updates the selector field label to it.
 *
 *
 * PARAMETERS:
 *
 * field
 *     selector item id
 */
void AskTimeToUser(UInt16 field, ClassVariables* pstVars) {
	Boolean ok = false;
	DateTimeType now;
	Int16 hour, minute;
	
	TimSecondsToDateTime(TimGetSeconds(), &now);
	hour = now.hour;
	minute = now.minute;
			
	ok = SelectOneTime(&hour, &minute, "Select time");
	if (ok) {
		if (field == ManageClassStartSelectorTrigger) {
			pstVars->record.classOcurrence[pstVars->selectedDoW].sHour = hour;
			pstVars->record.classOcurrence[pstVars->selectedDoW].sMinute = minute;
			
		} else {
			pstVars->record.classOcurrence[pstVars->selectedDoW].fHour = hour;
			pstVars->record.classOcurrence[pstVars->selectedDoW].fMinute = minute;
		}
		
		pstVars->record.classOcurrence[pstVars->selectedDoW].timeHasBeenSet = true;
	}
	
	SetTimeSelectorLabels(field, pstVars);
}

void SetTimeSelectorLabels(UInt16 field, ClassVariables* pstVars) {
	ControlType *fldP;
	char timeStr[timeStringLength];

	fldP = GetObjectPtr(field);
	
	if (!pstVars->record.classOcurrence[pstVars->selectedDoW].timeHasBeenSet) {
		CtlSetLabel(fldP, "Select time...");
		return;
	}
	
	
	if (field == ManageClassStartSelectorTrigger) {
		TimeToAscii(pstVars->record.classOcurrence[pstVars->selectedDoW].sHour, pstVars->record.classOcurrence[pstVars->selectedDoW].sMinute, tfColon24h, timeStr);
	} else {
		TimeToAscii(pstVars->record.classOcurrence[pstVars->selectedDoW].fHour, pstVars->record.classOcurrence[pstVars->selectedDoW].fMinute, tfColon24h, timeStr);
	}
	
	CtlSetLabel(fldP, timeStr);
}

/*
 * FUNCTION: ManageClassFormInit
 *
 * DESCRIPTION: This routine initializes the ManageClassForm form.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the ManageClass form.
 */
void ManageClassFormInit(FormType *frmP, ClassVariables* pstVars) {
	CheckForAlreadySelected(pstVars);
	autoSelectCurrentDay(pstVars);
	LoadDoW(pstVars);
}


void CheckForAlreadySelected(ClassVariables* pstVars) {
	UInt32 pstSharedInt, pstDbInt;
	SharedClassesVariables* pSharedPrefs;
	DmOpenRef gDB;
	ClassDB *rec;
	MemHandle recH, oldTextH, newTextH;
	FieldType *fldP;
	char *str;
	
	if (FtrGet(appFileCreator, ftrShrdClassesVarsNum, &pstSharedInt) == 0) {
		pSharedPrefs = (SharedClassesVariables *)pstSharedInt;
		
		FtrGet(appFileCreator, ftrClassesDBNum, &pstDbInt);
		gDB = (DmOpenRef) pstDbInt;
		recH = DmQueryRecord(gDB, pSharedPrefs->selectedClassIndex);
		rec = MemHandleLock(recH);
		
		pstVars->record = *rec;

		MemHandleUnlock(recH);

		// Update Class Name field
		// TODO: Extract this to a function
		fldP = GetObjectPtr(ManageClassNameField);
		oldTextH = FldGetTextHandle(fldP);
		newTextH = MemHandleNew(sizeof(pstVars->record.className));
		str = MemHandleLock(newTextH);
		StrCopy(str, pstVars->record.className);
		MemHandleUnlock(newTextH);
		FldSetTextHandle(fldP, newTextH);
		FldDrawField(fldP);
		if (oldTextH != NULL) {
			MemHandleFree(oldTextH);
		}		
		
		// Update Class Room field
		fldP = GetObjectPtr(ManageClassRoomField);
		oldTextH = FldGetTextHandle(fldP);
		newTextH = MemHandleNew(sizeof(pstVars->record.classRoom));
		str = MemHandleLock(newTextH);
		StrCopy(str, pstVars->record.classRoom);
		MemHandleUnlock(newTextH);
		FldSetTextHandle(fldP, newTextH);
		FldDrawField(fldP);
		if (oldTextH != NULL) {
			MemHandleFree(oldTextH);
		}
	}
}

/*
 * FUNCTION: AutoSelectCurrentDay
 *
 * DESCRIPTION: This routine selects the  pushbutton relative to current DoW
 *
 * PARAMETERS: No parameters
 *
 */
void autoSelectCurrentDay(ClassVariables* pstVars) {
	DateTimeType now;
	Int16 dowPushButtons[7] = {ManageClassSunPushButton, ManageClassMonPushButton, ManageClassTuesPushButton, ManageClassWedPushButton, ManageClassThursPushButton, ManageClassFriPushButton, ManageClassSatPushButton};


	TimSecondsToDateTime(TimGetSeconds(), &now);
	pstVars->selectedDoW = DayOfWeek(now.month, now.day, now.year);
	activateSelector(dowPushButtons[pstVars->selectedDoW]);
}


/*
 * FUNCTION: ActivateSelector
 *
 * DESCRIPTION: This routine make the referenced PushButton active.
 *
 * PARAMETERS: 
 * 
 * field
 *     The ID of the selected day of week
 *
 */
void activateSelector(UInt16 field) {
	ControlType *ctl = GetObjectPtr(field);
	CtlSetValue(ctl, 1);
}

/*
 * FUNCTION: ManageClassFormHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine is the event handler for the "ManageClassForm" of this 
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
Boolean ManageClassFormHandleEvent(EventPtr eventP) {
	Boolean handled = false;
	FormPtr frmP;
	ClassVariables* pstVars;

	switch (eventP->eType) {
		case frmOpenEvent: 
		{
			frmP = FrmGetActiveForm();
			FrmDrawForm(frmP);
			
			pstVars = (ClassVariables*)MemPtrNew(sizeof(ClassVariables));
			if ((UInt32)pstVars == 0) return -1;
			MemSet(pstVars, sizeof(ClassVariables), 0);
			FtrSet(appFileCreator, ftrManageClassNum, (UInt32)pstVars);
			ManageClassFormInit(frmP, pstVars);
			handled = true;
			break;
        }
        
        case frmCloseEvent:
        {
			// Free shared variables, if exists
        	void *temp;
        	if (FtrGet(appFileCreator, ftrShrdClassesVarsNum, (UInt32 *)&temp) == 0) {
	        	FtrPtrFree(appFileCreator, ftrShrdClassesVarsNum);
	        }
			
			// Free ManageClass variables
			FtrPtrFree(appFileCreator, ftrManageClassNum);
        	break;
        }    
			
		case ctlSelectEvent:
		{
			UInt32 pstInt;
			ClassVariables* pstVars;
			FtrGet(appFileCreator, ftrManageClassNum, &pstInt);
			pstVars = (ClassVariables *)pstInt;
			return ManageClassFormDoCommand(eventP->data.ctlSelect.controlID, pstVars);
			break;
		}
		
		default:
		{
			break;
		}
	}
    
	return handled;
}