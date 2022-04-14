#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS

#include <PalmOS.h>
#include <SelTime.h>
#include <StringMgr.h>
#include "Rsc/StudentOrganizer_Rsc.h"
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
		FormType *frmP;

		/* Clear the menu status from the display */
		MenuEraseStatus(0);

		/* Display the About Box. */
		frmP = FrmInitForm(AboutForm);
		FrmDoDialog(frmP);
		FrmDeleteForm(frmP);

		handled = true;
		break;
	}

	case MainManageClassesButton:
	{
		FrmGotoForm(ClassesForm);
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
	Char *CurrClass;
	
	CurrClass = (Char *)MemPtrNew(sizeof(Char[19]));
	if ((UInt32)CurrClass == 0)
		return;
	MemSet(CurrClass, sizeof(Char[19]), 0);
	
	ShowCurrentTime(frmP);
	ShowCurrentWeekday(frmP);
	SetCurrentClass(frmP, CurrClass);
	SetNextClass(frmP, CurrClass);
	MemPtrFree(CurrClass);
}

/**
 * @brief Set the Current Class label.
 *
 * @param frmP
 */
void SetCurrentClass(FormType *frmP, Char *className)
{
	DateTimeType now, start, finish;
	UInt32 pstInt, nowSec, startSec, finishSec;
	DmOpenRef gDB;
	UInt16 numRecs, i;
	ClassDB *rec;
	MemHandle recH;

	nowSec = TimGetSeconds();
	TimSecondsToDateTime(nowSec, &now);
	start = now;
	finish = now;

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	for (i = 0; i < numRecs; i++)
	{
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->classOcurrence[now.weekDay].active)
		{
			start.hour = rec->classOcurrence[now.weekDay].sHour;
			start.minute = rec->classOcurrence[now.weekDay].sMinute;
			startSec = TimDateTimeToSeconds(&start);

			finish.hour = rec->classOcurrence[now.weekDay].fHour;
			finish.minute = rec->classOcurrence[now.weekDay].fMinute;
			finishSec = TimDateTimeToSeconds(&finish);

			if (nowSec >= startSec && nowSec <= finishSec)
			{
				FrmCopyLabel(frmP, MainCurrentClassLabel, rec->className);
				StrCopy(className, rec->className);
			}
		}
	}
}

/**
 * @brief Set the Next Class label.
 *
 * @param frmP
 */
void SetNextClass(FormType *frmP, Char *currentClass)
{
	DateTimeType now, start, finish;
	UInt32 pstInt, nowSec, startSec, finishSec;
	DmOpenRef gDB;
	UInt16 numRecs, i, x;
	ClassDB *rec;
	MemHandle recH;
	
	nowSec = TimGetSeconds();
	TimSecondsToDateTime(nowSec, &now);
	start = now;
	finish = now;

	FtrGet(appFileCreator, ftrClassesDBNum, &pstInt);
	gDB = (DmOpenRef)pstInt;
	numRecs = DmNumRecords(gDB);

	for (i = 0; i < numRecs; i++)
	{
		nowSec = TimGetSeconds();
		TimSecondsToDateTime(nowSec, &now);
		
		recH = DmQueryRecord(gDB, i);
		rec = MemHandleLock(recH);
		MemHandleUnlock(recH);

		if (rec->classOcurrence[now.weekDay].active)
		{
			start.hour = rec->classOcurrence[now.weekDay].sHour;
			start.minute = rec->classOcurrence[now.weekDay].sMinute;
			startSec = TimDateTimeToSeconds(&start);

			finish.hour = rec->classOcurrence[now.weekDay].fHour;
			finish.minute = rec->classOcurrence[now.weekDay].fMinute;
			finishSec = TimDateTimeToSeconds(&finish);
			
			now.minute = 0;
			nowSec = TimDateTimeToSeconds(&now);
			
			x = now.hour;
			
			// while (now.hour <= 23) may work too
			for (x; x <= 23; x++) {
				if (nowSec >= startSec && nowSec <= finishSec)
				{
					if (StrCompare(rec->className, currentClass) != 0)
					{
						FrmCopyLabel(frmP, MainNextClassLabel, rec->className);
						return;
					}
				}
				nowSec += 3600; // Advance 1 hour
				TimSecondsToDateTime(nowSec, &now);
			}
		}
	}
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

void ShowCurrentTime(FormType *frmP)
{
	DateTimeType now;
	char timeStr[timeStringLength];

	TimSecondsToDateTime(TimGetSeconds(), &now);
	TimeToAscii(now.hour, now.minute, tfColon24h, timeStr);

	FrmCopyLabel(frmP, MainTimeLabel, timeStr);
}

void ShowCurrentWeekday(FormType *frmP)
{
	DateTimeType now;
	Char dowNameStr[dowDateStringLength];

	TimSecondsToDateTime(TimGetSeconds(), &now);
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
	FormType *frmP;

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