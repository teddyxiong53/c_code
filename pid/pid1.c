#include <stdio.h>


struct _pid 
{
	float SetSpeed;
	float ActualSpeed;
	float err;
	float err_last;//err last time
	float Kp,Ki,Kd;
	float voltage;
	float integral;//
} pid;

void PID_Init()
{
	printf("pid init begin \n");
	pid.SetSpeed = 0.0;
	pid.ActualSpeed = 0.0;
	pid.err = 0.0;
	pid.err_last = 0.0;
	pid.voltage = 0.0;
	pid.integral = 0.0;
	pid.Kp = 0.2;
	pid.Ki = 0.015;
	pid.Kd = 0.2;
	printf("pid init end \n");
}

float PID_Control(float speed)
{
	pid.SetSpeed = speed;
	pid.err = pid.SetSpeed - pid.ActualSpeed;
	pid.integral += pid.err;
	pid.voltage = pid.Kp*pid.err + pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_last);
	pid.err_last = pid.err;
	pid.ActualSpeed = pid.voltage * 1.0;
	return pid.ActualSpeed;
}

int main()
{
	PID_Init();
	int i = 0;
	while(i<1000)
	{
		float speed = PID_Control(200.0);
		printf("%f \n", speed);
		i ++;
	}
	return 0;
}

