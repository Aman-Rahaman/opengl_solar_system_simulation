
#ifndef _SIMULATION_CONFIGURATION_HPP_
#define _SIMULATION_CONFIGURATION_HPP_


#define CAMERA_RADIUS       35.0f
#define CAMERA_SPEED        0.02f


#define EARTH_REV_RADIUS    8.0f
#define SUN_REV_RADIUS      0.0f
#define MOON_REV_RADIUS     2.0f
#define SPACE_REV_RADIUS    0.0f


#define EARTH_REV_SPEED     0.005f
#define SUN_REV_SPEED       0.0f
#define MOON_REV_SPEED      0.015f
#define SPACE_REV_SPEED     0.0f


#define EARTH_ROT_SPEED     0.01f
#define SUN_ROT_SPEED       0.008f
#define MOON_ROT_SPEED      0.04f
#define SPACE_ROT_SPEED     0.000008f


#define EARTH_RADIUS    1.0f
#define SUN_RADIUS      5.0f
#define MOON_RADIUS     0.25f
#define SPACE_RADIUS    50.0f


// do no increase the TESSELLATION more than 7. it increases exponentially
#define EARTH_TESSELLATION    5
#define SUN_TESSELLATION      5
#define MOON_TESSELLATION     5
#define SPACE_TESSELLATION    3


#endif
