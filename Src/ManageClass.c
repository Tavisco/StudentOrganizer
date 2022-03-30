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
	autoSelectCurrentDay(pstVars);
	LoadDoW(pstVars);
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

	TimSecondsToDateTime(TimGetSeconds(), &now);
	pstVars->selectedDoW = DayOfWeek(now.month, now.day, now.year);
	
	Int16 dowPushButtons[7] = {ManageClassSunPushButton, ManageClassMonPushButton, ManageClassTuesPushButton, ManageClassWedPushButton, ManageClassThursPushButton, ManageClassFriPushButton, ManageClassSatPushButton};
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

	switch (eventP->eType) 
	{
		case frmOpenEvent: 
		{
			frmP = FrmGetActiveForm();
			FrmDrawForm(frmP);
			ClassVariables* pstVars = (ClassVariables*)MemPtrNew(sizeof(ClassVariables));
			if ((UInt32)pstVars == 0) return -1;
			MemSet(pstVars, sizeof(ClassVariables), 0);
			FtrSet(appFileCreator, ftrManageClassNum, (UInt32)pstVars);
			ManageClassFormInit(frmP, pstVars);
			handled = true;
			break;
        }
        
        case frmCloseEvent:
        {
			FtrPtrFree(appFileCreator, ftrManageClassNum);
        	break;
        }    
			
		case ctlSelectEvent:
		{
			UInt32 pstInt;
			FtrGet(appFileCreator, ftrManageClassNum, &pstInt);
			ClassVariables* pstVars = (ClassVariables *)pstInt;
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