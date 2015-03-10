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

	// Activate pwm on pin
	// Only activate slot once after device boot
	// -> Comment line below for further launches.
	// system("echo am33xx_pwm > /sys/devices/bone_capemgr.9/slots");
	system("echo bone_pwm_P9_14 > /sys/devices/bone_capemgr.9/slots");
	string pinPath = "/sys/devices/ocp.3/pwm_test_P9_14.15";

	// Wait previous command to finish
	usleep(10000);

	// Open PWM drivers registers
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

// Set PWM angle (not progressive)
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
		pwmSetAngle(f((float)elapsedTicks/(float)CLOCKS_PER_SEC)); // Set angle propotional (according to f lambda) to the elapsed time since last movement
		//cout<<elapsedTicks/(float)CLOCKS_PER_SEC<<":"<<f((float)elapsedTicks/(float)CLOCKS_PER_SEC)<<endl;

		auto fromLastLoop = clock()-lastLoop; // Loop time
		usleep( (period-fromLastLoop) / (float)CLOCKS_PER_SEC / 1000000.0 ); // Sleep remaining time (loop_measured_time - loop_theorical_period)

	}while(elapsedTicks<duration);
}

//===============================================================
// Movement formulas

// Integrate(sin(x)) = -cos(x) (integrate speed => position)
float sinSimple(float t, float angle_debut, float angle_fin, float duree_du_deplacement){
	auto deplacement = angle_fin - angle_debut ;
	return (1 - cos(t * PI / duree_du_deplacement)) * deplacement + angle_debut;
}

// Integrate(sin^2(x)) = 	-sin(2·x)/4 - x/2 (integrate speed => position)
float sinSquare(float t, float angle_debut, float angle_fin, float duree_du_deplacement){
	auto deplacement = angle_fin - angle_debut ;
	return ( ( 1 - sin( 4 * t * PI / duree_du_deplacement)) / 4 - t / 2 ) * deplacement + angle_debut;
}



//===============================================================
// Main

void cmd_servo_hard_progressive(float angle_debut, float angle_fin, float duree_du_deplacement, int Mode){

	// Use of Lambda function to pass the kind of movement to do
	pwmSetDutyFunction([&](float t){
		if(Mode == 1){
			return sinSquare(t, angle_debut, angle_fin, duree_du_deplacement);
		}
		else {
			return sinSimple(t, angle_debut, angle_fin, duree_du_deplacement);
		}
	}, duree_du_deplacement, 20000000);
}


int main(int argc, char const *argv[])
{
	pwmInit();

	// Set PWM drivers parameters
	pwmSetPeriod(20000);
	pwmSetPolarity(0);

	// Movement function parameters
	auto dur = 1.0;	// in seconds
	auto start = -45;
	auto end = 45;
	auto mode = 1;
	// END movement parameters

	// Continuous movement
	while(1){

		// move forward
		pwmSetDutyFunction([&](float t){
			auto deplacement = end - start ;
			return (1 - cos(t * (PI / 2.0) / dur)) * deplacement + start;
		}, dur, 2000000);


		// Move back
		pwmSetDutyFunction([&](float t){
			auto deplacement = start - end ;
			return (1 - cos(t * (PI / 2.0) / dur)) * deplacement + end;
		}, dur, 2000000);
	}

	return 0;
}
