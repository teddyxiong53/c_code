#include <stdio.h>


struct _pid 
{
	float SetSpeed;
	float ActualSpeed;
	float err;
	float err_next;//number k-1
	float err_last;//number k-2
	float Kp,Ki,Kd;
	float voltage;
	
} pid;

void PID_Init()
{
	printf("pid init begin \n");
	pid.SetSpeed = 0.0;
	pid.ActualSpeed = 0.0;
	pid.err = 0.0;
	pid.err_last = 0.0;
	pid.voltage = 0.0;
	pid.err_next = 0.0;
	pid.Kp = 0.2;
	pid.Ki = 0.015;
	pid.Kd = 0.2;
	printf("pid init end \n");
}

float PID_Control(float speed)
{
	pid.SetSpeed = speed;
	pid.err = pid.SetSpeed - pid.ActualSpeed;

	float incrementSpeed = pid.Kp*(pid.err - pid.err_next) + 
		pid.Ki * pid.err +
		pid.Kd * (pid.err - 2*pid.err_next + pid.err_last);
		
	pid.ActualSpeed += incrementSpeed;
	pid.err_last = pid.err_next;
	pid.err_next = pid.err;
	
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

