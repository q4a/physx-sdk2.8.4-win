#ifndef __NX_CELL_CONFIG
#define __NX_CELL_CONFIG

#include <cell/spurs.h>

class NxScene;

/** 
 *\brief This class provides methods to parameters specific to the PS3 port
 */
class NxCellConfig
{
public:
	enum NxCellConfigParam {
		NX_CELL_SCENE_PARAM_SPU_NARROWPHASE,
		NX_CELL_SCENE_PARAM_SPU_ISLAND_GEN,
		NX_CELL_SCENE_PARAM_SPU_DYNAMICS,
		NX_CELL_SCENE_PARAM_SPU_CLOTH,
		NX_CELL_SCENE_PARAM_SPU_HEIGHT_FIELD,
		NX_CELL_SCENE_PARAM_SPU_RAYCAST,
		NX_CELL_SCENE_PARAM_SPU_FLUID_PACKETS,
		NX_CELL_SCENE_PARAM_SPU_FLUID_DYNAMICS,
		NX_CELL_SCENE_PARAM_SPU_FLUID_COLLISION,
		NX_CELL_SCENE_PARAM_SPU_FLUID_PARTICLES_UPDATE,
		NX_CELL_SCENE_PARAM_SPU_NARROWPHASE1,
		NX_CELL_SCENE_PARAM_SPU_NARROWPHASE2,
		NX_CELL_SCENE_PARAM_SPU_NARROWPHASE3,
		NX_CELL_SCENE_PARAM_SPU_NARROWPHASE4
	};

	enum NxCellConfigParamReturn {
		NX_CELL_PARAM_OK=0,
		NX_CELL_PARAM_OUT_OF_RANGE,
		NX_CELL_PARAM_UNKNOWN
	};

	/**
	 *\brief Sets the value of a parameter for the given scene
	 * \param[in] scene             A pointer to the NxScene
	 * \param[in] param             The parameter to set
	 * \param[in] value             The value to set the parameter to
	 * \return Return is:
	 *NX_CELL_PARAM_OK on success
	 *NX_CELL_PARAM_OUT_OF_RANGE if the parameter is outside the allowable range
	 *NX_CELL_PARAM_UNKNOWN if the parameter is not recognised
	 */
	static NxCellConfigParamReturn setSceneParamInt(NxScene *scene, NxCellConfigParam param, int value);

	/**
	 *\brief Returns the current value of a parameter for the given scene 
	 * \param[in] scene              A pointer to the NxScene
	 * \param[in] param              The parameter whose value will be returned
	 * \return                       Returns current parameter value
	 */
	static  int getSceneParamInt(NxScene *scene, NxCellConfigParam param);

};


#define NX_CELL_SPURS_DEFAULT_SPU_COUNT 5

enum NxCellSpursReturn {
	NX_CELL_SPURS_OK=0,
	NX_CELL_SPURS_EBUSY,
	NX_CELL_SPURS_EINVAL,
	NX_CELL_SPURS_ENOINIT,
};

/**
 * \brief This class controls the SPU usage of SPURS as used by the PhysX SDK.
 *
 * There are three ways to initialize SPU usage in PhysX.  
 *
 * The first way is to initialize SPURS yourself, and to pass in a pointer to
 * SPURS as well as priorities for PhysX use of the SPUs, using initWithSpurs.
 * This is good if you intend to use SPURS elsewhere in your code, the management
 * can be shared across all processes.
 *
 * The second way is to control the number of SPUs used by SPURS with
 * initWithSpuCount.  PhysX will create its own instance of SPURS when it needs
 * it using that maximum number of SPUs.
 *
 * If you do neither of the two, PhysX will create its own instance of SPURS when
 * it needs it using NX_CELL_SPURS_DEFAULT_SPU_COUNT SPUs.
 *
 * The two explicit methods of initializing SPURS (initWithSpurs/initWithSpuCount)
 * need to be invoked after NxCreatePhysicsSDK is called.
 *
 * terminate() can be used to either detatch from an existing SPURS or to
 * terminate the PhysX-created SPURS.  Note that this will only work if all PhysX
 * scenes have been destroyed, otherwise SPURS would still be needed.
 *
 * Creating a new scene will cause SPURS to re-initialize.
 *
 * isSpursInitialized can be used to query whether PhysX is currently using SPURS.
 *
 **/
class NxCellSpursControl {
public:
	/**
	 * \brief Initializes PhysX SPUs given a pre-configured SPURS.  This call
	 * needs to be made after NxCreatePhysicsSDK but before a Scene is created.
	 * This call will cause the SPU printf handler thread to be created.
	 * \param[in] pSpurs            A pointer to SPURS
	 * \param[in] iSPUCount         The number of SPUs
	 * \param[in] auiPriorities     The priorities for the code to use
	 * \return Return is:
	 * NX_CELL_SPURS_OK on success
	 * NX_CELL_SPURS_EBUSY if SPU usage has already been initialized
	 * NX_CELL_SPURS_EINVAL if the priorities or SPURS pointer is invalid.
	 * NX_CELL_SPURS_ENOINIT if NxCreatePhysicsSDK has not been called.
	 */
	static int initWithSpurs(CellSpurs *pSpurs, int iSPUCount, uint8_t auiPriorities[8]);

	/**
	 * \brief Initializes PhysX SPUs given a pre-configured SPURS.  This call
	 * needs to be made after NxCreatePhysicsSDK but before a Scene is created.
	 * This call allows you to choose whether an SPU printf handler thread should
	 * be created.
	 * \param[in] pSpurs            A pointer to SPURS
	 * \param[in] iSPUCount         The number of SPUs
	 * \param[in] auiPriorities     The priorities for the code to use
	 * \param[in] bAddSPUPrintfSupport  True if PhysX should create an SPU printf handler
	 * \return Return is:
	 * NX_CELL_SPURS_OK on success
	 * NX_CELL_SPURS_EBUSY if SPU usage has already been initialized
	 * NX_CELL_SPURS_EINVAL if the priorities or SPURS pointer is invalid.
	 * NX_CELL_SPURS_ENOINIT if NxCreatePhysicsSDK has not been called.
	 */
	static int initWithSpurs(CellSpurs *pSpurs, int iSPUCount, uint8_t auiPriorities[8], bool bAddSPUPrintfSupport);

	/**
	 * \brief Sets the number of SPUs to be used by a PhysX-initialized SPURS.
	 * This call needs to be made after NxCreatePhysicsSDK but before a Scene
	 * is created.
	 * \param[in] iSPUCount         A valid value is in the range 1-6
	 * \return Return is:
	 * NX_CELL_SPURS_OK on success
	 * NX_CELL_SPURS_EBUSY if SPU usage has already been initialized
	 * NX_CELL_SPURS_EINVAL if iSPUCount is out of range or if SPURS couldn't be
	 *   initialized to that many SPUs.
	 * NX_CELL_SPURS_ENOINIT if NxCreatePhysicsSDK has not been called.
	 */
	static int initWithSpuCount(int iSPUCount=NX_CELL_SPURS_DEFAULT_SPU_COUNT);

	/**
	 * \brief Terminates (or disconnects from) SPURS.
	 * \return Return is:
	 * NX_CELL_SPURS_OK if SPURS terminates ok, or if it was previously terminated/
	 *   never initialized.
	 * NX_CELL_SPURS_EBUSY if there are existing Scenes which would need SPURS.
	 */
	static int terminate(void);

	/**
	 * \brief Queries whether SPU usage has been initialized.
	 * \return True if initialized.
	 */
	static bool isSpursInitialized(void);
};

#endif
