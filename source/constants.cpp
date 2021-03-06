#include "constants.h"
// bullet
#include "btBulletDynamicsCommon.h"
#include <osg/Array>

#include "RakNetTypes.h"  // MessageID

// http://stackoverflow.com/questions/3681140/how-do-i-avoid-both-global-variables-and-magic-numbers

namespace troen
{


	//////////////////////////////////////////////////////////////////////////
	//////////////////////#Script Modifiable #////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	int DEFAULT_MAINWINDOW_WIDTH(1024);
	int DEFAULT_MAINWINDOW_HEIGHT(768);

	int USE_CULLING(0);
	//ratio main to navi width
	double WINDOW_RATIO_FULLSCREEN(3.0/4.0); 

	// LOGIC
	double RESPAWN_DURATION(4000);
	double GAME_START_COUNTDOWN_DURATION(3000);
	double RANDOM_NUMBER_DURATION(800);
	double RANDOM_NUMBER_INTERVAL(8000);


	//Field of view
	double FOVY_INITIAL(29.1484f);
	double FOVY_ADDITION_MAX(30.f);
	double FOVY_INITIAL_NAVI(29.1484f);


	//Carengine
	double SPEED_TOREAL_RATIO(2.0);
	double DAMPING_FORCE(1000.0);

	//STEERING
	double BIKE_STEERING_INCREMENT(0.02f);
	double BIKE_STEERINGCLAMP(0.2f);

	// BENDED VIEWS
	int BENDED_VIEWS_ACTIVATED(3000);
	int BENDED_VIEWS_DEACTIVATED(12000);
	int TIME_TO_ACTIVATE_BENDED_VIEWS(10000);
	int BENDED_STYLING_PRESET(4);
	double BENDED_DEFORMATION_START(0.1);
	int BENDED_DEFORMATION_END(1000);

	// AUDIO
	int ENGINE_FREQUENCY_LOW(10000);
	int ENGINE_FREQUENCY_HIGH(100000);
	double AUDIO_PITCH_FACTOR(120000);

	double CAMERA_POSITION_OFFSET_X(0);
	double CAMERA_POSITION_OFFSET_Y(0);
	double CAMERA_POSITION_OFFSET_Z(0);

	double CAMERA_LOOKAT_POSITION_X(0);
	double CAMERA_LOOKAT_POSITION_Y(BIKE_DIMENSIONS.y() / 2);
	double CAMERA_LOOKAT_POSITION_Z(BIKE_DIMENSIONS.z() + 2);

	double CAMERA_EYE_POSITION_X(0);
	double CAMERA_EYE_POSITION_Y(-2);
	double CAMERA_EYE_POSITION_Z(0);

	double CAMERA_NAVI_EYE_POSITION_Z(40.0);
	double CAMERA_NAVI_CENTER_POSITION_Z(20.0);





	//////////////////////////////////////////////////////////////////////////
	////////////////////////Constant/////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////


	// GENERAL

	const int NUM_MULTISAMPLES(8);

	// GAME
	const float DEFAULT_SOUND_VOLUME(1.f);


	const double FOVY_DELTA_MAX(.7f);




	//PHYSIS
	const btVector3 DEFAULT_GRAVITY(0,0,-10);

	// BIKE
	const btVector3 BIKE_DIMENSIONS(1.9, 4.0, 1.2);
	const float BIKE_VIEW_SCALE_FACTOR(1.f / 5.f * BIKE_DIMENSIONS.y());
	const osg::Vec3f BIKE_VIEW_SCALE_FACTORS(BIKE_VIEW_SCALE_FACTOR, BIKE_VIEW_SCALE_FACTOR, BIKE_VIEW_SCALE_FACTOR);
	const osg::Vec3f BIKE_VIEW_TRANSLATE_VALUES(0,0,-BIKE_DIMENSIONS.z() * 4.9f / 12.5);

	const float BIKE_MASS(1200);
	const int BIKE_VELOCITY_MAX(BIKE_DIMENSIONS.y() * 50);
	const int BIKE_TURBO_VELOCITY_MAX(BIKE_VELOCITY_MAX * 1.7);
	const int BIKE_VELOCITY_MIN(BIKE_DIMENSIONS.y() * 30);
	const float BIKE_VELOCITY_DAMPENING_TERM(.3f);
	const float BIKE_ACCELERATION_FACTOR_MAX(1.5f);
	const float BIKE_DECELERATION_FACTOR(4.f);
	const float BIKE_TURN_FACTOR_MAX(15);
	const float BIKE_ANGULAR_DAMPENING_TERM(0.001f);
	const float BIKE_VIEWING_ANGLE_DAMPENING_TERM(0.1f);


	// BIKE_TILT_DAMPENING = 1 would lead to immediate/unsmooth tilt
	// 1 / BIKE_TILT_MAX specifies angle in radiant
	const float BIKE_TILT_DAMPENING(20.f);
	const float BIKE_TILT_MAX(BIKE_ROTATION_VALUE + BIKE_ROTATION_VALUE * BIKE_HANDBRAKE_FACTOR);
	const float BIKE_WHEELY_TILT_MAX(2.f);
	const float THRESHOLD_FOR_ABRUPT_VELOCITY_CHANGE(20.f);

	//INPUT
	const float BIKE_ROTATION_VALUE(12.0f);
	const float BIKE_HANDBRAKE_FACTOR(1.4f);
	// this should always be less than 1000/60FPS - the smaller, the more responsive is the input
	const int POLLING_DELAY_MS(8);
	const int VIBRATION_TIME_MS(500);

	// LEVEL
	const int LEVEL_SIZE(1463.38964);
	const double MAP_UNITS_PER_METER(1.0);

	// FENCE
	// determines how accurate the fence will be
	const float ROUTE_WIDTH_MODEL(BIKE_DIMENSIONS.x() * 10.f);
	const float ROUTE_WIDTH_VIEW(ROUTE_WIDTH_MODEL);
	const float ROUTE_HOVER_HEIGHT = 4.0f;
	const float FENCE_PART_LENGTH(BIKE_DIMENSIONS.y() / 2);
	const float FENCE_PART_WIDTH(BIKE_DIMENSIONS.x() * .3f);
	const int DEFAULT_MAX_FENCE_PARTS(400);
	const int FENCE_TO_MINIMAP_PARTS_RATIO(10);


	// CAMERA

	const float CAMERA_TILT_FACTOR(4.f);
	const int HUD_PROJECTION_SIZE(1000);

    osg::Vec3 CAMERA_POSITION_OFFSET(CAMERA_POSITION_OFFSET_X, CAMERA_POSITION_OFFSET_Y, CAMERA_POSITION_OFFSET_Z);
	osg::Vec3 CAMERA_EYE_POSITION(CAMERA_EYE_POSITION_X, CAMERA_EYE_POSITION_Y, CAMERA_EYE_POSITION_Z);
	const float CAMERA_ROTATION_OFFSET(0);


	const unsigned int CAMERA_MASK_MAIN(1 << 0);
	const unsigned int CAMERA_MASK_RADAR(1 << 1);
	const unsigned int CAMERA_MASK_ROUTE(1 << 8);
	const unsigned int CAMERA_MASK_PLAYER[6] {1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7};
	const unsigned int CAMERA_MASK_NONE(0);



	// PHYSICS
	const float BIKE_FENCE_IMPACT_THRESHOLD_LOW(BIKE_MASS*BIKE_VELOCITY_MIN/2);
	const float BIKE_FENCE_IMPACT_THRESHOLD_HIGH(BIKE_MASS*BIKE_VELOCITY_MAX / 3);

	const float BIKE_DEFAULT_HEALTH(BIKE_FENCE_IMPACT_THRESHOLD_LOW);






	// study setup
	const QString MAIN_NORMAL_NAVI_NORMAL("Main:Normal Nav:Normal");
	const QString MAIN_NORMAL_NAVI_BENDED("Main:Normal Nav:Bended");
	const QString MAIN_NORMAL_NAVI_MAP("Main:Normal Nav:Map");
	const QString windowSetupChoices[] { MAIN_NORMAL_NAVI_NORMAL, MAIN_NORMAL_NAVI_BENDED, MAIN_NORMAL_NAVI_MAP };



}

btQuaternion fromTwoVectors(btVector3 u, btVector3 v)
{
	float norm_u_norm_v = sqrt(u.dot(u) * v.dot(v));
	float real_part = norm_u_norm_v + u.dot(v);
	btVector3 w;

	if (real_part < 1.e-6f * norm_u_norm_v)
	{
		/* If u and v are exactly opposite, rotate 180 degrees
		* around an arbitrary orthogonal axis. Axis normalisation
		* can happen later, when we normalise the quaternion. */
		real_part = 0.0f;
		w = abs(u.x()) > abs(u.z()) ? btVector3(-u.y(), u.x(), 0.f)
			: btVector3(0.f, -u.z(), u.y());
	}
	else
	{
		/* Otherwise, build quaternion the standard way. */
		w = u.cross(v);
	}

	return btQuaternion(real_part, w.x(), w.y(), w.z()).normalized();
}

