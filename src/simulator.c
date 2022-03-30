#include <stdio.h>

// model specific constants
# define GUID "{d7bac2e2-f3c4-fc14-357b-41ecbe1c538d}"
#ifndef FMI2_FUNCTION_PREFIX
#define FMI2_FUNCTION_PREFIX RisingEdge_
#endif

// no runtime resources
#define RESOURCE_LOCATION "file:///C:/Users/schyan01/git/fmu_risingedge" // absolut path to the unziped fmu
#include "fmi2Functions.h"

// callback functions
static void cb_logMessage(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...) {
	printf("%s\n", message);
}

static void* cb_allocateMemory(size_t nobj, size_t size) {
	return calloc(nobj, size);
}

static void cb_freeMemory(void* obj) {
	free(obj);
}

#define CHECK_STATUS(S) { status = S; if (status != fmi2OK) goto TERMINATE; }

int main(int argc, char *argv[]) {

	fmi2Status status = fmi2OK;

	fmi2CallbackFunctions callbacks = {cb_logMessage, cb_allocateMemory, cb_freeMemory, NULL, NULL};
	
	fmi2Component c = RisingEdge_fmi2Instantiate("instance1", fmi2CoSimulation, GUID, RESOURCE_LOCATION, &callbacks, fmi2False, fmi2False);
	
	if (!c) return 1;

	fmi2Real Time = 0;
	fmi2Real stepSize = 0.5;
	fmi2Real stopTime = 10;

	// Informs the FMU to setup the experiment. Must be called after fmi2Instantiate and befor fmi2EnterInitializationMode
	CHECK_STATUS(RisingEdge_fmi2SetupExperiment(c, fmi2False, 0, Time, fmi2False, 0));
	
	// Informs the FMU to enter Initialization Mode.
	CHECK_STATUS(RisingEdge_fmi2EnterInitializationMode(c));

	fmi2ValueReference u_ref = 0;
	fmi2Boolean u = 0;

	fmi2ValueReference T_ref = 1;
	fmi2Boolean T;
	
	fmi2Real zaehler = 0;

	CHECK_STATUS(RisingEdge_fmi2SetBoolean(c, &u_ref, 1, &u));

	CHECK_STATUS(RisingEdge_fmi2ExitInitializationMode(c));

	printf("time, u, T\n");

	for (int nSteps = 0; nSteps <= 20; nSteps++) {

		Time = nSteps * stepSize;

		// set an input
		CHECK_STATUS(RisingEdge_fmi2SetBoolean(c, &u_ref, 1, &u));

		

		// perform a simulation step
		CHECK_STATUS(RisingEdge_fmi2DoStep(c, Time, stepSize, fmi2True));	//The computation of a time step is started.
		
		// get an output
		CHECK_STATUS(RisingEdge_fmi2GetBoolean(c, &T_ref, 1, &T));
		
		printf("%.2f, %d, %d\n", Time, u, T);

		if (zaehler == 4)
		{
			u = !u;
			zaehler = 0;
		}

		zaehler++;
	}

TERMINATE:

	// clean up
	if (status < fmi2Fatal) {
		RisingEdge_fmi2FreeInstance(c);
	}
	
	return status;
}
