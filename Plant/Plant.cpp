/**********************************************************
 * This file is generated by the 20-sim C++ Code Generator
 *
 *  file:  Plant.cpp
 *  subm:  Plant
 *  model: Plant
 *  expmt: JIWY-TheoreticalModel
 *  date:  July 1, 2021
 *  time:  2:56:11 PM
 *  user:  20-sim 4.8 Campus License
 *  from:  Universiteit Twente
 *  build: 4.8.3.10415
 **********************************************************/

/* Standard include files */
#include <stdio.h>
#include <math.h>
/* Include the header for memcpy and memset
 * You may need to change this into <memory.h> for older compilers
 */
#include <string.h>

/* 20-sim include files */
#include "Plant.h"

/* Delta margin used for end time checking */
const XXDouble c_delta = 1.0e-7;

/* this PRIVATE function sets the input variables from the input vector */
void Plant::CopyInputsToVariables(XXDouble *u)
{
	/* copy the input vector to the input variables */
	m_V[22] = u[0]; /* SteeringPan */
	m_V[23] = u[1]; /* SteeringTilt */
}

/* this PRIVATE function uses the output variables to fill the output vector */
void Plant::CopyVariablesToOutputs(XXDouble *y)
{
	/* copy the output variables to the output vector */
	y[0] = m_V[20]; /* AnglePan */
	y[1] = m_V[21]; /* AngleTilt */
}

Plant::Plant(void)
{
	m_number_constants = 0;
	m_number_parameters = 14;
	m_number_initialvalues = 4;
	m_number_variables = 28;
	m_number_states = 4;
	m_number_rates = 4;
	m_number_matrices = 0;
	m_number_unnamed = 0;

	/* the variable arrays */
	m_C = new XXDouble[0 + 1];	/* constants */
	m_P = new XXDouble[14 + 1]; /* parameters */
	m_I = new XXDouble[4 + 1];	/* initial values */
	m_V = new XXDouble[28 + 1]; /* variables */
	m_s = new XXDouble[4 + 1];	/* states */
	m_R = new XXDouble[4 + 1];	/* rates (or new states) */
	m_M = new XXMatrix[0 + 1];	/* matrices */
	m_U = new XXDouble[0 + 1];	/* unnamed */
	m_workarray = new XXDouble[0 + 1];

	Reset(0.0);
	m_finish_time = 300.0;
}

void Plant::Reset(XXDouble starttime)
{
	m_start_time = starttime;
	m_step_size = 0.001;
	m_time = starttime;
	m_major = true;
	m_stop_run = false;

	/* Clear the allocated variable memory */
	memset(m_C, 0, (0 + 1) * sizeof(XXDouble));
	memset(m_P, 0, (14 + 1) * sizeof(XXDouble));
	memset(m_I, 0, (4 + 1) * sizeof(XXDouble));
	memset(m_V, 0, (28 + 1) * sizeof(XXDouble));
	memset(m_s, 0, (4 + 1) * sizeof(XXDouble));
	memset(m_R, 0, (4 + 1) * sizeof(XXDouble));
	memset(m_M, 0, (0 + 1) * sizeof(XXDouble));
	memset(m_U, 0, (0 + 1) * sizeof(XXDouble));
	memset(m_workarray, 0, (0 + 1) * sizeof(XXDouble));

	state = initialrun;
}

bool Plant::IsFinished(void)
{
	return (state == finished);
}

Plant::~Plant(void)
{
	/* free memory */
	delete[] m_C;
	delete[] m_P;
	delete[] m_I;
	delete[] m_V;
	delete[] m_s;
	delete[] m_R;
	delete[] m_M;
	delete[] m_U;
	delete[] m_workarray;
}

/* the initialization function for submodel */
void Plant::Initialize(XXDouble *u, XXDouble *y, XXDouble t)
{
	/* initialization phase (allocating memory) */
	m_initialize = true;
	m_stop_run = false;

	/* set the constants */

	/* set the parameters */
	m_P[0] = 1.0;				  /* Plant\BeltPan\r */
	m_P[1] = 0.25;				  /* Plant\BeltTilt\r */
	m_P[2] = 2.0e-4;			  /* Plant\DcamPan\r */
	m_P[3] = 1.0e-4;			  /* Plant\DcamTilt\r */
	m_P[4] = 1.77e-6;			  /* Plant\DmotPan\r */
	m_P[5] = 4.0e-7;			  /* Plant\DmotTilt\r */
	m_P[6] = 0.05263157894736842; /* Plant\GearPan\r */
	m_P[7] = 0.05263157894736842; /* Plant\GearTilt\r */
	m_P[8] = 3.0e-4;			  /* Plant\JcamPan\i */
	m_P[9] = 4.5e-5;			  /* Plant\JcamTilt\i */
	m_P[10] = 2.06e-7;			  /* Plant\JmotPan\i */
	m_P[11] = 5.28e-8;			  /* Plant\JmotTilt\i */
	m_P[12] = 0.018;			  /* Plant\MotorPan\r */
	m_P[13] = 0.00701;			  /* Plant\MotorTilt\r */

	/* set the initial values */
	m_I[0] = 0.0; /* Plant\JmotPan\state_initial */
	m_I[1] = 0.0; /* Plant\JmotTilt\state_initial */
	m_I[2] = 0.0; /* Plant\ThetaPan\q_initial {rad} */
	m_I[3] = 0.0; /* Plant\ThetaTilt\q_initial {rad} */

	/* set the states */
	m_s[0] = m_I[0]; /* Plant\JmotPan\state */
	m_s[1] = m_I[1]; /* Plant\JmotTilt\state */
	m_s[2] = m_I[2]; /* Plant\ThetaPan\q {rad} */
	m_s[3] = m_I[3]; /* Plant\ThetaTilt\q {rad} */

	/* set the matrices */

	/* (re-)initialize the integration method */
	myintegmethod.Initialize(this);

	/* copy the inputs */
	m_time = t;
	CopyInputsToVariables(u);

	/* calculate initial and static equations */
	CalculateInitial();
	CalculateStatic();
	CalculateInput();
	CalculateDynamic();
	CalculateOutput();

	/* Set the outputs */
	CopyVariablesToOutputs(y);

	/* end of initialization phase */
	m_initialize = false;

	state = mainrun;
}

/* the function that calculates the submodel */
void Plant::Calculate(XXDouble *u, XXDouble *y /*, XXDouble t*/)
{
	switch (state)
	{
	case initialrun: /* calculate the model for the first time */
		Initialize(u, y, 0.0);
		break;
	case mainrun: /* calculate the model */
		if ((m_time <= (m_finish_time - m_step_size + c_delta)) || (m_finish_time == 0.0))
		{
			/* another precessor submodel could determine the parameters of this submodel
				   and therefore the static parameter calculations need to be performed. */
			CalculateStatic();
			CopyInputsToVariables(u);
			CalculateInput();
			myintegmethod.Step();
			CalculateOutput();
			CopyVariablesToOutputs(y);
		}
		else
		{
			state = finished;
		}

		if ((m_stop_run == true) || ((m_finish_time != 0.0) && (m_time + c_delta >= m_finish_time)))
		{
			state = finished;
		}
		break;
	case finished:
		break;
	default:
		break;
	}
}

/* the termination function for submodel */
void Plant::Terminate(XXDouble *u, XXDouble *y /*, XXDouble t */)
{
	/* copy the inputs */
	CopyInputsToVariables(u);

	/* calculate the final model equations */
	CalculateFinal();

	/* set the outputs */
	CopyVariablesToOutputs(y);
}

/* This function calculates the initial equations of the model.
 * These equations are calculated before anything else
 */
void Plant::CalculateInitial(void)
{
}

/* This function calculates the static equations of the model.
 * These equations are only dependent from parameters and constants
 */
void Plant::CalculateStatic(void)
{
}

/* This function calculates the input equations of the model.
 * These equations are dynamic equations that must not change
 * in calls from the integration method (like random and delay).
 */
void Plant::CalculateInput(void)
{
}

/* This function calculates the dynamic equations of the model.
 * These equations are called from the integration method
 * to calculate the new model rates (that are then integrated).
 */
void Plant::CalculateDynamic(void)
{
	/* Plant\JmotPan\p.f = Plant\JmotPan\state / Plant\JmotPan\i; */
	m_V[10] = m_s[0] / m_P[10];

	/* Plant\JmotTilt\p.f = Plant\JmotTilt\state / Plant\JmotTilt\i; */
	m_V[11] = m_s[1] / m_P[11];

	/* AnglePan = Plant\ThetaPan\q; */
	m_V[20] = m_s[2];

	/* AngleTilt = Plant\ThetaTilt\q; */
	m_V[21] = m_s[3];

	/* Plant\SteeringPan = SteeringPan; */
	m_V[18] = m_V[22];

	/* Plant\SteeringTilt = SteeringTilt; */
	m_V[19] = m_V[23];

	/* Plant\MotorPan\p1.e = Plant\MotorPan\r * Plant\JmotPan\p.f; */
	m_V[12] = m_P[12] * m_V[10];

	/* Plant\MotorTilt\p1.e = Plant\MotorTilt\r * Plant\JmotTilt\p.f; */
	m_V[14] = m_P[13] * m_V[11];

	/* Plant\DmotPan\p.e = Plant\DmotPan\r * Plant\JmotPan\p.f; */
	m_V[4] = m_P[4] * m_V[10];

	/* Plant\DmotTilt\p.e = Plant\DmotTilt\r * Plant\JmotTilt\p.f; */
	m_V[5] = m_P[5] * m_V[11];

	/* Plant\GearPan\p2.f = Plant\GearPan\r * Plant\JmotPan\p.f; */
	m_V[7] = m_P[6] * m_V[10];

	/* Plant\GearTilt\p2.f = Plant\GearTilt\r * Plant\JmotTilt\p.f; */
	m_V[9] = m_P[7] * m_V[11];

	/* Plant\MotorPan\p2.e = Plant\MotorPan\r * Plant\SteeringPan; */
	m_V[13] = m_P[12] * m_V[18];

	/* Plant\MotorTilt\p2.e = Plant\MotorTilt\r * Plant\SteeringTilt; */
	m_V[15] = m_P[13] * m_V[19];

	/* Plant\ThetaPan\p1.omega = Plant\BeltPan\r * Plant\GearPan\p2.f; */
	m_R[2] = m_P[0] * m_V[7];

	/* Plant\ThetaTilt\p1.omega = Plant\BeltTilt\r * Plant\GearTilt\p2.f; */
	m_R[3] = m_P[1] * m_V[9];

	/* Plant\DcamPan\p.e = Plant\DcamPan\r * Plant\ThetaPan\p1.omega; */
	m_V[2] = m_P[2] * m_R[2];

	/* Plant\DcamTilt\p.e = Plant\DcamTilt\r * Plant\ThetaTilt\p1.omega; */
	m_V[3] = m_P[3] * m_R[3];

	/* Plant\JcamPan\p.T_in = ((Plant\BeltPan\r * (Plant\GearPan\r * (((Plant\MotorPan\p2.e - Plant\DmotPan\p.e) - Plant\GearPan\r * (Plant\BeltPan\r * Plant\DcamPan\p.e)) / Plant\JmotPan\i))) * Plant\JcamPan\i) / (1.0 + (Plant\BeltPan\r * (Plant\GearPan\r * ((Plant\GearPan\r * Plant\BeltPan\r) / Plant\JmotPan\i))) * Plant\JcamPan\i); */
	m_V[26] = ((m_P[0] * (m_P[6] * (((m_V[13] - m_V[4]) - m_P[6] * (m_P[0] * m_V[2])) / m_P[10]))) * m_P[8]) / (1.0 + (m_P[0] * (m_P[6] * ((m_P[6] * m_P[0]) / m_P[10]))) * m_P[8]);

	/* Plant\JcamTilt\p.T_in = ((Plant\BeltTilt\r * (Plant\GearTilt\r * (((Plant\MotorTilt\p2.e - Plant\DmotTilt\p.e) - Plant\GearTilt\r * (Plant\BeltTilt\r * Plant\DcamTilt\p.e)) / Plant\JmotTilt\i))) * Plant\JcamTilt\i) / (1.0 + (Plant\BeltTilt\r * (Plant\GearTilt\r * ((Plant\GearTilt\r * Plant\BeltTilt\r) / Plant\JmotTilt\i))) * Plant\JcamTilt\i); */
	m_V[27] = ((m_P[1] * (m_P[7] * (((m_V[15] - m_V[5]) - m_P[7] * (m_P[1] * m_V[3])) / m_P[11]))) * m_P[9]) / (1.0 + (m_P[1] * (m_P[7] * ((m_P[7] * m_P[1]) / m_P[11]))) * m_P[9]);

	/* Plant\JcamPan\state = Plant\ThetaPan\p1.omega * Plant\JcamPan\i; */
	m_V[24] = m_R[2] * m_P[8];

	/* Plant\JcamTilt\state = Plant\ThetaTilt\p1.omega * Plant\JcamTilt\i; */
	m_V[25] = m_R[3] * m_P[9];

	/* Plant\OneJunction3\p3.T = Plant\DcamPan\p.e + Plant\JcamPan\p.T_in; */
	m_V[16] = m_V[2] + m_V[26];

	/* Plant\OneJunction4\p3.T = Plant\DcamTilt\p.e + Plant\JcamTilt\p.T_in; */
	m_V[17] = m_V[3] + m_V[27];

	/* Plant\BeltPan\p1.e = Plant\BeltPan\r * Plant\OneJunction3\p3.T; */
	m_V[0] = m_P[0] * m_V[16];

	/* Plant\BeltTilt\p1.e = Plant\BeltTilt\r * Plant\OneJunction4\p3.T; */
	m_V[1] = m_P[1] * m_V[17];

	/* Plant\GearPan\p1.e = Plant\GearPan\r * Plant\BeltPan\p1.e; */
	m_V[6] = m_P[6] * m_V[0];

	/* Plant\GearTilt\p1.e = Plant\GearTilt\r * Plant\BeltTilt\p1.e; */
	m_V[8] = m_P[7] * m_V[1];

	/* Plant\JmotPan\p.e = (Plant\MotorPan\p2.e - Plant\DmotPan\p.e) - Plant\GearPan\p1.e; */
	m_R[0] = (m_V[13] - m_V[4]) - m_V[6];

	/* Plant\JmotTilt\p.e = (Plant\MotorTilt\p2.e - Plant\DmotTilt\p.e) - Plant\GearTilt\p1.e; */
	m_R[1] = (m_V[15] - m_V[5]) - m_V[8];
}

/* This function calculates the output equations of the model.
 * These equations are not needed for calculation of the rates
 * and are kept separate to make the dynamic set of equations smaller.
 * These dynamic equations are called often more than one time for each
 * integration step that is taken. This makes model computation much faster.
 */
void Plant::CalculateOutput(void)
{
}

/* This function calculates the final equations of the model.
 * These equations are calculated after all the calculations
 * are performed
 */
void Plant::CalculateFinal(void)
{
}

bool Plant::SetFinishTime(XXDouble newtime)
{
	if ((newtime <= 0.0) || (newtime > m_time))
	{
		m_finish_time = newtime;
		return true;
	}

	return false;
}