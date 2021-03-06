#pragma once
#ifndef _CONSTANTS_H
#define _CONSTANTS_H 
// OSG
#include <osg/Array>
#include "forwarddeclarations.h"
#include <btBulletDynamicsCommon.h>
#include "qstring.h"

// http://stackoverflow.com/questions/3681140/how-do-i-avoid-both-global-variables-and-magic-numbers



namespace troen
{
	enum trackDifficulty
	{
		DIFFICULTY_EASY,
		DIFFICULTY_HARD
	};


	enum windowType
	{
		MAIN_WINDOW,
		NAVIGATION_WINDOW,
		BOTH_WINDOWS
	};

	typedef std::pair<btScalar, btScalar> btPoint;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////#Script Modifiable #////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	extern int DEFAULT_MAINWINDOW_WIDTH;
	extern int DEFAULT_MAINWINDOW_HEIGHT;
	extern double WINDOW_RATIO_FULLSCREEN;

	extern int USE_CULLING;


	extern double FOVY_INITIAL;
	extern double FOVY_ADDITION_MAX;
	extern double FOVY_INITIAL_NAVI;
	extern const double FOVY_DELTA_MAX;

	// LOGIC
	extern double RESPAWN_DURATION;
	extern double RANDOM_NUMBER_DURATION;
	extern double RANDOM_NUMBER_INTERVAL;
	extern double GAME_START_COUNTDOWN_DURATION;

	// AUDIO
	extern int ENGINE_FREQUENCY_LOW;
	extern int ENGINE_FREQUENCY_HIGH;
	extern double AUDIO_PITCH_FACTOR;


	//car engine
	extern double SPEED_TOREAL_RATIO;
	extern double DAMPING_FORCE;

	//STEERING
	extern double BIKE_STEERING_INCREMENT;
	extern double BIKE_STEERINGCLAMP;


	// BENDED VIEWS
	extern int BENDED_VIEWS_ACTIVATED;
	extern int BENDED_VIEWS_DEACTIVATED;
	extern int TIME_TO_ACTIVATE_BENDED_VIEWS;
	extern int BENDED_STYLING_PRESET;
	extern int BENDED_DEFORMATION_END;
	extern double BENDED_DEFORMATION_START;

	//CAMERA
	extern double CAMERA_POSITION_OFFSET_X;
	extern double CAMERA_POSITION_OFFSET_Y;
	extern double CAMERA_POSITION_OFFSET_Z;

	extern double CAMERA_LOOKAT_POSITION_X;
	extern double CAMERA_LOOKAT_POSITION_Y;
	extern double CAMERA_LOOKAT_POSITION_Z;


	extern double CAMERA_EYE_POSITION_X;
	extern double CAMERA_EYE_POSITION_Y;
	extern double CAMERA_EYE_POSITION_Z;

	extern double CAMERA_NAVI_EYE_POSITION_Z;
	extern double CAMERA_NAVI_CENTER_POSITION_Z;


	/////////////////////////////////////////////////////////////
	////////////////////Constant ////////////////////////////////
	/////////////////////////////////////////////////////////////


	// GENERAL

	// GAME
	extern const float DEFAULT_SOUND_VOLUME;

	extern const int NUM_MULTISAMPLES;

	//PHYSIS
	extern const btVector3 DEFAULT_GRAVITY;

	// BIKE
	extern const btVector3 BIKE_DIMENSIONS;
	extern const osg::Vec3f BIKE_VIEW_SCALE_FACTORS;
	extern const osg::Vec3f BIKE_VIEW_TRANSLATE_VALUES;

	extern const float BIKE_MASS;
	extern const int BIKE_VELOCITY_MAX;
	extern const int BIKE_VELOCITY_MIN;
	extern const int BIKE_TURBO_VELOCITY_MAX;
	extern const float BIKE_VELOCITY_DAMPENING_TERM;
	extern const float BIKE_ACCELERATION_FACTOR_MAX;
	extern const float BIKE_DECELERATION_FACTOR;
	extern const float BIKE_TURN_FACTOR_MAX;
	extern const float BIKE_ANGULAR_DAMPENING_TERM;
	extern const float THRESHOLD_FOR_ABRUPT_VELOCITY_CHANGE;
	extern const float BIKE_VIEWING_ANGLE_DAMPENING_TERM;



	extern const float TURBO_PHASE_LENGTH;

	extern const float BIKE_TILT_DAMPENING;
	extern const float BIKE_TILT_MAX;
	extern const float BIKE_WHEELY_TILT_MAX;


	//INPUT
	extern const float BIKE_MOVE_VALUE;
	extern const float BIKE_ROTATION_VALUE;
	extern const float BIKE_HANDBRAKE_FACTOR;
	extern const int POLLING_DELAY_MS;
	extern const int VIBRATION_TIME_MS;

	// LEVEL
	extern const int LEVEL_SIZE;
	extern const double MAP_UNITS_PER_METER;

	// FENCE
	// determines how accurate the fence will be
	extern const float ROUTE_WIDTH_MODEL;
	extern const float ROUTE_HOVER_HEIGHT;
	extern const float ROUTE_WIDTH_VIEW;
	extern const float FENCE_PART_LENGTH;
	extern const float FENCE_PART_WIDTH;
	extern const int DEFAULT_MAX_FENCE_PARTS;
	extern const int FENCE_TO_MINIMAP_PARTS_RATIO;

	// CAMERA
	extern osg::Vec3 CAMERA_POSITION_OFFSET;
	extern osg::Vec3 CAMERA_EYE_POSITION;
	extern const float CAMERA_ROTATION_OFFSET;

	extern const float CAMERA_TILT_FACTOR;
	extern const int HUD_PROJECTION_SIZE;

	extern const unsigned int CAMERA_MASK_MAIN;
	extern const unsigned int CAMERA_MASK_RADAR;
	extern const unsigned int CAMERA_MASK_ROUTE;
	extern const unsigned int CAMERA_MASK_PLAYER[6];
	extern const unsigned int CAMERA_MASK_NONE;

	// PHYSICS
	extern const float BIKE_FENCE_IMPACT_THRESHOLD_LOW;
	extern const float BIKE_FENCE_IMPACT_THRESHOLD_HIGH;

	extern const float BIKE_DEFAULT_HEALTH;



	//study Setup
	extern const QString MAIN_BENDED_NAVI_MAP;
	extern const QString MAIN_NORMAL_NAVI_NORMAL;
	extern const QString MAIN_NORMAL_NAVI_MAP;
	extern const QString MAIN_NORMAL_NAVI_BENDED;

	extern const QString windowSetupChoices[];

}

#endif

// MACROS FROM CG

#include <cmath>

// NOTE: When using powers, try to use Horner's Method

#define PI 3.1415926535897932384626433832795L
#define PI2  (PI * 2.00L)
#define PI_2 (PI * 0.50L)
#define PI4  (PI * 4.00L)
#define PI_4 (PI * 0.25L)

#define round(v)\
	floor(v + 0.5)

#define abs(v) \
	(((v) < 0) ? -(v) : (v))

#define sign(v) \
	(((v) < 0) ? -1 : 1)

#define intcast(v) \
	(static_cast<int>(v))

#define shortcast(v) \
	(static_cast<short>(v))

#define mod(a, m) \
	((a)-(m)* (intcast((a) / (m)) - ((a) < 0 ? 1 : 0)))

#define frac(x) \
	((x)-intcast(x))

#define clamp(l, u, x) \
	((x) < (l) ? (l) : (x) > (u) ? (u) : (x))

#define deg(rad) \
	((rad)* 180.0L / PI)

#define rad(deg) \
	((deg)* PI / 180.0L)

#define mi(a, b) \
	((a) < (b) ? (a) : (b))

#define ma(a, b) \
	((a) < (b) ? (b) : (a))

// same as hour
// note: if d is negative use -decimal(d, m, s) instead of decimal(-d, m, s)
#define decimal(d, m, s) \
	((d)+((m)+(s) / 60.0L) / 60.0L)

#define sind(deg) \
	(sin(rad(deg)))

#define cosd(deg) \
	(cos(rad(deg)))

#define tand(deg) \
	(tan(rad(deg)))

#define asind(rad) \
	(deg(asin(rad)))

#define acosd(rad) \
	(deg(acos(rad)))

#define atand(rad) \
	(deg(atan(rad)))

#define atan2d(x, y) \
	(deg(atan2(x, y)))

#define adiameter(D, r) \
	(2 * atan(r / D))

// normalizes an angle to between 0 and 2PI radians
#define rev(rad) \
	((rad)-floor((rad) / PI2) * PI2)

// normalizes an angle to between 0 and 360 degrees
#define revd(deg) \
	((deg)-floor((deg) / 360.0L) * 360.0L)

// cube root (e.g. needed for parabolic orbits)
#define cbrt(x) \
	(((x) > 0.0) ? exp(log(x) / 3.0L) : (((x) < 0.0) ? -cbrt(-(x)) : 0.0))


#define __b02(x) (     (x) | (     (x) >>  1))
#define __b04(x) (__b02(x) | (__b02(x) >>  2))
#define __b08(x) (__b04(x) | (__b04(x) >>  4))
#define __b16(x) (__b08(x) | (__b08(x) >>  8))
#define __b32(x) (__b16(x) | (__b16(x) >> 16))

// Returns the next power of an integer.
#define nextPowerOf2(x) \
	(__b32((x)-1) + 1)

#define prevPowerOf2(x) \
	(nextPowerOf2(x) >> 1)

#define randf(min, max) \
	(static_cast<float>(rand()) / RAND_MAX * ((max)-(min)) + (min))

#define _rand(min, max) \
	(static_cast<int>(static_cast<float>(rand()) / RAND_MAX * ((max)-(min)) + (min)))


// Interpolate

#define linear(t)       (t)

#define smoothstep(t)   ((t) * (t) * (3 - 2 * (t)))
#define smoothstep2(t)  (smoothstep(smoothstep(t)))
#define smoothstep3(t)  (smoothstep(smoothstep2(t)))

#define smootherstep(t) ((t) * (t) * (t) * ((t) * (6 * (t) - 15) + 10))

#define squared(t)      ((t) * (t))
#define invsquared(t)   (1 - (1 - (t)) * (1 - (t)))

#define cubed(t)        ((t) * (t) * (t))
#define invcubed(t)     (1 - (1 - (t)) * (1 - (t)) * (1 - (t)))

//#define sin(t)          (sin(t * 1.57079632679489661923))
#define invsin(t)       (1 - sin((1 - (t)) * 1.57079632679489661923))


#define smoothstep_ext(t, l, r) \
	((t) < (l) ? 0 : (r) < (t) ? 1 : smoothstep(((t)-(l)) / ((r)-(l))))

#define btToOSGVec3(v) \
	osg::Vec3(v.x(), v.y(), v.z())

#define btToOSGQuat(q) \
	osg::Quat(q.x(), q.y(), q.z(), q.w())

#define OSGToBtQuat(q) \
	btQuaternion(q.x(), q.y(), q.z(), q.w())

#define toVec2(v) \
	osg::Vec2(v.x(), v.y())

#define osgToBtVec3(v) \
	(btVector3(v.x(), v.y(), v.z()))

#define pairToVec2(v) \
	osg::Vec2(v.first, v.second)

//component Multiplication
#define compMult(v1,v2) \
	osg::Vec2(v1.x()*v2.x(), v1.y() * v2.y())

#define compDiv(v1,v2) \
	osg::Vec2(v1.x() / v2.x(), v1.y() / v2.y())

#define vecToString(v) \
	(std::to_string(v.x()) + " " + std::to_string(v.y()) + " " + std::to_string(v.z()))

#define Cout(str) \
	(std::cout << str << std::endl)

// Several interpolation methods in action: http://sol.gfxile.net/interpolation/

enum InterpolationMethod
{
	InterpolateLinear
	, InterpolateSmoothStep
	, InterpolateSmoothStep2
	, InterpolateSmoothStep3
	, InterpolateSmootherStep // Ken Perlin
	, InterpolateSquared
	, InterpolateInvSquared
	, InterpolateCubed
	, InterpolateInvCubed
	, InterpolateSin          // strong in, soft out
	, InterpolateInvSin       // soft in, strong out
};


template<typename T>
inline const T interpolate(
	const T t
	, const InterpolationMethod customCollisionDetection = InterpolateLinear)
{
	switch (customCollisionDetection)
	{
	case InterpolateSmoothStep:
		return smoothstep(t);
	case InterpolateSmoothStep2:
		return smoothstep2(t);
	case InterpolateSmoothStep3:
		return smoothstep3(t);
	case InterpolateSmootherStep:
		return smootherstep(t);
	case InterpolateSquared:
		return squared(t);
	case InterpolateInvSquared:
		return invsquared(t);
	case InterpolateCubed:
		return cubed(t);
	case InterpolateInvCubed:
		return invcubed(t);
	case InterpolateSin:
		return static_cast<T>(sin(t));
	case InterpolateInvSin:
		return static_cast<T>(invsin(t));
	default:
	case InterpolateLinear:
		return linear(t);
	}
}

/* Build a unit quaternion representing the rotation
* from u to v. The input vectors need not be normalised. */
btQuaternion fromTwoVectors(btVector3 u, btVector3 v);
