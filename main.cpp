#include <iostream>
#include <fstream>
#include <functional>
#include <cmath>
#include <ctime>
using namespace std;

#include <dirent.h>
#include <unistd.h>

#include <stdlib.h>

#define PI 3.14159265358979323846264338327950288419716939937510


#define PWM "P9_14"
ofstream pwmFiles[3];

//===============================================================
// PWM



void pwmInit(){

	//Activate pwm on pin
	// system("echo am33xx_pwm > /sys/devices/bone_capemgr.9/slots");
	system("echo bone_pwm_P9_14 > /sys/devices/bone_capemgr.9/slots");
	string pinPath = "/sys/devices/ocp.3/pwm_test_P9_14.15";

	usleep(10000);

	pwmFiles[0].open(string(pinPath+"/polarity"), ios_base::out);
	pwmFiles[1].open(string(pinPath+"/period"), ios_base::out);
	pwmFiles[2].open(string(pinPath+"/duty"), ios_base::out);

	if(pwmFiles[0].is_open()
	  +pwmFiles[1].is_open()
	  +pwmFiles[2].is_open()<3)
		cout<<"Error: Some PWM files could not be opened (pinpath="<<pinPath<<"): "<<pwmFiles[0].is_open()<<"|"<<pwmFiles[1].is_open()<<"|"<<pwmFiles[2].is_open()<<endl;
}

void pwmSetPolarity(bool polarity){
	pwmFiles[0]<<(int)polarity;
	pwmFiles[0].flush();
}
void pwmSetPeriod(long periodUS){
	pwmFiles[1]<<periodUS*1000.0;
	pwmFiles[1].flush();
}
void pwmSetDuty(long dutyNS){
	pwmFiles[2]<<dutyNS;
	pwmFiles[2].flush();
}


void pwmSetAngle(float angleDeg){
	pwmSetDuty(((angleDeg/90.0) + 1.5) * 1000000);
}


// f(float seconds){return angleDeg;}
void pwmSetDutyFunction(function<double(double)> f, float durationSec, long periodUS){

	clock_t start = clock();
	clock_t duration = durationSec*CLOCKS_PER_SEC;
	clock_t period = periodUS*(float)CLOCKS_PER_SEC/1000000.0;

	clock_t elapsedTicks;
	do{
		clock_t lastLoop = clock();

		elapsedTicks = clock()-start;
		pwmSetAngle(f((float)elapsedTicks/(float)CLOCKS_PER_SEC));
		//cout<<elapsedTicks/(float)CLOCKS_PER_SEC<<":"<<f((float)elapsedTicks/(float)CLOCKS_PER_SEC)<<endl;

		auto fromLastLoop = clock()-lastLoop;
		usleep( (period-fromLastLoop) / (float)CLOCKS_PER_SEC / 1000000.0 );

	}while(elapsedTicks<duration);
}



//===============================================================
// Main

void cmd_servo_hard_progressive(float angle_debut, float angle_fin, float duree_du_deplacement){

	pwmSetDutyFunction([&](float t){

		auto deplacement = angle_fin - angle_debut ;
		return (1 - cos(t * (PI / 2.0) / duree_du_deplacement)) * deplacement + angle_debut;
	}, duree_du_deplacement, 20000000);
}


int main(int argc, char const *argv[])
{
	pwmInit();

	pwmSetPeriod(20000);
	pwmSetPolarity(0);

	// // Function parameters
	auto dur = 1.0;
	auto start = -45;
	auto end = 45;
	// // END parameters

	while(1){

		pwmSetDutyFunction([&](float t){
			auto deplacement = end - start ;
			return (1 - cos(t * (PI / 2.0) / dur)) * deplacement + start;
		}, dur, 2000000);


		pwmSetDutyFunction([&](float t){
			auto deplacement = start - end ;
			return (1 - cos(t * (PI / 2.0) / dur)) * deplacement + end;
		}, dur, 2000000);
	}

	return 0;
}
