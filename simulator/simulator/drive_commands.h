#ifndef DRIVE_COMMANDS_H
#define DRIVE_COMMANDS_H

#define ROLL_MOTOR 0
#define LIFT_MOTOR 1

#define ROLL_DC_BRAKE 60
#define LIFT_DC_BRAKE 40

#define MB_SPEED     0x0002
#define MB_ACC_M1    0x1104
#define MB_DEC_M1    0x1106
#define MB_ACC_M2    0x2104
#define MB_DEC_M2    0x2106
#define MB_LOAD      0x100F

#define MB_COIL_DRIVESTATE  0x0001
#define MB_ENABLED 1
#define MB_DISABLED 0

#define MB_COIL_DIRECTION  0x0002
#define MB_FORWARD 1
#define MB_BACKWARD 0

#define MB_COIL_SELECT_MOTOR  0x0008
#define MB_MOTOR1 0
#define MB_MOTOR2 1

#define SPEED  121                     // "Speed" command to the Commander
#define ACC    211                        // "Acc" command to the Commander
#define DEC    221                        // "Dec" command to the Commander

#endif // DRIVE_COMMANDS_H
