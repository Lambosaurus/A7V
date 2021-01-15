
#include "GPIO.h"
#include "TIM.h"
#include "Buzzer.h"
#include "Timer.h"

/*
 * PRIVATE DEFINITIONS
 */


/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

void Buzzer_StartNote(const Note_t * note);

/*
 * PRIVATE VARIABLES
 */

struct {
	uint16_t count;
	const Note_t * notes;
	uint16_t index;
	Timer_t time;
} gState;

/*
 * PUBLIC FUNCTIONS
 */

void Buzzer_Init(void)
{
	gState.count = 0;
}

void Buzzer_Deinit(void)
{
	if (Buzzer_IsBusy())
	{
		Buzzer_Halt();
	}
}

/*
void Buzzer_Tone(uint32_t freq, uint32_t duration)
{
	TIM_Init(BUZZER_TIM, freq * 10, 10);

	TIM_SetPulse(BUZZER_TIM, BUZZER_TIM_CH, 5);
	TIM_EnablePwm(BUZZER_TIM, BUZZER_TIM_CH, BUZZER_GPIO, BUZZER_PIN, BUZZER_PIN_AF);

	TIM_Start(BUZZER_TIM);
	HAL_Delay(duration);
	TIM_Stop(BUZZER_TIM);

	TIM_Deinit(BUZZER_TIM);
}
*/

void Buzzer_Play(const Note_t * notes, uint16_t count)
{
	if (Buzzer_IsBusy())
	{
		Buzzer_Halt();
	}

	gState.count = count;
	gState.index = 0;
	gState.notes = notes;

	TIM_Init(BUZZER_TIM, 10, 10);
	TIM_EnablePwm(BUZZER_TIM, BUZZER_TIM_CH, BUZZER_GPIO, BUZZER_PIN, BUZZER_PIN_AF);

	Buzzer_StartNote(gState.notes);
}

void Buzzer_Halt(void)
{
	TIM_Stop(BUZZER_TIM);
	TIM_Deinit(BUZZER_TIM);

	gState.count = 0;
}

bool Buzzer_IsBusy(void)
{
	return gState.count > 0;
}

void Buzzer_Update(void)
{
	if (gState.count > 0)
	{
		if (Timer_IsElapsed(&gState.time))
		{
			gState.index += 1;
			if (gState.index < gState.count)
			{
				TIM_Stop(BUZZER_TIM);
				Buzzer_StartNote(gState.notes + gState.index);
			}
			else
			{
				Buzzer_Halt();
				gState.count = 0;
			}
		}
	}
}

/*
 * PRIVATE FUNCTIONS
 */

void Buzzer_StartNote(const Note_t * note)
{
	gState.time.period = note->duration;
	Timer_Reload(&gState.time);
	if (note->freq == 0)
	{
		TIM_SetPulse(BUZZER_TIM, BUZZER_TIM_CH, 0);
	}
	else
	{
		TIM_SetPulse(BUZZER_TIM, BUZZER_TIM_CH, 5);
		TIM_SetFreq(BUZZER_TIM, note->freq * 10);
	}
	TIM_Start(BUZZER_TIM);
}


/*
 * INTERRUPT ROUTINES
 */
