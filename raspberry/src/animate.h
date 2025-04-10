#ifndef ANIMATE_H
#define ANIMATE_H

#define TOP_HEAD 0
#define UPPER_NECK 1
#define LEFT_EYE 2
#define RIGHT_EYE 3
#define LEFT_TRACK 4
#define LEFT_ARM 5
#define RIGHT_TRACK 6
#define RIGHT_ARM 7
#define BOTTOM_NECK 8


#define HEAD_DEF 100
#define HEAD_MIN 25
#define HEAD_MAX 175

#define U_NECK_DEF 90
#define U_NECK_MIN 25
#define U_NECK_MAX 110

#define L_EYE_DEF 95
#define L_EYE_MIN 50
#define L_EYE_MAX 95

#define R_EYE_DEF 95
#define R_EYE_MIN 65
#define R_EYE_MAX 95

#define L_TRACK_DEF 180
#define L_TRACK_MIN 1
#define L_TRACK_MAX 180

#define L_ARM_DEF 120
#define L_ARM_MIN 1
#define L_ARM_MAX 180

#define R_TRACK_DEF 90
#define R_TRACK_MIN 1
#define R_TRACK_MAX 180
//
#define R_ARM_DEF 55
#define R_ARM_MIN 1
#define R_ARM_MAX 180

#define B_NECK_DEF 130
#define B_NECK_MIN 30
#define B_NECK_MAX 170

// Servo Motor Values (Default POS) - (Min POS) - (Max POS)
int controls[9][3] = {
    {HEAD_DEF, HEAD_MIN, HEAD_MAX}, // Head
    {U_NECK_DEF, U_NECK_MIN, U_NECK_MAX},  // Upper Neck
    {L_EYE_DEF, L_EYE_MIN, L_EYE_MAX},   // Left Eye
    {R_EYE_DEF, R_EYE_MIN, R_EYE_MAX},   // Right Eye
    {L_TRACK_DEF, L_TRACK_MIN, L_TRACK_MAX},  // Left Track
    {L_ARM_DEF, L_ARM_MIN, L_ARM_MAX},  // Left Arm
    {R_TRACK_DEF, R_TRACK_MIN, R_TRACK_MAX},    // Right Track
    {R_ARM_DEF, R_ARM_MIN, R_ARM_MAX},   // Right Arm
    {B_NECK_DEF, B_NECK_MIN, B_NECK_MAX}, // Bottom Neck
};

//Eye Animation
int a0[7][10] = { { 0, 0, L_EYE_MAX, R_EYE_MAX, 0, 0, 0, 0, 0, 0},
                  { 0, 0, L_EYE_MIN, 0, 0, 0, 0, 0, 0, 12 },
                  { 0, 0, 0, R_EYE_MIN, 0, 0, 0, 0, 0, 12 },
                  { 0, 0, L_EYE_MAX, 0, 0, 0, 0, 0, 0, 12 },
                  { 0, 0, 0, R_EYE_MAX, 0, 0, 0, 0, 0, 12 },
                  { 0, 0, L_EYE_MIN, R_EYE_MIN, 0, 0, 0, 0, 0, 25 },
                  { 0, 0, L_EYE_MAX, R_EYE_MAX, 0, 0, 0, 0, 0, 12 },
                };


#endif