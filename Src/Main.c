#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "StudentOrganizer_Rsc.h"
#include "StudentOrganizer.h"

/*
 * FUNCTION: MainFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:
 *
 * command
 *     menu item id
 */

Boolean MainFormDoCommand(UInt16 command)
{
	Boolean handled = false;

	switch (command)
	{
		case OptionsAboutStudentOrganizer:
		{
			FormType * frmP;

			/* Clear the menu status from the display */
			MenuEraseStatus(0);

			/* Display the About Box. */
			frmP = FrmInitForm (AboutForm);
			FrmDoDialog (frmP);                    
			FrmDeleteForm (frmP);

			handled = true;
			break;
		}
		
		case MainManageClassesButton:
		{
			FrmGotoForm (ClassesForm);
			handled = true;
			break;
		}
	}

	return handled;
}


/*
 * FUNCTION: MainFormInit
 *
 * DESCRIPTION: This routine initializes the MainForm form.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the MainForm form.
 */
void MainFormInit(FormType *frmP)
{
	ShowCurrentTime(frmP);
	ShowCurrentWeekday(frmP);
}


/*
 * FUNCTION: ShowCurrentTime
 *
 * DESCRIPTION: Update the label on the main form with the current time.
 *
 * PARAMETERS:
 *
 * frm
 *     pointer to the MainForm form.
 */
 
void ShowCurrentTime(FormType *frmP){
	DateTimeType now;
	char timeStr[timeStringLength];

	TimSecondsToDateTime(TimGetSeconds(), &now);
	TimeToAscii(now.hour, now.minute, tfColon24h, timeStr);

	FrmCopyLabel(frmP, MainTimeLabel, timeStr);
}

void ShowCurrentWeekday(FormType *frmP){
	DateTimeType now;
	Char dowNameStr[dowDateStringLength];
	//UInt16 dow;
	//char weekdayText[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

	TimSecondsToDateTime(TimGetSeconds(), &now);
	//dow = DayOfWeek(now.month, now.day, now.year);
	//DateToDOWDMFormat(now.month, now.day, now.year, dfMYMed, label);
	DateTemplateToAscii("^1l", now.month, now.day, now.year, dowNameStr, sizeof(dowNameStr));
	FrmCopyLabel(frmP, MainWeekdayLabel, dowNameStr);
}
/*
 * FUNCTION: MainFormHandleEvent
 *
 * DESCRIPTION:
 *
 * This routine is the event handler for the "MainForm" of this 
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

Boolean MainFormHandleEvent(EventPtr eventP)
{
	Boolean handled = false;
	FormType * frmP;

	switch (eventP->eType) 
	{
		case menuEvent:
			return MainFormDoCommand(eventP->data.menu.itemID);

		case frmOpenEvent:
			frmP = FrmGetActiveForm();
			FrmDrawForm(frmP);
			MainFormInit(frmP);
			handled = true;
			break;
            
        case frmUpdateEvent:
			/* 
			 * To do any custom drawing here, first call
			 * FrmDrawForm(), then do your drawing, and
			 * then set handled to true. 
			 */
			break;
			
		case ctlSelectEvent:
		{
			return MainFormDoCommand(eventP->data.menu.itemID);
		}
	}
    
	return handled;
}