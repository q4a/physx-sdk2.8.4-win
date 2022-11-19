#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4995)
#pragma warning(disable:4996)

#define NOMINMAX
#include "dxstdafx.h"
#include "resource.h"
#include "menu.h"
#include "PhysX.h"
#include "SoftBody.h"
#include "NxPhysicsSDK.h"
#include "cparser.h"
#include "filesystem.h"
#include "log.h"
#include "tui.h"
#include "sutil.h"
#include "CreateDynamics.h"
#include "PhysXViewer.h"
#include "MyGlobals.h"
#include "SoftMeshPSK.h"

using namespace SOFTBODY;

extern SoftFileInterface *SOFTBODY::gFileInterface;

CreateDynamicsAPI *getCreateDynamicsDLL(CreateDynamicsInterface *callback)
{
	CreateDynamicsAPI *ret = 0;

	char filename[512];
	wchar_t dname[512];

  #ifdef _DEBUG
  sprintf(filename, "CreateDynamicsDEBUG.dll");
  #else
  sprintf(filename, "CreateDynamics.dll");
  #endif

  const char* pf = SOFTBODY::gFileInterface->getSoftFileName(filename,true);
  strcpy(filename, pf);

  CharToWide(filename,dname,512);
  HMODULE module = LoadLibrary( dname );

  if ( module )
  {
	  void *proc = GetProcAddress(module,"getCreateDynamics");
	  if ( proc )
	  {
		  typedef CreateDynamicsAPI * (__cdecl * DLL_getCreateDynamics)(CreateDynamicsInterface *callback);
   		ret = ((DLL_getCreateDynamics)proc)(callback);
		}
	}

	return ret;
}

void initCallback_LoadSoftMeshPSK()
{
	if(SOFTBODY::gUserLoadSoftMeshPSK) return;

	char filename[512];
	wchar_t dname[512];

  #ifdef _DEBUG
  sprintf(filename, "CreateDynamicsDEBUG.dll");
  #else
  sprintf(filename, "CreateDynamics.dll");
  #endif

  const char* pf = SOFTBODY::gFileInterface->getSoftFileName(filename,true);
  strcpy(filename, pf);

  CharToWide(filename,dname,512);
  HMODULE module = LoadLibrary( dname );

  if ( module )
  {
	  SOFTBODY::gUserLoadSoftMeshPSK = (Callback_LoadSoftMeshPSK) GetProcAddress(module,"loadSoftMeshPSK");
  }
}

enum MenuOptions
{
	MO_IMPORT_OBJ = 10000,
	MO_IMPORT_PSK,
	MO_IMPORT_EZM,
	MO_IMPORT_XML,
	MO_IMPORT_COLLADA,
	MO_IMPORT_NXB,
	MO_EXPORT_OBJ,
	MO_EXPORT_EZM,
	MO_EXPORT_XML,
	MO_EXPORT_SURFACE,
  MO_CORE_DUMP_XML,
  MO_CORE_DUMP_NXB,
  MO_CORE_DUMP_DAE,
  MO_CORE_DUMP_ACTIVE_STATE,
  MO_RUN_SCRIPT,
	MO_EXIT,
//
  MO_DUMMY0,
	MO_STATIC,
	MO_DISABLE_COLLISION,
	MO_SELFCOLLISION,
	MO_VISUALIZATION,
	MO_GRAVITY,
	MO_VOLUME_CONSERVATION,
	MO_DAMPING,
	MO_COLLISION_TWOWAY,
	MO_TEARABLE,
	MO_HARDWARE,
	MO_COMDAMPING,
	MO_FLUID_COLLISION,
	MO_VISUALIZE_NONE,
	MO_VISUALIZE_BODY_AXES,
	MO_VISUALIZE_BODY_MASS_AXES,
	MO_VISUALIZE_BODY_LIN_VELOCITY,
	MO_VISUALIZE_BODY_ANG_VELOCITY,
	MO_VISUALIZE_BODY_JOINT_GROUPS,
	MO_VISUALIZE_JOINT_LOCAL_AXES,
	MO_VISUALIZE_JOINT_WORLD_AXES,
	MO_VISUALIZE_JOINT_LIMITS,
	MO_VISUALIZE_CONTACT_POINT,
	MO_VISUALIZE_CONTACT_NORMAL,
	MO_VISUALIZE_CONTACT_ERROR,
	MO_VISUALIZE_CONTACT_FORCE,
	MO_VISUALIZE_ACTOR_AXES,
	MO_VISUALIZE_COLLISION_AABBS,
	MO_VISUALIZE_COLLISION_SHAPES,
	MO_VISUALIZE_COLLISION_AXES,
	MO_VISUALIZE_COLLISION_COMPOUNDS,
	MO_VISUALIZE_COLLISION_VNORMALS,
	MO_VISUALIZE_COLLISION_FNORMALS,
	MO_VISUALIZE_COLLISION_EDGES,
	MO_VISUALIZE_COLLISION_SPHERES,
#if NX_SDK_VERSION_NUMBER < 280
	MO_VISUALIZE_COLLISION_SAP,
#endif
	MO_VISUALIZE_COLLISION_STATIC,
	MO_VISUALIZE_COLLISION_DYNAMIC,
	MO_VISUALIZE_COLLISION_FREE,
	MO_VISUALIZE_COLLISION_CCD,
	MO_VISUALIZE_COLLISION_SKELETONS,
	MO_VISUALIZE_FLUID_EMITTERS,
	MO_VISUALIZE_FLUID_POSITION,
	MO_VISUALIZE_FLUID_VELOCITY,
	MO_VISUALIZE_FLUID_KERNEL_RADIUS,
	MO_VISUALIZE_FLUID_BOUNDS,
	MO_VISUALIZE_FLUID_PACKETS,
	MO_VISUALIZE_FLUID_MOTION_LIMIT,
	MO_VISUALIZE_FLUID_DYN_COLLISION,
#if NX_SDK_VERSION_NUMBER >= 270
	MO_VISUALIZE_FLUID_STC_COLLISION,
	MO_VISUALIZE_FLUID_MESH_PACKETS,
#endif
	MO_VISUALIZE_FLUID_DRAINS,
#if NX_SDK_VERSION_NUMBER >= 270
	MO_VISUALIZE_CLOTH_MESH,
#endif
	MO_VISUALIZE_CLOTH_COLLISIONS,
	MO_VISUALIZE_CLOTH_SELFCOLLISIONS,
	MO_VISUALIZE_CLOTH_WORKPACKETS,
#if NX_SDK_VERSION_NUMBER >= 270
	MO_VISUALIZE_CLOTH_SLEEP,
	MO_VISUALIZE_CLOTH_TEARABLE_VERTICES,
	MO_VISUALIZE_CLOTH_TEARING,
	MO_VISUALIZE_CLOTH_ATTACHMENT,
#endif
#if NX_USE_SOFTBODY_API
	MO_VISUALIZE_SOFTBODY_MESH,
	MO_VISUALIZE_SOFTBODY_COLLISIONS,
	MO_VISUALIZE_SOFTBODY_WORKPACKETS,
	MO_VISUALIZE_SOFTBODY_SLEEP,
	MO_VISUALIZE_SOFTBODY_TEARABLE_VERTICES,
	MO_VISUALIZE_SOFTBODY_TEARING,
	MO_VISUALIZE_SOFTBODY_ATTACHMENT,
#endif
	MO_VISUALIZE_LAST,
	MO_VISUALIZE_COUNT = (MO_VISUALIZE_LAST - MO_VISUALIZE_BODY_AXES),


};

enum MyCommand
{
	MC_CREATE_DYNAMICS = SBC_LAST,
	MC_PSSCRIPT,
	MC_PSLOOKAT,
	MC_GO_CREATE_DYNAMICS,
	MC_GRAVITY,
	MC_ASYNC,
	MC_TIME_SCALE,
};

class MyMenu : public FileSystem, public CommandParserInterface, public CreateDynamicsInterface
{
public:
  MyMenu(void)
  {
  	mMainMenu = CreateMenu();

  	HMENU m = CreatePopupMenu();
		mFileMenu = m;

  	AppendMenu( mMainMenu, MF_POPUP, (UINT_PTR)m, L"&File" );
  	  AppendMenu( m, MF_STRING, MO_IMPORT_OBJ, L"Import Graphics Wavefront OBJ");
  	  AppendMenu( m, MF_STRING, MO_IMPORT_PSK, L"Import Graphics Unreal PSK");
	  AppendMenu( m, MF_STRING, MO_IMPORT_EZM, L"Import Graphics EZ-Mesh");
  	  AppendMenu( m, MF_STRING, MO_IMPORT_XML, L"Import NxuStream XML");
  	  AppendMenu( m, MF_STRING, MO_IMPORT_COLLADA, L"Import COLLADA DAE");
  	  AppendMenu( m, MF_STRING, MO_IMPORT_NXB, L"Import NxuStream NXB");
  	  AppendMenu( m, MF_STRING, MO_EXPORT_OBJ, L"Export as Wavefront OBJ and TET");
  	  AppendMenu( m, MF_STRING, MO_EXPORT_EZM, L"Export as EZ-Mesh");
  	  AppendMenu( m, MF_STRING, MO_EXPORT_XML, L"Export as NxuStream");
  	  AppendMenu( m, MF_STRING, MO_EXPORT_SURFACE, L"Export Surface Mesh as OBJ");
  	  AppendMenu( m, MF_STRING, MO_CORE_DUMP_XML,  L"CoreDump XML");
  	  AppendMenu( m, MF_STRING, MO_CORE_DUMP_NXB,  L"CoreDump NXB");
  	  AppendMenu( m, MF_STRING, MO_CORE_DUMP_DAE,  L"CoreDump DAE (COLLADA)");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_CORE_DUMP_ACTIVE_STATE,  L"CoreDump ActiveState");
  	  AppendMenu( m, MF_STRING, MO_RUN_SCRIPT,  L"Run Demo Script");
  	  AppendMenu( m, MF_STRING, MO_EXIT, L"E&xit");

#if NX_USE_SOFTBODY_API
  	m = CreatePopupMenu();
  	mSoftbodyFlags = m;

  	AppendMenu( mMainMenu, MF_POPUP, (UINT_PTR)m, L"&SoftBody Flags" );
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_STATIC, L"SBF_STATIC");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_DISABLE_COLLISION, L"SBF_DISABLE_COLLISION");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_SELFCOLLISION, L"SBF_SELFCOLLISION");
  	  AppendMenu( m, MF_STRING | MF_CHECKED,   MO_VISUALIZATION,  L"SBF_VISUALIZATION");
  	  AppendMenu( m, MF_STRING | MF_CHECKED,   MO_GRAVITY,  L"SBF_GRAVITY");
  	  AppendMenu( m, MF_STRING | MF_CHECKED,   MO_VOLUME_CONSERVATION,  L"SBF_VOLUME_CONSERVATION");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_DAMPING,  L"SBF_DAMPING");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_COLLISION_TWOWAY,  L"SBF_COLLISION_TWOWAY");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_TEARABLE,  L"SBF_TEARABLE");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_HARDWARE,  L"SBF_HARDWARE");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_COMDAMPING,  L"SBF_COMDAMPING");
  	  AppendMenu( m, MF_STRING | MF_UNCHECKED, MO_FLUID_COLLISION,  L"SBF_FLUID_COLLISION");
#endif

#if NX_USE_SOFTBODY_API
		mFlags = NX_SBF_GRAVITY | NX_SBF_VOLUME_CONSERVATION;
#else
		mFlags = 0;
#endif

  	m = CreatePopupMenu();
  	mVisualization = m;

  	AppendMenu( mMainMenu, MF_POPUP, (UINT_PTR)m, L"&Debug Visualization Flags" );


			AppendMenu(m,MF_STRING, MO_VISUALIZE_NONE,L"VISUALIZE NONE");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_BODY_AXES,L"BODY_AXES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_BODY_MASS_AXES,L"BODY_MASS_AXES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_BODY_LIN_VELOCITY,L"BODY_LIN_VELOCITY");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_BODY_ANG_VELOCITY,L"BODY_ANG_VELOCITY");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_BODY_JOINT_GROUPS,L"BODY_JOINT_GROUPS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_JOINT_LOCAL_AXES,L"JOINT_LOCAL_AXES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_JOINT_WORLD_AXES,L"JOINT_WORLD_AXES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_JOINT_LIMITS,L"JOINT_LIMITS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CONTACT_POINT,L"CONTACT_POINT");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CONTACT_NORMAL,L"CONTACT_NORMAL");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CONTACT_ERROR,L"CONTACT_ERROR");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CONTACT_FORCE,L"CONTACT_FORCE");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_ACTOR_AXES,L"ACTOR_AXES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_AABBS,L"COLLISION_AABBS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_SHAPES,L"COLLISION_SHAPES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_AXES,L"COLLISION_AXES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_COMPOUNDS,L"COLLISION_COMPOUNDS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_VNORMALS,L"COLLISION_VNORMALS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_FNORMALS,L"COLLISION_FNORMALS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_EDGES,L"COLLISION_EDGES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_SPHERES,L"COLLISION_SPHERES");
#if NX_SDK_VERSION_NUMBER < 280
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_SAP,L"COLLISION_SAP");
#endif
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_STATIC,L"COLLISION_STATIC");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_DYNAMIC,L"COLLISION_DYNAMIC");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_FREE,L"COLLISION_FREE");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_CCD,L"COLLISION_CCD");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_COLLISION_SKELETONS,L"COLLISION_SKELETONS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_EMITTERS,L"FLUID_EMITTERS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_POSITION,L"FLUID_POSITION");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_VELOCITY,L"FLUID_VELOCITY");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_KERNEL_RADIUS,L"FLUID_KERNEL_RADIUS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_BOUNDS,L"FLUID_BOUNDS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_PACKETS,L"FLUID_PACKETS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_MOTION_LIMIT,L"FLUID_MOTION_LIMIT");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_DYN_COLLISION,L"FLUID_DYN_COLLISION");
#if NX_SDK_VERSION_NUMBER >= 270
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_STC_COLLISION,L"FLUID_STC_COLLISION");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_MESH_PACKETS,L"FLUID_MESH_PACKETS");
#endif
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_FLUID_DRAINS,L"FLUID_DRAINS");
#if NX_SDK_VERSION_NUMBER >= 270
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_MESH,L"CLOTH_MESH");
#endif
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_COLLISIONS,L"CLOTH_COLLISIONS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_SELFCOLLISIONS,L"CLOTH_SELFCOLLISIONS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_WORKPACKETS,L"CLOTH_WORKPACKETS");
#if NX_SDK_VERSION_NUMBER >= 270
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_SLEEP,L"CLOTH_SLEEP");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_TEARABLE_VERTICES,L"CLOTH_TEARABLE_VERTICES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_TEARING,L"CLOTH_TEARING");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_CLOTH_ATTACHMENT,L"CLOTH_ATTACHMENT");
#endif
#if NX_USE_SOFTBODY_API
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_SOFTBODY_MESH,L"SOFTBODY_MESH");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_SOFTBODY_COLLISIONS,L"SOFTBODY_COLLISIONS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_SOFTBODY_WORKPACKETS,L"SOFTBODY_WORKPACKETS");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_SOFTBODY_SLEEP,L"SOFTBODY_SLEEP");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_SOFTBODY_TEARABLE_VERTICES,L"SOFTBODY_TEARABLE_VERTICES");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_SOFTBODY_TEARING,L"SOFTBODY_TEARING");
			AppendMenu(m,MF_STRING | MF_UNCHECKED, MO_VISUALIZE_SOFTBODY_ATTACHMENT,L"SOFTBODY_ATTACHMENT");
#endif

    memset(mVstate,0,sizeof(char)*MO_VISUALIZE_COUNT);

		NxPhysicsSDK *sdk = getSDK();
		if ( sdk )
		{
			for (unsigned int i=0; i<MO_VISUALIZE_COUNT; i++)
			{
				NxParameter p = (NxParameter) (i+NX_VISUALIZE_BODY_AXES);
				sdk->setParameter(p,0);
			}
		}

	  // ok, now initialize the scripted menu interface.
	  gAllGlobals.gFileSystem.set(this);
	  Log* tmpLog                     = new Log("TetraMaker.txt");
	  gAllGlobals.gLog.set(tmpLog);
	  CommandParser* tmpCommandParser = new CommandParser;
	  TextUserInterface* tmpTui              = new TextUserInterface("TetraMaker.tui");


		//
		AddToken("CreateDynamics", MC_CREATE_DYNAMICS );
		AddToken("PsScript", MC_PSSCRIPT);
		AddToken("PsLookAt", MC_PSLOOKAT);
		AddToken("SbcImport", SBC_IMPORT);

#if NX_USE_SOFTBODY_API
    AddToken("SbcSoftBody", SBC_SOFTBODY);

    createButton("Simulate Soft Body", SBC_SIMULATE_SOFTBODY, "SbcSimulateSoftBody");
    createButton("Stop Simulation",    SBC_STOP_SIMULATION,   "SbcStopSimuation");

    createButton("New SoftBody",       SBC_NEW_SOFTBODY,   "SbcNewSoftBody");
    createButton("SceneReset",        SBC_RESET_SCENE,    "SbcResetScene");
    createSlider("Volume",    SBC_VOLUME_STIFFNESS,     "SbcVolumeStiffness",0,1,1);
    createSlider("Stretching",SBC_STRETCHING_STIFFNESS,"SbcStretchingStiffness",0,1,1);
    createSlider("Damping" ,SBC_DAMPING_COEFFICIENT, "SbcDampingCoefficient",0,1,0.5f);
    createSlider("Friction",            SBC_FRICTION,            "SbcFriction", 0,1,0.5f);
    createSlider("ParticleRadius",     SBC_PARTICLE_RADIUS,     "SbcParticleRadius",  0.001f,0.5f,0.01f);
    createSlider("SolverIterations",   SBC_SOLVER_ITERATIONS,   "SbcSolverIterations",1,32,5);

    createCheckbox("SHOW_TETRAHEDRA",    SBC_SHOW_TETRAHEDRA,    "SbcShowTetrahedra",false);
    createCheckbox("SHOW_OVERLAY",       SBC_SHOW_OVERLAY,       "SbcShowOverlay",false);
	createCheckbox("SHOW_GRAPHICS_MESH", SBC_SHOW_GRAPHICS_MESH, "SbcShowGraphicsMesh",true);
	createCheckbox("SHOW_BACKFACES", SBC_SHOW_BACKFACES, "SbcShowBackfaces",false);
//    createCheckbox("RAYCAST_OK",         SBC_RAYCAST_OK,         "SbcRaycastOk",true);

    createSlider("SubdivisionLevel",SBC_SUBDIVISION_LEVEL, "SbcSubdivisionLevel", 1, 90, 20);
    createSlider("Mesh Detail",     SBC_MESH_DETAIL,       "SbcMeshDetail", 10, 100, 50 );

    createButton("Generate Volume", SBC_GENERATE_VOLUME,   "SbcGenerateVolume");
    createButton("Copy Surface",    SBC_COPY_SURFACE,      "SbcCopySurface");
    createButton("Iso Surface",     SBC_ISO_SURFACE,       "SbcIsoSurface");
    createCheckbox("IsoSingle",     SBC_ISO_SINGLE,        "SbcIsoSingle",true);

		createCheckbox("Asynchronous", MC_ASYNC, "SbcAsync", true );
    createSlider("Gravity",     MC_GRAVITY,       "SbcGravity", -40, 40, -9.8f );
    createSlider("TimeScale",     MC_TIME_SCALE,  "SbcTimeScale", 0, 4, 1 );


    CPARSER.Parse("TuiPageBegin SoftBodies");

    CPARSER.Parse("TuiName \"SoftBody Meshes\"");

//    CPARSER.Parse("TuiElement top");

    CPARSER.Parse("TuiElement SbcSimulateSoftBody");
    CPARSER.Parse("TuiElement SbcStopSimuation");
    CPARSER.Parse("TuiElement SbcNewSoftBody");
    CPARSER.Parse("TuiElement SbcResetScene");

    CPARSER.Parse("TuiElement SbcVolumeStiffness");
    CPARSER.Parse("TuiElement SbcStretchingStiffness");
    CPARSER.Parse("TuiElement SbcDampingCoefficient");
    CPARSER.Parse("TuiElement SbcFriction");
    CPARSER.Parse("TuiElement SbcParticleRadius");
    CPARSER.Parse("TuiElement SbcSolverIterations");
    CPARSER.Parse("TuiElement SbcStatic");
    CPARSER.Parse("TuiElement SbcDisableCollision");
    CPARSER.Parse("TuiElement SbcSelfCollision");
    CPARSER.Parse("TuiElement SbcVisualization");
    CPARSER.Parse("TuiElement SbcGravity");
    CPARSER.Parse("TuiElement SbcVolumeConservation");
    CPARSER.Parse("TuiElement SbcDamping");
    CPARSER.Parse("TuiElement SbcCollisionTwoWay");
    CPARSER.Parse("TuiElement SbcTearable");
    CPARSER.Parse("TuiElement SbcComDamping");
    CPARSER.Parse("TuiElement SbcFluidCollision");

    CPARSER.Parse("TuiElement SbcShowTetrahedra");
    CPARSER.Parse("TuiElement SbcShowOverlay");
  	CPARSER.Parse("TuiElement SbcShowGraphicsMesh");
	CPARSER.Parse("TuiElement SbcShowBackfaces");
  	CPARSER.Parse("TuiElement SbcRaycastOk");

    CPARSER.Parse("TuiElement SbcSubdivisionLevel");
    CPARSER.Parse("TuiElement SbcMeshDetail");
    CPARSER.Parse("TuiElement SbcGenerateVolume");
    CPARSER.Parse("TuiElement SbcCopySurface");
    CPARSER.Parse("TuiElement SbcIsoSurface");
    CPARSER.Parse("TuiElement SbcIsoSingle");
    CPARSER.Parse("TuiElement SbcAsync");
    CPARSER.Parse("TuiElement SbcGravity");
    CPARSER.Parse("TuiElement SbcTimeScale");

		CPARSER.Parse("TuiElement CreateDynamics");

    CPARSER.Parse("TuiEnd");
#else
    CPARSER.Parse("TuiPage CreateDynamics");
#endif

    CPARSER.Parse("TuiLoad PhysXViewer.psc");

		gAllGlobals.gLog.ref()->Display("Menu System Initialized\r\n");

  }


	bool isExplicit(const char *name)
	{
		bool ret = false;

		if ( strchr(name,':') ||
			   strchr(name,'\\') ||
				 strchr(name,'/') ) ret = true;

		return ret;
	}

  const char * cdi_getFileName(const char *fname,bool search) // convert file name into full path.
  {
  	return FileOpenString(fname,search);
  }

  void         cdi_output(const char *str)
  {
  	gAllGlobals.gLog.ref()->Display("%s", str );
  }


	const char * FileOpenString(const char *fname,bool search) // return the full path name for this file, set 'search' to true to try to 'find' the file.
	{
		return SOFTBODY::gFileInterface->getSoftFileName(fname,true);
		//const char *ret = fname;
		//if ( !isExplicit(fname) )
		//{
  //	  static char scratch[512];
  //	  sprintf(scratch,"%s\\%s", MEDIA_PATH, fname );
		//	ret = scratch;
		//}
  //	return ret;
	}

	NxParameter getParameter(MenuOptions o)
	{
		NxParameter ret =	(NxParameter) 0;
		switch ( o )
		{
      case	MO_VISUALIZE_BODY_AXES: ret =	NX_VISUALIZE_BODY_AXES; break;
      case	MO_VISUALIZE_BODY_MASS_AXES: ret =	NX_VISUALIZE_BODY_MASS_AXES; break;
      case	MO_VISUALIZE_BODY_LIN_VELOCITY: ret =	NX_VISUALIZE_BODY_LIN_VELOCITY; break;
      case	MO_VISUALIZE_BODY_ANG_VELOCITY: ret =	NX_VISUALIZE_BODY_ANG_VELOCITY; break;
      case	MO_VISUALIZE_BODY_JOINT_GROUPS: ret =	NX_VISUALIZE_BODY_JOINT_GROUPS; break;
      case	MO_VISUALIZE_JOINT_LOCAL_AXES: ret =	NX_VISUALIZE_JOINT_LOCAL_AXES; break;
      case	MO_VISUALIZE_JOINT_WORLD_AXES: ret =	NX_VISUALIZE_JOINT_WORLD_AXES; break;
      case	MO_VISUALIZE_JOINT_LIMITS: ret =	NX_VISUALIZE_JOINT_LIMITS; break;
      case	MO_VISUALIZE_CONTACT_POINT: ret =	NX_VISUALIZE_CONTACT_POINT; break;
      case	MO_VISUALIZE_CONTACT_NORMAL: ret =	NX_VISUALIZE_CONTACT_NORMAL; break;
      case	MO_VISUALIZE_CONTACT_ERROR: ret =	NX_VISUALIZE_CONTACT_ERROR; break;
      case	MO_VISUALIZE_CONTACT_FORCE: ret =	NX_VISUALIZE_CONTACT_FORCE; break;
      case	MO_VISUALIZE_ACTOR_AXES: ret =	NX_VISUALIZE_ACTOR_AXES; break;
      case	MO_VISUALIZE_COLLISION_AABBS: ret =	NX_VISUALIZE_COLLISION_AABBS; break;
      case	MO_VISUALIZE_COLLISION_SHAPES: ret =	NX_VISUALIZE_COLLISION_SHAPES; break;
      case	MO_VISUALIZE_COLLISION_AXES: ret =	NX_VISUALIZE_COLLISION_AXES; break;
      case	MO_VISUALIZE_COLLISION_COMPOUNDS: ret =	NX_VISUALIZE_COLLISION_COMPOUNDS; break;
      case	MO_VISUALIZE_COLLISION_VNORMALS: ret =	NX_VISUALIZE_COLLISION_VNORMALS; break;
      case	MO_VISUALIZE_COLLISION_FNORMALS: ret =	NX_VISUALIZE_COLLISION_FNORMALS; break;
      case	MO_VISUALIZE_COLLISION_EDGES: ret =	NX_VISUALIZE_COLLISION_EDGES; break;
      case	MO_VISUALIZE_COLLISION_SPHERES: ret =	NX_VISUALIZE_COLLISION_SPHERES; break;
#if NX_SDK_VERSION_NUMBER < 280
      case	MO_VISUALIZE_COLLISION_SAP: ret =	NX_VISUALIZE_COLLISION_SAP; break;
#endif
      case	MO_VISUALIZE_COLLISION_STATIC: ret =	NX_VISUALIZE_COLLISION_STATIC; break;
      case	MO_VISUALIZE_COLLISION_DYNAMIC: ret =	NX_VISUALIZE_COLLISION_DYNAMIC; break;
      case	MO_VISUALIZE_COLLISION_FREE: ret =	NX_VISUALIZE_COLLISION_FREE; break;
      case	MO_VISUALIZE_COLLISION_CCD: ret =	NX_VISUALIZE_COLLISION_CCD; break;
      case	MO_VISUALIZE_COLLISION_SKELETONS: ret =	NX_VISUALIZE_COLLISION_SKELETONS; break;
      case	MO_VISUALIZE_FLUID_EMITTERS: ret =	NX_VISUALIZE_FLUID_EMITTERS; break;
      case	MO_VISUALIZE_FLUID_POSITION: ret =	NX_VISUALIZE_FLUID_POSITION; break;
      case	MO_VISUALIZE_FLUID_VELOCITY: ret =	NX_VISUALIZE_FLUID_VELOCITY; break;
      case	MO_VISUALIZE_FLUID_KERNEL_RADIUS: ret =	NX_VISUALIZE_FLUID_KERNEL_RADIUS; break;
      case	MO_VISUALIZE_FLUID_BOUNDS: ret =	NX_VISUALIZE_FLUID_BOUNDS; break;
      case	MO_VISUALIZE_FLUID_PACKETS: ret =	NX_VISUALIZE_FLUID_PACKETS; break;
      case	MO_VISUALIZE_FLUID_MOTION_LIMIT: ret =	NX_VISUALIZE_FLUID_MOTION_LIMIT; break;
      case	MO_VISUALIZE_FLUID_DYN_COLLISION: ret =	NX_VISUALIZE_FLUID_DYN_COLLISION; break;
#if NX_SDK_VERSION_NUMBER >= 270
      case	MO_VISUALIZE_FLUID_STC_COLLISION: ret =	NX_VISUALIZE_FLUID_STC_COLLISION; break;
      case	MO_VISUALIZE_FLUID_MESH_PACKETS: ret =	NX_VISUALIZE_FLUID_MESH_PACKETS; break;
#endif
      case	MO_VISUALIZE_FLUID_DRAINS: ret =	NX_VISUALIZE_FLUID_DRAINS; break;
#if NX_SDK_VERSION_NUMBER >= 270
      case	MO_VISUALIZE_CLOTH_MESH: ret =	NX_VISUALIZE_CLOTH_MESH; break;
#endif
      case	MO_VISUALIZE_CLOTH_COLLISIONS: ret =	NX_VISUALIZE_CLOTH_COLLISIONS; break;
      case	MO_VISUALIZE_CLOTH_SELFCOLLISIONS: ret =	NX_VISUALIZE_CLOTH_SELFCOLLISIONS; break;
      case	MO_VISUALIZE_CLOTH_WORKPACKETS: ret =	NX_VISUALIZE_CLOTH_WORKPACKETS; break;
#if NX_SDK_VERSION_NUMBER >= 270
      case	MO_VISUALIZE_CLOTH_SLEEP: ret =	NX_VISUALIZE_CLOTH_SLEEP; break;
      case	MO_VISUALIZE_CLOTH_TEARABLE_VERTICES: ret =	NX_VISUALIZE_CLOTH_TEARABLE_VERTICES; break;
      case	MO_VISUALIZE_CLOTH_TEARING: ret =	NX_VISUALIZE_CLOTH_TEARING; break;
      case	MO_VISUALIZE_CLOTH_ATTACHMENT: ret =	NX_VISUALIZE_CLOTH_ATTACHMENT; break;
#endif
#if NX_USE_SOFTBODY_API
      case	MO_VISUALIZE_SOFTBODY_MESH: ret =	NX_VISUALIZE_SOFTBODY_MESH; break;
      case	MO_VISUALIZE_SOFTBODY_COLLISIONS: ret =	NX_VISUALIZE_SOFTBODY_COLLISIONS; break;
      case	MO_VISUALIZE_SOFTBODY_WORKPACKETS: ret =	NX_VISUALIZE_SOFTBODY_WORKPACKETS; break;
      case	MO_VISUALIZE_SOFTBODY_SLEEP: ret =	NX_VISUALIZE_SOFTBODY_SLEEP; break;
      case	MO_VISUALIZE_SOFTBODY_TEARABLE_VERTICES: ret =	NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES; break;
      case	MO_VISUALIZE_SOFTBODY_TEARING: ret =	NX_VISUALIZE_SOFTBODY_TEARING; break;
      case	MO_VISUALIZE_SOFTBODY_ATTACHMENT: ret =	NX_VISUALIZE_SOFTBODY_ATTACHMENT; break;
#endif
		}
		return ret;
	}

  bool processMenu(HWND hwnd,unsigned int cmd,NxBounds3 &bounds)
  {

  	bool ret = false;

    const char *fname = 0;

    bool groundPlane = false;


  	switch ( cmd )
  	{
  		case MO_EXPORT_SURFACE:
  			if ( 1 )
  			{
  				gSoftBodySystem->processCommand(SBC_EXPORT_SURFACE,1,0);
  			}
  			break;
  		case MO_RUN_SCRIPT:
  			if ( 1 )
  			{
  				CPARSER.Parse("PsScript");
  			}
  			break;
  		case MO_IMPORT_OBJ:
        gSoftBodySystem->processCommand(SBC_RESET_SCENE,1,0);
  			gSoftBodySystem->processCommand(SBC_IMPORT_OBJ,1,0);
				fname = gSoftBodySystem->getImportName(bounds);
        groundPlane = true;
  			break;
  		case MO_IMPORT_PSK:
			initCallback_LoadSoftMeshPSK();
        gSoftBodySystem->processCommand(SBC_RESET_SCENE,1,0);
  			gSoftBodySystem->processCommand(SBC_IMPORT_PSK,1,0);
				fname = gSoftBodySystem->getImportName(bounds);
        groundPlane = true;
  			break;
  		case MO_IMPORT_EZM:
        gSoftBodySystem->processCommand(SBC_RESET_SCENE,1,0);
  			gSoftBodySystem->processCommand(SBC_IMPORT_EZM,1,0);
				fname = gSoftBodySystem->getImportName(bounds);
        groundPlane = true;
  			break;
  		case MO_IMPORT_XML:
        gSoftBodySystem->processCommand(SBC_RESET_SCENE,1,0);
  			gSoftBodySystem->processCommand(SBC_IMPORT_XML,1,0);
        groundPlane = true;
  			break;
  		case MO_IMPORT_COLLADA:
        gSoftBodySystem->processCommand(SBC_RESET_SCENE,1,0);
  			gSoftBodySystem->processCommand(SBC_IMPORT_COLLADA,1,0);
        groundPlane = true;
  			break;
  		case MO_IMPORT_NXB:
        gSoftBodySystem->processCommand(SBC_RESET_SCENE,1,0);
  			gSoftBodySystem->processCommand(SBC_IMPORT_NXB,1,0);
        groundPlane = true;
  			break;
  		case MO_EXPORT_OBJ:
  			gSoftBodySystem->processCommand(SBC_EXPORT_OBJ,1,0);
  			break;
  		case MO_EXPORT_EZM:
  			gSoftBodySystem->processCommand(SBC_EXPORT_EZM,1,0);
  			break;
  		case MO_EXPORT_XML:
  			gSoftBodySystem->processCommand(SBC_EXPORT_XML,1,0);
  			break;
      case MO_CORE_DUMP_XML:
        gSoftBodySystem->processCommand(SBC_CORE_DUMP_XML,1,0);
        break;
      case MO_CORE_DUMP_DAE:
        gSoftBodySystem->processCommand(SBC_CORE_DUMP_DAE,1,0);
        break;
      case MO_CORE_DUMP_NXB:
        gSoftBodySystem->processCommand(SBC_CORE_DUMP_NXB,1,0);
        break;
    	case MO_EXIT:
    		SendMessage(hwnd,WM_CLOSE,0,0);
  		  break;
  		case MO_VISUALIZE_NONE:
  			if ( 1 )
  			{
      		NxPhysicsSDK *sdk = getSDK();
      		if ( sdk )
      		{
      			for (unsigned int i=0; i<MO_VISUALIZE_COUNT; i++)
      			{
      				NxParameter p = (NxParameter) (i+NX_VISUALIZE_BODY_AXES);
      				sdk->setParameter(p,0);
      				mVstate[i] = 0;
  	    			CheckMenuItem(mVisualization, i+1, MF_BYPOSITION | MF_UNCHECKED );
      			}
      		}
  			}
  			break;
  		case MO_CORE_DUMP_ACTIVE_STATE:
  			if ( 1 )
  			{
				  NxU32 index = MO_CORE_DUMP_ACTIVE_STATE - MO_IMPORT_OBJ;
				  if ( gSoftBodySystem->isActiveState() )
				  {
				  	gSoftBodySystem->setActiveState(false);
 	    			CheckMenuItem(mFileMenu, index, MF_BYPOSITION | MF_UNCHECKED );
 	    		}
				  else
				  {
				  	gSoftBodySystem->setActiveState(true);
  	    		CheckMenuItem(mFileMenu, index, MF_BYPOSITION | MF_CHECKED );
  	    	}
  			}
  			break;

			case MO_VISUALIZE_BODY_AXES:
			case MO_VISUALIZE_BODY_MASS_AXES:
			case MO_VISUALIZE_BODY_LIN_VELOCITY:
			case MO_VISUALIZE_BODY_ANG_VELOCITY:
			case MO_VISUALIZE_BODY_JOINT_GROUPS:
			case MO_VISUALIZE_JOINT_LOCAL_AXES:
			case MO_VISUALIZE_JOINT_WORLD_AXES:
			case MO_VISUALIZE_JOINT_LIMITS:
			case MO_VISUALIZE_CONTACT_POINT:
			case MO_VISUALIZE_CONTACT_NORMAL:
			case MO_VISUALIZE_CONTACT_ERROR:
			case MO_VISUALIZE_CONTACT_FORCE:
			case MO_VISUALIZE_ACTOR_AXES:
			case MO_VISUALIZE_COLLISION_AABBS:
			case MO_VISUALIZE_COLLISION_SHAPES:
			case MO_VISUALIZE_COLLISION_AXES:
			case MO_VISUALIZE_COLLISION_COMPOUNDS:
			case MO_VISUALIZE_COLLISION_VNORMALS:
			case MO_VISUALIZE_COLLISION_FNORMALS:
			case MO_VISUALIZE_COLLISION_EDGES:
			case MO_VISUALIZE_COLLISION_SPHERES:
#if NX_SDK_VERSION_NUMBER < 280
			case MO_VISUALIZE_COLLISION_SAP:
#endif
			case MO_VISUALIZE_COLLISION_STATIC:
			case MO_VISUALIZE_COLLISION_DYNAMIC:
			case MO_VISUALIZE_COLLISION_FREE:
			case MO_VISUALIZE_COLLISION_CCD:
			case MO_VISUALIZE_COLLISION_SKELETONS:
			case MO_VISUALIZE_FLUID_EMITTERS:
			case MO_VISUALIZE_FLUID_POSITION:
			case MO_VISUALIZE_FLUID_VELOCITY:
			case MO_VISUALIZE_FLUID_KERNEL_RADIUS:
			case MO_VISUALIZE_FLUID_BOUNDS:
			case MO_VISUALIZE_FLUID_PACKETS:
			case MO_VISUALIZE_FLUID_MOTION_LIMIT:
			case MO_VISUALIZE_FLUID_DYN_COLLISION:
#if NX_SDK_VERSION_NUMBER >= 270
			case MO_VISUALIZE_FLUID_STC_COLLISION:
			case MO_VISUALIZE_FLUID_MESH_PACKETS:
#endif
			case MO_VISUALIZE_FLUID_DRAINS:
#if NX_SDK_VERSION_NUMBER >= 270
			case MO_VISUALIZE_CLOTH_MESH:
#endif
			case MO_VISUALIZE_CLOTH_COLLISIONS:
			case MO_VISUALIZE_CLOTH_SELFCOLLISIONS:
			case MO_VISUALIZE_CLOTH_WORKPACKETS:
#if NX_SDK_VERSION_NUMBER >= 270
			case MO_VISUALIZE_CLOTH_SLEEP:
			case MO_VISUALIZE_CLOTH_TEARABLE_VERTICES:
			case MO_VISUALIZE_CLOTH_TEARING:
			case MO_VISUALIZE_CLOTH_ATTACHMENT:
#endif
#if NX_USE_SOFBODY_API
			case MO_VISUALIZE_SOFTBODY_MESH:
			case MO_VISUALIZE_SOFTBODY_COLLISIONS:
			case MO_VISUALIZE_SOFTBODY_WORKPACKETS:
			case MO_VISUALIZE_SOFTBODY_SLEEP:
			case MO_VISUALIZE_SOFTBODY_TEARABLE_VERTICES:
			case MO_VISUALIZE_SOFTBODY_TEARING:
			case MO_VISUALIZE_SOFTBODY_ATTACHMENT:
#endif
				if ( 1 )
				{
					float v;

					unsigned int index = cmd - MO_VISUALIZE_BODY_AXES;
					if ( mVstate[index] )
					{
						mVstate[index] = 0;
						v = 0;
	    			CheckMenuItem(mVisualization, index+1, MF_BYPOSITION | MF_UNCHECKED );
					}
					else
					{
						v = 1;
						mVstate[index] = 1;
	    			CheckMenuItem(mVisualization, index+1, MF_BYPOSITION | MF_CHECKED );
					}
					NxPhysicsSDK *sdk = getSDK();
					if ( sdk )
					{
						NxParameter p = getParameter((MenuOptions)cmd);
						sdk->setParameter(p,v);
					}

					const char *showDebug = "false";

					for (unsigned int i=0; i<MO_VISUALIZE_COUNT; i++)
					{
    				if ( mVstate[i] )
    				{
    					showDebug = "true";
    					break;
    				}
    			}
    			const char *argv[2];
    			argv[0] = "null";
    			argv[1] = showDebug;
    			gSoftBodySystem->processCommand(SBC_DEBUG_VISUALIZATION, 2, argv );

				}
				break;


	    case MO_STATIC:
	    case MO_DISABLE_COLLISION:
	    case MO_SELFCOLLISION:
	    case MO_VISUALIZATION:
	    case MO_GRAVITY:
	    case MO_VOLUME_CONSERVATION:
	    case MO_DAMPING:
	    case MO_COLLISION_TWOWAY:
	    case MO_TEARABLE:
	    case MO_HARDWARE:
	    case MO_COMDAMPING:
	    case MO_FLUID_COLLISION:
	    	if ( 1 )
	    	{

	    		SoftBodyCommand sbc;
	    		switch ( cmd )
	    		{
	    			case MO_STATIC: sbc = SBC_STATIC; break;
	    			case MO_DISABLE_COLLISION: sbc = SBC_DISABLE_COLLISION; break;
	    			case MO_SELFCOLLISION: sbc = SBC_SELFCOLLISION; break;
	    			case MO_VISUALIZATION: sbc = SBC_VISUALIZATION; break;
	    			case MO_GRAVITY: sbc = SBC_GRAVITY; break;
	    			case MO_VOLUME_CONSERVATION: sbc = SBC_VOLUME_CONSERVATION; break;
	    			case MO_DAMPING: sbc = SBC_DAMPING; break;
	    			case MO_COLLISION_TWOWAY: sbc = SBC_COLLISION_TWOWAY; break;
	    			case MO_TEARABLE: sbc = SBC_TEARABLE; break;
	    			case MO_HARDWARE: sbc = SBC_HARDWARE; break;
	    			case MO_COMDAMPING: sbc = SBC_COMDAMPING; break;
	    			case MO_FLUID_COLLISION: sbc = SBC_FLUID_COLLISION; break;
	    		}

	    		const char *argv[2];
	    		argv[0] = "null";
	    		unsigned int index = cmd - MO_DUMMY0;
	    		unsigned int flag  = 1<<(index);
	    		if ( mFlags & flag )
	    		{
	    			argv[1] = "false";
	    			mFlags&=~flag;
	    			CheckMenuItem(mSoftbodyFlags, index-1, MF_BYPOSITION | MF_UNCHECKED );
	    		}
	    		else
	    		{
	    			argv[1] = "true";
	    			mFlags|=flag;
	    			CheckMenuItem(mSoftbodyFlags, index-1, MF_BYPOSITION | MF_CHECKED );
	    		}
					gSoftBodySystem->processCommand(sbc,2,argv);
	    	}

	    	break;

  	}

		if ( fname )
		{
  		CPARSER.Parse("/cd_mesh \"%s\"",fname );
      ret = true;
  	}

    if ( groundPlane )
      px_createGroundPlane();

  	return ret;
  }

  int CommandCallback(int token,int count,const char **arglist)
  {
  	int ret = 0;

  	ret = gSoftBodySystem->processCommand( (SoftBodyCommand) token, count, arglist );

		switch ( token )
		{
			case MC_ASYNC:
				if ( count == 2 )
				{
					px_setAsync( stricmp(arglist[1],"true") == 0 );
				}
				break;
			case MC_TIME_SCALE:
				if ( count == 2 )
				{
					float scale = (float) atof( arglist[1] );
					px_setTimeScale(scale);
				}
				break;
		  case MC_GRAVITY:
		  	if ( count == 2 )
		  	{
		  		NxVec3 g;
		  		g.x = 0;
		  		g.y = (float) atof( arglist[1] );
		  		g.z = 0;
		  		px_setGravity(&g.x);
		  	}
		  	break;
			case MC_PSLOOKAT:
				//            0      1      2      3       4      5       6
				// Usage: PsLookAt <eyex> <eyey> <eyez> <lookx> <looky> <lookz>
				if ( count == 7 )
				{
					float eye[3];
					float look[3];

					eye[0] = (float) atof( arglist[1] );
					eye[1] = (float) atof( arglist[2] );
					eye[2] = (float) atof( arglist[3] );

					look[0] = (float) atof(arglist[4] );
					look[1] = (float) atof(arglist[5] );
					look[2] = (float) atof(arglist[6] );

					lookAt(eye,look);

				}
				break;
			case SBC_IMPORT:
        px_createGroundPlane();
				break;
			case MC_PSSCRIPT:
				if ( 1 )
				{
					const char *fname = 0;
					if ( count >= 2 )
					{
						fname = arglist[1];
					}
  				SoftFileInterface *sfi = gSoftBodySystem->getSoftFileInterface();
  				if ( sfi )
  				{
  					fname = sfi->getLoadFileName(".psc", "Select a demo script to run.");
  				}
  				if ( fname )
  				{
  					CPARSER.Parse("Run \"%s\"",fname);
  				}
				}
				break;
			case MC_CREATE_DYNAMICS:
				if ( count >= 3 )
				{
					const char *fname  = arglist[1];
					const char *output = arglist[2];

					CreateDynamicsAPI *api = getCreateDynamicsDLL(this);
          if ( api )
          {

            CD_Format format = CD_XML;
            if ( strcmp(output,"COLLADA") == 0 )
            	format = CD_COLLADA;

            CreateDynamicsResult result;

            bool ok = api->createDynamics(arglist[1], "rules.rul", format, &result );

            if ( ok )
            {
 							gAllGlobals.gLog.ref()->Display("===========================================================\r\n");
 							gAllGlobals.gLog.ref()->Display("BoneCount:           %4d\r\n", result.mBoneCount);
 							gAllGlobals.gLog.ref()->Display("IgnoreCount:         %4d\r\n", result.mIgnoreCount);
 							gAllGlobals.gLog.ref()->Display("CollapseCount:       %4d\r\n", result.mCollapseCount);
 							gAllGlobals.gLog.ref()->Display("BodyPairCount:       %4d\r\n", result.mBodyPairCount);
 							gAllGlobals.gLog.ref()->Display("BoxCount:            %4d\r\n", result.mBoxCount);
 							gAllGlobals.gLog.ref()->Display("SphereCount:         %4d\r\n", result.mSphereCount);
 							gAllGlobals.gLog.ref()->Display("CapsuleCount:        %4d\r\n", result.mCapsuleCount);
 							gAllGlobals.gLog.ref()->Display("ConvexCount:         %4d\r\n", result.mConvexCount);
 							gAllGlobals.gLog.ref()->Display("NullCount:           %4d\r\n", result.mNullCount );
 							gAllGlobals.gLog.ref()->Display("CompoundCount:       %4d\r\n", result.mCompoundCount);
 							gAllGlobals.gLog.ref()->Display("ClothCount:          %4d\r\n", result.mClothCount);
 							gAllGlobals.gLog.ref()->Display("ClothVertices:       %4d\r\n", result.mClothVertices);
 							gAllGlobals.gLog.ref()->Display("ClothTrianges:       %4d\r\n", result.mClothTriangles);
 							gAllGlobals.gLog.ref()->Display("ActorCount:          %4d\r\n", result.mActorCount);
 							gAllGlobals.gLog.ref()->Display("ShapeCount:          %4d\r\n", result.mShapeCount);
 							gAllGlobals.gLog.ref()->Display("JointCount:          %4d\r\n", result.mJointCount);
 							gAllGlobals.gLog.ref()->Display("===========================================================\r\n");

   						char scratch[512];
							char base[256];
							char path[256];
							char postfix[256];
							if ( IsDirectory(fname,path,base,postfix) )
							{
								strcpy(scratch,base);
							}
							else
							{
					      strcpy(scratch, fname );
							}

              char *dot = strchr(scratch,'.');

					    if ( dot )
					    {
   							*dot = 0;
								if ( format == CD_XML )
   							  strcat(scratch,".xml");
								else
									strcat(scratch,".dae");
   							const char *argv[2];
   							argv[0] = 0;
   							argv[1] = scratch;
					      gSoftBodySystem->processCommand(SBC_RESET_SCENE,1,0);
					      if ( format == CD_XML )
            			gSoftBodySystem->processCommand(SBC_IMPORT_XML,2,argv);
            		else
            			gSoftBodySystem->processCommand(SBC_IMPORT_COLLADA,2,argv);
   						}

   					}
   				}
				}
				break;
		}


    return ret;
  }


  void createSlider(const char *title,int token,const char *cmd,float smin,float smax,float sdefault)
  {
  	AddToken(cmd,token);
  	CPARSER.Parse("TuiSliderBegin %s", cmd);
  	CPARSER.Parse("TuiName \"%s\"",title);
  	CPARSER.Parse("TuiSliderRange %f %f",smin,smax);
  	CPARSER.Parse("TuiArg %f",sdefault);
  	CPARSER.Parse("TuiScript %s %%1",cmd);
  	if ( token == SBC_SOLVER_ITERATIONS ||
         token == SBC_SUBDIVISION_LEVEL ||
         token == SBC_MESH_DETAIL )
  		CPARSER.Parse("TuiInt");
  	CPARSER.Parse("TuiSliderEnd");
  }

  void createCheckbox(const char *title,int token,const char *cmd,bool state)
  {
  	AddToken(cmd,token);
  	CPARSER.Parse("TuiCheckboxBegin %s", cmd);
  	CPARSER.Parse("TuiName \"%s\"",title);
  	if ( state )
    	CPARSER.Parse("TuiArg true");
    else
    	CPARSER.Parse("TuiArg false");
  	CPARSER.Parse("TuiScript %s %%1",cmd);
  	CPARSER.Parse("TuiCheckboxEnd");
  }

  void createButton(const char *title,int token,const char *cmd)
  {
    char scratch[512];
    strcpy(scratch,cmd);
    char *space = strchr(scratch,' ');
    if ( space ) *space = 0;

  	AddToken(scratch,token);
  	CPARSER.Parse("TuiButtonBegin %s", scratch);
  	CPARSER.Parse("TuiName \"%s\"",title);
  	CPARSER.Parse("TuiScript %s",cmd);
  	CPARSER.Parse("TuiCheckboxEnd");
  }




	unsigned int mFlags;

  HMENU	mMainMenu;
  HMENU mFileMenu;
  HMENU mSoftbodyFlags;
  HMENU mVisualization;

  char mVstate[MO_VISUALIZE_COUNT];

};

static class MyMenu *gMyMenu=0;

HMENU createMyMenu(void)
{
	gMyMenu = new MyMenu;
	return gMyMenu->mMainMenu;
}



bool  processMenu(HWND hwnd,unsigned int cmd,NxBounds3 &bounds)
{
	bool ret = false;

  if ( gMyMenu )
  {
  	ret = gMyMenu->processMenu(hwnd,cmd,bounds);
  }
	return ret;

}


//==================================================================================
void CALLBACK gOnUIPageEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
{
	CDXUTDialog *dialog = pControl->m_pDialog;

	void *userdata = dialog->GetUserData();
	if ( userdata )
	{
		TuiElement *page = (TuiElement *) userdata;
		TextUserInterface *tui = gAllGlobals.gTui.ref();

		if ( page )
		{
			TuiElement *ret = page->GetElement( nControlID-1 );

			switch ( ret->GetType() )
			{
				case TT_MULTI_SELECT:
					if ( 1 )
					{
						switch( nEvent )
						{
							case EVENT_LISTBOX_ITEM_DBLCLK:
							{
								DXUTListBoxItem *pItem = ((CDXUTListBox *)pControl)->GetItem( ((CDXUTListBox *)pControl)->GetSelectedIndex( -1 ) );
								break;
							}
							case EVENT_LISTBOX_SELECTION:
							{
								CDXUTListBox *pListBox = (CDXUTListBox *)pControl;
								TuiChoiceVector &choices = ret->GetChoices();
								int count = (int)choices.size();
								for (int i=0; i<count; i++)
								{
									DXUTListBoxItem *item = pListBox->GetItem(i);
									TuiChoice  &choice = choices[i];
									assert(item);
									if ( item )
									{
										if ( choice.GetState() != item->bSelected )
										{
											choice.SetState(item->bSelected);
											const char *args[2];
											args[0] = choice.GetArg().Get();

											if ( choice.GetState() )
												args[1] = "true";
											else
												args[1] = "false";

											page->ExecuteElement( nControlID-1, 2, args, tui );
										}
									}
								}

							}
						}
					}
					break;
				case TT_SLIDER:
					if ( 1 )
					{
						CDXUTSlider *slider = (CDXUTSlider *) pControl;
						int v = slider->GetValue();
						float fv = ret->GetSliderValue(v);
						char scratch[512];
  						sprintf(scratch,"%0.3f", fv );

						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args, tui );
					}
					break;
				case TT_COMBO:
					if ( 1 )
					{
						CDXUTComboBox *combo = (CDXUTComboBox *) pControl;
						DXUTComboBoxItem *pItem = combo->GetSelectedItem();
						wchar_t *string = pItem->strText;
						char scratch[512];
						wcstombs(scratch, string, 512 );
						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args,tui );
					
 						// now, do we need to hide any items (or show them)?
 						page->OnComboBoxChange( scratch, nControlID-1 );
					}
					break;
				case TT_BUTTON:
				case TT_PAGE:
					page->ExecuteElement( nControlID-1, 0, 0, tui );
					break;
				case TT_CHECKBOX:
					if ( 1 )
					{
						CDXUTCheckBox *pCheck = (CDXUTCheckBox *)pControl;
						bool state = pCheck->GetChecked();
						const char *args[1];
						if ( state )
						{
							args[0] = "true";
						}
						else
						{
							args[0] = "false";
						}
						page->ExecuteElement( nControlID-1, 1, args, tui );

 						// now, do we need to hide any items (or show them)?
 						page->OnCheckboxChange( state, nControlID-1 );
					}
					break;
			}
			if ( tui )
			{
				if ( ret && ret->GetType() == TT_PAGE )
				{
					// tell the last page it is now exiting
					TuiElement *exitingPage = tui->GetPage();
					if ( exitingPage )
					{
						exitingPage->OnExit( exitingPage );
					}

					tui->SetPage(ret);

					// tell new page it is now loading
					TuiElement *loadingPage = tui->GetPage();
					if ( loadingPage )
					{
						loadingPage->OnLoad( loadingPage );
					}
				}
			}
		}
	}
	else
	{
		// it's for the console
		CDXUTEditBox *edit = (CDXUTEditBox *) pControl;
		switch ( nEvent )
		{
			case EVENT_EDITBOX_STRING:
				if ( 1 )
				{
					LPCWSTR text = edit->GetText();
					char scratch[1024];
					WideToChar(text,scratch,1024);
					gAllGlobals.gLog.ref()->Display("%s\r\n", scratch);
					CPARSER.Parse("%s",scratch);
					edit->SetText(L"",true);
				}
				break;
			case EVENT_EDITBOX_CHANGE:
				break;
			default:
				break;
		}
	}

}

void saveMenuState(void)
{
  CPARSER.Parse("TuiSave PhysXViewer.psc");
}

