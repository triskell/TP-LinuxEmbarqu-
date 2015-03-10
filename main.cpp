#include <iostream>
#include <fstream>
#include <functional>
#include <cmath>
using namespace std;

#include <dirent.h>

#include <stdlib.h>

#define PI 3.14159265358979323846264338327950288419716939937510


#define PWM "P9_14"
ofstream pwmFiles[3];

//===============================================================
// PWM



void pwmInit(){

	//Activate pwm on pin
	system((string("echo bone_pwm_")+PWM+" > /sys/devices/bone_capemgr.9/slots").c_str());
	string pinPath = string("/sys/devices/ocp.3/pwm_test_")+PWM;

	pwmFiles[0].open(string(pinPath+"/polarity"), ios_base::out);
	pwmFiles[1].open(string(pinPath+"/period"), ios_base::out);
	pwmFiles[2].open(string(pinPath+"/duty"), ios_base::out);

	if(pwmFiles[0].is_open()
	  +pwmFiles[1].is_open()
	  +pwmFiles[2].is_open()<3)
		cout<<"Error: Some PWM files could not be opened (pinpath="<<pinPath<<")"<<endl;
}

void pwmSetPolarity(bool polarity){
	pwmFiles[0]<<(int)polarity;
	pwmFiles[0].flush();
}
void pwmSetPeriod(long periodNS){
	pwmFiles[1]<<periodNS;
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

	// auto start = chrono::system_clock::now();
	// auto duration = chrono::microseconds((int)(durationSec*1000000));
	// auto period = chrono::microseconds(periodUS);

	// chrono::duration<double> elapsedTime;
	// auto lastLoop = chrono::system_clock::now();
	// do{
	// 	elapsedTime=chrono::system_clock::now()-start;
	// 	pwmSetAngle(f(chrono::duration_cast<chrono::microseconds>(elapsedTime).count()/1000000.0));

	// 	// this_thread::sleep_for(chrono::system_clock::now()-lastLoop);
	// 	lastLoop = chrono::system_clock::now();
	// }while(elapsedTime<duration);
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

	pwmSetPeriod(20000000);
	pwmSetPolarity(0);



	// // Function parameters
	// auto dur = 2.0;
	// auto start = 30;
	// auto end = 90;
	// // END parameters

	// pwmSetDutyFunction([&](float t){

	// 	auto deplacement = end - start ;
	// 	return (1 - cos(t * (PI / 2.0) / dur)) * deplacement + start;
	// }, dur, 20000000);

	cout<<"Yolooooo"<<endl;
	return 0;
}
