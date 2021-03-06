/**********************************************************
 * This file is generated by the 20-sim C++ Code Generator
 *
 *  file:  Submodel1.h
 *  subm:  Submodel1
 *  model: Submodel1
 *  expmt: 
 *  date:  June 24, 2021
 *  time:  4:27:19 PM
 *  user:  20-sim 4.8 Campus License
 *  from:  Universiteit Twente
 *  build: 4.8.3.10415
 *
 **********************************************************/

/* This file describes the model functions
   that are supplied for computation.

   The model itself is the Submodel1.cpp file
*/
#ifndef Submodel1_H
#define Submodel1_H

/* 20-sim include files */
#include "common/xxfuncs.h"
#include "common/xxmatrix.h"
#include "common/xxmodel.h"
#include "common/xxinteg.h"


class Submodel1: virtual public Submodel20sim
{
	public:
		enum stateflags_Submodel1 {initialrun, mainrun, finished};

		/**
		 * Submodel1 constructor
		 */
		Submodel1(void);

		/**
		 * Submodel1 destructor
		 */
		virtual ~Submodel1(void);

		/**
		 * Submodel1 Initialization of the model and calculation of the values for the start time t
		 * @param u	This is the array with all input signals for this submodel
		 * @param y	This is the array with all output signals from this submodel
		 * @param t This is the start time for this run
		 */
		void Initialize (XXDouble *u, XXDouble *y, XXDouble t);

		/**
		 * Submodel1 Calculate
		 * @param u	This is the array with all input signals for this submodel
		 * @param y	This is the array with all output signals from this submodel
		 */
		void Calculate (XXDouble *u, XXDouble *y /*, XXDouble t*/ );

		/**
		 * Submodel1 Terminate
		 * @param u	This is the array with all input signals for this submodel
		 * @param y	This is the array with all output signals from this submodel
		 */
		void Terminate (XXDouble *u, XXDouble *y /*, XXDouble t*/ );


		/**
		 * SetFinishTime()
		 * @brief Overrrides the default finish time (from 20-sim) for this submodel
		 * @param time is the wanted finish time. Use 0.0 to change it to infinite.
		 * 
		 */
		bool SetFinishTime(XXDouble newtime);

		bool IsFinished(void);
		/**
		 * Reset()
		 * @brief Resets the submodel back to its initial state for a new run
		 */
		void Reset(XXDouble starttime);

		stateflags_Submodel1 state;

	protected:
		/**
		 * CalculateDynamic()
		 * This function calculates the dynamic equations of the model.
		 * These equations are called from the integration method
		 * to calculate the new model rates (that are then integrated).
		 */
		void CalculateDynamic (void);

	private:
		/* internal submodel computation methods */

		/**
		 * CalculateInitial()
		 * This function calculates the initial equations of the model.
		 * These equations are calculated before anything else
		 */
		void CalculateInitial (void);

		/**
		 * CalculateStatic()
		 * This function calculates the static equations of the model.
		 * These equations are only dependent from parameters and constants
		 */
		void CalculateStatic (void);

		/**
		 * CalculateInput()
		 * This function calculates the input equations of the model.
		 * These equations are dynamic equations that must not change
		 * in calls from the integration method (like random and delay).
		 */
		void CalculateInput (void);

		/**
		 * CalculateOutput()
		 * This function calculates the output equations of the model.
		 * These equations are not needed for calculation of the rates
		 * and are kept separate to make the dynamic set of equations smaller.
		 * These dynamic equations are called often more than one time for each
		 * integration step that is taken. This makes model computation much faster.
		 */
		void CalculateOutput (void);

		/**
		 * CalculateFinal()
		 * This function calculates the final equations of the model.
		 * These equations are calculated after all the calculations
		 * are performed
		 */
		void CalculateFinal (void);

		/**
		 * CopyInputsToVariables
		 * This private function copies the input variables from the input vector
		 * @param u	This is the array with all input signals for this submodel
		 */
		void CopyInputsToVariables (XXDouble *u);

		/**
		 * CopyVariablesToOutputs
		 * This private function copies the output variables to the output vector
		 * @param y	This is the array with all output signals from this submodel
		 */
		void CopyVariablesToOutputs (XXDouble *y);


		RungeKutta4 myintegmethod;	///< pointer to the integration method for this submodel
};

#endif	/* Submodel1_H */

