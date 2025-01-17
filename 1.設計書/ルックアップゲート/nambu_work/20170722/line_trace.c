#include "line_trace.h"
#include "Distance.h"
/* ルックアップゲート状態遷移用 */
#include "look_up_gate.h"
#include "common.h"

#define DISTANCE_NOTIFY (1000.0)

//*****************************************************************************
// 関数名 : line_tarce_main
// 引数 : 
// 返り値 : なし
// 概要 : 
//       
//*****************************************************************************

signed char forward;      /* 前後進命令 */
signed char turn;         /* 旋回命令 */
signed char pwm_L, pwm_R; /* 左右モータPWM出力 */

void line_tarce_main()
{
    int32_t motor_ang_l, motor_ang_r;
    int gyro, volt;

    if (ev3_button_is_pressed(BACK_BUTTON)) return;

    tail_control(TAIL_ANGLE_DRIVE); /* バランス走行用角度に制御 */

    if (sonar_alert() == 1) /* 障害物検知 */
    {
        /* 試しにメイン関数にあったルックアップゲートの距離計測を移植 */
    	/* 障害物までの距離を計測，減速開始距離である */
            if (LOOK_UP_GATE_BRAKE_DISTANCE == look_up_gate_get_distance()) {
                /* 障害物を検知したらすぐに停止するのではなく */
                /* 徐々に減速し、ゲート5�p手前で停止させるようにする */
                /* 減速開始は、検知した障害物までの距離が15�p手前から(妥当か微妙) */
                /* 本番のコースはポールが設置してある為、誤検知等も考慮する */
                
                /* TODO */

                /* 障害物までの距離を計測，ゲート攻略開始距離である */
                if (LOOK_UP_GATE_DISTANCE == look_up_gate_get_distance()) {
                    /* ルックアップゲート攻略状態 */
                    main_status = STAT_LOOK_UP_GATE;

                    /* 走行体を停止 */
                    ev3_motor_stop(left_motor, true);
                    ev3_motor_stop(right_motor, true);
                }
            }
    }
    else
    {
        forward = 30; /* 前進命令 */
        if (ev3_color_sensor_get_reflect(color_sensor) >= (LIGHT_WHITE + LIGHT_BLACK)/2)
        {
            turn =  20; /* 左旋回命令 */
        }
        else
        {
            turn = -20; /* 右旋回命令 */
        }
    }

    /* 倒立振子制御API に渡すパラメータを取得する */
    motor_ang_l = ev3_motor_get_counts(left_motor);
    motor_ang_r = ev3_motor_get_counts(right_motor);
    gyro = ev3_gyro_sensor_get_rate(gyro_sensor);
    volt = ev3_battery_voltage_mV();

    /* 倒立振子制御APIを呼び出し、倒立走行するための */
    /* 左右モータ出力値を得る */
    balance_control(
        (float)forward,
        (float)turn,
        (float)gyro,
        (float)GYRO_OFFSET,
        (float)motor_ang_l,
        (float)motor_ang_r,
        (float)volt,
        (signed char*)&pwm_L,
        (signed char*)&pwm_R);

    /* EV3ではモーター停止時のブレーキ設定が事前にできないため */
    /* 出力0時に、その都度設定する */
    if (pwm_L == 0)
    {
         ev3_motor_stop(left_motor, true);
    }
    else
    {
        ev3_motor_set_power(left_motor, (int)pwm_L);
    }
    
    if (pwm_R == 0)
    {
         ev3_motor_stop(right_motor, true);
    }
    else
    {
        ev3_motor_set_power(right_motor, (int)pwm_R);
    }
	
	Distance_update(); /* 移動距離加算 */
	
	if( Distance_getDistance() > DISTANCE_NOTIFY )
	{
		/* DISTANCE_NOTIFY以上進んだら音を出す */
		ev3_speaker_set_volume(100); 
		ev3_speaker_play_tone(NOTE_C4, 100);
		
		/* 距離計測変数初期化 */
		Distance_init();
	}
	
}

/* end of file */
